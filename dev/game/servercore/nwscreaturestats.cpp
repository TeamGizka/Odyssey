#include "nwscreaturestats.h"
#include "nwsitem.h"
#include "nwitem.h"
#include "nwsobject.h"
#include "nwsplaceable.h"
#include "nwsarea.h"
#include "nwitem.h"
#include "nwmessage.h"
#include "nwrules.h"
#include "nwrace.h"
#include "nwspell.h"
#include "nwfeat.h"
#include "nwskill.h"
#include "nwclass.h"
#include "nwscombatround.h"
#include "swmain.h"
#include <math.h>
#include <stdlib.h>
#include "appmanager.h"
#include "serverexoapp.h"
#include "serverinfo.h"
#include "serveraimaster.h"
#include "nwsmodule.h"
#include "nwsscriptevent.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwcreaturelevelstats.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "resgff.h"
#include "swmain.h"
#include "partytables.h"
#include "nwsplayer.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "nwscript.h"

static inline void NwscreaturestatsTrace2();
static inline void NwscreaturestatsTrace1();
#define NWSCREATURESTATS_TRACE() NwscreaturestatsTrace1()

enum { DEFAULT_CRITICAL_HIT_ROLL = 20, INVALID_FACTION = -1,
       NO_DUPLICATING_HEAD = 255 };
// PC 0x006955A0
int CSWSCreatureStats::GetFavoredEnemyBonus(CSWSCreature* pCreature)
{

    for (int nClass = 0; nClass < m_nClassCount; nClass++)
    {
    }
    return 0;
}
// PC 0x006FB590
CSWSStats_SpellLikeAbility::CSWSStats_SpellLikeAbility()
{
    m_nSpell = 0;
    m_nReady = 1;
    m_nLevel = 0;
}
// PC 0x005F0720
void CSWSCreatureStats_ClassInfo::AddKnownSpell(unsigned char nLevel,
                                               unsigned long nSpell)
{
    if (nLevel < 1)
    {
        if (m_lstKnownSpells[nLevel].IndexOf(nSpell) == -1)
        {
            m_lstKnownSpells[nLevel].Add(nSpell);
        }
    }
}

int CSWSCreatureStats::GetWeaponFocus(CSWSItem* pItem)
{
    int nBonus = 0;
    if (pItem != NULL && pItem->GetBaseItem() != NULL)
    {
        unsigned short nFeat = pItem->GetBaseItem()->m_nFocFeat;
        if (nFeat != 0 && HasFeat(nFeat))
        {
            nBonus = 1;
        }
    }
    return nBonus;
}
// PC 0x005F7850
CSWSCreatureStats_ClassInfo::CSWSCreatureStats_ClassInfo()
{
    m_nClass = 0;
    m_nLevel = 0;
    m_nNegativeLevels = 0;
    m_nClassData = 0;
    m_nClassState[0] = 0;
    m_nClassState[1] = 0;
    m_nClassState[2] = 1;

    memset(m_nSpellsPerDayLeft, 0, sizeof(m_nSpellsPerDayLeft));
    memset(m_nAdditionalSpellCounts, 0, sizeof(m_nAdditionalSpellCounts));
    memset(m_nSpellCounts, 0, sizeof(m_nSpellCounts));
}
// PC 0x005F0690
CSWSCreatureStats_ClassInfo::~CSWSCreatureStats_ClassInfo()
{
    m_lstKnownSpells[0].SetSize(0);
}
// PC 0x005F0790
int CSWSCreatureStats::GetMeleeAttackBonus(int bOffHand, int bIncludeBase,
                                         int bTouchAttack, int bForceOnHand)
{
    int nBonus = 0;
    CSWSItem* pWeapon;
    if (bOffHand && !bForceOnHand)
    {
        pWeapon = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    }
    else
    {
        pWeapon = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    }

    if (pWeapon != NULL &&
        g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nRangedWeapon > 0)
    {
        return 0;
    }

    if (bIncludeBase == 1)
    {
        int nCapped = 0;
        nBonus = m_pCreature->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_ATTACK, NULL, nCapped, 0, 0, 0, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, bOffHand);
        nBonus += GetBaseAttackBonus();
    }

    int nStrengthBonus = 0;
    int nDexterityBonus = 0;
    if (GetWeaponFinesse(pWeapon))
    {
        nDexterityBonus += GetDEXMod(0);
    }
    else
    {
        nStrengthBonus += m_nStrengthModifier;
    }
    nBonus += nDexterityBonus + nStrengthBonus;
    nBonus += GetWeaponFocus(pWeapon);

    int nLightsaberBonus = 0;
    if (pWeapon != NULL && pWeapon->IsLightsaber())
    {
        if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_LIGHTSABER_III))
        {
            nLightsaberBonus = 3;
        }
        else if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_LIGHTSABER_II))
        {
            nLightsaberBonus = 2;
        }
        else if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_LIGHTSABER_I))
        {
            nLightsaberBonus = 1;
        }
    }
    nBonus += nLightsaberBonus;

    if (!bTouchAttack)
    {
        int nTwoWeaponPenalty = 0;
        CSWSItem* pOtherWeapon;
        if (!bForceOnHand)
        {
            pOtherWeapon = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        }
        else
        {
            pOtherWeapon = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        }
        if (!bOffHand)
        {
            if (pOtherWeapon != NULL &&
                g_pRules->m_pBaseItems->GetBaseItem(pOtherWeapon->GetBaseItemType())->m_nWeaponType != 0)
            {
                nTwoWeaponPenalty = -6;
                if (m_pCreature->GetRelativeWeaponSize(pOtherWeapon) == -1 ||
                    g_pRules->m_pBaseItems->GetBaseItem(pOtherWeapon->GetBaseItemType())->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
                {
                    nTwoWeaponPenalty = -4;
                }
                if (HasFeat(CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING))
                {
                    nTwoWeaponPenalty += 4;
                }
                else if (HasFeat(CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING))
                {
                    nTwoWeaponPenalty += 2;
                }
                if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_III))
                {
                    nTwoWeaponPenalty += 2;
                }
                else if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_II))
                {
                    nTwoWeaponPenalty += 2;
                }
                else if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_I))
                {
                    nTwoWeaponPenalty += 1;
                }
            }
        }
        else if (pOtherWeapon != NULL)
        {
            nTwoWeaponPenalty = -10;
            if (HasFeat(CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponPenalty = -2;
            }
            else if (HasFeat(CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponPenalty = -4;
            }
            else if (HasFeat(CSWFeat::FEAT_AMBIDEXTERITY))
            {
                nTwoWeaponPenalty = -6;
            }
            if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_III))
            {
                nTwoWeaponPenalty += 1;
            }
        }
        nBonus += nTwoWeaponPenalty;
    }

    int nDuelingBonus = 0;
    CSWSItem* pRightHand = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    CSWSItem* pLeftHand = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    int bDueling = FALSE;
    if (pRightHand != NULL && pLeftHand == NULL)
    {
        CSWBaseItem* pBaseItem = pRightHand->GetBaseItem();
        if (pBaseItem != NULL &&
            (pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 ||
             pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4))
        {
            bDueling = TRUE;
        }
    }
    else if (pRightHand == NULL && pLeftHand == NULL)
    {
        bDueling = TRUE;
    }
    if (bDueling)
    {
        if (HasFeat(CSWFeat::FEAT_MASTER_DUELING))
        {
            nDuelingBonus = 3;
        }
        else if (HasFeat(CSWFeat::FEAT_IMPROVED_DUELING))
        {
            nDuelingBonus = 2;
        }
        else if (HasFeat(CSWFeat::FEAT_DUELING))
        {
            nDuelingBonus = 1;
        }
    }
    return nBonus + nDuelingBonus;
}
// PC 0x005F0AE0
int CSWSCreatureStats::GetMeleeDamageBonus(int bOffHand)
{
    CSWSItem* pItem;
    if (bOffHand)
    {
        pItem = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        if (pItem == NULL ||
            g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nWeaponWield !=
                CSWBaseItem::WIELD_DOUBLE_WEAPON)
        {
            pItem = NULL;
        }
    }
    else
    {
        pItem = m_pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    }

    if (pItem != NULL &&
        g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nRangedWeapon > 0)
    {
        return 0;
    }

    int nDamageBonus;
    if (bOffHand)
    {
        int nStrengthBonus;
        if (m_nStrengthModifier > 0)
            nStrengthBonus = (int)(m_nStrengthModifier * 0.5f);
        else
            nStrengthBonus = m_nStrengthModifier;
        nDamageBonus = nStrengthBonus;
    }
    else
    {
        int bTwoHanded = 0;
        if (pItem != NULL)
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem->m_nWeaponSize - m_pCreature->m_nCreatureSize == 1 &&
                pBaseItem->m_nWeaponWield != CSWBaseItem::WIELD_DOUBLE_WEAPON)
            {
                bTwoHanded = 1;
            }
        }

        if (bTwoHanded)
        {
            int nStrengthBonus;
            if (m_nStrengthModifier > 0)
                nStrengthBonus = (int)(m_nStrengthModifier * 1.5f);
            else
                nStrengthBonus = m_nStrengthModifier;
            nDamageBonus = nStrengthBonus;
        }
        else
        {
            nDamageBonus = m_nStrengthModifier;
        }
    }

    int nSpecializationBonus = 0;
    if (GetWeaponSpecialization(pItem))
        nSpecializationBonus = WEAPON_SPECIALIZATION_DAMAGE_BONUS;
    nDamageBonus += nSpecializationBonus;

    if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_III) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_III))
    {
        nDamageBonus += 6;
    }
    else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_II) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_II))
    {
        nDamageBonus += 4;
    }
    else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_I) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_I))
    {
        nDamageBonus += 2;
    }
    return nDamageBonus;
}
// PC 0x005F0CA0
int CSWSCreatureStats::GetRangedAttackBonus(int bOffHand, int bIncludeBase,
                                            int bTouchAttack)
{
    int nBonus = 0;
    CSWSItem* pWeapon = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_RIGHTHAND);

    if (pWeapon != NULL &&
        g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nRangedWeapon <= 0)
    {
        return 0;
    }

    if (bIncludeBase == 1)
    {
        int nCapped = 0;
        nBonus = m_pCreature->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_ATTACK, NULL, nCapped, 0, 0, 0, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, bOffHand);
        nBonus += GetBaseAttackBonus();
    }

    int nTwoWeaponPenalty = 0;
    CSWSItem* pOffHand = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_LEFTHAND);
    if (!bOffHand)
    {
        if (pOffHand != NULL &&
            g_pRules->m_pBaseItems->GetBaseItem(pOffHand->GetBaseItemType())->m_nWeaponType != 0)
        {
            nTwoWeaponPenalty = -6;
            if (m_pCreature->GetRelativeWeaponSize(pWeapon) <= -1 ||
                g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
            {
                nTwoWeaponPenalty = -4;
            }
            if (HasFeat(CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponPenalty += 4;
            }
            else if (HasFeat(CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponPenalty += 2;
            }
            if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_III))
            {
                nTwoWeaponPenalty += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_II))
            {
                nTwoWeaponPenalty += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_I))
            {
                nTwoWeaponPenalty += 1;
            }
        }
    }
    else if (pWeapon != NULL)
    {
        nTwoWeaponPenalty = -10;
        if (HasFeat(CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING))
        {
            nTwoWeaponPenalty = -2;
        }
        else if (HasFeat(CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING))
        {
            nTwoWeaponPenalty = -4;
        }
        else if (HasFeat(CSWFeat::FEAT_AMBIDEXTERITY))
        {
            nTwoWeaponPenalty = -6;
        }
        if (HasFeat(CSWFeat::FEAT_SUPER_WEAPON_FOCUS_2_WEAPON_III))
        {
            nTwoWeaponPenalty += 1;
        }
    }
    nBonus += nTwoWeaponPenalty;
    nBonus += GetDEXMod(0);
    nBonus += GetWeaponFocus(pWeapon);

    int nFeatBonus = 0;
    if (HasFeat(CSWFeat::FEAT_TARGETING_10))
    {
        nFeatBonus = 10;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_9))
    {
        nFeatBonus = 9;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_8))
    {
        nFeatBonus = 8;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_7))
    {
        nFeatBonus = 7;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_6))
    {
        nFeatBonus = 6;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_5))
    {
        nFeatBonus = 5;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_4))
    {
        nFeatBonus = 4;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_3))
    {
        nFeatBonus = 3;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_2))
    {
        nFeatBonus = 2;
    }
    else if (HasFeat(CSWFeat::FEAT_TARGETING_1))
    {
        nFeatBonus = 1;
    }
    nBonus += nFeatBonus;

    int nDuelingBonus = 0;
    CSWSItem* pRightHand = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_RIGHTHAND);
    CSWSItem* pLeftHand = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_LEFTHAND);
    int bDueling = FALSE;
    if (pRightHand != NULL && pLeftHand == NULL)
    {
        CSWBaseItem* pBaseItem = pRightHand->GetBaseItem();
        if (pBaseItem != NULL &&
            (pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 ||
             pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4))
        {
            bDueling = TRUE;
        }
    }
    else if (pRightHand == NULL && pLeftHand == NULL)
    {
        bDueling = TRUE;
    }
    if (bDueling)
    {
        if (HasFeat(CSWFeat::FEAT_MASTER_DUELING))
        {
            nDuelingBonus = 3;
        }
        else if (HasFeat(CSWFeat::FEAT_IMPROVED_DUELING))
        {
            nDuelingBonus = 2;
        }
        else if (HasFeat(CSWFeat::FEAT_DUELING))
        {
            nDuelingBonus = 1;
        }
    }
    return nBonus + nDuelingBonus;
}
// PC 0x005F78E0
CSWSCreatureStats::CSWSCreatureStats(CSWSCreature* pCreature)
{
    m_lstLevelStats.Add(new CSWLevelStats);
    m_nClassCount = 1;
    m_nStartingPackage = 0;
    m_cConversation = "";
    m_bInterruptable = 1;
    m_bIsPC = 1;
    m_bIsAIDisabledPossess = 0;
    m_bIsAIDisabledOverride = 0;
    m_nGender = 0;
    m_nAge = 0;
    m_nRace = CSWRace::CRACE_HUMAN;
    m_sSubrace = "";
    m_nSubraceIndex = 0;
    SetClassLevel(0, 1);
    SetClass(0, 0);
    m_nClassCount = MAX_CLASSES;
    for (int nClass = 1; nClass < MAX_CLASSES; nClass++)
    {
        SetClassLevel(nClass, 0);
        SetClass(nClass, CSWClass::CCLASS_INVALID);
    }
    m_nClassCount = 1;
    m_nSkillPoints = 0;
    m_nArmorCheckPenalty = 0;
    m_nShieldCheckPenalty = 0;
    m_nAttacksPerRound = 0;
    m_nExperience = 0;
    m_nGoodEvilAlignment = 0;
    m_n18C = -1;
    m_pCreature = pCreature;
    m_nDamageRoll = 0;
    m_nCriticalHitRoll = DEFAULT_CRITICAL_HIT_ROLL;
    m_nCriticalHitMultiplier = 0;
    m_c120 = 0xFF;
    m_n124 = 0;
    m_nACArmorBonus = 0;
    m_nACArmorPenalty = 0;
    m_nACShieldBonus = 0;
    m_nACShieldPenalty = 0;
    m_nACDodgeBonus = 0;
    m_nACDodgePenalty = 0;
    m_nACDeflectionBonus = 0;
    m_nACDeflectionPenalty = 0;
    m_nACNaturalBonus = 0;
    m_nACNaturalPenalty = 0;
    m_nACNaturalBase = 0;
    m_nACArmorBase = 0;
    m_nACShieldBase = 0;
    m_nEnemyAttackBonus = 0;
    m_nACEffectIndex = 0;
    m_nAttackEffectIndex = 0;
    m_nDamageReductionEffectIndex = 0;
    m_nDamageResistanceEffectIndex = 0;
    m_nDamageIncreaseEffectIndex = 0;
    m_nSetAIStateEffectIndex = 0;
    m_nEffectIndex12C = 0;
    m_nEffectIconEffectIndex = 0;
    m_nSanctuaryEffectIndex = 0;
    m_nInvisibilityEffectIndex = 0;
    m_nAbilityEffectIndex = 0;
    m_nForceResistanceEffectIndex = 0;
    m_nSeeInvisibleEffectIndex = 0;
    m_nTrueSeeingEffectIndex = 0;
    m_nMissChanceEffectIndex = 0;
    m_nConcealmentEffectIndex = 0;
    m_nBlindnessEffectIndex = 0;
    m_nSpecialWalkEffectIndex = 0;
    m_nSpellLevelAbsorptionEffectIndex = 0;
    m_nSpellImmunityEffectIndex = 0;
    m_nDamageShieldEffectIndex = 0;
    m_nSkillDecreaseEffectIndex = 0;
    m_nSavingThrowEffectIndex = 0;
    m_nHasteEffectIndex = 0;
    m_nImmunityEffectIndex = 0;
    m_nBaseAttackBonus = 0;
    m_pCombatInformation = new CCombatInformation;
    m_nAIState = 0;
    m_nMovementRate = 0;
    m_fWalkRate = 0.0f;
    m_fRunRate = 0.0f;
    m_pSkillRanks = new unsigned char[g_pRules->m_nNumSkills];
    memset(m_pSkillRanks, 0, g_pRules->m_nNumSkills);
    m_nArcaneSpellFailure = 0;
    m_nBaseShieldArcaneSpellFailure = 0;
    m_nBaseArmorArcaneSpellFailure = 0;
    m_nForceResistance = 0;
    m_nForceResistancePenalty = 0;
    m_nStrengthBase = 0;
    m_nStrengthModifier = 0;
    m_nDexterityBase = 0;
    m_nDexterityModifier = 0;
    m_nConstitutionBase = 0;
    m_nConstitutionModifier = 0;
    m_nIntelligenceBase = 0;
    m_nIntelligenceModifier = 0;
    m_nWisdomBase = 0;
    m_nWisdomModifier = 0;
    m_nCharismaBase = 0;
    m_nCharismaModifier = 0;
    m_nFortSaveBonus = 0;
    m_nWillSaveBonus = 0;
    m_nReflexSaveBonus = 0;
    m_cPortrait = "";
    m_nSkinColor = 0;
    m_nHairColor = 0;
    m_nTattoo1Color = 0;
    m_nTattoo2Color = 0;
    m_nAppearance = 0;
    m_nPhenotype = 0;
    m_nAppearanceHead = 0;
    m_bUseBackupHead = 0;
    m_nDuplicatingHead = NO_DUPLICATING_HEAD;
    m_nTail = 0;
    m_nWings = 0;
    m_nFactionId = INVALID_FACTION;
    m_f84 = 120.0f;
    m_fChallengeRating = 0.0f;
    CExoArrayList<CSWSStats_SpellLikeAbility>* pAbilities =
        new CExoArrayList<CSWSStats_SpellLikeAbility>(0);
    m_pSpellLikeAbilities = pAbilities;
    m_nForcePoints = 0;
    m_nCurrentForcePoints = 0;
    m_nTemporaryForcePoints = 0;
    m_fHPAccumulator = 0.0f;
    m_fFPAccumulator = 0.0f;
}
// PC 0x005F7DC0
CSWSCreatureStats::~CSWSCreatureStats()
{
    if (m_pSkillRanks != NULL)
    {
        delete[] m_pSkillRanks;
        m_pSkillRanks = NULL;
    }

    ClearFeats();

    int nLevels = m_lstLevelStats.num;
    for (int i = nLevels - 1; i >= 0; --i)
    {
        CSWLevelStats* pLevelStats = m_lstLevelStats[i];
        m_lstLevelStats.DelIndex(i);
        if (pLevelStats != NULL)
        {
            delete pLevelStats;
        }
    }

    if (m_pSpellLikeAbilities != NULL)
    {
        delete m_pSpellLikeAbilities;
        m_pSpellLikeAbilities = NULL;
    }

    if (m_pCombatInformation != NULL)
    {
        delete m_pCombatInformation;
        m_pCombatInformation = NULL;
    }
}
// PC 0x005F7F60
short CSWSCreatureStats::GetArmorClassVersus(CSWSCreature* pTarget,
                                           int bTouchAttack)
{
    int nAttack = pTarget->GetCombatRound()->m_nCurrentAttack;
    CSWSCombatAttackData* pAttack = pTarget->GetCombatRound()->GetAttack(nAttack);
    int nClassBonus = 0;
    for (int nClass = 0; nClass < GetNumClasses(); ++nClass)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(nClass)];
        nClassBonus += pClass->GetArmorBonus(GetClassLevel(nClass, 0));
    }
    pAttack->m_Flags.m_nClassArmorModifier = nClassBonus;
    int nAC = SW_BASE_ARMOR_CLASS + nClassBonus;
    int nArmorAC;
    int nNaturalAC;
    int nShieldAC;
    nNaturalAC = nArmorAC = nShieldAC = 0;
    if (!bTouchAttack)
    {
        GetACNaturalBase();
        char nNaturalPenalty = m_nACNaturalPenalty;
        unsigned char nNaturalBase = m_nACNaturalBase;
        char nNaturalBonus = m_nACNaturalBonus;
        char nArmorPenalty = m_nACArmorPenalty;
        nNaturalAC = (char)(nNaturalBase +
                           (nNaturalBonus - nNaturalPenalty));
        unsigned char nArmorBase = m_nACArmorBase;
        nArmorAC = (char)(nArmorBase + (m_nACArmorBonus - nArmorPenalty));
        nShieldAC = (char)(m_nACShieldBase +
                          (m_nACShieldBonus - m_nACShieldPenalty));
    }
    nAC += nNaturalAC + nArmorAC + nShieldAC;
    pAttack->m_Flags.m_nArmorModifier = nArmorAC;
    pAttack->m_Flags.m_nNaturalArmorModifier = nNaturalAC;
    if (g_bEnableCombatDebugging)
    {
        CExoString sDebug(pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText);
        CExoString sBonus;
        if (nNaturalAC)
        {
            sBonus.Format(" + %d (Natural AC)", nNaturalAC);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nArmorAC)
        {
            sBonus.Format(" + %d (Armor AC)", nArmorAC);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nShieldAC)
        {
            sBonus.Format(" + %d (Shield AC)", nShieldAC);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
    }
    int nDexMod = 0;
    int nDodgeMod = 0;
    if (!pTarget->GetInvisible(m_pCreature->AsSWSObject()))
    {
        CSWVisibilityNode* pVisibility = m_pCreature->GetVisibleListElement(pTarget->GetId());
        if (pVisibility != NULL &&
            pVisibility->m_bSeen)
        {
            nDodgeMod = (char)min(SW_MAX_AC_DODGE_BONUS, m_nACDodgeBonus - m_nACDodgePenalty);
            nDexMod = GetDEXMod(1);
            if (g_bEnableCombatDebugging)
            {
                CExoString sDebug(pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText);
                CExoString sBonus;
                if (FALSE)
                {
                    pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
                }
            }
        }
    }
    else if (nDexMod == 0 && GetDEXMod(1) < 0)
    {
        nDexMod = GetDEXMod(1);
    }
    int nBaseDexMod = GetDEXMod(0);
    nAC += nDexMod + nDodgeMod;
    pAttack->m_Flags.m_nDodgeDeflectionModifier = nDodgeMod;
    pAttack->m_Flags.m_nDexterityModifier = nDexMod;
    pAttack->m_Flags.m_nBaseDexterityModifier = nBaseDexMod;
    if (g_bEnableCombatDebugging)
    {
        CExoString sDebug(pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText);
        CExoString sBonus;
        if (nDexMod)
        {
            sBonus.Format(" + %d (Dex Mod)", nDexMod);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nDodgeMod)
        {
            sBonus.Format(" + %d (Dodge Mod)", nDodgeMod);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
    }
    int nDodgeBonus = 0;
    int nArmorBonus = m_nACArmorBonus;
    int nShieldBonus = m_nACShieldBonus;
    int nDeflectionBonus = m_nACDeflectionBonus;
    int nNaturalBonus = 0;
    int nDeflectionPenalty = 0;
    int nArmorPenalty;
    int nShieldPenalty;
    nArmorPenalty = nShieldPenalty = 0;
    int nAlignment = pTarget->m_pStats->GetSimpleAlignmentGoodEvil();
    int nRace = pTarget->m_pStats->m_nRace;
    unsigned long nDamageFlags = pTarget->GetDamageFlags();
    CSWSCreature* pCreature = m_pCreature;
    if (pCreature->m_lstEffects.num > 0)
    {
        for (int nEffect = m_nACEffectIndex; nEffect < pCreature->m_lstEffects.num; nEffect++)
        {
            CGameEffect* pEffect = pCreature->m_lstEffects[nEffect];
            if (pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_AC_INCREASE &&
                pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_AC_DECREASE)
                break;
            int nType = pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_TYPE);
            int nBonus = pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS);
            int nEffectRace = pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_RACIAL_GROUP);
            int nEffectAlignment = pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_ALIGNMENT_GROUP2);
            int nEffectDamage = pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_DAMAGE_FLAGS);
            pEffect->GetInteger(AC_EFFECT_OPTION_INDEX);
            if (nEffectRace == g_pRules->GetNumRaces() && nEffectAlignment == 0 &&
                nEffectDamage == CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL)
                continue;
            int bRace = nEffectRace == g_pRules->GetNumRaces() || nEffectRace == nRace;
            int bAlignment = nEffectAlignment == 0 || nEffectAlignment == nAlignment;
            int bDamage = nEffectDamage == CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL ||
                (nEffectDamage == CSWSEffectListHandler::DAMAGE_FLAG_BLUDGEONING &&
                 nDamageFlags == CSWSCombatAttackData::DAMAGE_INDEX_BLUDGEONING) ||
                (nEffectDamage == CSWSEffectListHandler::DAMAGE_FLAG_PIERCING &&
                 nDamageFlags == CSWSCombatAttackData::DAMAGE_INDEX_PIERCING) ||
                (nEffectDamage == CSWSEffectListHandler::DAMAGE_FLAG_SLASHING &&
                 nDamageFlags == CSWSCombatAttackData::DAMAGE_INDEX_SLASHING);
            if (bRace && bAlignment && bDamage)
            {
                if (nType == CSWSEffectListHandler::AC_DODGE)
                {
                    if (pCreature->m_lstEffects[nEffect]->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
                        nDodgeBonus += nBonus;
                    else
                        nDodgeBonus -= nBonus;
                }
                if (nType == CSWSEffectListHandler::AC_NATURAL)
                {
                    if (pCreature->m_lstEffects[nEffect]->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
                        nNaturalBonus += nBonus;
                    else
                        nNaturalBonus -= nBonus;
                }
                if (pCreature->m_lstEffects[nEffect]->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
                {
                    if (!bTouchAttack && nType == CSWSEffectListHandler::AC_ARMOUR_BONUS && nBonus > nArmorBonus)
                        nArmorBonus = nBonus;
                }
                else if (!bTouchAttack && nType == CSWSEffectListHandler::AC_ARMOUR_BONUS && nBonus > nArmorPenalty)
                    nArmorPenalty = nBonus;
                if (pCreature->m_lstEffects[nEffect]->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
                {
                    if (!bTouchAttack && nType == CSWSEffectListHandler::AC_SHIELD_BONUS && nBonus > nShieldBonus)
                        nShieldBonus = nBonus;
                }
                else if (!bTouchAttack && nType == CSWSEffectListHandler::AC_SHIELD_BONUS && nBonus > nShieldPenalty)
                    nShieldPenalty = nBonus;
                if (pCreature->m_lstEffects[nEffect]->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
                {
                    if (nType == CSWSEffectListHandler::AC_DEFLECTION && nBonus > nDeflectionBonus)
                        nDeflectionBonus = nBonus;
                }
                else if (!bTouchAttack && nType == CSWSEffectListHandler::AC_SHIELD_BONUS && nBonus > nDeflectionPenalty)
                    nDeflectionPenalty = nBonus;
            }
        }
    }
    if (g_bEnableCombatDebugging)
    {
        CExoString sDebug(pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText);
        CExoString sBonus;
        if (nDodgeBonus)
        {
            sBonus.Format(" + %d (Effect AC Dodge Bonus)", nDodgeBonus);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nNaturalBonus)
        {
            sBonus.Format(" + %d (Effect AC Natural Bonus)", nNaturalBonus);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nArmorBonus)
        {
            sBonus.Format(" + %d (Effect AC Armor Bonus)", nArmorBonus + nArmorPenalty);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nShieldBonus)
        {
            sBonus.Format(" + %d (Effect AC Shield Bonus)", nShieldBonus + nShieldPenalty);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
        if (nDeflectionBonus)
        {
            sBonus.Format(" + %d (Effect AC Deflection Bonus)", nDeflectionBonus + nDeflectionPenalty);
            pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = pTarget->GetCombatRound()->GetAttack(nAttack)->m_sAttackDebugText;
        }
    }
    nAC += nDodgeBonus + nDeflectionBonus + nDeflectionPenalty;
    pAttack->m_Flags.m_nDodgeDeflectionModifier += nDodgeBonus + nDeflectionBonus + nDeflectionPenalty;
    if (bTouchAttack)
        nAC += nArmorBonus + nShieldBonus + nArmorPenalty + nShieldPenalty;
    int nFeatBonus = GetTotalACFeatMod();
    pAttack->m_Flags.m_nFeatArmorModifier = nFeatBonus;
    nAC += nFeatBonus;
    if (m_pCreature->IsDebilitated())
    {
        nAC -= SW_DEBILITATED_AC_PENALTY;
        pAttack->m_Flags.m_nDebilitatedModifier = -SW_DEBILITATED_AC_PENALTY;
    }
    EXOLOG("CSWSCreatureStats::GetArmorClassVersus: effect natural bonus %p", &nNaturalBonus);
    return nAC;
}
// PC 0x005F8F90
void CSWSCreatureStats::UpdateCombatInformation()
{
    CExoArrayList<CGameEffect*> lstEffects;

    if (m_pCombatInformation != NULL)
    {
        delete m_pCombatInformation;
        m_pCombatInformation = new CCombatInformation;
    }

    lstEffects = GetCreature()->GetFilteredEffectList(CSWSCreature::FILTERED_EFFECT_ATTACK);
    if (lstEffects.num > 0)
    {
        for (int nEffect = 0; nEffect < lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = lstEffects[nEffect];
            if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_ONHAND ||
                pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_OFFHAND ||
                pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_UNARMED ||
                pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_MISC)
            {
                CCombatInformationNode* pNode = new CCombatInformationNode;
                if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE)
                    pNode->m_nModifier = pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS);
                else
                    pNode->m_nModifier = -pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS);

                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_ONHAND)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_ONHAND;
                else if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_OFFHAND)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_OFFHAND;
                else if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_MISC)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_MISC;
                else if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) == CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_UNARMED;

                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP) != g_pRules->GetNumRaces())
                    pNode->m_nVersusRace = pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP);
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2) != 0)
                    pNode->m_nVersusGoodEvil = pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2);
                m_pCombatInformation->m_lstAttacks.Add(pNode);
            }
        }
    }

    lstEffects = GetCreature()->GetFilteredEffectList(CSWSCreature::FILTERED_EFFECT_DAMAGE);
    if (lstEffects.num > 0)
    {
        for (int nEffect = 0; nEffect < lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = lstEffects[nEffect];
            if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_ONHAND ||
                pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND ||
                pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_MISC ||
                pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
            {
                CCombatInformationNode* pNode = new CCombatInformationNode;
                if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
                    pNode->m_nModifier = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                else
                    pNode->m_nModifier = -pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);

                unsigned short nDamageType = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE);
                if (nDamageType <= (CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL & ~CSWSEffectListHandler::DAMAGE_FLAG_BASE_DAMAGE))
                {
                    if (nDamageType == CSWSEffectListHandler::DAMAGE_FLAG_PIERCING)
                        pNode->m_nModifierType = CCombatInformationNode::DAMAGE_PIERCING;
                    else if (nDamageType == CSWSEffectListHandler::DAMAGE_FLAG_SLASHING)
                        pNode->m_nModifierType = CCombatInformationNode::DAMAGE_SLASHING;
                    else if (nDamageType == CSWSEffectListHandler::DAMAGE_FLAG_BLUDGEONING)
                        pNode->m_nModifierType = CCombatInformationNode::DAMAGE_BLUDGEONING;
                    else
                        pNode->m_nModifierType = CCombatInformationNode::DAMAGE_UNKNOWN;
                }
                else
                    pNode->m_nModifierType = Log_Base2(nDamageType);

                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_ONHAND)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_ONHAND;
                else if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_OFFHAND;
                else if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_MISC)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_MISC;
                else if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) == CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
                    pNode->m_nWeaponWield = CCombatInformationNode::WEAPON_UNARMED;

                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) != g_pRules->GetNumRaces())
                    pNode->m_nVersusRace = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP);
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2) != 0)
                    pNode->m_nVersusGoodEvil = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2);
                CCombatInformation& cInfo = *m_pCombatInformation;
                cInfo.m_lstDamage.Add(pNode);
            }
        }
    }

    m_pCombatInformation->SetNumAttacks(1);
    CSWSItem* pOnHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    CSWSItem* pOffHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    if (pOffHand != NULL && g_pRules->m_pBaseItems->GetBaseItem(pOffHand->GetBaseItemType())->m_nWeaponType != 0)
        m_pCombatInformation->SetOffHandWeaponEquipped(1);
    else if (pOnHand != NULL && g_pRules->m_pBaseItems->GetBaseItem(pOnHand->GetBaseItemType())->m_nWeaponWield != CSWBaseItem::WIELD_DOUBLE_WEAPON)
        m_pCombatInformation->SetOffHandWeaponEquipped(0);

    if (pOffHand != NULL)
        m_pCombatInformation->SetLeftEquipment(pOffHand->GetId());
    else
        m_pCombatInformation->SetLeftEquipment(OBJECT_INVALID);

    if (pOnHand != NULL && g_pRules->m_pBaseItems->GetBaseItem(pOnHand->GetBaseItemType())->m_nRangedWeapon > 0)
    {
        m_pCombatInformation->SetOnHandAttackMod(GetRangedAttackBonus(0, 0, 0));
        m_pCombatInformation->SetOnHandDamageMod(GetRangedDamageBonus());
    }
    else
    {
        m_pCombatInformation->SetOnHandAttackMod(GetMeleeAttackBonus(0, 0, 0, 0));
        m_pCombatInformation->SetOnHandDamageMod(GetMeleeDamageBonus(0));
    }
    if (pOnHand != NULL)
        m_pCombatInformation->SetRightEquipment(pOnHand->GetId());
    else
        m_pCombatInformation->SetRightEquipment(OBJECT_INVALID);

    if (m_pCombatInformation->m_nOffHandWeaponEquipped == 1)
    {
        m_pCombatInformation->SetOffHandAttackMod(GetMeleeAttackBonus(1, 0, 0, 0));
        m_pCombatInformation->SetOffHandDamageMod(GetMeleeDamageBonus(1));
    }
    m_pCombatInformation->SetForceResistance(GetForceResistance());
    m_pCombatInformation->SetArmorCheckPenalty(-(m_nArmorCheckPenalty + m_nShieldCheckPenalty));
    m_pCombatInformation->SetUnarmedDamageDice(GetUnarmedDamageDice());
    m_pCombatInformation->SetUnarmedDamageDie(GetUnarmedDamageDie());
    m_pCombatInformation->SetOnHandCriticalMultiplier(GetCriticalHitMultiplier(0));
    m_pCombatInformation->SetOnHandCriticalRange(21 - GetCriticalHitRoll(0));
    m_pCombatInformation->SetOffHandCriticalMultiplier(GetCriticalHitMultiplier(1));
    m_pCombatInformation->SetOffHandCriticalRange(21 - GetCriticalHitRoll(1));
}
// PC 0x005F1040
int CSWSCreatureStats::GetRangedDamageBonus()
{
    int nDamageBonus = 0;
    CSWSItem* pItem = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_RIGHTHAND);

    if (pItem != NULL)
    {
        if (g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nRangedWeapon <= 0)
        {
            return 0;
        }

        int nStrengthBonus = 0;
        if (g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_THROWN)
        {
            nStrengthBonus = m_nStrengthModifier;
        }
        else
        {
            if (m_nStrengthModifier < 0)
            {
                nStrengthBonus = m_nStrengthModifier;
            }
            CSWItemProperty* pProperty;
            if (pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_MIGHTY, 0))
            {
                nStrengthBonus = (unsigned short)min(m_nStrengthModifier, pProperty->m_nCostValue);
            }
        }

        nDamageBonus = nStrengthBonus;
    }

    int nSpecializationBonus = 0;
    if (GetWeaponSpecialization(pItem))
    {
        nSpecializationBonus = 2;
    }
    nDamageBonus += nSpecializationBonus;

    if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_III))
    {
        nDamageBonus += 6;
    }
    else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_II))
    {
        nDamageBonus += 4;
    }
    else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_I))
    {
        nDamageBonus += 2;
    }

    if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_V))
    {
        nDamageBonus += 8;
    }
    else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_IV))
    {
        nDamageBonus += 6;
    }
    else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_III))
    {
        nDamageBonus += 4;
    }
    else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_II))
    {
        nDamageBonus += 2;
    }
    else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_I))
    {
        nDamageBonus += 1;
    }
    return nDamageBonus;
}
// PC 0x005F11E0
int CSWSCreatureStats::GetAttackModifierVersus(CSWSCreature* pTarget)
{
    int nCurrentAttack = GetCreature()->GetCombatRound()->m_nCurrentAttack;
    CSWSCombatRound* pCombatRound = GetCreature()->GetCombatRound();
    CSWSCombatAttackData* pAttack = pCombatRound->GetAttack(nCurrentAttack);
    int bRangedAttack = pAttack->m_bRangedAttack;
    int nWeaponAttackType = pAttack->m_nWeaponAttackType;
    int nBaseAttackBonus = 0;
    pCombatRound = GetCreature()->GetCombatRound();
    CSWSItem* pWeapon = pCombatRound->GetCurrentAttackWeapon(nWeaponAttackType);
    if (pWeapon != NULL)
    {
        CSWBaseItem* pBaseItem =
            g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType());
    }

    if (nWeaponAttackType != CSWSCombatRound::WEAPON_ATTACK_LEFTHAND &&
        nWeaponAttackType != CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND &&
        nWeaponAttackType != CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
    {
        nBaseAttackBonus = GetBaseAttackBonus();
    }
    else if (nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND)
    {
        nBaseAttackBonus = GetBaseAttackBonus();
        GetCreature()->GetCombatRound()->m_nOffHandTaken =
            GetCreature()->GetCombatRound()->GetOffHandTaken() + 1;
    }
    else if (nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND ||
             nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
    {
        nBaseAttackBonus = GetBaseAttackBonus();
    }
    pAttack->m_Extra1.m_nBaseAttackBonus = nBaseAttackBonus;
    if (g_bEnableCombatDebugging)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Base Attack Bonus)", nBaseAttackBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nTwoWeaponPenalty = 0;
    int nWeaponSizeBonus = 0;
    int nBaseTwoWeaponPenalty = 0;
    int nTwoWeaponFeatBonus = 0;
    int nTwoWeaponFeat = 0;
    CSWSItem* pOffHand =
        GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    if (pOffHand == NULL)
    {
        pOffHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pOffHand != NULL)
        {
            CSWBaseItem* pBaseItem =
                g_pRules->m_pBaseItems->GetBaseItem(pOffHand->GetBaseItemType());
            if (pBaseItem != NULL && pBaseItem->m_nWeaponWield != CSWBaseItem::WIELD_DOUBLE_WEAPON)
            {
                pOffHand = NULL;
            }
        }
    }
    if (nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_RIGHTHAND)
    {
        pAttack->m_Extra1.m_nOffHand = 0;
        if (pOffHand != NULL &&
            g_pRules->m_pBaseItems->GetBaseItem(pOffHand->GetBaseItemType())->m_nWeaponType != 0)
        {
            nBaseTwoWeaponPenalty = -6;
            nTwoWeaponPenalty = -6;
            if (GetCreature()->GetRelativeWeaponSize(pOffHand) == -1 ||
                g_pRules->m_pBaseItems->GetBaseItem(pOffHand->GetBaseItemType())->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
            {
                nWeaponSizeBonus = 2;
                nTwoWeaponPenalty = -4;
            }
            if (HasFeat(CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponFeatBonus = 4;
                nTwoWeaponFeat = CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING;
            }
            else if (HasFeat(CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponFeatBonus = 2;
                nTwoWeaponFeat = CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING;
            }
            nTwoWeaponPenalty += nTwoWeaponFeatBonus;
        }
    }
    else if (nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND)
    {
        pAttack->m_Extra1.m_nOffHand = 1;
        if (pOffHand != NULL)
        {
            nBaseTwoWeaponPenalty = -10;
            if (HasFeat(CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponFeatBonus = 8;
                nTwoWeaponFeat = CSWFeat::FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING;
            }
            else if (HasFeat(CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING))
            {
                nTwoWeaponFeatBonus = 6;
                nTwoWeaponFeat = CSWFeat::FEAT_DOUBLE_WEAPON_FIGHTING;
            }
            else if (HasFeat(CSWFeat::FEAT_AMBIDEXTERITY))
            {
                nTwoWeaponFeatBonus = 4;
                nTwoWeaponFeat = CSWFeat::FEAT_AMBIDEXTERITY;
            }
            nTwoWeaponPenalty = -10 + nTwoWeaponFeatBonus;
        }
    }
    int nAttackBonus = nBaseAttackBonus + nTwoWeaponPenalty;
    pAttack->m_Extra1.m_nTwoWeaponPenalty = nBaseTwoWeaponPenalty;
    pAttack->m_Extra1.m_nWeaponSizeBonus = nWeaponSizeBonus;
    pAttack->m_Extra1.m_nTwoWeaponFeatBonus = nTwoWeaponFeatBonus;
    pAttack->m_Extra1.m_nTwoWeaponFeat = nTwoWeaponFeat;
    if (g_bEnableCombatDebugging && nTwoWeaponPenalty != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Offhand Attack Penalty)", nTwoWeaponPenalty);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nSpecialAttackBonus = ResolveSpecialAttackAttackBonus(pTarget);
    nAttackBonus += nSpecialAttackBonus;
    pAttack->m_Extra1.m_nSpecialAttackBonus = nSpecialAttackBonus;
    if (g_bEnableCombatDebugging && nSpecialAttackBonus != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Special Attack To Hit Modifier)", nSpecialAttackBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nDuelingBonus = 0;
    int nDuelingFeat = 0;
    CSWSItem* pRightHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    CSWSItem* pLeftHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    if (pRightHand != NULL && pLeftHand == NULL)
    {
        CSWBaseItem* pBaseItem = pRightHand->GetBaseItem();
        if (pBaseItem != NULL &&
            (pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 ||
             pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4))
        {
            if (HasFeat(CSWFeat::FEAT_MASTER_DUELING))
            {
                nDuelingBonus = 3;
                nDuelingFeat = CSWFeat::FEAT_MASTER_DUELING;
            }
            else if (HasFeat(CSWFeat::FEAT_IMPROVED_DUELING))
            {
                nDuelingBonus = 2;
                nDuelingFeat = CSWFeat::FEAT_IMPROVED_DUELING;
            }
            else if (HasFeat(CSWFeat::FEAT_DUELING))
            {
                nDuelingBonus = 1;
                nDuelingFeat = CSWFeat::FEAT_DUELING;
            }
        }
    }
    pAttack->m_Extra1.m_nDuelingBonus = nDuelingBonus;
    pAttack->m_Extra1.m_nDuelingFeat = nDuelingFeat;
    nAttackBonus += nDuelingBonus;
    if (g_bEnableCombatDebugging && nSpecialAttackBonus != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Dueling To Hit Modifier)", nDuelingBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nGoodAimBonus = 0;
    if (bRangedAttack && pTarget != NULL)
    {
        Vector vDelta = GetCreature()->m_vPosition - pTarget->m_vPosition;
        float fDistanceSquared = vDelta * vDelta;
        if (fDistanceSquared <= (float)GOOD_AIM_RANGE_SQUARED)
        {
            nGoodAimBonus = GOOD_AIM_ATTACK_BONUS;
        }
    }
    pAttack->m_Extra1.m_nGoodAimBonus = nGoodAimBonus;
    nAttackBonus += nGoodAimBonus;
    if (g_bEnableCombatDebugging && nGoodAimBonus != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Good Aim)", nGoodAimBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nOffensiveTrainingBonus = 0;
    if (!bRangedAttack && pTarget != NULL)
    {
        CSWSItem* pTargetWeapon = pTarget->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pTargetWeapon != NULL && pTargetWeapon->GetBaseItem()->m_nRangedWeapon > 0)
        {
            nOffensiveTrainingBonus = MELEE_VERSUS_RANGED_ATTACK_BONUS;
        }
    }
    pAttack->m_Extra1.m_nOffensiveTrainingBonus = nOffensiveTrainingBonus;
    nAttackBonus += nOffensiveTrainingBonus;
    if (g_bEnableCombatDebugging && nOffensiveTrainingBonus != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Offensive Training Bonus (vs Race))", nOffensiveTrainingBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nDexterityBonus = 0;
    int nStrengthBonus = 0;
    if (bRangedAttack)
    {
        nDexterityBonus = GetDEXMod(0);
    }
    else if (GetWeaponFinesse(pWeapon) == 1)
    {
        nDexterityBonus = GetDEXMod(0);
    }
    else
    {
        nStrengthBonus = GetSTRMod();
    }
    CSWSCreature* pCreature = GetCreature();
    int nAbilityBonus = nDexterityBonus + nStrengthBonus;
    nAttackBonus += nAbilityBonus;
    pCombatRound = pCreature->GetCombatRound();
    pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack)->m_Extra1.m_nStrengthModifier = nStrengthBonus;
    pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack)->m_Extra1.m_nDexterityModifier = nDexterityBonus;
    if (g_bEnableCombatDebugging)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        if (nDexterityBonus != 0)
        {
            sBonus.Format(" + %d (Dex Modifier)", nDexterityBonus);
            GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText;
        }
        if (nStrengthBonus != 0)
        {
            sBonus.Format(" + %d (Str Modifier)", nStrengthBonus);
            GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
            sDebug = GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText;
        }
    }

    int nWeaponFocusBonus = GetWeaponFocus(pWeapon);
    pAttack->m_Extra1.m_nWeaponFocusBonus = nWeaponFocusBonus;
    nAttackBonus += nWeaponFocusBonus;
    if (g_bEnableCombatDebugging && nWeaponFocusBonus != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Weapon Focus Bonus)", nWeaponFocusBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }

    int nCapped = 0;
    int nEffectAttackBonus;
    if (pTarget != NULL)
    {
        nEffectAttackBonus = GetCreature()->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_ATTACK, pTarget->AsSWSObject(), nCapped, 0, 0, 0, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0);
    }
    else
    {
        nEffectAttackBonus = GetCreature()->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_ATTACK, NULL, nCapped, 0, 0, 0, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0);
    }
    nAttackBonus += nEffectAttackBonus;
    pAttack->m_Extra1.m_nEffectAttackBonus = nEffectAttackBonus;
    if (g_bEnableCombatDebugging && nEffectAttackBonus != 0)
    {
        CExoString sDebug(GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText);
        CExoString sBonus;
        sBonus.Format(" + %d (Effect Attack Bonus)", nEffectAttackBonus);
        GetCreature()->GetCombatRound()->GetAttack(nCurrentAttack)->m_sAttackDebugText = sDebug + sBonus;
    }
    return nAttackBonus;
}
// PC 0x005F2B90
char CSWSCreatureStats::GetDamageBonus(CSWSCreature* pTarget, int bOffHand, int bRecordDamage)
{
    int nAttack = GetCreature()->GetCombatRound()->m_nCurrentAttack;
    CSWSCombatAttackData* pAttack = GetCreature()->GetCombatRound()->GetAttack(nAttack);
    int bRanged = pAttack->m_bRangedAttack;
    int bCritical = pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT;
    int nWeaponAttackType = pAttack->m_nWeaponAttackType;
    CSWSItem* pWeapon = GetCreature()->GetCombatRound()->GetCurrentAttackWeapon(nWeaponAttackType);
    int nBonus = 0;

    if (bOffHand)
    {
        int nStrength;
        if (m_nStrengthModifier > 0)
            nStrength = (int)(m_nStrengthModifier * 0.5f);
        else
            nStrength = m_nStrengthModifier;
        pAttack->m_DamageBreakdown.m_nStrengthDamage = nStrength;
        if (g_bEnableCombatDebugging)
        {
            CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
            CExoString sBonus;
            if (bCritical)
                sBonus.Format(" + %d (Offhand Strength Bonus) (Critical x%d)",
                    nStrength * GetCriticalHitMultiplier(bOffHand), GetCriticalHitMultiplier(bOffHand));
            else
                sBonus.Format(" + %d (Offhand Strength Bonus)", nStrength);
            GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sBonus;
        }
    }
    else
    {
        CSWBaseItem* pBaseItem = NULL;
        CSWSItem* pMainHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pMainHand != NULL)
            pBaseItem = pMainHand->GetBaseItem();
        CSWSItem* pOffHand = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        int nStrength;
        if (bRanged)
        {
            nStrength = 0;
            pAttack->m_DamageBreakdown.m_nStrengthDamage = nStrength;
        }
        else if (pMainHand != NULL && pOffHand == NULL &&
                 (pBaseItem->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON ||
                  pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2))
        {
            if (m_nStrengthModifier > 0)
                nStrength = (int)(m_nStrengthModifier * 1.5f);
            else
                nStrength = m_nStrengthModifier;
            pAttack->m_DamageBreakdown.m_nStrengthDamage = nStrength;
        }
        else
        {
            nStrength = m_nStrengthModifier;
            pAttack->m_DamageBreakdown.m_nStrengthDamage = nStrength;
        }
        if (g_bEnableCombatDebugging && nBonus)
        {
            CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
            CExoString sBonus;
            if (bCritical)
                sBonus.Format(" + %d (Strength Bonus) (Critical x%d)",
                    nBonus * GetCriticalHitMultiplier(bOffHand), GetCriticalHitMultiplier(bOffHand));
            else
                sBonus.Format(" + %d (Strength Bonus)", nBonus);
            GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sBonus;
        }
    }
    if (pTarget != NULL)
    {

        for (int i = 0; i < GetNumClasses(); ++i)
        {
        }
    }
    int nPowerBonus = 0;
    if (GetCreature()->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK)
        nPowerBonus = 3;
    unsigned char nCombatMode = GetCreature()->m_nActiveCombatMode;
    if (nCombatMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK)
        nPowerBonus = 7;
    if (g_bEnableCombatDebugging && nPowerBonus)
    {
        CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
        CExoString sBonus;
        if (bCritical)
            sBonus.Format(" + %d (Power/Improved Power Attack Damage Bonus) (Critical x%d)",
                nPowerBonus * GetCriticalHitMultiplier(bOffHand), GetCriticalHitMultiplier(bOffHand));
        else
            sBonus.Format(" + %d (Power/Improved Power Attack Damage Bonus)", nPowerBonus);
        GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sBonus;
    }
    int nSpecialization = 0;
    if (GetWeaponSpecialization(pWeapon))
        nSpecialization = WEAPON_SPECIALIZATION_DAMAGE_BONUS;
    pAttack->m_DamageBreakdown.m_nWeaponSpecializationDamage = nSpecialization;
    if (g_bEnableCombatDebugging && nSpecialization)
    {
        CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
        CExoString sBonus;
        if (bCritical)
            sBonus.Format(" + %d (Weapon Specialization Bonus) (Critical x%d)",
                nSpecialization * GetCriticalHitMultiplier(bOffHand), GetCriticalHitMultiplier(bOffHand));
        else
            sBonus.Format(" + %d (Weapon Specialization Bonus)", nSpecialization);
        GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sBonus;
    }
    int nSpecialBonus = ResolveSpecialAttackDamageBonus(pTarget);
    nBonus = nSpecialBonus + nPowerBonus;
    if (g_bEnableCombatDebugging && nSpecialBonus)
    {
        CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
        CExoString sBonus;
        if (bCritical)
            sBonus.Format(" + %d (Special Attack Damage Bonus) (Critical x%d)",
                nSpecialBonus * GetCriticalHitMultiplier(bOffHand), GetCriticalHitMultiplier(bOffHand));
        else
            sBonus.Format(" + %d (Special Attack Damage Bonus)", nSpecialBonus);
        GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sBonus;
    }
    if (pAttack->m_bRangedAttack)
    {
        if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_III))
        {
            nBonus += 6;
        }
        else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_II))
        {
            nBonus += 4;
        }
        else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_I))
        {
            nBonus += 2;
        }
        if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_V))
        {
            nBonus += 8;
        }
        else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_IV))
        {
            nBonus += 6;
        }
        else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_III))
        {
            nBonus += 4;
        }
        else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_II))
        {
            nBonus += 2;
        }
        else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_I))
        {
            nBonus += 1;
        }
    }
    else
    {
        if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_III) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_III))
        {
            nBonus += 6;
        }
        else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_II) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_II))
        {
            nBonus += 4;
        }
        else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_I) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_I))
        {
            nBonus += 2;
        }
    }
    if (m_c120 != -1)
    {
        nBonus += m_c120;
    }
    int nFormBonus = 0;
    if (GetCreature()->IsWieldingLightsaber() && GetCreature()->m_n1110 == CSWSpell::FORM_SABER_II_MAKASHI)
    {
        nFormBonus = 3;
    }
    nBonus += nFormBonus;
    int bUnarmed = nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
                   nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED;
    if (bUnarmed)
    {
        int nDice = 0;
        if (HasFeat(CSWFeat::FEAT_ECHANI_STRIKE_III))
        {
            nDice = 3;
        }
        else if (HasFeat(CSWFeat::FEAT_ECHANI_STRIKE_II))
        {
            nDice = 2;
        }
        else if (HasFeat(CSWFeat::FEAT_ECHANI_STRIKE_I))
        {
            nDice = 1;
        }
        if (nDice > 0)
        {
            nBonus += g_pRules->RollDice(nDice, 4);
        }
        nDice = 0;
        if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_VIII))
        {
            nDice = 8;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_VII))
        {
            nDice = 7;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_VI))
        {
            nDice = 6;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_V))
        {
            nDice = 5;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_IV))
        {
            nDice = 4;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_III))
        {
            nDice = 3;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_II))
        {
            nDice = 2;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_I))
        {
            nDice = 1;
        }
        if (nDice > 0)
        {
            nBonus += g_pRules->RollDice(nDice, 4);
        }
    }
    return nBonus;
}
// PC 0x005F3430
int CSWSCreatureStats::GetDamageRoll(CSWSObject* pTarget, int bOffHand,
    int bCritical, int bSneakAttack, int bMaxDamage, int bRecordDamage)
{
    int nNumDice, nDie;
    nNumDice = nDie = 0;
    int nAttack = GetCreature()->GetCombatRound()->GetCurrentAttack();
    short nDamage = 0;
    int nDifficulty = 100;
    CSWSCombatAttackData* pAttack = GetCreature()->GetCombatRound()->GetAttack(nAttack);
    CSWItemProperty* pProperty;
    CSWSItem* pWeapon = NULL;
    if (bOffHand)
    {
        pWeapon = GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        if (pWeapon == NULL)
            pWeapon = GetCreature()->GetCombatRound()->GetCurrentAttackWeapon(0);
    }
    else
        pWeapon = GetCreature()->GetCombatRound()->GetCurrentAttackWeapon(0);

    int bPCToNPC = 0;
    int bNPCToPC = 0;
    if (pTarget != NULL && pTarget->AsSWSCreature() != NULL)
    {
        if (GetIsPC() == 1 && pTarget->AsSWSCreature()->GetIsPC() == 0)
        {
            bPCToNPC = 1;
            nDifficulty = g_pAppManager->GetServerExoApp()->GetDifficultyOption(CSWRules::DIFFICULTY_PC_DAMAGE);
        }
        else if (GetIsPC() == 0 && pTarget->AsSWSCreature()->GetIsPC() == 1)
        {
            bNPCToPC = 1;
            nDifficulty = g_pAppManager->GetServerExoApp()->GetDifficultyOption(CSWRules::DIFFICULTY_NPC_DAMAGE);
        }
    }

    if (pWeapon != NULL && pWeapon->GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_UNARMED_DAMAGE_19 && pWeapon->GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_20)
    {
        if (!bMaxDamage)
        {
            if (GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_CREATURE1 || GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_CREATURE2 || GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_CREATURE3)
            {
                if (pWeapon->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_MONSTER_DAMAGE, 0))
                {
                    CSWItemProperty* pProperty;
                    pWeapon->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_MONSTER_DAMAGE, 0);
                    if (pProperty != NULL && pProperty->m_nCostValue != 0)
                    {
                        int nRow = pProperty->m_nCostValue;
#ifdef _XBOX
                        { CExoString sColumn("NumDice"); g_pRules->Get2DArrays()->GetIPRPMonsterDamage()->GetINTEntry(nRow, sColumn, &nNumDice); }
                        { CExoString sColumn("Die"); g_pRules->Get2DArrays()->GetIPRPMonsterDamage()->GetINTEntry(nRow, sColumn, &nDie); }
#else
                        { CExoString sColumn("NumDice");
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPMonsterDamage;
                            pTable->GetINTEntry(nRow, sColumn, &nNumDice);
                        }
                        { CExoString sColumn("Die");
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPMonsterDamage;
                            pTable->GetINTEntry(nRow, sColumn, &nDie);
                        }
#endif
                        short nMaximum = nNumDice * nDie;
                        C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
                        float fDamageMult = 0.0f;
                        int nAutoBalance = m_pCreature->m_c250;
                        if (nAutoBalance != 0)
                        {
                            pAutoBalance->GetFLOATEntry(nAutoBalance, "DamageMult", &fDamageMult);
                        }
                        if (m_pCreature->m_bInParty == 0 && nAutoBalance != 0)
                        {
                            char nLevel = m_pCreature->m_c251;
                            if ((unsigned char)nLevel < 1)
                            {
                                nLevel = 1;
                            }
                            nMaximum *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
                        }
                        if (bCritical)
                        {
                            for (int i = 0; i < GetCriticalHitMultiplier(bOffHand); ++i)
                            {
                                short nRoll = g_pRules->RollDice(nNumDice, nDie);
                                if (m_pCreature->m_bInParty == 0 && nAutoBalance != 0)
                                {
                                    char nLevel = m_pCreature->m_c251;
                                    if ((unsigned char)nLevel < 1)
                                    {
                                        nLevel = 1;
                                    }
                                    nRoll *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
                                }
                                nDamage += g_pRules->GetWeightedDamageAmount(nRoll, nMaximum, bPCToNPC, bNPCToPC, nDifficulty);
                            }
                        }
                        else
                        {
                            short nRoll = g_pRules->RollDice(nNumDice, nDie);
                            if (m_pCreature->m_bInParty == 0 && nAutoBalance != 0)
                            {
                                char nLevel = m_pCreature->m_c251;
                                if ((unsigned char)nLevel < 1)
                                {
                                    nLevel = 1;
                                }
                                nRoll *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
                            }
                            nDamage = g_pRules->GetWeightedDamageAmount(nRoll, nMaximum, bPCToNPC, bNPCToPC, nDifficulty);
                        }
                    }
                }
            }
            else
            {
                nNumDice = g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->GetNumDice();
                nDie = g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->GetDieToRoll();
                short nMaximum = nNumDice * nDie;
                C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
                float fDamageMult;
                int nAutoBalance = m_pCreature->m_c250;
                if (nAutoBalance != 0)
                {
                    pAutoBalance->GetFLOATEntry(nAutoBalance, "DamageMult", &fDamageMult);
                }
                if (m_pCreature->m_bInParty == 0 && nAutoBalance != 0)
                {
                    char nLevel = m_pCreature->m_c251;
                    if ((unsigned char)nLevel < 1)
                    {
                        nLevel = 1;
                    }
                    nMaximum *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
                }
                if (bCritical)
                {
                    for (int i = 0; i < GetCriticalHitMultiplier(bOffHand); ++i)
                    {
                        short nRoll = g_pRules->RollDice(nNumDice, nDie);
                        if (m_pCreature->m_bInParty == 0 && nAutoBalance != 0)
                        {
                            char nLevel = m_pCreature->m_c251;
                            if ((unsigned char)nLevel < 1)
                            {
                                nLevel = 1;
                            }
                            nRoll *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
                        }
                        nDamage += g_pRules->GetWeightedDamageAmount(nRoll, nMaximum, bPCToNPC, bNPCToPC, nDifficulty);
                    }
                }
                else
                {
                    short nRoll = g_pRules->RollDice(nNumDice, nDie);
                    if (m_pCreature->m_bInParty == 0 && nAutoBalance != 0)
                    {
                        char nLevel = m_pCreature->m_c251;
                        if ((unsigned char)nLevel < 1)
                        {
                            nLevel = 1;
                        }
                        nRoll *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
                    }
                    nDamage = g_pRules->GetWeightedDamageAmount(nRoll, nMaximum, bPCToNPC, bNPCToPC, nDifficulty);
                }
            }
        }
        else
        {
            if (GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_CREATURE1 ||
                GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_CREATURE2 ||
                GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_CREATURE3)
            {
                if (pWeapon->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_MONSTER_DAMAGE, 0))
                {
                    pWeapon->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_MONSTER_DAMAGE, 0);
                    if (pProperty != NULL && pProperty->m_nCostValue != 0)
                    {
                        int nRow = pProperty->m_nCostValue;
#ifdef _XBOX
                        { CExoString sColumn("NumDice"); g_pRules->Get2DArrays()->GetIPRPMonsterDamage()->GetINTEntry(nRow, sColumn, &nNumDice); }
                        { CExoString sColumn("Die"); g_pRules->Get2DArrays()->GetIPRPMonsterDamage()->GetINTEntry(nRow, sColumn, &nDie); }
#else
                        { CExoString sColumn("NumDice");
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPMonsterDamage;
                            pTable->GetINTEntry(nRow, sColumn, &nNumDice);
                        }
                        { CExoString sColumn("Die");
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPMonsterDamage;
                            pTable->GetINTEntry(nRow, sColumn, &nDie);
                        }
#endif
                    }
                }
            }
            else if (GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
                GetCreature()->m_pcCombatRound->GetWeaponAttackType() == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
            {
                nNumDice = GetUnarmedDamageDice();
                nDie = GetUnarmedDamageDie();
            }
            else
            {
                nNumDice = g_pRules->GetBaseItemArray()->GetBaseItem(pWeapon->GetBaseItemType())->GetNumDice();
                nDie = g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->GetDieToRoll();
            }
            nDamage = nNumDice * nDie;
        }
        if (pWeapon->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_NO_DAMAGE, 0))
            nDamage = 0;
    }
    else
    {
        if (bCritical)
        {
            for (int i = 0; i < GetCriticalHitMultiplier(bOffHand); ++i)
                nDamage += GetUnarmedDamageRoll(pTarget);
        }
        else
            nDamage = GetUnarmedDamageRoll(pTarget);
    }

    if (bRecordDamage && nDamage > 0)
    {
        pAttack->m_DamageBreakdown.m_nBaseDamage = nDamage;
        pAttack->AddDamage(GetCreature()->GetDamageFlags(), nDamage, 0);
    }
    if (g_bEnableCombatDebugging)
    {
        CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
        CExoString sDamage;
        if (bCritical)
            sDamage.Format("Damage Roll: %d (Critical x%d)", nDamage, GetCriticalHitMultiplier(bOffHand));
        else
            sDamage.Format("Damage Roll: %d", nDamage);
        GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sDamage;
    }
    if (bSneakAttack)
    {
        int nSneakDice = 0;
        if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_10D6)) nSneakDice = 10;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_9D6)) nSneakDice = 9;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_8D6)) nSneakDice = 8;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_7D6)) nSneakDice = 7;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_6D6)) nSneakDice = 6;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_5D6)) nSneakDice = 5;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_4D6)) nSneakDice = 4;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_3D6)) nSneakDice = 3;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_2D6)) nSneakDice = 2;
        else if (HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_1D6)) nSneakDice = 1;
        int nSneakDamage = g_pRules->RollDice(nSneakDice, 6);
        if (g_bEnableCombatDebugging)
        {
            CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
            CExoString sDamage;
            sDamage.Format(" + %d (Sneak Attack Damage)", nSneakDamage);
            GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sDamage;
        }
        nDamage += nSneakDamage;
        pAttack->m_DamageBreakdown.m_nSneakAttackDamage = nSneakDamage;
    }
    int nBonus;
    if (pTarget != NULL && pTarget->AsSWSCreature() != NULL)
        nBonus = GetDamageBonus(pTarget->AsSWSCreature(), bOffHand, bRecordDamage);
    else
        nBonus = GetDamageBonus(NULL, bOffHand, bRecordDamage);
    pAttack->m_DamageBreakdown.m_nDamageBonus = nBonus;
    pAttack->m_DamageBreakdown.m_nUnusedDamageFeedback = 0;
    if (bCritical)
    {
        int nStrength = 0;
        int nSpecialization = 0;
        int nOtherBonus = 0;
        for (int i = 0; i < GetCriticalHitMultiplier(bOffHand); ++i)
        {
            nDamage += nBonus;
            nStrength += pAttack->m_DamageBreakdown.m_nStrengthDamage;
            nSpecialization += pAttack->m_DamageBreakdown.m_nWeaponSpecializationDamage;
            nOtherBonus += pAttack->m_DamageBreakdown.m_nDamageBonus;
        }
        pAttack->m_DamageBreakdown.m_nStrengthDamage = nStrength;
        pAttack->m_DamageBreakdown.m_nWeaponSpecializationDamage = nSpecialization;
        pAttack->m_DamageBreakdown.m_nDamageBonus = nOtherBonus;
        nDamage += pAttack->m_DamageBreakdown.m_nStrengthDamage + pAttack->m_DamageBreakdown.m_nWeaponSpecializationDamage;
        if (pWeapon != NULL && pWeapon->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_MASSIVE_CRITICALS, 0))
        {
            for (int i = 0; i < pWeapon->m_nPassiveProperties; ++i)
            {
                CSWItemProperty* pProperty = pWeapon->GetPassiveProperty(i);
                if (pProperty != NULL && pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_MASSIVE_CRITICALS)
                {
                    int nCriticalDice;
                    int nCriticalDie;
                    int nCriticalDamage;
                    int nRow = pProperty->m_nCostValue;
                    {
                        CExoString sColumn("NumDice");
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPDamage;
                        pTable->GetINTEntry(nRow, sColumn, &nCriticalDice);
                    }
                    if (nCriticalDice == 0)
                        nCriticalDamage = nRow;
                    else
                    {
                        {
                            CExoString sColumn("Die");
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPDamage;
                            pTable->GetINTEntry(nRow, sColumn, &nCriticalDie);
                        }
                        nCriticalDamage = g_pRules->RollDice(nCriticalDice, nCriticalDie);
                    }
                    nDamage += nCriticalDamage;
                    pAttack->m_DamageBreakdown.m_nDamageBonus += nCriticalDamage;
                }
            }
        }
    }
    else
        nDamage += nBonus + pAttack->m_DamageBreakdown.m_nStrengthDamage + pAttack->m_DamageBreakdown.m_nWeaponSpecializationDamage;

    int nCapped = 0;
    int nEffectDamage = 0;
    int nEffectBonus = GetCreature()->GetTotalEffectBonus(CSWSCreature::EFFECT_BONUS_DAMAGE,
        pTarget, nCapped, 0, bMaxDamage, 0, 0,
        CSWSCreature::EFFECT_BONUS_FILTER_ALL, CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0, &nEffectDamage);
    pAttack->m_DamageBreakdown.m_nEffectDamageBonus = nEffectDamage;
    nDamage += nEffectDamage;
    if (g_bEnableCombatDebugging && nEffectBonus)
    {
        CExoString sDebug = GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText;
        CExoString sDamage;
        if (bCritical == 1)
            sDamage.Format(" + %d (Effect Damage Bonus) (Critical x%d)", nEffectBonus, GetCriticalHitMultiplier(bOffHand));
        else
            sDamage.Format(" + %d (Effect Damage Bonus)", nEffectBonus);
        GetCreature()->GetCombatRound()->GetAttack(nAttack)->m_sDamageDebugText = sDebug + sDamage;
    }

    if (nDamage < 1)
    {
        nDamage = 1;
        pAttack->m_DamageBreakdown.m_nBaseDamage = 1;
        pAttack->AddDamage(m_pCreature->GetDamageFlags(), 1, 0);
    }
    unsigned long nDamageFlags = m_pCreature->GetDamageFlags();
    if (pTarget != NULL)
    {
        unsigned char nDamagePower = m_pCreature->CalculateDamagePower(pTarget);
        nDamage = pTarget->DoDamageImmunity(GetCreature(), nDamage, nDamageFlags, bMaxDamage, 1);
        nDamage = pTarget->DoDamageResistance(m_pCreature, nDamage, nDamageFlags, bMaxDamage, 1, 1);
        nDamage = pTarget->DoDamageReduction(GetCreature(), nDamage, nDamagePower, bMaxDamage, 1);
        if ((pTarget->AsSWSDoor() != NULL || pTarget->AsSWSPlaceable() != NULL) &&
            pWeapon != NULL && pWeapon->m_n2DC)
        {
            nDamage = pTarget->GetMaxHitPoints(1);
        }
        if (nDamage < 0)
            nDamage = 0;
    }
    return nDamage;
}
// PC 0x005F44E0
void CSWSCreatureStats::GetTotalDamageRange(CSWSCreature* pCreature,
                                          CSWSItem* pItem,
                                          int& nMinimum, int& nMaximum,
                                          int bOffHand, int bIncludeEffects)
{
    int nMinDamage;
    int nMaxDamage;

    if (pCreature != NULL && pItem != NULL)
    {
        CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
        nMinDamage = pBaseItem->m_nNumDice;
        pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
        nMaxDamage = nMinDamage * pBaseItem->m_nDieToRoll;

        if (pItem->GetBaseItem()->m_nRangedWeapon <= 0)
        {
            int nStrengthBonus;
            if (bOffHand)
            {
                nStrengthBonus = (int)(m_nStrengthModifier * 0.5f);
            }
            else if (pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND) != NULL)
            {
                nStrengthBonus = m_nStrengthModifier;
            }
            else if (pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON ||
                     pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2)
            {
                nStrengthBonus = m_nStrengthModifier > 0 ? (int)(m_nStrengthModifier * 1.5f) : m_nStrengthModifier;
            }
            else
            {
                nStrengthBonus = m_nStrengthModifier;
            }
            nMinDamage += nStrengthBonus;
            nMaxDamage += nStrengthBonus;

            if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_III) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_III))
            {
                nMinDamage += 6;
                nMaxDamage += 6;
            }
            else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_II) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_II))
            {
                nMinDamage += 4;
                nMaxDamage += 4;
            }
            else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_I) || HasFeat(CSWFeat::FEAT_INCREASE_MELEE_DAMAGE_I))
            {
                nMinDamage += 2;
                nMaxDamage += 2;
            }
        }
        else
        {
            if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_III))
            {
                nMinDamage += 6;
                nMaxDamage += 6;
            }
            else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_II))
            {
                nMinDamage += 4;
                nMaxDamage += 4;
            }
            else if (HasFeat(CSWFeat::FEAT_INCREASE_COMBAT_DAMAGE_I))
            {
                nMinDamage += 2;
                nMaxDamage += 2;
            }

            if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_V))
            {
                nMinDamage += 8;
                nMaxDamage += 8;
            }
            else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_IV))
            {
                nMinDamage += 6;
                nMaxDamage += 6;
            }
            else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_III))
            {
                nMinDamage += 4;
                nMaxDamage += 4;
            }
            else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_II))
            {
                nMinDamage += 2;
                nMaxDamage += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_PRECISE_SHOT_I))
            {
                nMinDamage += 1;
                nMaxDamage += 1;
            }
        }

        if (GetWeaponSpecialization(pItem))
        {
            nMinDamage += WEAPON_SPECIALIZATION_DAMAGE_BONUS;
            nMaxDamage += WEAPON_SPECIALIZATION_DAMAGE_BONUS;
        }
    }
    else
    {
        nMinDamage = GetUnarmedDamageDice();
        unsigned char nDamageDie = GetUnarmedDamageDie();
        nMaxDamage = nMinDamage * nDamageDie;

        if (pCreature != NULL)
        {
            nMinDamage += m_nStrengthModifier;
            nMaxDamage += m_nStrengthModifier;
        }

        if (HasFeat(CSWFeat::FEAT_ECHANI_STRIKE_III))
        {
            nMinDamage += 3;
            nMaxDamage += 12;
        }
        else if (HasFeat(CSWFeat::FEAT_ECHANI_STRIKE_II))
        {
            nMinDamage += 2;
            nMaxDamage += 8;
        }
        else if (HasFeat(CSWFeat::FEAT_ECHANI_STRIKE_I))
        {
            nMinDamage += 1;
            nMaxDamage += 4;
        }

        if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_VIII))
        {
            nMinDamage += 8;
            nMaxDamage += 32;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_VII))
        {
            nMinDamage += 7;
            nMaxDamage += 28;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_VI))
        {
            nMinDamage += 6;
            nMaxDamage += 24;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_V))
        {
            nMinDamage += 5;
            nMaxDamage += 20;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_IV))
        {
            nMinDamage += 4;
            nMaxDamage += 16;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_III))
        {
            nMinDamage += 3;
            nMaxDamage += 12;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_II))
        {
            nMinDamage += 2;
            nMaxDamage += 8;
        }
        else if (HasFeat(CSWFeat::FEAT_UNARMED_SPECIALIST_I))
        {
            nMinDamage += 1;
            nMaxDamage += 4;
        }
    }

    if (bIncludeEffects)
    {
        int nCappedBonus = 0;
        int nBonus = GetCreature()->GetTotalEffectBonus(CSWSCreature::EFFECT_BONUS_DAMAGE,
            NULL, nCappedBonus, 0, 1, 0, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, bOffHand);
        nMinDamage += nBonus + nCappedBonus;
        nMaxDamage += nBonus;
    }

    nMinimum = nMinDamage;
    nMaximum = nMaxDamage;
    if (nMinimum < 1)
        nMinimum = 1;
    if (nMaximum < 1)
        nMaximum = 1;
}
// PC 0x005F48F0
char CSWSCreatureStats::GetTotalACFeatMod()
{
    char nBonus = 0;
    CSWSItem* pRightHand = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_RIGHTHAND);
    CSWSItem* pLeftHand = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_LEFTHAND);

    int bDueling = FALSE;
    if (pRightHand != NULL && pLeftHand == NULL)
    {
        CSWBaseItem* pBaseItem = pRightHand->GetBaseItem();
        if (pBaseItem != NULL &&
            (pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 ||
             pBaseItem->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4))
        {
            bDueling = TRUE;
        }
    }
    else if (pRightHand == NULL && pLeftHand == NULL)
    {
        bDueling = TRUE;
    }
    if (bDueling)
    {
        if (HasFeat(CSWFeat::FEAT_MASTER_DUELING))
        {
            nBonus = 3;
        }
        else if (HasFeat(CSWFeat::FEAT_IMPROVED_DUELING))
        {
            nBonus = 2;
        }
        else if (HasFeat(CSWFeat::FEAT_DUELING))
        {
            nBonus = 1;
        }
    }
    return nBonus;
}
// PC 0x005F4980
void CSWSCreatureStats::SetSTRBase(unsigned char nScore)
{
    m_nStrengthBase = nScore;
    UpdateSTRModifier();
}
// PC 0x005F49C0
void CSWSCreatureStats::SetDEXBase(unsigned char nScore)
{
    m_nDexterityBase = nScore;
    UpdateDEXModifier();
}
// PC 0x005F4A00
void CSWSCreatureStats::SetCONBase(unsigned char nValue, int bUpdate)
{
    m_nConstitutionBase = nValue;

    short nOldMaxHitPoints;
    if (bUpdate == 1)
    {
        nOldMaxHitPoints = m_pCreature->GetMaxHitPoints(1);
    }

    UpdateCONModifier();

    if (bUpdate == 1)
    {
        CSWSCreature* pCreature = m_pCreature;
        pCreature->SetCurrentHitPoints(
            pCreature->GetCurrentHitPoints(1) - nOldMaxHitPoints +
            pCreature->GetMaxHitPoints(1));
    }
}
// PC 0x005F4B10
void CSWSCreatureStats::SetINTBase(unsigned char nScore)
{
    m_nIntelligenceBase = nScore;
    UpdateINTModifier();
    AdjustSpellUsesPerDay();
}
// PC 0x005F4AD0
void CSWSCreatureStats::SetWISBase(unsigned char nScore)
{
    m_nWisdomBase = nScore;
    UpdateWISModifier();
    AdjustSpellUsesPerDay();
}
// PC 0x005F4A90
void CSWSCreatureStats::SetCHABase(unsigned char nScore)
{
    m_nCharismaBase = nScore;
    UpdateCHAModifier();
    AdjustSpellUsesPerDay();
}

void CSWSCreatureStats::SetStatById(int nStatId, float fValue)
{
    switch (nStatId)
    {
    case STAT_AGE:
        m_nAge = (int)fValue;
        break;
    case STAT_CHALLENGE_RATING:
        m_fChallengeRating = fValue;
        break;
    case STAT_FORTITUDE_SAVE:
        m_nFortSaveBonus = (char)fValue;
        break;
    case STAT_REFLEX_SAVE:
        m_nReflexSaveBonus = (char)fValue;
        break;
    case STAT_WILL_SAVE:
        m_nWillSaveBonus = (char)fValue;
        break;
    case STAT_STRENGTH:
        SetSTRBase((unsigned char)fValue);
        break;
    case STAT_DEXTERITY:
        SetDEXBase((unsigned char)fValue);
        break;
    case STAT_CONSTITUTION:
        SetCONBase((unsigned char)fValue, TRUE);
        break;
    case STAT_INTELLIGENCE:
        SetINTBase((unsigned char)fValue);
        break;
    case STAT_WISDOM:
        SetWISBase((unsigned char)fValue);
        break;
    case STAT_CHARISMA:
        SetCHABase((unsigned char)fValue);
        break;
    case STAT_BASE_ATTACK_BONUS:
        m_nBaseAttackBonus = (unsigned char)fValue;
        break;
    case STAT_FORCE_RESISTANCE:
        m_nForceResistance = (char)fValue;
        UpdateCombatInformation();
        break;
    }
}
// PC 0x005EEA60
unsigned char CSWSCreatureStats::GetSTRStat()
{
    return (m_nStrengthBase + (char)GetTotalSTRBonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nStrengthAdjust) > MINIMUM_ABILITY_SCORE
        ? (m_nStrengthBase + (char)GetTotalSTRBonus() +
           (char)g_pRules->m_pRaces[m_nRace].m_nStrengthAdjust)
        : MINIMUM_ABILITY_SCORE;
}
// PC 0x005EEB20
unsigned char CSWSCreatureStats::GetCONStat()
{
    return (m_nConstitutionBase + (char)GetTotalCONBonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nConstitutionAdjust) > MINIMUM_ABILITY_SCORE
        ? (m_nConstitutionBase + (char)GetTotalCONBonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nConstitutionAdjust)
        : MINIMUM_ABILITY_SCORE;
}
// PC 0x005EEBE0
unsigned char CSWSCreatureStats::GetINTStat()
{
    return (m_nIntelligenceBase + (char)GetTotalINTBonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nIntelligenceAdjust) > 3
        ? (m_nIntelligenceBase + (char)GetTotalINTBonus() +
           (char)g_pRules->m_pRaces[m_nRace].m_nIntelligenceAdjust)
        : 3;
}
// PC 0x005EECA0
unsigned char CSWSCreatureStats::GetWISStat()
{
    return (m_nWisdomBase + (char)GetTotalWISBonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nWisdomAdjust) > MINIMUM_ABILITY_SCORE
        ? (m_nWisdomBase + (char)GetTotalWISBonus() +
           (char)g_pRules->m_pRaces[m_nRace].m_nWisdomAdjust)
        : MINIMUM_ABILITY_SCORE;
}
// PC 0x005EED60
unsigned char CSWSCreatureStats::GetCHAStat()
{
    return (m_nCharismaBase + (char)GetTotalCHABonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nCharismaAdjust) > MINIMUM_ABILITY_SCORE
        ? (m_nCharismaBase + (char)GetTotalCHABonus() +
           (char)g_pRules->m_pRaces[m_nRace].m_nCharismaAdjust)
        : MINIMUM_ABILITY_SCORE;
}
// PC 0x005EEE20
unsigned char CSWSCreatureStats::GetDEXStat()
{
    return (m_nDexterityBase + (char)GetTotalDEXBonus() +
            (char)g_pRules->m_pRaces[m_nRace].m_nDexterityAdjust) > MINIMUM_ABILITY_SCORE
        ? (m_nDexterityBase + (char)GetTotalDEXBonus() +
           (char)g_pRules->m_pRaces[m_nRace].m_nDexterityAdjust)
        : MINIMUM_ABILITY_SCORE;
}
// PC 0x005EEEE0
unsigned long CSWSCreatureStats::GetExpNeededForLevelUp()
{
    return g_pRules->m_anExperience[GetLevel(0)];
}
// PC 0x005F4B50
char CSWSCreatureStats::GetBaseFortSavingThrow()
{
    char nBonus = 0;
    for (int nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass((unsigned char)nClassIndex)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetFortSaveBonus(GetClassLevel((unsigned char)nClassIndex, 0));
        }
    }

    if (m_pCreature->m_bInParty == 0)
    {
        int nAutoBalance = m_pCreature->m_c250;
        if (nAutoBalance != 0)
        {
            C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
            float fSaveMult;
            pAutoBalance->GetFLOATEntry(nAutoBalance, "SaveMult", &fSaveMult);
            char nLevel = m_pCreature->m_c251;
            if ((unsigned char)nLevel < 1)
            {
                nLevel = 1;
            }
            nBonus += (char)(nLevel * fSaveMult);
        }
    }

    if (HasFeat(CSWFeat::FEAT_LIGHTNING_REFLEXES))
    {
        nBonus += 3;
    }
    else if (HasFeat(CSWFeat::FEAT_IRON_WILL))
    {
        nBonus += 2;
    }
    else if (HasFeat(CSWFeat::FEAT_GREAT_FORTITUDE))
    {
        ++nBonus;
    }
    return nBonus;
}
// PC 0x005F4D50
unsigned char CSWSCreatureStats::GetBaseWillSavingThrow()
{
    unsigned char nBonus = 0;
    for (int nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass((unsigned char)nClassIndex)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetWillSaveBonus(GetClassLevel((unsigned char)nClassIndex, 0));
        }
    }

    if (m_pCreature->m_bInParty == 0)
    {
        int nAutoBalance = m_pCreature->m_c250;
        if (nAutoBalance != 0)
        {
            C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
            float fSaveMult;
            pAutoBalance->GetFLOATEntry(nAutoBalance, "SaveMult", &fSaveMult);
            char nLevel = m_pCreature->m_c251;
            if ((unsigned char)nLevel < 1)
            {
                nLevel = 1;
            }
            nBonus += (char)(nLevel * fSaveMult);
        }
    }

    if (HasFeat(CSWFeat::FEAT_LIGHTNING_REFLEXES))
    {
        nBonus += 3;
    }
    else if (HasFeat(CSWFeat::FEAT_IRON_WILL))
    {
        nBonus += 2;
    }
    else if (HasFeat(CSWFeat::FEAT_GREAT_FORTITUDE))
    {
        ++nBonus;
    }
    return nBonus;
}
// PC 0x005F4F50
unsigned char CSWSCreatureStats::GetBaseReflexSavingThrow()
{
    char nBonus = 0;
    for (int nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass((unsigned char)nClassIndex)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetRefSaveBonus(GetClassLevel((unsigned char)nClassIndex, 0));
        }
    }

    if (m_pCreature->m_bInParty == 0)
    {
        int nAutoBalance = m_pCreature->m_c250;
        if (nAutoBalance != 0)
        {
            C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
            float fSaveMult;
            pAutoBalance->GetFLOATEntry(nAutoBalance, "SaveMult", &fSaveMult);
            char nLevel = m_pCreature->m_c251;
            if ((unsigned char)nLevel < 1)
            {
                nLevel = 1;
            }
            nBonus += (char)(nLevel * fSaveMult);
        }
    }

    if (HasFeat(CSWFeat::FEAT_LIGHTNING_REFLEXES))
    {
        nBonus += 3;
    }
    else if (HasFeat(CSWFeat::FEAT_IRON_WILL))
    {
        nBonus += 2;
    }
    else if (HasFeat(CSWFeat::FEAT_GREAT_FORTITUDE))
    {
        ++nBonus;
    }
    return nBonus;
}
// PC 0x005F9750
void CSWSCreatureStats::ModifyAlignment(short nAdjustment, int bNoPartyUpdate)
{
    if (m_pCreature->m_bPlayerCreated)
    {
        short nCurrent = m_nGoodEvilAlignment;
        short nAlignment = max(0, min(100, nCurrent + nAdjustment));

        m_nGoodEvilAlignment = min(100, max(0, nAlignment));

        if (!bNoPartyUpdate)
        {
            {
                CExoString sVariable("G_PC_Align_Val");
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(sVariable, (char)nAlignment);
            }

            for (int i = 0; i < 12; i++)
            {
                unsigned long oidNPC = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetNPCObject(i, 0, 1);
                if (oidNPC != OBJECT_INVALID)
                {
                    CSWSCreature* pNPC = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidNPC);
                    if (pNPC != NULL)
                    {
                        pNPC->HandleAlignmentInfluence();
                    }
                }
            }
        }
    }
    else if (m_pCreature->m_bInParty)
    {
        short nAlignment = max(0, min(100, m_n18C + nAdjustment));

        m_n18C = min(100, max(-1, nAlignment));

        m_pCreature->HandleAlignmentInfluence();
    }
    else
    {
        short nAlignment = max(0, min(100, m_nGoodEvilAlignment + nAdjustment));

        m_nGoodEvilAlignment = min(100, max(0, nAlignment));
    }

    if (m_pCreature != NULL)
    {
        m_pCreature->UpdatePureGoodEvilPowers();
    }
    UpdateCombatInformation();
}
// PC 0x005F9960
void CSWSCreatureStats::UpdateLastStatsObject(unsigned __int64 nUpdateMask,
                                            CSWCreatureStatsUpdate* pUpdate)
{
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_ABILITIES)
    {
        pUpdate->m_nStrength = GetSTRStat();
        pUpdate->m_nDexterity = GetDEXStat();
        pUpdate->m_nConstitution = GetCONStat();
        pUpdate->m_nIntelligence = GetINTStat();
        pUpdate->m_nWisdom = GetWISStat();
        pUpdate->m_nCharisma = GetCHAStat();
        pUpdate->m_nStrengthBase = m_nStrengthBase;
        pUpdate->m_nDexterityBase = m_nDexterityBase;
        pUpdate->m_nConstitutionBase = m_nConstitutionBase;
        pUpdate->m_nIntelligenceBase = m_nIntelligenceBase;
        pUpdate->m_nWisdomBase = m_nWisdomBase;
        pUpdate->m_nCharismaBase = m_nCharismaBase;
        pUpdate->m_nStrengthBonus = GetTotalSTRBonus();
        pUpdate->m_nDexterityBonus = GetTotalDEXBonus();
        pUpdate->m_nConstitutionBonus = GetTotalCONBonus();
        pUpdate->m_nIntelligenceBonus = GetTotalINTBonus();
        pUpdate->m_nWisdomBonus = GetTotalWISBonus();
        pUpdate->m_nCharismaBonus = GetTotalCHABonus();
        pUpdate->m_nDexterityModifierBonus = GetDEXMod(1) - GetDEXMod(0);
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_FORTITUDE)
        pUpdate->m_nFortitudeSave = GetFortSavingThrow(0);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_WILL)
        pUpdate->m_nWillSave = GetWillSavingThrow(0);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_REFLEX)
        pUpdate->m_nReflexSave = GetReflexSavingThrow(0);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_EXPERIENCE)
        pUpdate->m_nExperience = GetExperience();
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_ATTACK_BONUS)
    {
        pUpdate->m_nBaseAttackBonus = GetBaseAttackBonus();
        pUpdate->m_bUseMonkAttackTables = GetUseMonkAttackTables(0);
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_COMBAT_INFORMATION)
        pUpdate->SetCombatInformation(m_pCombatInformation);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_ARMOR_CLASS)
        pUpdate->m_nArmorClass = m_pCreature->GetArmorClass();
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_SKILL_RANKS)
    {
        for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
        {
            char nRank = GetSkillRank(nSkill, NULL, 0);
            pUpdate->m_pSkillRanks[nSkill] = nRank;
        }
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_FEAT_LISTS)
    {
        pUpdate->m_oidCreature = m_pCreature->GetId();
        pUpdate->m_lstFeats.num = 0;
        for (unsigned short nFeat = 0; nFeat < (unsigned short)GetNumFeats(); ++nFeat)
            pUpdate->m_lstFeats.Add(GetFeat(nFeat));
        pUpdate->m_lstBonusFeats.num = 0;
        for (unsigned short nBonusFeat = 0;
             nBonusFeat < (unsigned short)GetNumBonusFeats(); ++nBonusFeat)
            pUpdate->m_lstBonusFeats.Add(GetBonusFeat(nBonusFeat));
    }
}
// PC 0x005F9E20
void CSWSCreatureStats::ReadSpellsFromGff(CResGFF* pRes, CResStruct* pStruct)
{
    CResList lClassList;
    CResStruct cClassStruct;
    CResStruct cSpellStruct;
    int bSuccess;
    if (pRes->GetList(&lClassList, pStruct, "ClassList"))
    {
        for (int nClass = 0; nClass < m_nClassCount; ++nClass)
        {
            if (pRes->GetListElement(&cClassStruct, &lClassList, nClass))
            {
                CExoString sKnownList;
                CResList lKnownList;
                sKnownList.Format("%s%d", "KnownList", 0);
                if (pRes->GetList(&lKnownList, &cClassStruct,
                                  (char*)sKnownList.CStr()))
                {
                    for (unsigned short nSpellIndex = 0;
                         nSpellIndex < pRes->GetListCount(&lKnownList);
                         ++nSpellIndex)
                    {
                        if (pRes->GetListElement(&cSpellStruct, &lKnownList,
                                                 nSpellIndex))
                        {
                            int nSpell = pRes->ReadFieldWORD(&cSpellStruct,
                                "Spell", bSuccess,
                                (unsigned short)CSWSpell::SPELL_INVALID);
                            if (nSpell != CSWSpell::SPELL_INVALID)
                            {
                                AddKnownSpell(nClass, nSpell);
                            }
                        }
                    }
                }
            }
        }
    }
}
// PC 0x005F9F80
void CSWSCreatureStats::SaveClassInfo(CResGFF* pRes, CResStruct* pStruct)
{
    CExoString sKnownList;
    CResList lClassList;
    CResList lLevelStatsList;
    CResList lSpellsPerDayList;
    CResList lKnownList;
    CResList lSkillList;
    CResList lFeatList;
    CResList lSpecAbilityList;
    CResStruct cClassStruct;
    CResStruct cSpellStruct;
    CResStruct cLevelStatStruct;
    CResStruct cSkillStruct;
    CResStruct cFeatStruct;
    CResStruct cAbilityStruct;

    pRes->AddList(&lClassList, pStruct, "ClassList");
    for (unsigned char nClass = 0; nClass < GetNumClasses(); ++nClass)
    {
        unsigned char nClassID = GetClass(nClass);
        pRes->AddListElement(&cClassStruct, &lClassList, GFF_CLASS);
        pRes->WriteFieldINT(&cClassStruct, nClassID, "Class");
        pRes->WriteFieldSHORT(&cClassStruct, GetClassLevel(nClass, 0), "ClassLevel");
        if (CSWClass::IsJedi(GetClass(nClass)))
        {
            CResStruct cSpellsPerDayStruct;
            pRes->AddList(&lSpellsPerDayList, &cClassStruct, "SpellsPerDayList");
            pRes->AddListElement(&cSpellsPerDayStruct, &lSpellsPerDayList,
                                 GFF_SPELLS_PER_DAY);
            pRes->WriteFieldBYTE(&cSpellsPerDayStruct,
                                 GetSpellsPerDayLeft(nClass, 0), "NumSpellsLeft");
        }
        for (unsigned char nSpellLevel = 0; nSpellLevel < 1; ++nSpellLevel)
        {
            short nNumSpells = GetNumberKnownSpells(nClass, nSpellLevel);
            if (nNumSpells > 0)
            {
                sKnownList.Format("%s%d", "KnownList", nSpellLevel);
                pRes->AddList(&lKnownList, &cClassStruct, (char*)sKnownList.CStr());
                for (unsigned char nSpell = 0; nSpell < nNumSpells; ++nSpell)
                {
                    pRes->AddListElement(&cSpellStruct, &lKnownList, GFF_KNOWN_SPELL);
                    pRes->WriteFieldWORD(&cSpellStruct, GetKnownSpell(nClass, nSpellLevel, nSpell),
                                         "Spell");
                }
            }
        }
    }

    if (GetIsPC() &&
        g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nControlledNPC == -1)
    {
        pRes->AddList(&lLevelStatsList, pStruct, "LvlStatList");
        for (unsigned char nLevel = 0; nLevel < GetLevel(0); ++nLevel)
        {
            CSWLevelStats* pLevelStats = GetLevelStats(nLevel);
            pRes->AddListElement(&cLevelStatStruct, &lLevelStatsList, GFF_LEVEL_STAT);
            if (nLevel > 0 && pLevelStats->GetAbilityIncrease() != CSWLevelStats::NO_ABILITY_INCREASE)
                pRes->WriteFieldBYTE(&cLevelStatStruct, pLevelStats->GetAbilityIncrease(),
                                     "LvlStatAbility");
            pRes->WriteFieldBYTE(&cLevelStatStruct, pLevelStats->GetHitDie(), "LvlStatHitDie");
            pRes->WriteFieldBYTE(&cLevelStatStruct, pLevelStats->GetForceDie(), "LvlStatForce");
            pRes->WriteFieldBYTE(&cLevelStatStruct, pLevelStats->GetClass(), "LvlStatClass");
            unsigned short nKnownSpells = (unsigned char)pLevelStats->m_lstSpellsAdded[0].num;
            if (nKnownSpells)
            {
                CExoString sList;
                sList.Format("%s%d", "KnownList", 0);
                pRes->AddList(&lKnownList, &cLevelStatStruct, (char*)sList.CStr());
                for (unsigned char nSpell = 0; nSpell < nKnownSpells; ++nSpell)
                {
                    pRes->AddListElement(&cSpellStruct, &lKnownList, 0);
                    pRes->WriteFieldWORD(&cSpellStruct,
                        pLevelStats->GetKnownSpell(0, nSpell), "Spell");
                }
            }
            unsigned short nRemovedSpells = (unsigned char)pLevelStats->m_lstSpellsRemoved[0].num;
            if (nRemovedSpells)
            {
                CExoString sList;
                sList.Format("%s%d", "KnownRemoveList", 0);
                pRes->AddList(&lKnownList, &cLevelStatStruct, (char*)sList.CStr());
                for (unsigned char nSpell = 0; nSpell < nRemovedSpells; ++nSpell)
                {
                    pRes->AddListElement(&cSpellStruct, &lKnownList, 0);
                    pRes->WriteFieldWORD(&cSpellStruct,
                        pLevelStats->GetRemovedSpell(0, nSpell), "Spell");
                }
            }
            pRes->WriteFieldWORD(&cLevelStatStruct, pLevelStats->GetSkillPoints(), "SkillPoints");
            pRes->AddList(&lSkillList, &cLevelStatStruct, "SkillList");
            for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
            {
                pRes->AddListElement(&cSkillStruct, &lSkillList, 0);
                pRes->WriteFieldBYTE(&cSkillStruct,
                    pLevelStats->GetSkillRankChange(nSkill), "Rank");
            }
            pRes->AddList(&lFeatList, &cLevelStatStruct, "FeatList");
            for (unsigned short nFeat = 0;
                 nFeat < (unsigned short)pLevelStats->m_lstFeats.num; ++nFeat)
            {
                pRes->AddListElement(&cFeatStruct, &lFeatList, 0);
                unsigned short nFeatID = pLevelStats->m_lstFeats[nFeat];
                pRes->WriteFieldWORD(&cFeatStruct, nFeatID, "Feat");
            }
        }
    }

    unsigned short nSkillPoints = m_nSkillPoints;
    pRes->WriteFieldWORD(pStruct, nSkillPoints, "SkillPoints");
    pRes->AddList(&lSkillList, pStruct, "SkillList");
    for (unsigned char nSkill = 0; nSkill < CSWRules::SKILL_MAX_SKILLS; ++nSkill)
    {
        pRes->AddListElement(&cSkillStruct, &lSkillList, 0);
        pRes->WriteFieldBYTE(&cSkillStruct, GetSkillRank(nSkill, NULL, 1), "Rank");
    }
    if (GetNumFeats() > 0)
    {
        pRes->AddList(&lFeatList, pStruct, "FeatList");
        for (unsigned short nFeat = 0; nFeat < GetNumFeats(); ++nFeat)
        {
            pRes->AddListElement(&cFeatStruct, &lFeatList, GFF_FEAT);
            pRes->WriteFieldWORD(&cFeatStruct, GetFeat(nFeat), "Feat");
        }
    }
    if (m_pSpellLikeAbilities->num > 0)
    {
        pRes->AddList(&lSpecAbilityList, pStruct, "SpecAbilityList");
        for (int nAbility = 0; nAbility < m_pSpellLikeAbilities->num; ++nAbility)
        {
            pRes->AddListElement(&cAbilityStruct, &lSpecAbilityList, GFF_SPECIAL_ABILITY);
            pRes->WriteFieldWORD(&cAbilityStruct,
                m_pSpellLikeAbilities->element[nAbility].m_nSpell, "Spell");
            pRes->WriteFieldBYTE(&cAbilityStruct,
                m_pSpellLikeAbilities->element[nAbility].m_nReady, "SpellFlags");
            pRes->WriteFieldBYTE(&cAbilityStruct,
                m_pSpellLikeAbilities->element[nAbility].m_nLevel, "SpellCasterLevel");
        }
    }
}
// PC 0x005FA770
void CSWSCreatureStats::SetExperience(unsigned long nExperience, int bUseThreshold)
{
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
        GetCreature()->GetId());
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    int nExperienceChange = nExperience - m_nExperience;
    CExoString sLog;

    if (nExperience < m_nExperience)
    {
        CExoString sError;
        const char* pszTag = "<NO TAG>";
        if (m_pCreature != NULL && m_pCreature->m_sTag.CStr() != "")
            pszTag = m_pCreature->m_sTag.CStr();
        sError.Format("Tried to reduce XP of creature '%s' to '%d'. Cannot reduce XP.",
            pszTag, nExperience);
    }
    else
    {
        m_nExperience = nExperience;

        int bFeedback = FALSE;
        if (pPlayer != NULL && bFeedback)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetInteger(CSWSMessage::FEEDBACK_INTEGER_EXPERIENCE, nExperienceChange);
            pMessage->SendServerToPlayerCCMessage(pPlayer->m_nPlayerId,
                CSWSMessage::FEEDBACK_MESSAGE_EXPERIENCE_GAINED, pData, NULL);
        }
        if (bUseThreshold)
        {
            if (nExperienceChange < 0)
            {
                for (unsigned char nLevel = GetLevel(0);
                     nLevel > g_pAppManager->GetServerExoApp()->GetServerInfo()->m_nMinLevel;
                     --nLevel)
                {
                    if (nLevel > 0)
                    {
                        if (g_pRules->GetExperienceForLevel(nLevel - 1) > m_nExperience)
                            LevelDown(!m_bIsPC ? NULL : GetLevelStats(nLevel - 1));
                        else
                            break;
                    }
                    else
                        break;
                }
            }
            else if (nExperienceChange > 0 && CanLevelUp())
            {
                do
                {
                    int nLevels = m_lstLevelStats.num;
                    if (nLevels - 1 < GetLevel(0))
                        break;
                    CSWLevelStats* pLevelStats = m_lstLevelStats[GetLevel(0)];
                    if (pLevelStats != NULL)
                        LevelUp(pLevelStats, 0);
                }
                while (CanLevelUp());
            }

            if (pPlayer != NULL && CanLevelUp())
            {
                if (nExperience - nExperienceChange < g_pRules->GetExperienceForLevel(GetLevel(0)))
                {
                    GetCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_LEVEL_UP, NULL);
                }
                if (m_pCreature->m_bPlayerCreated &&
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState == 0)
                {
                    char bFired = 0;
                    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(
                        "000_Level_Dlg_Fired", bFired);
                    if (!bFired)
                    {
                        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(
                            "000_Level_Dlg_Fired", 1);
                        CExoString sScript("k_level_dlg");
                        g_pVirtualMachine->RunScript(&sScript, 0, 0);
                    }
                }
            }
        }
    }
}
// PC 0x005FAA80
void CSWSCreatureStats::AddExperience(unsigned long nExperience)
{
    unsigned char nClassCount = m_nClassCount;
    if (nClassCount > 1)
    {
        double fPenalty = 0.0;
        unsigned char nHighestLevel = 0;
        unsigned char nHighestClass = 0;
        unsigned char nFavoredClass = g_pRules->m_pRaces[m_nRace].m_nFavoredClass;

        if (nFavoredClass == CSWRace::FAVORED_CLASS_HIGHEST)
        {
            if (GetClassLevel(0, 0) < GetClassLevel(1, 0))
            {
                nHighestClass = 1;
            }
            if (GetClassLevel(nHighestClass, 0) < GetClassLevel(2, 0))
            {
                nHighestClass = 2;
            }
            nFavoredClass = GetClass(nHighestClass);
        }

        unsigned char i;
        for (i = 0; i < nClassCount; ++i)
        {
            if (nFavoredClass != GetClass(i))
            {
                nHighestLevel = nHighestLevel > GetClassLevel(i, 0)
                    ? nHighestLevel : GetClassLevel(i, 0);
            }
        }

        for (i = 0; i < nClassCount; ++i)
        {
            if (nFavoredClass != GetClass(i))
            {
                if (nHighestLevel - GetClassLevel(i, 0) >=
                    SW_MULTICLASS_EXPERIENCE_LEVEL_DIFFERENCE)
                {
                    fPenalty += 1.0;
                }
            }
        }
        nExperience = (unsigned long)(nExperience *
            (1.0 - fPenalty * SW_MULTICLASS_EXPERIENCE_PENALTY));
    }
    SetExperience(m_nExperience + nExperience, 1);
}
// PC 0x005FAC60
int CSWSCreatureStats::GetCanUseSkill(unsigned char nSkill)
{
    unsigned char nBaseSkill;
    if (nSkill == CSWSkill::SUBSKILL_FLAGTRAP ||
        nSkill == CSWSkill::SUBSKILL_RECOVERTRAP ||
        nSkill == CSWSkill::SUBSKILL_EXAMINETRAP)
    {
        nBaseSkill = CSWSkill::SKILL_DEMOLITIONS;
    }
    else
    {
        nBaseSkill = nSkill;
    }

    if (nBaseSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }

    if (nBaseSkill == CSWRules::SKILL_STEALTH)
    {
        if (HasSpell(0, CSWSpell::FORCE_POWER_FORCE_CAMOUFLAGE, 0) || HasSpell(0, CSWSpell::FORCE_POWER_IMPROVED_FORCE_CAMOUFLAGE, 0) || HasSpell(0, CSWSpell::FORCE_POWER_MASTER_FORCE_CAMOUFLAGE, 0) || HasFeat(CSWFeat::FEAT_PERSONAL_CLOAKING_SHIELD))
        {
            return 1;
        }
    }

    if (g_pRules->m_pSkills[nBaseSkill].m_bUntrained)
    {
        for (unsigned char nClassIndex = 0; nClassIndex < m_nClassCount; nClassIndex++)
        {
            if (g_pRules->m_pClasses[GetClass(nClassIndex)].IsSkillUseable(nSkill))
            {
                return 1;
            }
        }
        return 0;
    }

    if (GetSkillRank(nBaseSkill, NULL, 1) > 0)
    {
        return 1;
    }
    return 0;
}

char CSWSCreatureStats::GetPrimaryMod(unsigned char nMultiClass)
{
    unsigned char nAbility = ABILITY_STRENGTH;
    CSWClass* pClass = &g_pRules->m_pClasses[GetClass(nMultiClass)];
    if (pClass != NULL)
    {
        nAbility = pClass->m_nPrimaryAbility;
    }
    switch (nAbility)
    {
    case ABILITY_STRENGTH:
        return m_nStrengthModifier;
    case ABILITY_DEXTERITY:
        return GetDEXMod(FALSE);
    case ABILITY_CONSTITUTION:
        return m_nConstitutionModifier;
    case ABILITY_INTELLIGENCE:
        return m_nIntelligenceModifier;
    case ABILITY_WISDOM:
        return m_nWisdomModifier;
    case ABILITY_CHARISMA:
        return m_nCharismaModifier;
    }
    return 0;
}
// PC 0x005F5150
char CSWSCreatureStats::GetSkillRank(unsigned char nSkill, CSWSObject* pTarget,
                                     int bBaseRank)
{
    if (nSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }

    int nRank = (char)m_pSkillRanks[nSkill];

    if (bBaseRank == 0)
    {
        if (nSkill == CSWRules::SKILL_STEALTH && nRank == 0 && HasSpell(0, CSWSpell::FORCE_POWER_FORCE_CAMOUFLAGE, 0))
        {
            nRank = 1;
        }

        if (!g_pRules->m_pSkills[nSkill].m_bUntrained && nRank == 0)
        {
            return 0;
        }

        int nCapped = 0;
        nRank += m_pCreature->GetTotalEffectBonus(5, pTarget, nCapped, 0, 0, 0,
                                                  0, nSkill, 0xFF, 0);

        int nKeyAbility = g_pRules->m_pSkills[nSkill].m_nKeyAbility;
        if (nKeyAbility == CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH)
        {
            nRank += m_nStrengthModifier;
            if (m_pCreature->GetBlind())
            {
                nRank -= 4;
            }
        }
        else if (nKeyAbility == CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY)
        {
            nRank += GetDEXMod(0);
            if (m_pCreature->GetBlind())
            {
                nRank -= 4;
            }
        }
        else if (nKeyAbility == CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION)
        {
            nRank += m_nConstitutionModifier;
        }
        else if (nKeyAbility == CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE)
        {
            nRank += m_nIntelligenceModifier;
        }
        else if (nKeyAbility == CSWSEffectListHandler::EFFECT_ABILITY_WISDOM)
        {
            nRank += m_nWisdomModifier;
        }
        else if (nKeyAbility == CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA)
        {
            nRank += m_nCharismaModifier;
        }

        if (g_pRules->m_pSkills[nSkill].m_bArmorCheckPenalty)
        {
            nRank += (char)(m_nShieldCheckPenalty + m_nArmorCheckPenalty);
        }

        unsigned long oidDialogOwner = m_pCreature->m_oidDialogOwner;
        CSWSPlaceable* pPlaceable = g_pAppManager->GetServerExoApp()
            ->GetPlaceableByGameObjectID(oidDialogOwner);

        switch (nSkill)
        {
        case CSWRules::SKILL_COMPUTER_USE:
            if (HasFeat(CSWFeat::FEAT_GEAR_HEAD_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_GEAR_HEAD_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_GEAR_HEAD))
            {
                nRank += 1;
            }
            if (pPlaceable != NULL)
            {
                nRank += pPlaceable->m_n43C;
            }
            break;
        case CSWRules::SKILL_DEMOLITIONS:
            if (HasFeat(CSWFeat::FEAT_CAUTIOUS_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_CAUTIOUS_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_CAUTIOUS))
            {
                nRank += 1;
            }
            break;
        case CSWRules::SKILL_STEALTH:
            if (HasFeat(CSWFeat::FEAT_CAUTIOUS_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_CAUTIOUS_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_CAUTIOUS))
            {
                nRank += 1;
            }
            if (HasSpell(0, CSWSpell::FORCE_POWER_MASTER_FORCE_CAMOUFLAGE, 0))
            {
                nRank += 8;
            }
            else if (HasSpell(0, CSWSpell::FORCE_POWER_IMPROVED_FORCE_CAMOUFLAGE, 0))
            {
                nRank += 4;
            }
            break;
        case CSWRules::SKILL_AWARENESS:
            if (HasFeat(CSWFeat::FEAT_EMPATHY_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_EMPATHY_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_EMPATHY))
            {
                nRank += 1;
            }
            break;
        case CSWRules::SKILL_PERSUADE:
            if (HasFeat(CSWFeat::FEAT_EMPATHY_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_EMPATHY_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_EMPATHY))
            {
                nRank += 1;
            }
            break;
        case CSWRules::SKILL_REPAIR:
            if (HasFeat(CSWFeat::FEAT_GEAR_HEAD_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_GEAR_HEAD_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_GEAR_HEAD))
            {
                nRank += 1;
            }
            break;
        case CSWRules::SKILL_SECURITY:
            if (HasFeat(CSWFeat::FEAT_GEAR_HEAD_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_GEAR_HEAD_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_GEAR_HEAD))
            {
                nRank += 1;
            }
            break;
        case CSWRules::SKILL_TREAT_INJURY:
            if (HasFeat(CSWFeat::FEAT_EMPATHY_3))
            {
                nRank += 3;
            }
            else if (HasFeat(CSWFeat::FEAT_EMPATHY_2))
            {
                nRank += 2;
            }
            else if (HasFeat(CSWFeat::FEAT_EMPATHY))
            {
                nRank += 1;
            }
            break;
        }
    }

    return max(min(nRank, 127), -127);
}
// PC 0x005F5490
void CSWSCreatureStats::AddFeat(unsigned short nFeat)
{
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
    if (pFeat != NULL)
    {
        m_lstFeats.AddUnique(nFeat);
        int bFound = 0;
        if (pFeat->m_nUsesPerDay > 0 ||
            pFeat->m_nUsesPerDay == CSWFeat::USES_PER_DAY_UNLIMITED)
        {
            for (unsigned short i = 0; !bFound && i < m_lstFeatUses.num; ++i)
            {
                if (m_lstFeatUses[i]->m_nFeat == nFeat)
                {
                    bFound = 1;
                }
            }
            if (!bFound)
            {
                CFeatUseListEntry* pFeatUse = new CFeatUseListEntry;
                pFeatUse->m_nFeat = nFeat;
                pFeatUse->m_nUses = 0;
                m_lstFeatUses.Add(pFeatUse);
            }
        }
    }
}
// PC 0x005F5540
void CSWSCreatureStats::ClearFeats()
{
    m_lstFeats.SetSize(0);

    while (m_lstFeatUses.num != 0)
    {
        CFeatUseListEntry* pFeatUse =
            m_lstFeatUses[m_lstFeatUses.num - 1];

        m_lstFeatUses.DelIndex(m_lstFeatUses.num - 1);
        if (pFeatUse != NULL)
        {
            delete pFeatUse;
        }
    }
}
// PC 0x005EEF00
unsigned char CSWSCreatureStats::GetPotentialLevel()
{
    unsigned char nLevel = GetLevel(0);
    if (m_bIsPC == 1)
    {
        while (m_nExperience >= g_pRules->m_anExperience[nLevel] &&
               nLevel < g_pAppManager->m_pServerExoApp->GetServerInfo()->m_nMaxLevel)
        {
            nLevel++;
        }
    }
    return nLevel;
}
// PC 0x005EEF60
int CSWSCreatureStats::HasFeat(unsigned short nFeat)
{
    int nResult = 0;
    for (int i = 0; i < m_lstFeats.num; i++)
    {
        if (m_lstFeats[i] == nFeat)
        {
            nResult = 1;
            break;
        }
    }
    if (nResult == 0)
    {
        for (int j = 0; j < m_lstBonusFeats.num; j++)
        {
            if (m_lstBonusFeats[j] == nFeat)
            {
                nResult = 1;
                break;
            }
        }
    }
    return nResult;
}
// PC 0x005EEFB0
unsigned char CSWSCreatureStats::GetFeatRemainingUses(unsigned short nFeat)
{
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
    if (pFeat != NULL && HasFeat(nFeat))
    {
        int bFound = 0;
        CFeatUseListEntry* pFeatUse = NULL;
        for (unsigned short i = 0; !bFound && i < m_lstFeatUses.num; ++i)
        {
            pFeatUse = m_lstFeatUses[i];
            if (pFeatUse->m_nFeat == nFeat)
            {
                bFound = 1;
            }
        }

        if (bFound && pFeatUse != NULL)
        {
            unsigned char nRemainingUses = pFeat->m_nUsesPerDay - pFeatUse->m_nUses;
            return min(max(nRemainingUses, 0), CSWFeat::MAX_FEAT_USES);
        }
        return CSWFeat::MAX_FEAT_USES;
    }
    return 0;
}
// PC 0x005EF050
void CSWSCreatureStats::DecrementFeatRemainingUses(unsigned short nFeat)
{
    for (unsigned short i = 0; i < m_lstFeatUses.num; ++i)
    {
        CFeatUseListEntry* pFeatUse = m_lstFeatUses[i];
        if (pFeatUse->m_nFeat == nFeat)
        {
            ++pFeatUse->m_nUses;
        }
    }
}

inline int CSWSCreatureStats_ClassInfo::GetKnownSpell(unsigned char nLevel,
                                                      unsigned char nIndex)
{
    int nSpell;
    if (nIndex >= m_lstKnownSpells[nLevel].num)
        nSpell = -1;
    else
        nSpell = m_lstKnownSpells[nLevel][nIndex];
    return nSpell;
}
// PC 0x005EF080
int CSWSCreatureStats::GetKnownSpell(unsigned char nClass,
                                     unsigned char nSpellLevel,
                                     unsigned char nSpellIndex)
{
    if (nClass >= m_nClassCount)
    {
        return -1;
    }

    CSWSCreatureStats_ClassInfo* pInfo = &m_aClassInfo[nClass];
    if (nSpellLevel >= 1)
    {
        return -1;
    }
    return pInfo->GetKnownSpell(nSpellLevel, nSpellIndex);
}
// PC 0x005F55C0
void CSWSCreatureStats::RemoveFeat(unsigned short nFeat)
{
    NwscreaturestatsTrace2();
    m_lstFeats.Remove(nFeat);
    int nFeatUses = m_lstFeatUses.num;
    if (nFeatUses > 0)
    {
        for (short i = nFeatUses - 1; i >= 0; --i)
        {
            CFeatUseListEntry* pFeatUse = m_lstFeatUses[i];
            if (pFeatUse->m_nFeat == nFeat)
            {
                m_lstFeatUses.DelIndex(i);
                delete pFeatUse;
                break;
            }
        }
    }
}
// PC 0x005F5630
void CSWSCreatureStats::AddKnownSpell(unsigned char nClass,
                                      unsigned long nSpell)
{
    if (nClass < m_nClassCount)
    {
        if (g_pRules->m_pSpellArray->GetSpell(nSpell) != NULL)
        {
            if (!m_bIsPC || CSWClass::IsJedi(GetClass(nClass)))
            {
                m_aClassInfo[nClass].AddKnownSpell(0, nSpell);
            }
        }
    }
}
// PC 0x005F56B0
int CSWSCreatureStats::GetCreatureHasTalent(int nTalentType, int nTalent)
{
    if (nTalentType == INVALID_TALENT || nTalent == INVALID_TALENT)
    {
        return 0;
    }

    if (((m_pCreature->m_nAIStateFlags &
          (CSWSCreature::AI_STATE_CAN_MOVE |
           CSWSCreature::AI_STATE_CAN_USE_HANDS)) !=
             (CSWSCreature::AI_STATE_CAN_MOVE |
              CSWSCreature::AI_STATE_CAN_USE_HANDS) &&
         m_pCreature->GetDead() != 0) ||
        m_pCreature->GetIsPCDying() != 0 ||
        m_pCreature->GetDeadTemp() != 0)
    {
        if (nTalentType == CScriptTalent::TALENT_TYPE_SPELL)
        {
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nTalent);
        }
        return 0;
    }

    if (nTalentType == CScriptTalent::TALENT_TYPE_SPELL)
    {
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nTalent);
        if (pSpell == NULL)
        {
            return 0;
        }

        int bHasTalent = 0;
        for (unsigned char nClass = 0; nClass < m_nClassCount; nClass++)
        {
            if (CSWClass::IsJedi(GetClass(nClass)))
            {
                if (HasSpell(0, nTalent, 1) &&
                    GetSpellUsesLeft(nTalent, nClass, 0, 0))
                {
                    bHasTalent = 1;
                }
            }
            else if (GetIsInSpellLikeAbilityList(nTalent) ||
                     (HasSpell(0, nTalent, 1) &&
                      GetSpellUsesLeft(nTalent, nClass, 0, 0)) ||
                     GetHasPowerFromItem(nTalent))
            {
                bHasTalent = 1;
            }
        }

        return bHasTalent &&
               (m_pCreature->m_nAIStateFlags & CSWSCreature::AI_STATE_CAN_SPEAK);
    }
    else if (nTalentType == CScriptTalent::TALENT_TYPE_FEAT)
    {
        return HasFeat(nTalent) && GetFeatRemainingUses(nTalent);
    }
    else if (nTalentType == CScriptTalent::TALENT_TYPE_SKILL)
    {
        return GetSkillRank(nTalent, NULL, 0) > 0 ||
               g_pRules->m_pSkills[(unsigned short)nTalent].m_bUntrained;
    }

    return 0;
}
// PC 0x005F5870
void CSWSCreatureStats::LevelUp(CSWLevelStats* pLevelStats, int bStoreLevelStats)
{
    int bNewClass = 1;
    unsigned char nClass = 0;
    m_pCreature->GetMaxHitPoints(1);
    short nOldForcePoints = m_pCreature->GetMaxForcePoints();

    if (bStoreLevelStats)
        m_lstLevelStats.Add(pLevelStats);

    for (unsigned char i = 0; i < m_nClassCount; ++i)
    {
        if (GetClass(i) == (unsigned char)pLevelStats->m_nClass)
        {
            bNewClass = 0;
            nClass = i;
            SetClassLevel(i, GetClassLevel(i, 0) + 1);
        }
    }
    if (bNewClass)
    {
        ++m_nClassCount;
        nClass = m_nClassCount - 1;
        SetClass(nClass, pLevelStats->m_nClass);
        SetClassLevel(nClass, 1);
    }

    m_pCreature->m_nMaxHitPoints = (short)m_pCreature->m_nMaxHitPoints + pLevelStats->m_nHitDie;
    m_nForcePoints += (unsigned char)pLevelStats->m_nForceDie;
    switch ((unsigned char)pLevelStats->m_nAbilityIncrease)
    {
    case CSWRules::ABILITY_STRENGTH:
        ++m_nStrengthBase;
        UpdateSTRModifier();
        break;
    case CSWRules::ABILITY_DEXTERITY:
        ++m_nDexterityBase;
        UpdateDEXModifier();
        break;
    case CSWRules::ABILITY_CONSTITUTION:
        SetCONBase(m_nConstitutionBase + 1, 0);
        break;
    case CSWRules::ABILITY_INTELLIGENCE:
        SetINTBase(m_nIntelligenceBase + 1);
        break;
    case CSWRules::ABILITY_WISDOM:
        SetWISBase(m_nWisdomBase + 1);
        break;
    case CSWRules::ABILITY_CHARISMA:
        SetCHABase(m_nCharismaBase + 1);
        break;
    }

    for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
    {
        if (pLevelStats->GetSkillRankChange(nSkill))
        {
            char nRank = GetSkillRank(nSkill, NULL, 1);
            SetSkillRank(nSkill, nRank + pLevelStats->GetSkillRankChange(nSkill));
        }
    }
    m_nSkillPoints = pLevelStats->m_nSkillPoints;
    for (unsigned char nFeat = 0; nFeat < (unsigned short)pLevelStats->m_lstFeats.num; ++nFeat)
        AddFeat(pLevelStats->m_lstFeats[nFeat]);
    for (unsigned char nSpell = 0; nSpell < (unsigned char)pLevelStats->m_lstSpellsRemoved[0].num; ++nSpell)
        RemoveKnownSpell(nClass, pLevelStats->m_lstSpellsRemoved[0][nSpell]);
    for (unsigned char nSpell = 0; nSpell < (unsigned char)pLevelStats->m_lstSpellsAdded[0].num; ++nSpell)
        AddKnownSpell(nClass, pLevelStats->m_lstSpellsAdded[0][nSpell]);

    CSWSCreature* pCreature = m_pCreature;
    pCreature->SetCurrentHitPoints(pCreature->GetMaxHitPoints(1));
    m_nCurrentForcePoints += m_pCreature->GetMaxForcePoints() - nOldForcePoints;
}
// PC 0x005F5E60
void CSWSCreatureStats::LevelDown(CSWLevelStats* pLevelStats)
{
    unsigned long nSlotMask;
    short nOldMaxHitPoints = m_pCreature->GetMaxHitPoints(1);
    unsigned char nClass;
    for (nClass = 0; nClass < GetNumClasses(); ++nClass)
    {
        if (GetClass(nClass) == (unsigned char)pLevelStats->m_nClass)
        {
            SetClassLevel(nClass, GetClassLevel(nClass, 0) - 1);
            break;
        }
    }

    unsigned char nNumClasses = GetNumClasses();
#ifdef _XBOX
    if (GetClassLevel(nClass, 0) < 1)
    {
        if (nClass == CLASS_TYPE_INVALID)
        {
            --m_nClassCount;
            SetClass(nClass, 0);
        }
        else
        {
            EXOXBOXASSERT(0);
        }
    }
#else
    if ((GetClassLevel(nClass, 0) < 1 && nClass == CLASS_TYPE_INVALID) ||
        GetNumClasses() != nNumClasses)
    {
        --m_nClassCount;
        SetClass(nClass, 0);
    }
#endif
    m_pCreature->m_nMaxHitPoints =
        (short)m_pCreature->m_nMaxHitPoints - pLevelStats->m_nHitDie;

    switch ((unsigned char)pLevelStats->m_nAbilityIncrease)
    {
    case CSWRules::ABILITY_STRENGTH:
        --m_nStrengthBase;
        UpdateSTRModifier();
        break;
    case CSWRules::ABILITY_DEXTERITY:
        --m_nDexterityBase;
        UpdateDEXModifier();
        break;
    case CSWRules::ABILITY_CONSTITUTION: SetCONBase(m_nConstitutionBase - 1, 0); break;
    case CSWRules::ABILITY_INTELLIGENCE: SetINTBase(m_nIntelligenceBase - 1); break;
    case CSWRules::ABILITY_WISDOM: SetWISBase(m_nWisdomBase - 1); break;
    case CSWRules::ABILITY_CHARISMA: SetCHABase(m_nCharismaBase - 1); break;
    }

    for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
    {
        if (pLevelStats->GetSkillRankChange(nSkill) != 0)
        {
            SetSkillRank(nSkill, GetSkillRank(nSkill, NULL, 1) -
                         pLevelStats->GetSkillRankChange(nSkill));
        }
    }
    m_nSkillPoints = 0;
    for (unsigned char nFeat = 0;
         nFeat < (unsigned short)pLevelStats->m_lstFeats.num; ++nFeat)
    {
        RemoveFeat(pLevelStats->m_lstFeats[nFeat]);
    }
    for (unsigned char nSpell = 0;
         nSpell < (unsigned char)pLevelStats->m_lstSpellsAdded[0].num; ++nSpell)
    {
        RemoveKnownSpell(nClass, pLevelStats->m_lstSpellsAdded[0][nSpell]);
    }
    for (unsigned char nRemovedSpell = 0;
         nRemovedSpell < (unsigned char)pLevelStats->m_lstSpellsRemoved[0].num;
         ++nRemovedSpell)
    {
        AddKnownSpell(nClass, pLevelStats->m_lstSpellsRemoved[0][nRemovedSpell]);
    }
    m_lstLevelStats.Remove(pLevelStats);
    delete pLevelStats;

    for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
    {
        nSlotMask = 1 << nSlot;
        CSWSItem* pItem = m_pCreature->GetInventory()->GetItemInSlot(nSlotMask);
        if (pItem != NULL &&
            !m_pCreature->CanEquipItem(pItem, &nSlotMask, 0, 0, 1))
        {
            m_pCreature->UnequipItem(pItem);
        }
    }
    short nNewMaxHitPoints = m_pCreature->GetMaxHitPoints(1);
    CSWSCreature* pCreature = m_pCreature;
    pCreature->SetCurrentHitPoints(nNewMaxHitPoints - nOldMaxHitPoints +
                                 pCreature->GetCurrentHitPoints(1));
}
// PC 0x005F6490
char CSWSCreatureStats::GetFortSavingThrow(int bBaseOnly)
{
    int nEffectBonus = 0;
    if (!bBaseOnly)
    {
        int nCapped = 0;
        nEffectBonus = GetCreature()->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_SAVING_THROW, NULL, nCapped, 0, 0,
            CSWSCreature::SAVING_THROW_FORT, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0);
    }
    unsigned char nConModifier = m_nConstitutionModifier;
    unsigned char nSaveBonus = m_nFortSaveBonus;
    return GetBaseFortSavingThrow() + nEffectBonus + nConModifier + nSaveBonus;
}
// PC 0x005F6500
char CSWSCreatureStats::GetWillSavingThrow(int bBaseOnly)
{
    int nEffectBonus = 0;
    if (!bBaseOnly)
    {
        int nCapped = 0;
        nEffectBonus = GetCreature()->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_SAVING_THROW, NULL, nCapped, 0, 0,
            CSWSCreature::SAVING_THROW_WILL, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0);
    }
    unsigned char nWisModifier = m_nWisdomModifier;
    unsigned char nSaveBonus = m_nWillSaveBonus;
    return GetBaseWillSavingThrow() + nEffectBonus + nWisModifier + nSaveBonus;
}
// PC 0x005F6570
char CSWSCreatureStats::GetReflexSavingThrow(int bBaseOnly)
{
    int nEffectBonus = 0;
    if (!bBaseOnly)
    {
        int nCapped = 0;
        nEffectBonus = m_pCreature->GetTotalEffectBonus(
            CSWSCreature::EFFECT_BONUS_SAVING_THROW, NULL, nCapped, 0, 0,
            CSWSCreature::SAVING_THROW_REFLEX, 0,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL,
            CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0);
    }
    int nDexModifier = GetDEXMod(0);
    unsigned char nSaveBonus = m_nReflexSaveBonus;
    return GetBaseReflexSavingThrow() + nEffectBonus + nDexModifier + nSaveBonus;
}
// PC 0x005F6600
int CSWSCreatureStats::CheckPowerSuitability(
    int nTalent, CSWSpell* pSpell, int nPowerSuitability, int nUnknown1,
    int nUnknown2, int nUnknown3, int nUnknown4, int nInvalidTalent)
{
    if (nTalent == nInvalidTalent)
    {
        return 0;
    }

    if (GetCreatureHasTalent(0, nTalent) == 0)
    {
        return 0;
    }

    if (m_pCreature->GetArea() != NULL)
    {
        int nCategory = pSpell->m_nCategory;
        if (CheckPowerSuitability(nCategory, nPowerSuitability) == 0)
        {
            return 0;
        }
    }

    return 1;
}
// PC 0x005F6660
int CSWSCreatureStats::GetCreatureTalentRandomSpecialAbility(
    int nFilter, int nSelection, int& nTalentType, int& nTalent,
    unsigned char& nClass, unsigned char& nCasterLevel,
    int nExclusion, int nExcludedPower)
{
    int bFound = 0;
    CExoArrayList<int>* pTalents = new CExoArrayList<int>(0);
    CExoArrayList<int>* pLevels = new CExoArrayList<int>(0);
    int nMaxCR = TALENT_INITIAL_MAX_CR;
    if (m_pSpellLikeAbilities->num > 0)
    {
        for (int nAbility = 0; nAbility < m_pSpellLikeAbilities->num; ++nAbility)
        {
            int nSpell = (*m_pSpellLikeAbilities)[nAbility].m_nSpell;
            if ((*m_pSpellLikeAbilities)[nAbility].m_nReady)
            {
                unsigned char nLevel = (*m_pSpellLikeAbilities)[nAbility].m_nLevel;
                CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
                if (pSpell != NULL &&
                    CheckPowerSuitability(nSpell, pSpell, nFilter, nSelection,
                                          0, 0, nExcludedPower, INVALID_TALENT) == 1)
                {
                    if (CheckInclusionMatch(CScriptTalent::TALENT_TYPE_SPELL, nSpell, nExclusion) == 1)
                    {
                        if (nSelection == TALENT_SELECTION_RANDOM)
                        {
                            pTalents->Add(nSpell);
                            pLevels->Add(nLevel);
                            EXOLOG("Talent special ability: %s\n", pSpell->m_sLabel.CStr());
                        }
                        else if (pSpell->m_nMaxCR > nMaxCR)
                        {
                            delete pTalents;
                            pTalents = new CExoArrayList<int>(0);
                            pTalents->Add(nSpell);
                            delete pLevels;
                            pLevels = new CExoArrayList<int>(0);
                            pLevels->Add(nLevel);
                            EXOLOG("Talent special ability: %s\n", pSpell->m_sLabel.CStr());
                            nMaxCR = pSpell->m_nMaxCR;
                        }
                        else if (pSpell->m_nMaxCR == nMaxCR)
                        {
                            pTalents->Add(nSpell);
                            pLevels->Add(nLevel);
                            EXOLOG("Talent special ability: %s\n", pSpell->m_sLabel.CStr());
                        }
                    }
                }
            }
        }
    }
    int nTalentCount = pTalents->num;
    if (nTalentCount > 0)
    {
        int nChoice = rand() % nTalentCount;
        nTalentType = CScriptTalent::TALENT_TYPE_SPELL;
        bFound = 1;
        nTalent = (*pTalents)[nChoice];
        nCasterLevel = (unsigned char)(*pLevels)[nChoice];
        nClass = TALENT_CLASS_SPECIAL_ABILITY;
    }
    delete pTalents;
    delete pLevels;
    return bFound;
}

inline int CSWSItem::GetActiveProperties() { return m_nActiveProperties; }
// PC 0x005F6980
int CSWSCreatureStats::GetCreatureTalentRandomPowerFromItem(
    int nFilter, int nSelection, int& nTalentType, int& nTalent,
    unsigned char& nClass, unsigned long& oidItem, int& nItemProperty,
    int nExclusion, int nExcludedPower)
{
    CSWSItem* pItem;
    CSWItemProperty* pProperty;
    int nProperty;
    unsigned short nType;
    unsigned char bUseable;
    unsigned short nSubtype;
    int nPower;
    int nSlot;
    int bFound = 0;
    CExoArrayList<int>* pPowers = new CExoArrayList<int>(0);
    CExoArrayList<unsigned long>* pItems = new CExoArrayList<unsigned long>(0);
    CExoArrayList<int>* pProperties = new CExoArrayList<int>(0);
    int nMaxCR = TALENT_INITIAL_MAX_CR;

    if (GetCreature()->GetInventory() != NULL)
    {
        for (nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
        {
            pItem = GetCreature()->GetInventory()->GetItemInSlot(1 << nSlot);
            if (pItem != NULL && pItem->GetPropertyByTypeExists(
                    CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
            {
                for (nProperty = 0; nProperty < pItem->GetActiveProperties(); ++nProperty)
                {
                    pProperty = pItem->GetActiveProperty(nProperty);
                    nType = pProperty->m_nPropertyType;
                    if (nType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
                    {
                        bUseable = pItem->GetActiveProperty(nProperty)->m_bUseable;
                        if (bUseable != 0)
                        {
                            pProperty = pItem->GetActiveProperty(nProperty);
                            if (pItem->GetIsUpgradeActive(pProperty->m_nUpgradeType))
                            {
                                pProperty = pItem->GetActiveProperty(nProperty);
                                nSubtype = pProperty->m_nSubtype;
                                nPower = nSubtype;
                                CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nPower);
                                if (pSpell != NULL &&
                                    CheckPowerSuitability(nPower, pSpell, nFilter,
                                        nSelection, 0, 0, nExcludedPower, INVALID_TALENT) == 1 &&
                                    CheckInclusionMatch(CScriptTalent::TALENT_TYPE_SPELL,
                                        nPower, nExclusion) == 1)
                                {
                                    if (nSelection == TALENT_SELECTION_RANDOM)
                                    {
                                        pPowers->Add(nPower);
                                        pItems->Add(pItem->GetId());
                                        pProperties->Add(nProperty);
                                    }
                                    else if (pSpell->m_nMaxCR > nMaxCR)
                                    {
                                        delete pPowers;
                                        delete pItems;
                                        delete pProperties;
                                        pPowers = new CExoArrayList<int>(0);
                                        pItems = new CExoArrayList<unsigned long>(0);
                                        pProperties = new CExoArrayList<int>(0);
                                        pPowers->Add(nPower);
                                        pItems->Add(pItem->GetId());
                                        pProperties->Add(nProperty);
                                        nMaxCR = pSpell->m_nMaxCR;
                                    }
                                    else if (pSpell->m_nMaxCR == nMaxCR)
                                    {
                                        pPowers->Add(nPower);
                                        pItems->Add(pItem->GetId());
                                        pProperties->Add(nProperty);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    CItemRepository* pRepository = m_pCreature->GetItemRepository(1);
    if (pRepository != NULL)
    {
        for (unsigned int nItem = 0; nItem < (unsigned int)pRepository->m_lstItems.num; ++nItem)
        {
            pItem = pRepository->ItemListGetItem(nItem);
            if (pItem != NULL &&
                g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType())->m_nEquipableSlots == 0 &&
                pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
            {
                for (nProperty = 0; nProperty < pItem->GetActiveProperties(); ++nProperty)
                {
                    pProperty = pItem->GetActiveProperty(nProperty);
                    nType = pProperty->m_nPropertyType;
                    if (nType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
                    {
                        bUseable = pItem->GetActiveProperty(nProperty)->m_bUseable;
                        if (bUseable != 0 || pItem->m_nCharges > 0)
                        {
                            pProperty = pItem->GetActiveProperty(nProperty);
                            if (pItem->GetIsUpgradeActive(pProperty->m_nUpgradeType))
                            {
                                pProperty = pItem->GetActiveProperty(nProperty);
                                nSubtype = pProperty->m_nSubtype;
                                nPower = nSubtype;
                                CSWSpellArray* pSpellArray = g_pRules->m_pSpellArray;
                                CSWSpell* pSpell = pSpellArray->GetSpell(nPower);
                                if (pSpell != NULL)
                                {
                                    nSlot = pSpell->m_nCategory;
                                    if (CheckPowerSuitability(nSlot, nFilter) == 1)
                                    {
                                        if (nSelection == TALENT_SELECTION_RANDOM)
                                        {
                                            pPowers->Add(nPower);
                                            pItems->Add(pItem->GetId());
                                            pProperties->Add(nProperty);
                                        }
                                        else if (pSpell->m_nMaxCR > nMaxCR)
                                        {
                                            delete pPowers;
                                            delete pItems;
                                            delete pProperties;
                                            pPowers = new CExoArrayList<int>(0);
                                            pItems = new CExoArrayList<unsigned long>(0);
                                            pProperties = new CExoArrayList<int>(0);
                                            pPowers->Add(nPower);
                                            pItems->Add(pItem->GetId());
                                            pProperties->Add(nProperty);
                                            nMaxCR = pSpell->m_nMaxCR;
                                        }
                                        else if (pSpell->m_nMaxCR == nMaxCR)
                                        {
                                            pPowers->Add(nPower);
                                            pItems->Add(pItem->GetId());
                                            pProperties->Add(nProperty);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    nProperty = pPowers->num;
    if (nProperty > 0)
    {
        nProperty = rand() % nProperty;
        nTalentType = CScriptTalent::TALENT_TYPE_SPELL;
        bFound = 1;
        nTalent = (*pPowers)[nProperty];
        oidItem = (*pItems)[nProperty];
        nItemProperty = (*pProperties)[nProperty];
    }
    delete pPowers;
    delete pItems;
    delete pProperties;
    return bFound;
}
// PC 0x005F6FF0
int CSWSCreatureStats::GetCreatureTalentRandomFeat(
    int nFilter, int nSelection, int& nTalentType, int& nTalent,
    unsigned char& nClass, int nExclusion, int nExcludedFeat)
{
    int bFound = 0;
    CExoArrayList<int>* pTalents = new CExoArrayList<int>(0);
    int nMaxCR = TALENT_INITIAL_MAX_CR;
    unsigned short nIndex = 0;
    EXOLOG("CSWSCreatureStats::GetCreatureTalentRandomFeat: feat index %p", &nIndex);
    while (nIndex < GetNumFeats())
    {
        unsigned short nFeat = m_lstFeats[nIndex];
        CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
        if (pFeat != NULL)
        {
            int nCategory = pFeat->m_nCategory;
            if (CheckPowerSuitability(nCategory, nFilter))
            {
                if (CheckInclusionMatch(CScriptTalent::TALENT_TYPE_FEAT, nFeat, nExclusion) &&
                    nExcludedFeat != nFeat &&
                    nFeat != INVALID_TALENT &&
                    !(((m_pCreature->m_nAIStateFlags &
                        (CSWSCreature::AI_STATE_CAN_MOVE |
                         CSWSCreature::AI_STATE_CAN_USE_HANDS)) !=
                           (CSWSCreature::AI_STATE_CAN_MOVE |
                            CSWSCreature::AI_STATE_CAN_USE_HANDS) &&
                       m_pCreature->GetDead() != 0) ||
                      m_pCreature->GetIsPCDying() != 0 ||
                      m_pCreature->GetDeadTemp() != 0) &&
                    HasFeat(nFeat) && GetFeatRemainingUses(nFeat))
                {
                    if (nSelection == TALENT_SELECTION_RANDOM)
                    {
                        pTalents->Add(nFeat);
                    }
                    else if (pFeat->m_nMaxCR > nMaxCR)
                    {
                        delete pTalents;
                        pTalents = new CExoArrayList<int>(0);
                        pTalents->Add(nFeat);
                        nMaxCR = pFeat->m_nMaxCR;
                    }
                    else if (pFeat->m_nMaxCR == nMaxCR)
                    {
                        pTalents->Add(nFeat);
                    }
                }
                else
                {
                    ++nIndex;
                    continue;
                }
            }
        }
        ++nIndex;
    }
    nMaxCR = pTalents->num;
    if (nMaxCR > 0)
    {
        nTalentType = CScriptTalent::TALENT_TYPE_FEAT;
        nTalent = (*pTalents)[rand() % nMaxCR];
        bFound = 1;
    }
    delete pTalents;
    return bFound;
}
// PC 0x005FAD90
int CSWSCreatureStats::ValidateLevelUp(CSWLevelStats* pLevelStats)
{
    if (!CanLevelUp())
    {
        return 0;
    }

    if (m_bIsPC)
    {
        CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
        if (pModule)
        {
            CScriptEvent* pEvent = new CScriptEvent;
            if (pEvent)
            {
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_LEVEL_UP;
                unsigned long oidModule = pModule->m_nId;
                unsigned long oidCreature = m_pCreature->m_nId;
                CServerExoApp* pApp = g_pAppManager->m_pServerExoApp;
                pApp->GetServerAIMaster()->AddEventAbsoluteTime(
                    0, 0, oidCreature, oidModule,
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }

    LevelUp(pLevelStats, 1);
    UpdateCombatInformation();
    return 1;
}
// PC 0x005FAE70
int CSWSCreatureStats::GetCreatureTalentRandomPower(
    int nFilter, int nSelection, int& nTalentType, int& nTalent,
    unsigned char& nClass, int nUnknown, int nExcludedPower)
{
    int bFound = 0;
    for (int nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
    {
            unsigned char nClassType = GetClass((unsigned char)nClassIndex);
            if (CSWClass::IsJedi((int)nClassType) != 0)
            {
                bFound = GetCreatureTalentRandomKnownPower(
                    nFilter, nSelection, nTalentType, nTalent, nClass,
                    nClassIndex, nUnknown, nExcludedPower);
                if (bFound == 1)
                {
                    break;
                }
            }
    }
    return bFound;
}
// PC 0x005F7260
int CSWSCreatureStats::GetCreatureTalentRandomKnownPower(
    int nFilter, int nSelection, int& nTalentType, int& nTalent,
    unsigned char& nClass, int nClassIndex, int nUnknown, int nExcludedPower)
{
    int nSpell = 0;
    CSWSpell* pSpell = NULL;
    int bAdjust = 0;
    short nAlignment = 0;
    int nForcePoints = 0;
    CExoArrayList<int>* pTalents = new CExoArrayList<int>(0);
    CExoArrayList<unsigned char>* pClasses = new CExoArrayList<unsigned char>(0);
    int bFound = 0;
    for (unsigned char nSpellIndex = 0;
         nSpellIndex < GetNumberKnownSpells((unsigned char)nClassIndex, 0);
         ++nSpellIndex)
    {
        nSpell = GetKnownSpell((unsigned char)nClassIndex, 0, nSpellIndex);
        pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
        if (pSpell != NULL)
        {
            CSWSArea* pArea = m_pCreature->GetArea();
            int nRoomForceRating = 0;
            if (pArea != NULL)
                nRoomForceRating = pArea->GetRoomForceRating(m_pCreature->m_vPosition);
            int bEnoughPoints = 0;
            int nForceBodyLevel = -1;
            CExoArrayList<CGameEffect*>* pEffects = &m_pCreature->m_lstEffects;
            for (int i = 0; i < pEffects->num; i++)
            {
                CGameEffect* pEffect = (*pEffects)[i];
                if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_FORCEBODY)
                {
                    nForceBodyLevel = pEffect->GetInteger(0);
                    break;
                }
            }
            if (nForceBodyLevel != -1)
            {
                int nPercent = 0;
                int nFullCost = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
                    m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), m_pCreature->GetStats()->GetCHAMod());
                switch (nForceBodyLevel)
                {
                case 0:
                    nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_0;
                    break;
                case 1:
                    nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_1;
                    break;
                case 2:
                    nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_2;
                    break;
                }
                int nHitPointCost = nFullCost * nPercent / 100;
                if (nHitPointCost <= GetCurrentForcePoints())
                {
                    if (m_pCreature->GetCurrentHitPoints(0) - nHitPointCost > 1)
                    {
                        bEnoughPoints = 1;
                    }
                    else
                    {
                        m_pCreature->GetCurrentHitPoints(0);
                    }
                }
            }
            else
            {
                nForcePoints = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
                    m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), m_pCreature->GetStats()->GetCHAMod());
                if (nForcePoints <= GetCurrentForcePoints())
                {
                    bEnoughPoints = 1;
                }
            }
            if (bEnoughPoints &&
                CheckPowerSuitability(nSpell, pSpell, nFilter, nSelection, 0,
                                      nSpellIndex, nClassIndex, nExcludedPower))
            {
                if (nSelection == TALENT_SELECTION_RANDOM)
                {
                    pTalents->Add(nSpell);
                    pClasses->Add((unsigned char)nClassIndex);
                    pSpell->m_sLabel.CStr();
                }
                else if (pSpell->m_nMaxCR > TALENT_INITIAL_MAX_CR)
                {
                    delete pTalents;
                    delete pClasses;
                    pTalents = new CExoArrayList<int>(0);
                    pClasses = new CExoArrayList<unsigned char>(0);
                    pTalents->Add(nSpell);
                    pClasses->Add((unsigned char)nClassIndex);
                    pSpell->m_sLabel.CStr();
                }
                else if (pSpell->m_nMaxCR == TALENT_INITIAL_MAX_CR)
                {
                    pTalents->Add(nSpell);
                    pClasses->Add((unsigned char)nClassIndex);
                    pSpell->m_sLabel.CStr();
                }
            }
        }
    }
    int nTalentCount = pTalents->num;
    if (nTalentCount > 0)
    {
        nSpell = rand() % nTalentCount;
        nTalentType = CScriptTalent::TALENT_TYPE_SPELL;
        bFound = 1;
        nTalent = (*pTalents)[nSpell];
        nClass = (*pClasses)[nSpell];
    }
    delete pTalents;
    delete pClasses;
    return bFound;
}

inline int CSWSCreatureStats::GetNumBonusFeats()
{
    return m_lstBonusFeats.num;
}
// PC 0x005F7770
int CSWSCreatureStats::CanChooseSpell(unsigned long nSpell, unsigned char nMax,
                                   CExoArrayList<unsigned long>* pList, int bNextLevel)
{
    unsigned char nClass = m_nClassCount - 1;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    if (pSpell == NULL)
    {
        return 0;
    }
    else
    {
        if (pSpell->m_nUserType != CSWSpell::USER_TYPE_FORCE_POWER &&
            pSpell->m_nUserType != CSWSpell::USER_TYPE_LEVEL_UP_ALTERNATE)
        {
            return 0;
        }
        if (pSpell->GetSpellLevel(GetClass(nClass)) == CSWSpell::SPELL_LEVEL_INVALID)
        {
            return 0;
        }
        if (nMax == 0)
        {
            return 0;
        }
        if (HasSpell(0, nSpell, 0))
        {
            return 0;
        }
        for (unsigned short j = 0; j < pList->num; j++)
        {
            if (nSpell == pList->element[j])
            {
                return 0;
            }
        }
        if (!HasSpellPrereq(pSpell, pList, bNextLevel))
        {
            return 0;
        }
        return nMax - pList->num > 0;
    }
}
// PC 0x005FAF00
void CSWSCreatureStats::SetForceResistance(char nResistance)
{
    m_nForceResistance = nResistance;
    UpdateCombatInformation();
}
// PC 0x005FAF20
void CSWSCreatureStats::SetForceResistancePenalty(char nPenalty)
{
    m_nForceResistancePenalty = nPenalty;
    UpdateCombatInformation();
}
int CSWSCreatureStats::FeatRequirementsMet(unsigned short nFeat,  // PC 0x005FAF40
                                         CExoArrayList<unsigned short>* pList,
                                         int bNextLevel)
{
    int nLevel;
    if (bNextLevel)
    {
        nLevel = GetLevel(0) + 1;
    }
    else
    {
        nLevel = GetLevel(0);
    }
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
    if (pFeat == NULL)
    {
        return 0;
    }

    CSWClass* pLeveledClass = &g_pRules->m_pClasses[GetClassLeveledUpIn()];
    unsigned char nGrantedLevel = pLeveledClass->GetLevelFeatGranted(nFeat);
    if (nGrantedLevel != CSWClass::FEAT_GRANTED_LEVEL_INVALID &&
        nGrantedLevel > nLevel)
    {
        return 0;
    }

    if (pFeat->m_nMinSpellLevel != 0)
    {
        int bSpellLevelMet = 0;
        unsigned char i = 0;
        while (!bSpellLevelMet)
        {
            if (i >= m_nClassCount)
            {
                break;
            }
            unsigned char nClass = GetClass(i);
            CSWClass* pClass = &g_pRules->m_pClasses[nClass];
            if (pClass->m_bSpellCaster)
            {
                unsigned char nCharisma = m_nCharismaBase;
                unsigned char nSpellLevel = pFeat->m_nMinSpellLevel;
                unsigned short nRace = m_nRace;
                unsigned char nClassLevel = GetClassLevel(i, 0);
                if (pClass->GetSpellsKnownPerLevel(nClassLevel,
                        nSpellLevel, nClass, nRace, nCharisma) != 0)
                {
                    bSpellLevelMet = 1;
                }
            }
            i++;
        }
        if (!bSpellLevelMet)
        {
            return 0;
        }
    }

    unsigned char nMinAttackBonus = pFeat->m_nMinAttackBonus;
    if (nMinAttackBonus > GetBaseAttackBonus())
    {
        return 0;
    }
    if (pFeat->m_nMinStrength > m_nStrengthBase)
    {
        return 0;
    }
    if (pFeat->m_nMinDexterity > m_nDexterityBase)
    {
        return 0;
    }
    if (pFeat->m_nMinIntelligence > m_nIntelligenceBase)
    {
        return 0;
    }
    if (pFeat->m_nMinWisdom > m_nWisdomBase)
    {
        return 0;
    }
    if (!HasFeatPrereq(pFeat, pList))
    {
        return 0;
    }
    unsigned short nSkill = pFeat->m_nRequiredSkill;
    if (nSkill != CSWSkill::SKILL_INVALID && !GetCanUseSkill(nSkill))
    {
        return 0;
    }
    return 1;
}

inline unsigned char CSWSCreatureStats::GetGender()
{
    return m_nGender;
}

inline unsigned char CSWSCreatureStats::GetPhenotype()
{
    return m_nPhenotype;
}

inline unsigned char CSWSCreatureStats::GetAppearanceHead()
{
    return m_nAppearanceHead;
}

inline unsigned char CSWSCreatureStats::GetDuplicatingHead()
{
    return m_nDuplicatingHead;
}

inline int CSWSCreatureStats::GetUseBackupHead()
{
    return m_bUseBackupHead;
}

inline unsigned char CSWSCreatureStats::GetTail()
{
    return m_nTail;
}

inline unsigned char CSWSCreatureStats::GetWings()
{
    return m_nWings;
}

inline unsigned char CSWSCreatureStats::GetSkinColor()
{
    return m_nSkinColor;
}

inline unsigned char CSWSCreatureStats::GetHairColor()
{
    return m_nHairColor;
}

inline unsigned char CSWSCreatureStats::GetTattoo1Color()
{
    return m_nTattoo1Color;
}

inline unsigned char CSWSCreatureStats::GetTattoo2Color()
{
    return m_nTattoo2Color;
}
// PC 0x005FB110
int CSWSCreatureStats::ReadStatsFromGff(CResGFF* pRes, CResStruct* pStruct,
                                     CSWSCreatureAppearanceInfo* pAppearance)
{
    int bSuccess;
    CExoLocString lsName;
    lsName = pRes->ReadFieldCExoLocString(pStruct, "FirstName", bSuccess,
                                         CExoLocString());
    m_lsFirstName = lsName;
    lsName = pRes->ReadFieldCExoLocString(pStruct, "LastName", bSuccess,
                                         CExoLocString());
    m_lsLastName = lsName;
    m_lsDescription = pRes->ReadFieldCExoLocString(pStruct, "Description",
                                                 bSuccess, m_lsDescription);
    m_bIsPC = pRes->ReadFieldBYTE(pStruct, "IsPC", bSuccess, GetIsPC());
    GetCreature()->SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess,
                                                m_pCreature->m_sTag));
    m_cConversation = pRes->ReadFieldCResRef(pStruct, "Conversation", bSuccess,
                                           CResRef(m_cConversation));
    int bInterruptable = m_bInterruptable;
    bInterruptable = pRes->ReadFieldBYTE(pStruct, "Interruptable", bSuccess,
                                        bInterruptable);
    m_bInterruptable = bInterruptable;
    int nAge = m_nAge;
    nAge = pRes->ReadFieldINT(pStruct, "Age", bSuccess, nAge);
    m_nAge = nAge;
    unsigned char nGender = m_nGender;
    nGender = pRes->ReadFieldBYTE(pStruct, "Gender", bSuccess, nGender);
    m_nGender = nGender > GENDER_NONE ? GENDER_NONE : nGender;
    unsigned char nStartingPackage = m_nStartingPackage;
    nStartingPackage = pRes->ReadFieldBYTE(pStruct, "StartingPackage",
                                           bSuccess, nStartingPackage);
    m_nStartingPackage = nStartingPackage;
    unsigned short nRace = m_nRace;
    nRace = pRes->ReadFieldBYTE(pStruct, "Race", bSuccess, nRace);
    if (nRace >= g_pRules->m_nNumRaces)
        return LOAD_STATS_INVALID_RACE;
    m_nRace = nRace;
    m_sSubrace = pRes->ReadFieldCExoString(pStruct, "Subrace", bSuccess, m_sSubrace);
    unsigned char nSubraceIndex = m_nSubraceIndex;
    nSubraceIndex = pRes->ReadFieldBYTE(pStruct, "SubraceIndex", bSuccess,
                                       nSubraceIndex);
    m_nSubraceIndex = nSubraceIndex;
    m_sDeity = pRes->ReadFieldCExoString(pStruct, "Deity", bSuccess, m_sDeity);
    unsigned char nStrength = m_nStrengthBase;
    nStrength = pRes->ReadFieldBYTE(pStruct, "Str", bSuccess, nStrength);
    SetSTRBase(nStrength);
    unsigned char nDexterity = m_nDexterityBase;
    nDexterity = pRes->ReadFieldBYTE(pStruct, "Dex", bSuccess, nDexterity);
    SetDEXBase(nDexterity);
    unsigned char nIntelligence = m_nIntelligenceBase;
    nIntelligence = pRes->ReadFieldBYTE(pStruct, "Int", bSuccess, nIntelligence);
    SetINTBase(nIntelligence);
    unsigned char nWisdom = m_nWisdomBase;
    nWisdom = pRes->ReadFieldBYTE(pStruct, "Wis", bSuccess, nWisdom);
    SetWISBase(nWisdom);
    unsigned char nConstitution = m_nConstitutionBase;
    nConstitution = pRes->ReadFieldBYTE(pStruct, "Con", bSuccess, nConstitution);
    SetCONBase(nConstitution, 0);
    unsigned char nCharisma = m_nCharismaBase;
    nCharisma = pRes->ReadFieldBYTE(pStruct, "Cha", bSuccess, nCharisma);
    SetCHABase(nCharisma);
    m_nACNaturalBase = pRes->ReadFieldBYTE(pStruct, "NaturalAC", bSuccess,
                                         m_nACNaturalBase);
    GetCreature()->m_nSoundSetFile = pRes->ReadFieldWORD(pStruct, "SoundSetFile",
        bSuccess, GetCreature()->GetSoundSetFile());
    if (!GetCreature()->m_bInParty)
        GetCreature()->SetGold(pRes->ReadFieldDWORD(pStruct, "Gold", bSuccess,
                                                 GetCreature()->GetGold()));
    int bInvulnerable = pRes->ReadFieldBYTE(pStruct, "Invulnerable", bSuccess,
                                           GetCreature()->GetPlotObject());
    if (!bSuccess)
        bInvulnerable = pRes->ReadFieldBYTE(pStruct, "Plot", bSuccess,
                                           GetCreature()->GetPlotObject());
    GetCreature()->m_bPlotObject = bInvulnerable;
    int bMin1HP = pRes->ReadFieldBYTE(pStruct, "Min1HP", bSuccess,
                                      GetCreature()->GetMin1HP());
    GetCreature()->m_bMin1HP = bMin1HP;
    int bPartyInteract = pRes->ReadFieldBYTE(pStruct, "PartyInteract", bSuccess,
                                             GetCreature()->GetPartyInteract());
    GetCreature()->m_bPartyInteract = bPartyInteract;
    int bReorienting = !pRes->ReadFieldBYTE(pStruct, "NotReorienting", bSuccess,
                                            !GetCreature()->m_bDialogFaceSpeaker);
    GetCreature()->m_bDialogFaceSpeaker = bReorienting;
    int bDisarmable = pRes->ReadFieldBYTE(pStruct, "Disarmable", bSuccess,
                                          GetCreature()->GetDisarmable());
    GetCreature()->m_nDisarmable = bDisarmable;
    SetExperience(pRes->ReadFieldDWORD(pStruct, "Experience", bSuccess,
                                      GetExperience()), 0);
    unsigned short nPortraitId = pRes->ReadFieldWORD(pStruct, "PortraitId",
        bSuccess, CSWSObject::PORTRAIT_ID_INVALID);
    if (nPortraitId >= CSWSObject::PORTRAIT_ID_RESREF_FIRST)
    {
        GetCreature()->SetPortraitId(nPortraitId);
        SetPortrait(pRes->ReadFieldCResRef(pStruct, "Portrait", bSuccess,
                                           GetPortrait()));
        GetPortrait().GetResRefStr();
    }
    else
        GetCreature()->SetPortraitId(nPortraitId);
    short nGoodEvil = m_nGoodEvilAlignment;
    nGoodEvil = pRes->ReadFieldBYTE(pStruct, "GoodEvil", bSuccess, nGoodEvil);
    m_nGoodEvilAlignment = min(100, max(0, nGoodEvil));
    unsigned char nSkinColor = m_nSkinColor;
    m_nSkinColor = pRes->ReadFieldBYTE(pStruct, "Color_Skin", bSuccess, nSkinColor);
    unsigned char nHairColor = m_nHairColor;
    m_nHairColor = pRes->ReadFieldBYTE(pStruct, "Color_Hair", bSuccess, nHairColor);
    unsigned char nTattoo1Color = m_nTattoo1Color;
    m_nTattoo1Color = pRes->ReadFieldBYTE(pStruct, "Color_Tattoo1", bSuccess,
                                        nTattoo1Color);
    unsigned char nTattoo2Color = m_nTattoo2Color;
    m_nTattoo2Color = pRes->ReadFieldBYTE(pStruct, "Color_Tattoo2", bSuccess,
                                        nTattoo2Color);
    m_nPhenotype = pRes->ReadFieldINT(pStruct, "Phenotype", bSuccess, m_nPhenotype);
    m_nAppearance = pRes->ReadFieldWORD(pStruct, "Appearance_Type", bSuccess,
                                       GetAppearanceType());
    unsigned char nAppearanceHead = m_nAppearanceHead;
    m_nAppearanceHead = pRes->ReadFieldBYTE(pStruct, "Appearance_Head", bSuccess,
                                           nAppearanceHead);
    m_nAppearanceHead = (m_nAppearanceHead == 0) ? 1 : m_nAppearanceHead;
    unsigned char nDuplicatingHead = m_nDuplicatingHead;
    m_nDuplicatingHead = pRes->ReadFieldBYTE(pStruct, "DuplicatingHead", bSuccess,
                                            nDuplicatingHead);
    int bUseBackupHead = m_bUseBackupHead;
    m_bUseBackupHead = pRes->ReadFieldBYTE(pStruct, "UseBackupHead", bSuccess,
                                          bUseBackupHead);
    m_nTail = 0;
    m_nWings = 0;
    int nFactionId = m_nFactionId;
    m_nFactionId = pRes->ReadFieldWORD(pStruct, "FactionID", bSuccess, nFactionId);
    m_fChallengeRating = pRes->ReadFieldFLOAT(pStruct, "ChallengeRating",
                                             bSuccess, GetChallengeRating());
    m_nAIState = pRes->ReadFieldINT(pStruct, "AIState", bSuccess, m_nAIState);
    unsigned char nBodyBag = GetCreature()->m_nBodyBag;
    GetCreature()->m_nBodyBag = pRes->ReadFieldBYTE(pStruct, "BodyBag", bSuccess,
                                                nBodyBag);
    if (GetIsPC())
    {
        float fRange;
        g_pRules->Get2DArrays()->GetRanges()->GetFLOATEntry(
            PERCEPTION_RANGE_PC, "PrimaryRange", &fRange);
        GetCreature()->m_fSpotCheckDistance = fRange;
        g_pRules->Get2DArrays()->GetRanges()->GetFLOATEntry(
            PERCEPTION_RANGE_PC, "SecondaryRange", &fRange);
        GetCreature()->m_fListenCheckDistance = fRange;
    }
    else
    {
        float fRange;
        int nPerceptionRange = pRes->ReadFieldBYTE(pStruct, "PerceptionRange",
            bSuccess, PERCEPTION_RANGE_DEFAULT);
        if (nPerceptionRange == PERCEPTION_RANGE_PC)
            return LOAD_STATS_INVALID_PERCEPTION;
        if (nPerceptionRange == PERCEPTION_RANGE_DEFAULT)
        {
            int nPerceptionDist;
            if (!g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
                    GetAppearanceType(), "PERCEPTIONDIST", &nPerceptionDist))
                return LOAD_STATS_INVALID_PERCEPTION;
            if (!g_pRules->Get2DArrays()->GetRanges()->GetFLOATEntry(
                    nPerceptionDist, "PrimaryRange", &fRange))
                return LOAD_STATS_INVALID_PERCEPTION;
            GetCreature()->m_fSpotCheckDistance = fRange;
            if (!g_pRules->Get2DArrays()->GetRanges()->GetFLOATEntry(
                    nPerceptionDist, "SecondaryRange", &fRange))
                return LOAD_STATS_INVALID_PERCEPTION;
            GetCreature()->m_fListenCheckDistance = fRange;
        }
        else
        {
            if (!g_pRules->Get2DArrays()->GetRanges()->GetFLOATEntry(
                    nPerceptionRange, "PrimaryRange", &fRange))
                return LOAD_STATS_INVALID_PERCEPTION;
            GetCreature()->m_fSpotCheckDistance = fRange;
            if (!g_pRules->Get2DArrays()->GetRanges()->GetFLOATEntry(
                    nPerceptionRange, "SecondaryRange", &fRange))
                return LOAD_STATS_INVALID_PERCEPTION;
            GetCreature()->m_fListenCheckDistance = fRange;
        }
    }
    CResList lClassList;
    if (pRes->GetList(&lClassList, pStruct, "ClassList"))
    {
        CResStruct cClassStruct;
        CResList lSpellsPerDayList;
        if (pRes->GetListCount(&lClassList) == 0)
            return LOAD_STATS_EMPTY_CLASS_LIST;
        m_nClassCount = pRes->GetListCount(&lClassList);
        if (m_nClassCount > MAX_CLASSES)
            m_nClassCount = MAX_CLASSES;
        for (unsigned char nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
        {
            if (pRes->GetListElement(&cClassStruct, &lClassList, nClassIndex))
            {
                int nClass = pRes->ReadFieldINT(&cClassStruct, "Class", bSuccess,
                                               GetClass(nClassIndex));
                if (bSuccess == 1 && nClass != -1)
                {
                    if (nClass >= g_pRules->m_nNumClasses)
                        return LOAD_STATS_INVALID_CLASS;
                    SetClass(nClassIndex, nClass);
                }
                if (GetClass(nClassIndex) != CSWClass::CCLASS_INVALID)
                {
                    SetClassLevel(nClassIndex, pRes->ReadFieldSHORT(&cClassStruct,
                        "ClassLevel", bSuccess, GetClassLevel(nClassIndex, 0)));
                    m_aClassInfo[nClassIndex].m_nNegativeLevels = 0;
                    if (CSWClass::IsJedi(GetClass(nClassIndex)) &&
                        pRes->GetList(&lSpellsPerDayList, &cClassStruct,
                                      "SpellsPerDayList") == 1)
                    {
                        CResStruct cSpellsPerDayStruct;
                        for (unsigned char nSpellLevel = 0;
                             nSpellLevel < pRes->GetListCount(&lSpellsPerDayList);
                             ++nSpellLevel)
                        {
                            pRes->GetListElement(&cSpellsPerDayStruct,
                                &lSpellsPerDayList, nSpellLevel);
                            unsigned char nSpells = pRes->ReadFieldBYTE(
                                &cSpellsPerDayStruct, "NumSpellsLeft", bSuccess, 0);
                            if (nSpellLevel < CSWSCreatureStats_ClassInfo::NUM_SPELL_LEVELS)
                            {
                                m_aClassInfo[nClassIndex].m_nSpellsPerDayLeft[nSpellLevel] = nSpells;
                                m_aClassInfo[nClassIndex].m_nAdditionalSpellCounts[nSpellLevel] = nSpells;
                            }
                        }
                    }
                }
            }
        }

        if (m_nClassCount > 2 && GetClass(2) == CSWClass::CCLASS_INVALID)
            m_nClassCount = 2;
        if (m_nClassCount > 1 && GetClass(1) == CSWClass::CCLASS_INVALID)
            m_nClassCount = 1;
    }
    else if (GetClassLevel(0, 0) == 0)
        return LOAD_STATS_MISSING_CLASS;
    if ((m_nClassCount > 1 && GetClass(0) == GetClass(1)) ||
        (m_nClassCount > 2 && (GetClass(0) == GetClass(2) ||
                              GetClass(1) == GetClass(2))))
        return LOAD_STATS_INVALID_CLASS;
    m_nWillSaveBonus = pRes->ReadFieldSHORT(pStruct, "willbonus", bSuccess,
                                          m_nWillSaveBonus);
    m_nFortSaveBonus = pRes->ReadFieldSHORT(pStruct, "fortbonus", bSuccess,
                                          m_nFortSaveBonus);
    m_nReflexSaveBonus = pRes->ReadFieldSHORT(pStruct, "refbonus", bSuccess,
                                            m_nReflexSaveBonus);
    CResList lLevelList, lKnownList, lSkillList, lFeatList;
    if (GetIsPC())
    {
        CResStruct cLevelStruct;
        pRes->GetList(&lLevelList, pStruct, "LvlStatList");
        for (unsigned char nLevel = 0; nLevel < GetLevel(0); ++nLevel)
        {
            CSWLevelStats* pLevelStats;
            if (nLevel > 0)
            {
                pLevelStats = new CSWLevelStats;
                m_lstLevelStats.Add(pLevelStats);
            }
            else
                pLevelStats = GetLevelStats(nLevel);
            if (pRes->GetListElement(&cLevelStruct, &lLevelList, nLevel))
            {
                pLevelStats->m_nAbilityIncrease = pRes->ReadFieldBYTE(&cLevelStruct,
                    "LvlStatAbility", bSuccess, pLevelStats->GetAbilityIncrease());
                pLevelStats->m_nHitDie = pRes->ReadFieldBYTE(&cLevelStruct,
                    "LvlStatHitDie", bSuccess, pLevelStats->GetHitDie());
                pLevelStats->m_nForceDie = pRes->ReadFieldBYTE(&cLevelStruct,
                    "LvlStatForce", bSuccess, pLevelStats->GetForceDie());
                pLevelStats->m_nClass = pRes->ReadFieldBYTE(&cLevelStruct,
                    "LvlStatClass", bSuccess, pLevelStats->GetClass());
                for (unsigned char nSpellLevel = 0; nSpellLevel < 1; ++nSpellLevel)
                {
                    CExoString sKnownList;
                    sKnownList.Format("%s%d", "KnownList", nSpellLevel);
                    if (pRes->GetList(&lKnownList, &cLevelStruct, (char*)sKnownList.CStr()))
                    {
                        CResStruct cSpellStruct;
                        for (unsigned short nSpell = 0;
                             nSpell < pRes->GetListCount(&lKnownList); ++nSpell)
                        {
                            if (pRes->GetListElement(&cSpellStruct, &lKnownList, nSpell))
                            {
                                int nSpellId = pRes->ReadFieldWORD(&cSpellStruct,
                                    "Spell", bSuccess, (unsigned short)CSWSpell::SPELL_INVALID);
                                if (nSpellId != CSWSpell::SPELL_INVALID)
                                    pLevelStats->m_lstSpellsAdded[nSpellLevel].Add(nSpellId);
                            }
                        }
                    }
                    sKnownList.Format("%s%d", "KnownRemoveList", nSpellLevel);
                    if (pRes->GetList(&lKnownList, &cLevelStruct, (char*)sKnownList.CStr()))
                    {
                        CResStruct cSpellStruct;
                        for (unsigned short nSpell = 0;
                             nSpell < pRes->GetListCount(&lKnownList); ++nSpell)
                        {
                            if (pRes->GetListElement(&cSpellStruct, &lKnownList, nSpell))
                            {
                                int nSpellId = pRes->ReadFieldWORD(&cSpellStruct,
                                    "Spell", bSuccess, (unsigned short)CSWSpell::SPELL_INVALID);
                                if (nSpellId != CSWSpell::SPELL_INVALID)
                                    pLevelStats->m_lstSpellsRemoved[nSpellLevel].Add(nSpellId);
                            }
                        }
                    }
                }
                pLevelStats->m_nSkillPoints = pRes->ReadFieldWORD(&cLevelStruct,
                    "SkillPoints", bSuccess, GetSkillPoints());
                if (pRes->GetList(&lSkillList, &cLevelStruct, "SkillList"))
                {
                    CResStruct cLevelSkillStruct;
                    memset(m_pSkillRanks, 0, CSWSkill::NUM_SKILLS);
                    for (unsigned short nSkill = 0; nSkill < CSWSkill::NUM_SKILLS; ++nSkill)
                    {
                        if (pRes->GetListElement(&cLevelSkillStruct, &lSkillList, nSkill))
                            pLevelStats->SetSkillRankChange(nSkill,
                                pRes->ReadFieldBYTE(&cLevelSkillStruct, "Rank", bSuccess,
                                    pLevelStats->GetSkillRankChange(nSkill)));
                    }
                }
                if (pRes->GetList(&lFeatList, &cLevelStruct, "FeatList"))
                {
                    CResStruct cFeatStruct;
                    unsigned short nCount = pRes->GetListCount(&lFeatList);
                    for (unsigned short nFeat = 0; nFeat < nCount; ++nFeat)
                    {
                        if (pRes->GetListElement(&cFeatStruct, &lFeatList, nFeat))
                        {
                            unsigned short nFeatId = pRes->ReadFieldWORD(
                                &cFeatStruct, "Feat", bSuccess, 0);
                            if (bSuccess)
                                pLevelStats->AddFeat(nFeatId);
                        }
                    }
                }
            }
        }
    }
    short nHitPoints = GetCreature()->m_nMaxHitPoints;
    nHitPoints = pRes->ReadFieldSHORT(pStruct, "HitPoints", bSuccess, nHitPoints);
    GetCreature()->m_nMaxHitPoints = nHitPoints;
    short nForcePoints = GetCreature()->GetStats()->m_nForcePoints;
    nForcePoints = pRes->ReadFieldSHORT(pStruct, "ForcePoints", bSuccess, nForcePoints);
    CSWSCreatureStats* pStats = GetCreature()->m_pStats;
    pStats->m_nForcePoints = nForcePoints;
    short nCurrentHitPoints = GetCreature()->m_nMaxHitPoints;
    nCurrentHitPoints = pRes->ReadFieldSHORT(pStruct, "CurrentHitPoints", bSuccess,
                                             nCurrentHitPoints);
    int nDefaultForce = GetCreature()->GetStats()->m_nForcePoints;
    short nCurrentForce = pRes->ReadFieldSHORT(pStruct, "CurrentForce", bSuccess, nDefaultForce);
    if (GetIsPC())
    {
        short nBaseHitPoints = GetCreature()->m_nMaxHitPoints;
        nCurrentHitPoints += GetCreature()->GetMaxHitPoints(1) - nBaseHitPoints;
        GetCreature()->SetCurrentHitPoints(nCurrentHitPoints);
        short nBaseForcePoints = GetCreature()->GetStats()->m_nForcePoints;
        nCurrentForce += GetCreature()->GetMaxForcePoints() - nBaseForcePoints;
        m_nCurrentForcePoints = nCurrentForce;
    }
    else
    {
        if (nCurrentHitPoints + GetCONMod() * GetLevel(0) < GetLevel(0))
            GetCreature()->SetCurrentHitPoints(GetLevel(0));
        else
            GetCreature()->SetCurrentHitPoints(nCurrentHitPoints + GetCONMod() * GetLevel(0));
        char nWIS = GetWISMod();
        char nCHA = GetCHAMod();
        int nForce = nCurrentForce;
        if (nForce + (nWIS + nCHA) * GetLevel(0) < GetLevel(0))
        {
            pStats = GetCreature()->GetStats();
            nDefaultForce = GetLevel(0);
            pStats->m_nCurrentForcePoints = nDefaultForce;
        }
        else
        {
            pStats = GetCreature()->GetStats();
            nDefaultForce = nForce + (GetWISMod() + GetCHAMod()) * GetLevel(0);
            pStats->m_nCurrentForcePoints = nDefaultForce;
        }
        if (nCurrentHitPoints < nHitPoints)
        {
            if (nHitPoints + GetCONMod() * GetLevel(0) <= GetLevel(0))
                GetCreature()->SetCurrentHitPoints(nCurrentHitPoints -
                                                 nHitPoints + GetLevel(0));
            else
                GetCreature()->SetCurrentHitPoints(nCurrentHitPoints +
                                                 GetCONMod() * GetLevel(0));
        }
        else
        {
            if (nHitPoints + GetCONMod() * GetLevel(0) <= GetLevel(0))
                GetCreature()->SetCurrentHitPoints(GetLevel(0));
            else
                GetCreature()->SetCurrentHitPoints(nCurrentHitPoints +
                                                 GetCONMod() * GetLevel(0));
        }
        m_nCurrentForcePoints =
            (nForce + GetForcePointAbilityMod() * GetLevel(0) < GetLevel(0)) ? GetLevel(0) :
            nForce + GetForcePointAbilityMod() * GetLevel(0);
        if (nCurrentForce < nForcePoints)
        {
            int nFP = nForcePoints;
            if (nFP + GetForcePointAbilityMod() * GetLevel(0) <= GetLevel(0))
            {
                nFP = nForce - nFP + GetLevel(0);
                m_nCurrentForcePoints = nFP;
            }
            else
            {
                nFP = nForce + GetForcePointAbilityMod() * GetLevel(0);
                m_nCurrentForcePoints = nFP;
            }
        }
        else
        {
            m_nCurrentForcePoints =
                (nForcePoints + GetForcePointAbilityMod() * GetLevel(0) <= GetLevel(0)) ? GetLevel(0) :
                nForce + GetForcePointAbilityMod() * GetLevel(0);
        }
    }
    short nOldMaxHitPoints = GetCreature()->GetMaxHitPoints(1);
    short nOldMaxForcePoints = GetCreature()->GetMaxForcePoints();
    if (pRes->GetList(&lFeatList, pStruct, "FeatList"))
    {
        CResStruct cFeatStruct;
        unsigned short nCount = pRes->GetListCount(&lFeatList);
        for (unsigned short nFeat = 0; nFeat < nCount; ++nFeat)
        {
            if (pRes->GetListElement(&cFeatStruct, &lFeatList, nFeat))
            {
                unsigned short nFeatId = pRes->ReadFieldWORD(&cFeatStruct,
                    "Feat", bSuccess, 0);
                if (bSuccess)
                    AddFeat(nFeatId);
            }
        }
    }
    short nMaxHitPoints = GetCreature()->GetMaxHitPoints(1);
    short nMaxForcePoints = GetCreature()->GetMaxForcePoints();
    if (nOldMaxHitPoints != nMaxHitPoints)
        GetCreature()->SetCurrentHitPoints(nMaxHitPoints - nOldMaxHitPoints +
                                         GetCreature()->GetCurrentHitPoints(1));
    if (nOldMaxForcePoints != nMaxForcePoints)
        m_nCurrentForcePoints += nMaxForcePoints - nOldMaxForcePoints;
    CResList lPerceptionList;
    if (pRes->GetList(&lPerceptionList, pStruct, "PerceptionList"))
    {
        CResStruct cPerceptionStruct;
        int nCount = pRes->GetListCount(&lPerceptionList);
        for (int nPerception = 0; nPerception < nCount; ++nPerception)
        {
            if (pRes->GetListElement(&cPerceptionStruct, &lPerceptionList, nPerception))
            {
                unsigned long oidObject = pRes->ReadFieldDWORD(&cPerceptionStruct,
                    "ObjectId", bSuccess, 0);
                unsigned char nData = pRes->ReadFieldBYTE(&cPerceptionStruct,
                    "PerceptionData", bSuccess, 0);
                GetCreature()->AddToVisibleList(oidObject,
                    (nData & PERCEPTION_DATA_SEEN) != 0,
                    (nData & PERCEPTION_DATA_HEARD) != 0,
                    (nData & PERCEPTION_DATA_INAUDIBLE) != 0,
                    (nData & PERCEPTION_DATA_VANISHED) != 0);
            }
        }
    }
    CResStruct cCombatRoundStruct;
    if (pRes->GetStructFromStruct(&cCombatRoundStruct, pStruct, "CombatRoundData") == 1)
        GetCreature()->GetCombatRound()->LoadCombatRound(pRes, &cCombatRoundStruct);
    pAppearance->m_nAppearance = GetAppearanceType();
    pAppearance->m_nAppearancePart1 = GetGender();
    pAppearance->m_nAppearancePart0 = GetPhenotype();
    pAppearance->m_nAppearancePart6 = GetAppearanceHead();
    pAppearance->m_nAppearanceHead = GetDuplicatingHead();
    pAppearance->m_bUseBackupHead = GetUseBackupHead();
    pAppearance->m_nAppearancePart7 = GetTail();
    pAppearance->m_nAppearancePart8 = GetWings();
    pAppearance->m_nAppearancePart2 = GetSkinColor();
    pAppearance->m_nAppearancePart3 = GetHairColor();
    pAppearance->m_nAppearancePart4 = GetTattoo1Color();
    pAppearance->m_nAppearancePart5 = GetTattoo2Color();
    m_nSkillPoints = pRes->ReadFieldWORD(pStruct, "SkillPoints", bSuccess, GetSkillPoints());
    if (pRes->GetList(&lSkillList, pStruct, "SkillList"))
    {
        CResStruct cSkillStruct;
        memset(m_pSkillRanks, 0, CSWSkill::NUM_SKILLS);
        for (unsigned char nSkill = 0; nSkill < CSWSkill::NUM_SKILLS; ++nSkill)
        {
            if (pRes->GetListElement(&cSkillStruct, &lSkillList, nSkill))
            {
                char nRank = pRes->ReadFieldBYTE(&cSkillStruct, "Rank", bSuccess,
                                                GetSkillRank(nSkill, NULL, 0));
                if (nSkill < g_pRules->m_nNumSkills)
                    m_pSkillRanks[nSkill] = nRank;
            }
        }
    }
    SetMovementRate(pRes->ReadFieldBYTE(pStruct, "MovementRate", bSuccess,
                                       GetMovementRate()));
    if (!bSuccess)
        SetMovementRate(pRes->ReadFieldINT(pStruct, "WalkRate", bSuccess,
                                          GetMovementRate()));
    CResList lSpecialAbilityList;
    CResStruct cSpecialAbilityStruct;
    if (pRes->GetList(&lSpecialAbilityList, pStruct, "SpecAbilityList") &&
        (unsigned char)pRes->GetListCount(&lSpecialAbilityList) != 0)
    {
        unsigned char nCount = pRes->GetListCount(&lSpecialAbilityList);
        if (nCount > 0)
        {
            for (int nAbility = 0; nAbility < nCount; ++nAbility)
            {
                if (pRes->GetListElement(&cSpecialAbilityStruct, &lSpecialAbilityList, nAbility))
                {
                    int nSpell = pRes->ReadFieldWORD(&cSpecialAbilityStruct,
                        "Spell", bSuccess, 0);
                    int nFlags = pRes->ReadFieldBYTE(&cSpecialAbilityStruct,
                        "SpellFlags", bSuccess, 0);
                    int nLevel = pRes->ReadFieldBYTE(&cSpecialAbilityStruct,
                        "SpellCasterLevel", bSuccess, 0);
                    AddSpellLikeAbilityToList(nSpell, nFlags, nLevel);
                }
            }
        }
    }
    CCombatInformation* pCombatInformation = m_pCombatInformation;
    pCombatInformation->LoadData(pRes, pStruct);
    return 0;
}
// PC 0x005FD2A0
void CSWSCreatureStats::SaveStats(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldCExoLocString(pStruct, m_lsFirstName, "FirstName");
    pRes->WriteFieldCExoLocString(pStruct, m_lsLastName, "LastName");
    pRes->WriteFieldCExoLocString(pStruct, m_lsDescription, "Description");
    pRes->WriteFieldBYTE(pStruct, GetIsPC(), "IsPC");
    pRes->WriteFieldCExoString(pStruct, m_pCreature->m_sTag, "Tag");
    CResRef cConversation(m_cConversation);
    pRes->WriteFieldCResRef(pStruct, cConversation, "Conversation");
    int bInterruptable = m_bInterruptable;
    pRes->WriteFieldBYTE(pStruct, bInterruptable, "Interruptable");
    int nAge = m_nAge;
    pRes->WriteFieldINT(pStruct, nAge, "Age");
    unsigned char nGender = m_nGender;
    pRes->WriteFieldBYTE(pStruct, nGender, "Gender");
    unsigned short nRace = m_nRace;
    pRes->WriteFieldBYTE(pStruct, nRace, "Race");
    pRes->WriteFieldCExoString(pStruct, m_sSubrace, "Subrace");
    unsigned char nSubraceIndex = m_nSubraceIndex;
    pRes->WriteFieldBYTE(pStruct, nSubraceIndex, "SubraceIndex");
    unsigned char nStartingPackage = m_nStartingPackage;
    pRes->WriteFieldBYTE(pStruct, nStartingPackage, "StartingPackage");
    pRes->WriteFieldCExoString(pStruct, m_sDeity, "Deity");
    pRes->WriteFieldBYTE(pStruct, GetNumClasses() - 1, "MClassLevUpIn");
    char nWillSaveBonus = m_nWillSaveBonus;
    pRes->WriteFieldSHORT(pStruct, nWillSaveBonus, "willbonus");
    char nFortSaveBonus = m_nFortSaveBonus;
    pRes->WriteFieldSHORT(pStruct, nFortSaveBonus, "fortbonus");
    char nReflexSaveBonus = m_nReflexSaveBonus;
    pRes->WriteFieldSHORT(pStruct, nReflexSaveBonus, "refbonus");
    pRes->WriteFieldDWORD(pStruct, m_pCreature->GetGold(), "Gold");
    pRes->WriteFieldCHAR(pStruct, GetReflexSavingThrow(0), "RefSaveThrow");
    pRes->WriteFieldCHAR(pStruct, GetWillSavingThrow(0), "WillSaveThrow");
    pRes->WriteFieldCHAR(pStruct, GetFortSavingThrow(0), "FortSaveThrow");
    pRes->WriteFieldSHORT(pStruct, m_pCreature->GetArmorClass(), "ArmorClass");
    unsigned char nStrength = m_nStrengthBase;
    pRes->WriteFieldBYTE(pStruct, nStrength, "Str");
    unsigned char nDexterity = m_nDexterityBase;
    pRes->WriteFieldBYTE(pStruct, nDexterity, "Dex");
    unsigned char nIntelligence = m_nIntelligenceBase;
    pRes->WriteFieldBYTE(pStruct, nIntelligence, "Int");
    unsigned char nWisdom = m_nWisdomBase;
    pRes->WriteFieldBYTE(pStruct, nWisdom, "Wis");
    unsigned char nConstitution = m_nConstitutionBase;
    pRes->WriteFieldBYTE(pStruct, nConstitution, "Con");
    unsigned char nCharisma = m_nCharismaBase;
    pRes->WriteFieldBYTE(pStruct, nCharisma, "Cha");
    pRes->WriteFieldBYTE(pStruct, m_nACNaturalBase, "NaturalAC");
    unsigned short nSoundSetFile = GetCreature()->m_nSoundSetFile;
    pRes->WriteFieldWORD(pStruct, nSoundSetFile, "SoundSetFile");
    pRes->WriteFieldBYTE(pStruct, GetCreature()->GetPlotObject(), "Plot");
    int bDisarmable = GetCreature()->m_nDisarmable;
    pRes->WriteFieldBYTE(pStruct, bDisarmable, "Disarmable");
    pRes->WriteFieldBYTE(pStruct, GetCreature()->GetMin1HP(), "Min1HP");
    int bPartyInteract = GetCreature()->m_bPartyInteract;
    pRes->WriteFieldBYTE(pStruct, bPartyInteract, "PartyInteract");
    int bReorienting = GetCreature()->m_bDialogFaceSpeaker;
    pRes->WriteFieldBYTE(pStruct, !bReorienting, "NotReorienting");
    unsigned char nBodyBag = GetCreature()->m_nBodyBag;
    pRes->WriteFieldBYTE(pStruct, nBodyBag, "BodyBag");
    short nBaseHitPoints = GetCreature()->CSWSObject::GetMaxHitPoints(1);
    pRes->WriteFieldSHORT(pStruct, nBaseHitPoints, "HitPoints");
    short nCurrentHitPoints = GetCreature()->GetCurrentHitPoints(1) -
        (GetCreature()->GetMaxHitPoints(1) - GetCreature()->CSWSObject::GetMaxHitPoints(1));
    pRes->WriteFieldSHORT(pStruct, nCurrentHitPoints, "CurrentHitPoints");
    pRes->WriteFieldSHORT(pStruct, m_pCreature->GetMaxHitPoints(1), "MaxHitPoints");
    pRes->WriteFieldSHORT(pStruct, m_pCreature->GetCurrentHitPoints(1), "PregameCurrent");
    short nBaseForcePoints = GetCreature()->GetStats()->m_nForcePoints;
    pRes->WriteFieldSHORT(pStruct, nBaseForcePoints, "ForcePoints");
    CSWSCreatureStats* pStats = GetCreature()->GetStats();
    unsigned short nForcePoints = GetCreature()->GetStats()->m_nForcePoints;
    short nMaxForcePoints = m_pCreature->GetMaxForcePoints();
    unsigned short nCurrentForcePoints = nForcePoints + pStats->m_nCurrentForcePoints;
    pRes->WriteFieldSHORT(pStruct, nCurrentForcePoints - nMaxForcePoints, "CurrentForce");
    pRes->WriteFieldSHORT(pStruct, m_pCreature->GetMaxForcePoints(), "MaxForcePoints");
    pRes->WriteFieldDWORD(pStruct, GetExperience(), "Experience");
    int nMovementRate = m_nMovementRate;
    pRes->WriteFieldBYTE(pStruct, nMovementRate, "MovementRate");
    if (m_pCreature->GetPortraitId() == CSWSObject::PORTRAIT_ID_INVALID)
        pRes->WriteFieldCResRef(pStruct, m_pCreature->GetPortrait(), "Portrait");
    else
        pRes->WriteFieldWORD(pStruct, m_pCreature->GetPortraitId(), "PortraitId");
    short nGoodEvil = m_nGoodEvilAlignment;
    pRes->WriteFieldBYTE(pStruct, nGoodEvil, "GoodEvil");
    unsigned char nSkinColor = m_nSkinColor;
    pRes->WriteFieldBYTE(pStruct, nSkinColor, "Color_Skin");
    unsigned char nHairColor = m_nHairColor;
    pRes->WriteFieldBYTE(pStruct, nHairColor, "Color_Hair");
    unsigned char nTattoo1Color = m_nTattoo1Color;
    pRes->WriteFieldBYTE(pStruct, nTattoo1Color, "Color_Tattoo1");
    unsigned char nTattoo2Color = m_nTattoo2Color;
    pRes->WriteFieldBYTE(pStruct, nTattoo2Color, "Color_Tattoo2");
    unsigned char nPhenotype = m_nPhenotype;
    pRes->WriteFieldINT(pStruct, nPhenotype, "Phenotype");
    pRes->WriteFieldWORD(pStruct, GetAppearanceType(), "Appearance_Type");
    unsigned char nAppearanceHead = m_nAppearanceHead;
    pRes->WriteFieldBYTE(pStruct, nAppearanceHead, "Appearance_Head");
    char nDuplicatingHead = m_nDuplicatingHead;
    pRes->WriteFieldBYTE(pStruct, nDuplicatingHead, "DuplicatingHead");
    int bUseBackupHead = m_bUseBackupHead;
    pRes->WriteFieldBYTE(pStruct, bUseBackupHead, "UseBackupHead");
    unsigned char nTail = m_nTail;
    pRes->WriteFieldBYTE(pStruct, nTail, "Tail");
    unsigned char nWings = m_nWings;
    pRes->WriteFieldBYTE(pStruct, nWings, "Wings");
    int nFactionId = m_nFactionId;
    pRes->WriteFieldWORD(pStruct, nFactionId, "FactionID");
    pRes->WriteFieldFLOAT(pStruct, GetChallengeRating(), "ChallengeRating");
    pRes->WriteFieldINT(pStruct, m_nAIState, "AIState");
    SaveClassInfo(pRes, pStruct);

    CExoString sWeaponName;
    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
        m_pCombatInformation->GetLeftEquipment());
    if (pItem)
    {
        CExoString* psLeftString = &m_pCombatInformation->m_sLeftString;
        pItem->m_sLocalizedName.GetString(0, psLeftString, 0);
    }
    CCombatInformation* pCombatInformation = m_pCombatInformation;
    pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
        pCombatInformation->GetRightEquipment());
    if (pItem)
    {
        CExoString* psRightString = &m_pCombatInformation->m_sRightString;
        pItem->m_sLocalizedName.GetString(0, psRightString, 0);
        CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType());
        if (pBaseItem)
        {
            m_pCombatInformation->SetDamageDice(pBaseItem->m_nNumDice);
            m_pCombatInformation->SetDamageDie(pBaseItem->m_nDieToRoll);
        }
    }
    m_pCombatInformation->SaveData(pRes, pStruct);
}
// PC 0x005FDC20
int CSWSCreatureStats::GetCreatureTalentRandom(
    int nFilter, int nSelection, int& nTalentType, int& nTalent,
    unsigned char& nClass, unsigned long& oidItem, int& nItemProperty,
    unsigned char& nCasterLevel, int nExclusion, int nExcludedTalentType,
    int nExcludedTalent)
{
    nTalentType = INVALID_TALENT;
    nTalent = INVALID_TALENT;
    nClass = INVALID_TALENT_CLASS;
    oidItem = OBJECT_INVALID;
    nItemProperty = INVALID_TALENT_ITEM_PROPERTY;
    nCasterLevel = INVALID_TALENT_CASTER_LEVEL;

    int bFound = 0;
    int bPowersExhausted = 0;
    int bFeatsExhausted = 0;
    while (!bFound && (!bPowersExhausted || !bFeatsExhausted))
    {
        int nChoice = rand() % 2;
        if (nChoice == 0)
        {
            if (!bPowersExhausted)
            {
                int nExcludedPower;
                if (nExcludedTalentType == CScriptTalent::TALENT_TYPE_SPELL)
                    nExcludedPower = nExcludedTalent;
                else
                    nExcludedPower = INVALID_TALENT;

                if (GetCreatureTalentRandomSpecialAbility(
                        nFilter, nSelection, nTalentType, nTalent, nClass,
                        nCasterLevel, nExclusion, nExcludedPower) ||
                    GetCreatureTalentRandomPower(
                        nFilter, nSelection, nTalentType, nTalent, nClass,
                        nExclusion, nExcludedPower) ||
                    GetCreatureTalentRandomPowerFromItem(
                        nFilter, nSelection, nTalentType, nTalent, nClass,
                        oidItem, nItemProperty, nExclusion, nExcludedPower))
                {
                    bFound = 1;
                    break;
                }
                bPowersExhausted = 1;
            }
        }
        else if (nChoice == 1)
        {
            if (!bFeatsExhausted)
            {
                int nExcludedFeat;
                if (nExcludedTalentType == CScriptTalent::TALENT_TYPE_FEAT)
                {
                    nExcludedFeat = nExcludedTalent;
                    if (nExcludedTalent == INVALID_TALENT)
                        bPowersExhausted = 1;
                }
                else
                    nExcludedFeat = INVALID_TALENT;

                if (GetCreatureTalentRandomFeat(
                        nFilter, nSelection, nTalentType, nTalent, nClass,
                        nExclusion, nExcludedFeat))
                {
                    bFound = 1;
                    break;
                }
                bFeatsExhausted = 1;
            }
        }
    }
    if (!bFound)
    {
        nTalentType = INVALID_TALENT;
        nTalent = INVALID_TALENT;
    }
    return bFound;
}
// PC 0x005FDDB0
int CSWSCreatureStats::CanChooseFeat(unsigned short nFeat, unsigned char nNormal,
                                  unsigned char nBonus,
                                  CExoArrayList<unsigned short>* pList,
                                  int bNextLevel)
{
    int bNormal = 0;
    int bBonus = 0;
    if (nFeat == 207 || nFeat == 238)
    {
        return 0;
    }
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
    if (pFeat == NULL)
    {
        return 0;
    }
    if (nNormal == 0 && nBonus == 0)
    {
        return 0;
    }
    if ((m_pCreature->m_sTag.CompareNoCase("baodur") && pFeat->m_bBaoDur == 0xFF) ||
        (m_pCreature->m_sTag.CompareNoCase("kreia") && pFeat->m_bKreia == 0xFF))
    {
        return 0;
    }
    int nNumFeats = m_lstFeats.num;
    for (unsigned short i = 0; i < (unsigned short)nNumFeats; i++)
    {
        if (nFeat == GetFeat(i))
        {
            return 0;
        }
    }
    for (unsigned short j = 0; j < pList->num; j++)
    {
        if (nFeat == pList->element[j])
        {
            return 0;
        }
    }
    if (!FeatRequirementsMet(nFeat, pList, bNextLevel))
    {
        return 0;
    }
    SetNormalBonusFlags(nFeat, bNormal, bBonus);
    if (!bNormal && !bBonus)
    {
        return 0;
    }

    int bAssigned = 0;
    unsigned char nNormalLeft = nNormal;
    unsigned char nBonusLeft = nBonus;
    CExoArrayList<int> lstAssigned;
    lstAssigned.SetSize(pList->num);
    for (int k = 0; k < pList->num; k++)
    {
        lstAssigned[k] = 0;
    }
    for (unsigned short l = 0; l < pList->num; l++)
    {
        SetNormalBonusFlags(pList->element[l], bNormal, bBonus);
        if (bNormal && !bBonus)
        {
            if (nNormalLeft == 0)
            {
                return 0;
            }
            nNormalLeft--;
            lstAssigned[l] = 1;
        }
        else if (!bNormal && bBonus && nBonusLeft)
        {
            if (nBonusLeft == 0)
            {
                return 0;
            }
            nBonusLeft--;
            lstAssigned[l] = 1;
        }
    }
    SetNormalBonusFlags(nFeat, bNormal, bBonus);
    if (bNormal && !bBonus)
    {
        if (nNormalLeft == 0)
        {
            return 0;
        }
        nNormalLeft--;
        bAssigned = 1;
    }
    if (!bNormal && bBonus)
    {
        if (nBonusLeft == 0)
        {
            return 0;
        }
        nBonusLeft--;
        bAssigned = 1;
    }
    for (unsigned short m = 0; m < pList->num; m++)
    {
        if (!lstAssigned[m])
        {
            if (nNormalLeft)
            {
                nNormalLeft--;
            }
            else if (nBonusLeft)
            {
                nBonusLeft--;
            }
            else
            {
                return 0;
            }
        }
    }
    if (!bAssigned && !nNormalLeft && !nBonusLeft)
    {
        return 0;
    }
    return 1;
}
// PC 0x005FE090
void CSWSCreatureStats::AutoLevelup(int bFeedback)
{
    CExoArrayList<unsigned short> lstFeats;
    CExoArrayList<unsigned long> lstSpells;
    unsigned char nMultiClass = GetNumClasses() - 1;
    unsigned char nClass = GetClass(nMultiClass);
    CSWClass* pClass = &g_pRules->m_pClasses[nClass];

    while (GetExperience() >= (unsigned long)g_pRules->GetExperienceForLevel(GetLevel(0)))
    {
        unsigned char nClassLevel = GetClassLevel(nMultiClass, 0) + 1;
        CSWLevelStats* pLevelStats = new CSWLevelStats;
        pLevelStats->m_nClass = nClass;
        if (nClassLevel % 4 == 0)
        {
            pLevelStats->m_nAbilityIncrease = pClass->m_nPrimaryAbility;
        }
        pLevelStats->m_nHitDie = pClass->GetHitDie();
        pLevelStats->m_nForceDie = pClass->GetForceDie();

        int nSkillPoints = m_nSkillPoints;
        if (nClassLevel == 1)
        {
            nSkillPoints += 4 * max(GetINTMod() + pClass->GetSkillPointBase() / 2, 1);
        }
        else
        {
            nSkillPoints += max((pClass->GetSkillPointBase() + GetINTMod()) / 2, 1);
        }

        int nRecommended = 1;
        int bPlayer = GetIsPC() &&
            g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nControlledNPC == -1;
        while (nSkillPoints > 0)
        {
            if (nRecommended > g_pRules->m_nNumSkills)
                break;
            int nSkill = pClass->GetRecommendedSkill(nRecommended);
            if (nSkill != -1)
            {
                unsigned char nMaxRank = nClassLevel + 3;
                CSWSkill* pSkill = &g_pRules->m_pSkills[(unsigned short)nSkill];
                if (pSkill != NULL &&
                    (pSkill->m_bAllClassesCanUse || pClass->IsSkillUseable(nSkill)))
                {
                    int bClassSkill;
                    if (pClass->IsSkillClassSkill(nSkill))
                        bClassSkill = 1;
                    else
                    {
                        bClassSkill = 0;
                        nMaxRank /= 2;
                    }
                    int nRanks = nMaxRank - GetSkillRank(nSkill, NULL, 1);
                    unsigned short nRace;
                    if ((bPlayer || (pSkill->m_bNPCCanUse &&
                        ((nRace = m_nRace) != CSWRace::CRACE_DROID || pSkill->m_bDroidCanUse))) &&
                        nRanks > 0)
                    {
                        int nRankChange;
                        if (bClassSkill)
                        {
                            nRankChange = (unsigned char)min(nRanks, nSkillPoints);
                            nSkillPoints -= nRankChange;
                        }
                        else
                        {
                            nRankChange = (unsigned char)min(nRanks, nSkillPoints / 2);
                            nSkillPoints -= 2 * nRankChange;
                        }
                        pLevelStats->SetSkillRankChange(nSkill, nRankChange);
                    }
                }
            }
            ++nRecommended;
        }
        pLevelStats->m_nSkillPoints = nSkillPoints;

        unsigned char nGrantedLevel;
        for (int nFeat = 0; nFeat < g_pRules->m_nNumFeats; ++nFeat)
        {
            if (pClass->IsGrantedFeat(nFeat, nGrantedLevel) && nGrantedLevel == nClassLevel)
                AddFeat(nFeat);
        }

        unsigned char nNormalFeats, nBonusFeats;
        CalcLevelUpNumberFeats(nClass, nNormalFeats, nBonusFeats);
        int nLeft = nNormalFeats + nBonusFeats;
        nRecommended = 1;
        int nNumFeats = g_pRules->m_nNumFeats;
        lstFeats.RemoveAll(0);
        while (nLeft > 0)
        {
            if (nRecommended > nNumFeats)
                break;
            unsigned short nFeat = pClass->GetRecommendedFeat(nRecommended);
            if (nFeat != -1)
            {
                CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
                if (pFeat != NULL && pFeat->m_nMinCharLevel <= nClassLevel &&
                    CanChooseFeat(nFeat, nNormalFeats, nBonusFeats, &lstFeats, 1))
                {
                    pLevelStats->AddFeat(nFeat);
                    lstFeats.Add(nFeat);
                    --nLeft;
                }
            }
            ++nRecommended;
        }

        if (pClass->m_bSpellCaster)
        {
            nLeft = ComputeNumberKnownSpellsLeft(nMultiClass, 1);
            int nNumSpells = g_pRules->m_pSpellArray->GetNumSpells();
            nRecommended = 1;
            lstSpells.RemoveAll(0);
            while (nLeft > 0)
            {
                if (nRecommended > nNumSpells)
                    break;
                unsigned long nSpell = (unsigned short)pClass->GetRecommendedSpell(
                    GetSimpleAlignmentGoodEvil() != ALIGNMENT_EVIL, nRecommended);
                if (nSpell != -1 && CanChooseSpell(nSpell, nLeft, &lstSpells, 1))
                {
                    pLevelStats->m_lstSpellsAdded[0].Add(nSpell);
                    lstSpells.Add(nSpell);
                    --nLeft;
                }
                ++nRecommended;
            }
        }

        if (!ValidateLevelUp(pLevelStats))
            delete pLevelStats;
    }

    if (bFeedback)
    {
        unsigned long oidCreature = m_pCreature->GetId();
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        pClient->AddFloatyText(pClient->ServerToClientObjectId(oidCreature),
                             CClientExoApp::FLOATY_TEXT_LEVEL, GetLevel(0));
    }
}
// PC 0x005EF0D0
unsigned short CSWSCreatureStats::GetNumberKnownSpells(
    unsigned char nClass, unsigned char nSpellLevel)
{
    NWSCREATURESTATS_TRACE();
    if (nClass >= m_nClassCount)
    {
        return 0;
    }

    return m_aClassInfo[nClass].GetNumberKnownSpells(nSpellLevel);
}
// PC 0x005EF1B0
int CSWSCreatureStats::GetWeaponSpecialization(CSWSItem* pItem)
{
    if (pItem != NULL && pItem->GetBaseItem() != NULL)
    {
        unsigned short nSpecFeat = pItem->GetBaseItem()->m_nSpecFeat;

        if (nSpecFeat != 0)
        {
            return HasFeat(nSpecFeat);
        }
    }

    return 0;
}
// PC 0x005EF570
int CSWSCreatureStats::CanLevelUp()
{
    CServerExoApp* pApp = g_pAppManager->m_pServerExoApp;
    int nLevel = GetLevel(0);
    return nLevel < pApp->GetServerInfo()->m_nMaxLevel &&
           m_nExperience >= g_pRules->m_anExperience[GetLevel(0)] &&
           !m_pCreature->GetDead() && !m_pCreature->GetDeadTemp();
}
// PC 0x005EF600
void CSWSCreatureStats::SetSpellLikeAbilityReady(
    unsigned long nSpell, unsigned char bReady, unsigned char nClass)
{
    for (int nIndex = 0; nIndex < m_pSpellLikeAbilities->num; ++nIndex)
    {
        if ((*m_pSpellLikeAbilities)[nIndex].m_nSpell == nSpell &&
            (*m_pSpellLikeAbilities)[nIndex].m_nReady != bReady &&
            (*m_pSpellLikeAbilities)[nIndex].m_nLevel == nClass)
        {
            (*m_pSpellLikeAbilities)[nIndex].m_nReady = bReady;
            break;
        }
    }
}
// PC 0x005EF660
int CSWSCreatureStats::GetIsInSpellLikeAbilityList(unsigned long nSpell)
{
    CExoArrayList<CSWSStats_SpellLikeAbility>* pAbilities;
    int i = 0;

    pAbilities = m_pSpellLikeAbilities;
    if (pAbilities->num > 0)
    {
        while ((*pAbilities)[i].m_nSpell != nSpell ||
               (*pAbilities)[i].m_nReady != 1)
        {
            ++i;
            if (i >= pAbilities->num)
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}
// PC 0x005EF6A0
unsigned short CSWSCreatureStats::GetFeat(unsigned short nIndex)
{
    if (nIndex >= m_lstFeats.num)
    {
        return 0xFFFF;
    }

    return m_lstFeats[nIndex];
}
// PC 0x005EF6C0
int CSWSCreatureStats::HasClassSkillFeat(unsigned short nSkill)
{
    unsigned short nFeat = 0xFFFF;
    switch (nSkill)
    {
    case CSWSkill::SKILL_AWARENESS:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_AWARENESS;
        break;
    case CSWSkill::SKILL_COMPUTER_USE:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_COMPUTER_USE;
        break;
    case CSWSkill::SKILL_DEMOLITIONS:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_DEMOLITIONS;
        break;
    case CSWSkill::SKILL_REPAIR:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_REPAIR;
        break;
    case CSWSkill::SKILL_SECURITY:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_SECURITY;
        break;
    case CSWSkill::SKILL_STEALTH:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_STEALTH;
        break;
    case CSWSkill::SKILL_TREAT_INJURY:
        nFeat = CSWFeat::FEAT_CLASS_SKILL_TREAT_INJURY;
        break;
    }
    if (nFeat != -1)
    {
        return HasFeat(nFeat);
    }
    return 0;
}
// PC 0x005EF740
unsigned short CSWSCreatureStats::GetBonusFeat(unsigned short nIndex)
{
    if (nIndex >= m_lstBonusFeats.num)
    {
        return 0xFFFF;
    }

    return m_lstBonusFeats[nIndex];
}
// PC 0x005EF760
int CSWSCreatureStats::GetEffectImmunity(unsigned char nImmunityType,
                                        CSWSCreature* pCreator)
{
    int nCreatorRace;
    int nCreatorAlignment;
    if (pCreator)
    {
        nCreatorRace = pCreator->m_pStats->m_nRace;
        nCreatorAlignment = pCreator->m_pStats->GetSimpleAlignmentGoodEvil();
    }
    else
    {
        nCreatorRace = g_pRules->m_nNumRaces;
        nCreatorAlignment = -1;
    }

    CExoArrayList<CGameEffect*>* pEffects = &m_pCreature->m_lstEffects;
    for (int i = m_nImmunityEffectIndex; i < pEffects->num; ++i)
    {
        CGameEffect* pEffect = (*pEffects)[i];
        if (pEffect->GetEffectType() > CSWSEffectListHandler::EFFECT_IMMUNITY)
        {
            break;
        }

        int nType = pEffect->GetInteger(0);
        int nRace = pEffect->GetInteger(1);
        int nAlignment = pEffect->GetInteger(2);
        if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_IMMUNITY &&
            (nType == CSWSEffectListHandler::IMMUNITY_TYPE_ALL || nType == nImmunityType) &&
            (nRace == g_pRules->m_nNumRaces || nRace == nCreatorRace) &&
            (nAlignment == 0 || nAlignment == nCreatorAlignment))
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x005EF890
int CSWSCreatureStats::GetEffectLinkImmunity(CGameEffect* pEffect)
{
    int bImmune = 0;
    if (pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_LINK &&
        pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_VISUALEFFECT)
    {
        int nEffectId = EffectId2Effect2DAId(pEffect->GetEffectType(), pEffect->GetInteger(0));
        if (nEffectId != 0)
        {
            for (int nImmunity = 0;
                 nImmunity < CSWSEffectListHandler::IMMUNITY_TYPE_MAX;
                 ++nImmunity)
            {
                int bApplies;
#ifdef _XBOX
                CTwoDimArrays* pArraysForGameEffects = g_pRules->Get2DArrays();
                EXOXBOXASSERT(pArraysForGameEffects->m_pGameEffects);
                C2DA* pGameEffects = pArraysForGameEffects->m_pGameEffects;
#else
                C2DA* pGameEffects = g_pRules->Get2DArrays()->m_pGameEffects;
#endif
                pGameEffects->GetINTEntry(nEffectId, nImmunity + 1, &bApplies);
                if (bApplies != 0)
                {
                    bImmune = GetEffectImmunity(nImmunity, NULL);
                    if (bImmune != 0)
                    {
                        unsigned short nFeedback;
                        if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_MIND_AFFECTING)
                            nFeedback = CSWSCreature::FEEDBACK_MIND_SPELLS_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_POISON)
                            nFeedback = CSWSCreature::FEEDBACK_POISON_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_DISEASE)
                            nFeedback = CSWSCreature::FEEDBACK_DISEASE_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_FEAR)
                            nFeedback = CSWSCreature::FEEDBACK_FEAR_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS)
                            nFeedback = CSWSCreature::FEEDBACK_PARALYSIS_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_BLINDNESS)
                            nFeedback = CSWSCreature::FEEDBACK_BLINDNESS_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_DEAFNESS)
                            nFeedback = CSWSCreature::FEEDBACK_DEAFNESS_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_SLOW)
                            nFeedback = CSWSCreature::FEEDBACK_SLOW_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_ENTANGLE)
                            nFeedback = CSWSCreature::FEEDBACK_ENTANGLE_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_SILENCE)
                            nFeedback = CSWSCreature::FEEDBACK_SILENCE_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_STUN)
                            nFeedback = CSWSCreature::FEEDBACK_STUN_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_SLEEP)
                            nFeedback = CSWSCreature::FEEDBACK_SLEEP_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_CONFUSED)
                            nFeedback = CSWSCreature::FEEDBACK_CONFUSED_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_CURSED)
                            nFeedback = CSWSCreature::FEEDBACK_CURSED_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_DAZED)
                            nFeedback = CSWSCreature::FEEDBACK_DAZED_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_KNOCKDOWN)
                            nFeedback = CSWSCreature::FEEDBACK_KNOCKDOWN_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_NEGATIVE_LEVEL)
                            nFeedback = CSWSCreature::FEEDBACK_NEGATIVE_LEVEL_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_CRITICAL_HITS)
                            nFeedback = CSWSCreature::FEEDBACK_CRITICAL_HITS_IMMUNITY;
                        else if (nImmunity == CSWSEffectListHandler::IMMUNITY_TYPE_DEATH_MAGIC)
                            nFeedback = CSWSCreature::FEEDBACK_DEATH_IMMUNITY;
                        else
                            return bImmune;

                        unsigned long oidCreator = pEffect->m_oidCreator;
                        CSWSCreature* pCreator =
                            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                                oidCreator);
                        CSWCCMessageData* pMessageData = new CSWCCMessageData;
                        pMessageData->SetObjectID(0, GetCreature()->GetId());
                        if (pCreator != NULL)
                        {
                            CSWCCMessageData* pCreatorMessageData = new CSWCCMessageData;
                            pMessageData->CopyTo(pCreatorMessageData);
                            pCreator->SendFeedbackMessage(nFeedback, pCreatorMessageData);
                        }
                        GetCreature()->SendFeedbackMessage(nFeedback, pMessageData);
                        return bImmune;
                    }
                }
            }
        }
    }
    if (pEffect->m_pLinkLeft != NULL)
        bImmune = GetEffectLinkImmunity(pEffect->m_pLinkLeft);
    if (pEffect->m_pLinkRight != NULL && bImmune == 0)
        bImmune |= GetEffectLinkImmunity(pEffect->m_pLinkRight);
    return bImmune;
}
// PC 0x005EFB90
void CSWSCreatureStats::AdjustSpellUsesPerDay()
{
    for (unsigned char nMultiClass = 0; nMultiClass < m_nClassCount; ++nMultiClass)
    {
        if (CSWClass::IsJedi((int)GetClass(nMultiClass)))
        {
            unsigned char nOldSpellGain = m_aClassInfo[nMultiClass].m_nAdditionalSpellCounts[0];
            unsigned char nSpellGain = GetSpellGainWithBonus(nMultiClass, 0);
            char nChange = nSpellGain - nOldSpellGain;
            m_aClassInfo[nMultiClass].m_nAdditionalSpellCounts[0] = nSpellGain;
            if (nChange < 0)
            {
                if (nChange < m_aClassInfo[nMultiClass].m_nSpellsPerDayLeft[0])
                {
                    m_aClassInfo[nMultiClass].m_nSpellsPerDayLeft[0] += nChange;
                }
                else
                {
                    m_aClassInfo[nMultiClass].m_nSpellsPerDayLeft[0] = 0;
                }
            }
        }
    }
}
// PC 0x005EFC40
unsigned char CSWSCreatureStats::GetSpellLikeAbilityCasterLevel(
    unsigned long nSpell)
{
    for (int nIndex = 0; nIndex < m_pSpellLikeAbilities->num; ++nIndex)
    {
        if ((*m_pSpellLikeAbilities)[nIndex].m_nSpell == nSpell)
        {
            return (*m_pSpellLikeAbilities)[nIndex].m_nLevel;
        }
    }
    return 0xFF;
}
// PC 0x005ED210
CExoString CSWSCreatureStats::GetFullName()
{
    CExoString sName;

    CSWSMessage::GetLocStringServer(0xFFFFFFFF, m_lsFirstName, m_lsLastName,
                                    sName, 0);
    return sName;
}
// PC 0x005ED350
unsigned char CSWSCreatureStats::GetAttacksPerRound()
{
    return 1;
}
// PC 0x005ED360
int CSWSCreatureStats::GetUseMonkAttackTables(int bForceUnarmed)
{
    if (!m_pCreature->GetUseMonkAbilities())
        return 0;

    CSWSItem* pWeapon = m_pCreature->m_pInventory->GetItemInSlot(
        INVENTORY_SLOT_RIGHTHAND);

    if (bForceUnarmed)
    {
        if (pWeapon == NULL)
        {
            return 1;
        }
    }
    else if (pWeapon == NULL)
    {
        return 1;
    }
    return 0;
}
// PC 0x005ED3A0
unsigned char CSWSCreatureStats::GetUnarmedDamageDice()
{
    int nCreatureSize = m_pCreature->m_nCreatureSize;

    unsigned char nDice = 1;

    if (m_pCreature->GetUseMonkAbilities() == 1)
    {

        for (int nClass = 0; nClass < GetNumClasses(); ++nClass)
        {
        }

        if (nCreatureSize > 2)
        {
            nDice = 1;
        }
        else
        {
            nDice = 1;
        }
    }

    return nDice;
}
// PC 0x005ED3C0
unsigned char CSWSCreatureStats::GetUnarmedDamageDie()
{
    int nCreatureSize = m_pCreature->m_nCreatureSize;
    int nDamageDie;

    if (m_pCreature->GetUseMonkAbilities() == 1)
    {

        for (int nClass = 0; nClass < GetNumClasses(); ++nClass)
        {
        }

        nDamageDie = nCreatureSize > 2 ? 6 : 4;
    }
    else
    {
        nDamageDie = 4;
    }
    return nDamageDie;
}
// PC 0x005EE1C0
void CSWSCreatureStats::SetArcaneSpellFailure(unsigned char nFailure)
{
}
// PC 0x005ED3F0
int CSWSCreatureStats::GetUnarmedDamageRoll(CSWSObject* pTarget)
{
    int nDifficulty = 100;
    int nDamageDie = GetUnarmedDamageDie();
    int nDamage = g_pRules->RollDice(GetUnarmedDamageDice(), nDamageDie);
    int bPCToNPC = FALSE;
    int bNPCToPC = FALSE;

    if (pTarget != NULL && pTarget->AsSWSCreature() != NULL)
    {
        if (m_bIsPC == TRUE &&
            pTarget->AsSWSCreature()->GetIsPC() == FALSE)
        {
            bPCToNPC = TRUE;
            nDifficulty = g_pAppManager->GetServerExoApp()->GetDifficultyOption(
                CSWRules::DIFFICULTY_PC_DAMAGE);
        }
        else if (m_bIsPC == FALSE &&
                 pTarget->AsSWSCreature()->GetIsPC() == TRUE)
        {
            bNPCToPC = TRUE;
            nDifficulty = g_pAppManager->GetServerExoApp()->GetDifficultyOption(
                CSWRules::DIFFICULTY_NPC_DAMAGE);
        }
    }

    if (m_pCreature->m_bInParty == 0)
    {
        int nAutoBalance = m_pCreature->m_c250;
        if (nAutoBalance != 0)
        {
            C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
            float fDamageMult;
            pAutoBalance->GetFLOATEntry(nAutoBalance, "DamageMult", &fDamageMult);
            char nLevel = m_pCreature->m_c251;
            if ((unsigned char)nLevel < 1)
            {
                nLevel = 1;
            }
            nDamage *= (unsigned int)((nLevel - 1) * fDamageMult) + 1;
        }
    }

    return g_pRules->GetWeightedDamageAmount(nDamage, nDamageDie,
                                            bPCToNPC, bNPCToPC, nDifficulty);
}
// PC 0x005EFC80
int CSWSCreatureStats::HasSpell(unsigned char nSpellLevel,
                               unsigned long nSpell, int bCheckForcePoints)
{
    unsigned char nClassCount = m_nClassCount;
    int bHasSpell = 0;
    int bResult = 0;
    for (unsigned char nClass = 0; nClass < nClassCount; ++nClass)
    {
        CSWSCreatureStats_ClassInfo* pInfo = &m_aClassInfo[nClass];
        for (int nSpellIndex = 0;
             nSpellIndex < (unsigned short)pInfo->m_lstKnownSpells[nSpellLevel].num;
             ++nSpellIndex)
        {
            if (pInfo->m_lstKnownSpells[nSpellLevel][nSpellIndex] == nSpell)
            {
                bHasSpell = 1;
                break;
            }
        }
    }
    if (bCheckForcePoints)
    {
        if (bHasSpell)
        {
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
            CSWSArea* pArea = m_pCreature->GetArea();
            int nRoomForceRating = 0;
            if (pArea != NULL)
                nRoomForceRating = pArea->GetRoomForceRating(m_pCreature->m_vPosition);
            int nForceBodyLevel = -1;
            CExoArrayList<CGameEffect*>* pEffects = &m_pCreature->m_lstEffects;
            for (int i = 0; i < pEffects->num; i++)
            {
                CGameEffect* pEffect = (*pEffects)[i];
                if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_FORCEBODY)
                {
                    nForceBodyLevel = pEffect->GetInteger(0);
                    break;
                }
            }
            if (nForceBodyLevel != -1)
            {
                int nPercent = 0;
                int nFullCost = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
                    m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), m_pCreature->GetStats()->GetCHAMod());
                switch (nForceBodyLevel)
                {
                case 0:
                    nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_0;
                    break;
                case 1:
                    nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_1;
                    break;
                case 2:
                    nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_2;
                    break;
                }
                int nHitPointCost = nFullCost * nPercent / 100;
                if (nHitPointCost <= (short)(m_nCurrentForcePoints + m_nTemporaryForcePoints) &&
                    m_pCreature->GetCurrentHitPoints(0) - nHitPointCost > 1)
                {
                    bResult = 1;
                }
            }
            else
            {
                int nForcePoints = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
                    m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), GetCHAMod());
                short nForcePointsLeft = m_nCurrentForcePoints + m_nTemporaryForcePoints;
                if (nForcePoints <= nForcePointsLeft)
                {
                    bResult = 1;
                }
            }
        }
    }
    else
    {
        bResult = bHasSpell;
    }
    return bResult;
}
// PC 0x005EFEA0
void CSWSCreatureStats::CalcLevelUpNumberFeats(unsigned char nClass,
                                             unsigned char& nNormalFeats,
                                             unsigned char& nBonusFeats)
{
    unsigned char nMultiClass = m_nClassCount - 1;
    CSWClass* pClass = &g_pRules->m_pClasses[GetClassLeveledUpIn()];
    unsigned char nLevel = GetClassLevel(nMultiClass, 0) + 1;

    nNormalFeats = nLevel - 1 < CSWClass::MAX_FEAT_GAIN_LEVELS
        ? pClass->m_aFeatGain[nLevel - 1] : 0;
    nBonusFeats = nLevel - 1 < CSWClass::MAX_FEAT_GAIN_LEVELS
        ? pClass->m_aBonusFeatGain[nLevel - 1] : 0;
}
// PC 0x005EFF40
void CSWSCreatureStats::SetNormalBonusFlags(unsigned short nFeat,
                                           int& bNormal, int& bBonus)
{
    bNormal = 0;
    bBonus = 0;

    if (g_pRules->GetFeat(nFeat) != NULL)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClassLeveledUpIn()];
        if (pClass != NULL)
        {
            if (pClass->IsNormalFeat(nFeat))
            {
                bNormal = 1;
            }
            if (pClass->IsBonusFeat(nFeat))
            {
                bBonus = 1;
            }
        }
    }
}
// PC 0x005ED5E0
int CSWSCreatureStats::ResolveSpecialAttackAttackBonus(CSWSCreature* pTarget)
{
    CSWSCombatAttackData* pAttack = m_pCreature->m_pcCombatRound->GetAttack(
        m_pCreature->m_pcCombatRound->m_nCurrentAttack);
    int nBonus = 0;
    CSWSItem* pWeapon;

    switch (pAttack->m_nAttackType)
    {
    case 11:
        nBonus = -4;
        break;
    case 91:
        nBonus = -2;
        break;
    case 20:
    case 31:
    case 77:
        pWeapon = m_pCreature->m_pcCombatRound->GetCurrentAttackWeapon(0);
        if (pWeapon != NULL && pWeapon->m_n2E8)
        {
            nBonus = 4;
        }
        break;
    case 103:
        nBonus = 4;
        break;
    case 30:
        nBonus = -4;
        pWeapon = m_pCreature->m_pcCombatRound->GetCurrentAttackWeapon(0);
        if (pWeapon != NULL && pWeapon->m_n2E4)
        {
            nBonus = -3;
        }
        break;
    case 17:
    case 18:
    case 28:
    case 29:
    case 82:
    case 83:
        nBonus = -3;
        break;
    case 92:
        nBonus = -2;
        pWeapon = m_pCreature->m_pcCombatRound->GetCurrentAttackWeapon(0);
        if (pWeapon != NULL && pWeapon->m_n2E4)
        {
            nBonus = -1;
        }
        break;
    case 26:
        nBonus = -1;
        pWeapon = m_pCreature->m_pcCombatRound->GetCurrentAttackWeapon(0);
        if (pWeapon != NULL && pWeapon->m_n2E4)
        {
            nBonus = 0;
        }
        break;
    case 53:
    case 101:
        nBonus = 0;
        break;
    case 102:
        nBonus = 2;
        break;
    }

    return nBonus;
}
// PC 0x005ED780
int CSWSCreatureStats::ResolveSpecialAttackDamageBonus(CSWSCreature* pTarget)
{
    CSWSCombatRound* pCombatRound = m_pCreature->m_pcCombatRound;
    CSWSCombatAttackData* pAttack =
        pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack);
    int nBonus = 0;

    switch (pAttack->m_nAttackType)
    {
    case CSWFeat::FEAT_POWER_ATTACK:
    case CSWFeat::FEAT_POWER_BLAST:
        nBonus = 3;
        break;
    case CSWFeat::FEAT_IMPROVED_POWER_ATTACK:
    case CSWFeat::FEAT_IMPROVED_POWER_BLAST:
        nBonus = 7;
        break;
    case CSWFeat::FEAT_MASTER_POWER_BLAST:
    case CSWFeat::FEAT_MASTER_POWER_ATTACK:
        nBonus = 12;
        break;
    case CSWFeat::FEAT_FORCE_JUMP:
        nBonus = 0;
        break;
    case CSWFeat::FEAT_FORCE_JUMP_ADVANCED:
        nBonus = 2;
        break;
    case CSWFeat::FEAT_FORCE_JUMP_MASTERY:
        nBonus = 4;
        break;
    }

    return nBonus;
}
// PC 0x005ED860
unsigned char CSWSCreatureStats::GetClassLevel(unsigned char nClass,
                                               int bExcludeNegativeLevels)
{
    if (nClass >= m_nClassCount)
    {
        return 0;
    }

    int nLevel = m_aClassInfo[nClass].m_nLevel;
    int nNegativeLevels = 0;

    if (bExcludeNegativeLevels != 0)
    {
        nNegativeLevels = m_aClassInfo[nClass].m_nNegativeLevels;
    }

    if (nLevel - nNegativeLevels < 0)
    {
        return 0;
    }

    return nLevel - nNegativeLevels;
}
// PC 0x005ED8B0
unsigned char CSWSCreatureStats::GetClassNegativeLevels(unsigned char nClass)
{
    if (nClass >= m_nClassCount)
    {
        return 0;
    }
    return m_aClassInfo[nClass].m_nNegativeLevels;
}
// PC 0x005ED8E0
int CSWSCreatureStats::GetCriticalHitRoll(int bOffHand)
{
    int nCriticalRange = 0;

    if (GetCreature)
    {
        CSWSItem* pWeapon = m_pCreature->m_pInventory->GetItemInSlot(
            INVENTORY_SLOT_RIGHTHAND);
        if (bOffHand && pWeapon != NULL &&
            g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nWeaponWield != 3)
        {
            pWeapon = m_pCreature->m_pInventory->GetItemInSlot(
                INVENTORY_SLOT_LEFTHAND);
        }
        if (pWeapon == NULL)
        {
            pWeapon = m_pCreature->m_pInventory->GetItemInSlot(
                INVENTORY_SLOT_HANDS);
        }

        int nThreat;
        if (pWeapon == NULL)
        {
            nThreat = 1;
        }
        else
        {
            nThreat = g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nCritThreat;
            CSWItemProperty* pProperty = NULL;
            if (pWeapon->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_KEEN, 0))
            {
                nCriticalRange += nThreat;
            }
        }
        nCriticalRange += nThreat;

        CSWSCombatRound* pCombatRound = m_pCreature->m_pcCombatRound;
        CSWSCombatAttackData* pAttack =
            pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack);
        int nFeatBonus = 0;
        switch (pAttack->m_nAttackType)
        {
        case CSWFeat::FEAT_CRITICAL_STRIKE:
            nFeatBonus = nThreat;
            break;
        case CSWFeat::FEAT_IMPROVED_CRITICAL_STRIKE:
            nFeatBonus = nThreat * 2;
            break;
        case CSWFeat::FEAT_MASTER_CRITICAL_STRIKE:
            nFeatBonus = nThreat * 3;
            break;
        case CSWFeat::FEAT_SNIPER_SHOT:
            nFeatBonus = nThreat;
            break;
        case CSWFeat::FEAT_IMPROVED_SNIPER_SHOT:
            nFeatBonus = nThreat * 2;
            break;
        case CSWFeat::FEAT_MASTER_SNIPER_SHOT:
            nFeatBonus = nThreat * 3;
            break;
        }
        nCriticalRange += nFeatBonus;
    }

    return 21 - nCriticalRange;
}
// PC 0x005EDA60
int CSWSCreatureStats::GetCriticalHitMultiplier(int bOffHand)
{
    int nCriticalHitMultiplier = 2;
    CSWSItem* pWeapon;

    if (bOffHand != 0)
    {
        pWeapon = GetCreature()->GetInventory()->GetItemInSlot(
            INVENTORY_SLOT_RIGHTHAND);
        if (pWeapon != NULL &&
            g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nWeaponWield !=
                CSWBaseItem::WIELD_DOUBLE_WEAPON)
        {
            pWeapon = GetCreature()->GetInventory()->GetItemInSlot(
                INVENTORY_SLOT_LEFTHAND);
        }
    }
    else
    {
        pWeapon = GetCreature()->GetInventory()->GetItemInSlot(
            INVENTORY_SLOT_RIGHTHAND);
    }

    if (pWeapon != NULL)
    {
        nCriticalHitMultiplier =
            g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nCritHitMult;
    }

    CSWSCombatRound* pCombatRound = GetCreature()->m_pcCombatRound;
    switch (pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack)->m_nAttackType)
    {
    case 17:
    case 18:
    case 28:
    case 29:
    case 82:
    case 83:
        nCriticalHitMultiplier++;
        break;
    }

    if (GetCreature()->IsWieldingLightsaber() && GetCreature()->m_n1110 == 262)
    {
        nCriticalHitMultiplier++;
    }

    return nCriticalHitMultiplier;
}
// PC 0x005EDB70
char CSWSCreatureStats::GetDEXMod(int bWithEffects)
{
    int nDexBonus = 0;
    int bUseArmorLimit = 0;

    if (bWithEffects == 1 && GetCreature()->IsDebilitated() == 0)
    {
        CSWSItem* pArmor =
            GetCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_ARMOUR);

        if (pArmor != NULL && pArmor->ComputeArmorClass() > 0)
        {
            nDexBonus = pArmor->GetBaseItem()->m_nDexBonus + pArmor->m_n2EC;
            bUseArmorLimit = 1;
        }
    }

    int nDexModifier;

    if ((bUseArmorLimit && nDexBonus >= 0) || m_pCreature->IsDebilitated() != 0)
    {
        nDexModifier = min(m_nDexterityModifier, nDexBonus);
    }
    else
    {
        nDexModifier = m_nDexterityModifier;
    }

    return nDexModifier;
}
// PC 0x005EDC00
CExoString CSWSCreatureStats::GetClassString(unsigned char nClass)
{
    if (nClass >= m_nClassCount)
    {
        return CExoString("");
    }
    CSWClass* pClass = g_pRules->m_pClasses + m_aClassInfo[nClass].m_nClass;
    if (pClass != NULL)
    {
        return pClass->GetNameText();
    }
    return CExoString("");
}
// PC 0x005EDC70
void CSWSCreatureStats::SetClass(unsigned char nMultiClass,
                                 unsigned char nClass)
{
    if (nMultiClass < m_nClassCount)
    {
        m_aClassInfo[nMultiClass].m_nClass = nClass;
    }
}

inline void CSWSCreatureStats_ClassInfo::RemoveKnownSpell(unsigned char nSpellLevel,
                                                          unsigned long nSpell)
{
    if (nSpellLevel < NUM_SPELL_LEVELS)
    {
        int nIndex = m_lstKnownSpells[nSpellLevel].IndexOf(nSpell);
        if (nIndex >= 0)
            m_lstKnownSpells[nSpellLevel].Remove(nSpell);
    }
}

inline void CSWSCreatureStats::RemoveKnownSpell(unsigned char nClass,
                                               unsigned long nSpell)
{
    if (g_pRules->m_pSpellArray->GetSpell(nSpell) != NULL)
    {
        m_aClassInfo[nClass].RemoveKnownSpell(0, nSpell);
    }
}
// PC 0x005EDC90
void CSWSCreatureStats::SetClassNegativeLevels(unsigned char nClass,
                                               unsigned char nLevels)
{
    if (nClass < m_nClassCount)
    {
        m_aClassInfo[nClass].m_nNegativeLevels = nLevels;
    }
}
// PC 0x005EDCB0
int CSWSCreatureStats::CalcStatModifier(unsigned char nScore)
{
    int nModifier;

    if (nScore < 10)
    {
        nModifier = nScore - 11;
    }
    else
    {
        nModifier = nScore - 10;
    }

    return nModifier / 2;
}

unsigned long CSWSCreatureStats::CalculateExpNeededForNextLevel()
{
    for (int nLevel = 0; nLevel <= 20; ++nLevel)
    {
        if (m_nExperience < g_pRules->m_anExperience[(unsigned char)nLevel])
        {
            return g_pRules->m_anExperience[(unsigned char)nLevel];
        }
    }

    return 0;
}
// PC 0x005EDCE0
void CSWSCreatureStats::SetSkillRank(unsigned char nSkill, char nRank)
{
    if (nSkill < g_pRules->m_nNumSkills)
    {
        m_pSkillRanks[nSkill] = nRank;
    }
}
// PC 0x005EF100
int CSWSCreatureStats::GetWeaponFinesse(CSWSItem* pItem)
{
    if (pItem != NULL && GetDEXMod(0) > m_nStrengthModifier)
    {
        if ((pItem->IsLightsaber() && HasFeat(CSWFeat::FEAT_FINESSE_LIGHTSABERS)) ||
            ((pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_1 ||
              pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 ||
              pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON) &&
             HasFeat(CSWFeat::FEAT_FINESSE_MELEE_WEAPONS)))
        {
            return 1;
        }
    }

    return 0;
}
// PC 0x005EF1F0
bool CSWSCreatureStats::GetSpellUsesLeft(
    unsigned long nSpell, unsigned char nMultiClass,
    unsigned char nSpellLevel, unsigned char nSpellIndex)
{
    if (nMultiClass >= m_nClassCount)
    {
        return 0;
    }

    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    if (pSpell == NULL)
    {
        return 0;
    }

    CSWSArea* pArea = m_pCreature->GetArea();
    int nRoomForceRating = 0;
    if (pArea != NULL)
        nRoomForceRating = pArea->GetRoomForceRating(m_pCreature->m_vPosition);
    bool bUsesLeft = 0;
    unsigned long nForcePoints = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
        m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), m_pCreature->GetStats()->GetCHAMod());
    int nForceBodyLevel = -1;
    CExoArrayList<CGameEffect*>* pEffects = &m_pCreature->m_lstEffects;
    for (int i = 0; i < pEffects->num; i++)
    {
        CGameEffect* pEffect = (*pEffects)[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_FORCEBODY)
        {
            nForceBodyLevel = pEffect->GetInteger(0);
            break;
        }
    }
    if (nForceBodyLevel != -1)
    {
        int nPercent = 0;
        switch (nForceBodyLevel)
        {
        case 0:
            nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_0;
            break;
        case 1:
            nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_1;
            break;
        case 2:
            nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_2;
            break;
        }
        int nHitPointCost = (int)(nForcePoints * nPercent) / 100;
        if (nForcePoints - nHitPointCost <= (short)(m_nCurrentForcePoints + m_nTemporaryForcePoints) &&
            m_pCreature->GetCurrentHitPoints(0) - nHitPointCost >= 1)
        {
            bUsesLeft = 1;
        }
    }
    else if (nForcePoints <= (short)(m_nCurrentForcePoints + m_nTemporaryForcePoints))
    {
        bUsesLeft = 1;
    }
    return bUsesLeft;
}
// PC 0x005EF360
int CSWSCreatureStats::UnReadySpell(unsigned long nSpell)
{
    int nHitPointCost = 0;
    int bSuccess = 0;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    if (pSpell != NULL)
    {
        CSWSArea* pArea = m_pCreature->GetArea();
        int nRoomForceRating = 0;
        if (pArea != NULL)
            nRoomForceRating = pArea->GetRoomForceRating(m_pCreature->m_vPosition);
        int nForceBodyLevel = -1;
        CExoArrayList<CGameEffect*>* pEffects = &m_pCreature->m_lstEffects;
        for (int i = 0; i < pEffects->num; i++)
        {
            CGameEffect* pEffect = (*pEffects)[i];
            if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_FORCEBODY)
            {
                nForceBodyLevel = pEffect->GetInteger(0);
                break;
            }
        }
        int nForcePoints;
        if (nForceBodyLevel != -1)
        {
            int nPercent = 0;
            int nFullCost = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
                m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), m_pCreature->GetStats()->GetCHAMod());
            switch (nForceBodyLevel)
            {
            case 0:
                nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_0;
                break;
            case 1:
                nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_1;
                break;
            case 2:
                nPercent = CSWSpell::FORCE_BODY_COST_PERCENT_2;
                break;
            }
            nHitPointCost = nFullCost * nPercent / 100;
            nForcePoints = nHitPointCost;
        }
        else
        {
            nForcePoints = pSpell->GetAdjustedForcePoints(GetGoodEvilAlignment(),
                m_pCreature->GetInParty(), nRoomForceRating, m_pCreature->GetForm(), m_pCreature->GetStats()->GetCHAMod());
        }
        int nTemporaryForcePoints = m_nTemporaryForcePoints;
        if (nTemporaryForcePoints >= nForcePoints)
        {
            m_nTemporaryForcePoints = nTemporaryForcePoints - nForcePoints;
            return 1;
        }
        m_nTemporaryForcePoints = 0;
        m_nCurrentForcePoints += nTemporaryForcePoints - nForcePoints;
        if (m_nCurrentForcePoints < 0)
        {
            m_nCurrentForcePoints = 0;
        }
        int nTemporaryHitPoints = (short)m_pCreature->m_nTemporaryHitPoints;
        if (nTemporaryHitPoints >= nHitPointCost)
        {
            m_pCreature->m_nTemporaryHitPoints = nTemporaryHitPoints - nHitPointCost;
            return 1;
        }
        m_pCreature->m_nTemporaryHitPoints = 0;
        m_pCreature->SetCurrentHitPoints(nTemporaryHitPoints - nHitPointCost +
                                         m_pCreature->GetCurrentHitPoints(1));
        if (m_pCreature->GetCurrentHitPoints(0) < 0)
        {
            m_pCreature->SetCurrentHitPoints(0);
        }
        bSuccess = 1;
    }
    return bSuccess;
}
// PC 0x005EDD10
unsigned char CSWSCreatureStats::GetSpellsPerDayLeft(unsigned char nMultiClass,
                                                     unsigned char nSpellLevel)
{
    if (nMultiClass >= m_nClassCount)
    {
        return 0;
    }
    if (nSpellLevel >= 1)
    {
        return 0;
    }
    return m_aClassInfo[nMultiClass].m_nSpellsPerDayLeft[nSpellLevel];
}
// PC 0x005EDD40
void CSWSCreatureStats::SetMovementRate(int nRate)
{
    if (nRate == MOVEMENT_SPEED_DEFAULT)
    {
        CExoString sMoveRate;
        CExoString s2DAName;
        if (g_pRules->Get2DArrays()->m_pAppearance->GetCExoStringEntry(
                GetAppearanceType(), "MOVERATE", &sMoveRate))
        {
            for (nRate = 0;
                 nRate < g_pRules->Get2DArrays()->GetCreatureSpeedTable()->m_nNumRows;
                 ++nRate)
            {
                if (g_pRules->Get2DArrays()->GetCreatureSpeedTable()->GetCExoStringEntry(
                        nRate, "2DAName", &s2DAName))
                {
                    if (sMoveRate.LowerCase() == s2DAName.LowerCase())
                    {
                        break;
                    }
                }
            }
            if (nRate >= g_pRules->Get2DArrays()->GetCreatureSpeedTable()->m_nNumRows)
            {
                nRate = MOVEMENT_SPEED_PC;
            }
        }
        else
        {
            nRate = MOVEMENT_SPEED_PC;
        }
    }

    m_nMovementRate = nRate;
    g_pRules->Get2DArrays()->GetCreatureSpeedTable()->GetFLOATEntry(
        nRate, CTwoDimArrays::COL2DA_CREATURESPEED_WALKRATE, &m_fWalkRate);
    g_pRules->Get2DArrays()->GetCreatureSpeedTable()->GetFLOATEntry(
        m_nMovementRate, CTwoDimArrays::COL2DA_CREATURESPEED_RUNRATE, &m_fRunRate);
    if (nRate == MOVEMENT_SPEED_IMMOBILE)
    {
        m_pCreature->m_nAIStateFlags = ~CSWSCreature::AI_STATE_CAN_MOVE;
    }
}
// PC 0x005EDF70
char CSWSCreatureStats::GetTotalSTRBonus()
{
    CSWSCreature* pCreature = m_pCreature;
    int nBonus = 0;

    int nResult = pCreature->GetTotalEffectBonus(4, 0, nBonus, 0, 0, 0, 0, 255, 0,
                                            0);
    return nResult;
}
// PC 0x005EDFA0
char CSWSCreatureStats::GetTotalDEXBonus()
{
    CSWSCreature* pCreature = m_pCreature;
    int nBonus = 0;

    int nResult = pCreature->GetTotalEffectBonus(4, 0, nBonus, 0, 0, 0, 0, 255, 1,
                                            0);
    return nResult;
}
// PC 0x005EDFD0
char CSWSCreatureStats::GetTotalCONBonus()
{
    CSWSCreature* pCreature = m_pCreature;
    int nBonus = 0;

    int nResult = pCreature->GetTotalEffectBonus(4, 0, nBonus, 0, 0, 0, 0, 255, 2,
                                            0);
    return nResult;
}
// PC 0x005EE000
char CSWSCreatureStats::GetTotalINTBonus()
{
    CSWSCreature* pCreature = m_pCreature;
    int nBonus = 0;

    int nResult = pCreature->GetTotalEffectBonus(4, 0, nBonus, 0, 0, 0, 0, 255, 3,
                                            0);
    return nResult;
}
// PC 0x005EE030
char CSWSCreatureStats::GetTotalWISBonus()
{
    CSWSCreature* pCreature = m_pCreature;
    int nBonus = 0;

    int nResult = pCreature->GetTotalEffectBonus(4, 0, nBonus, 0, 0, 0, 0, 255, 4,
                                            0);
    return nResult;
}
// PC 0x005EE060
char CSWSCreatureStats::GetTotalCHABonus()
{
    CSWSCreature* pCreature = m_pCreature;
    int nBonus = 0;

    int nResult = pCreature->GetTotalEffectBonus(4, 0, nBonus, 0, 0, 0, 0, 255, 5,
                                            0);
    return nResult;
}
// PC 0x005EE090
unsigned char CSWSCreatureStats::ComputeNumberKnownSpellsLeft(unsigned char nClass, int bNextLevel)
{
    unsigned char nClassType = GetClass(nClass);
    CSWClass* pClass = &g_pRules->m_pClasses[nClassType];
    if (pClass == NULL)
    {
        return 0;
    }
    if (nClassType != CSWClass::CCLASS_JEDICONSULAR &&
        nClassType != CSWClass::CCLASS_JEDIGUARDIAN &&
        nClassType != CSWClass::CCLASS_JEDISENTINEL &&
        nClassType != CSWClass::CCLASS_JEDIWATCHMAN &&
        nClassType != CSWClass::CCLASS_JEDIWEAPONMASTER &&
        nClassType != CSWClass::CCLASS_JEDIMASTER &&
        nClassType != CSWClass::CCLASS_SITHASSASSIN &&
        nClassType != CSWClass::CCLASS_SITHMARAUDER &&
        nClassType != CSWClass::CCLASS_SITHLORD)
    {
        return 0;
    }
    unsigned char nLevel = GetClassLevel(nClass, 0);
    if (bNextLevel)
    {
        nLevel++;
    }
    unsigned char nSpells = pClass->GetSpellGain(nLevel, 0);
    if (nSpells == CSWClass::SPELL_GAIN_INVALID)
    {
        return 0;
    }
    return nSpells;
}
// PC 0x005EE180
int CSWSCreatureStats::GetForceResistance()
{
    char nResistance = m_nForceResistance - m_nForceResistancePenalty;
    if (nResistance < 0)
    {
        return 0;
    }
    return nResistance;
}
// PC 0x005EE1A0
unsigned char CSWSCreatureStats::GetACNaturalBase()
{
    if (GetCreature()->GetUseMonkAbilities() == 1)
    {

    }

    return m_nACNaturalBase;
}
int CSWSCreatureStats::EffectId2Effect2DAId(int nEffectId, int nEffectInteger)  // PC 0x005EE1D0
{
    switch (nEffectId)
    {
    case 18:
        return 1;
    case 35:
        return 2;
    case 37:
        return 3;
    case 11:
        return 4;
    case 14:
        return 5;
    case 17:
        return 6;
    case 49:
        return 7;
    case 29:
        return 8;
    case 27:
        return 9;
    case 34:
        return 10;
    case 56:
        return 11;
    case 93:
        return 12;
    case 94:
        return 13;
    case 95:
        return 14;
    case 97:
        return 15;
    case 99:
        return 16;
    case 100:
        return 17;
    case 8:
    {
        int n2DAId = 0;
        if (nEffectInteger == 8)
        {
            n2DAId = 13;
        }
        else if (nEffectInteger == 7)
        {
            n2DAId = 15;
        }
        else if (nEffectInteger == 5)
        {
            n2DAId = 18;
        }
        else if (nEffectInteger == 4)
        {
            n2DAId = 19;
        }
        else if (nEffectInteger == 3)
        {
            n2DAId = 21;
        }
        else if (nEffectInteger == 1)
        {
            n2DAId = 22;
        }
        else if (nEffectInteger == 6)
        {
            n2DAId = 23;
        }
        else if (nEffectInteger == 2)
        {
            n2DAId = 24;
        }
        else if (nEffectInteger == 19)
        {
            n2DAId = 25;
        }
        else if (nEffectInteger == 18)
        {
            n2DAId = 26;
        }
        return n2DAId;
    }
    case 19:
        return 20;
    }

    return 0;
}
// PC 0x005EE3B0
int CSWSCreatureStats::AdjustHPAccumulatorBy(float fAdjustment)
{
    int nAdjustment;

    m_fHPAccumulator += fAdjustment;
    nAdjustment = (int)floor(m_fHPAccumulator);
    m_fHPAccumulator -= (short)nAdjustment;
    return nAdjustment;
}
// PC 0x005EE400
int CSWSCreatureStats::AdjustFPAccumulatorBy(float fAdjustment)
{
    int nAdjustment;

    m_fFPAccumulator += fAdjustment;
    nAdjustment = (int)(m_fFPAccumulator >= 0.0 ? floor(m_fFPAccumulator)
                                                : ceil(m_fFPAccumulator));
    m_fFPAccumulator -= (short)nAdjustment;
    return nAdjustment;
}
// PC 0x005EE460
void CSWSCreatureStats::AddClass(unsigned char nClass, unsigned char nLevel)
{
    if (m_nClassCount < MAX_CLASSES)
    {
        ++m_nClassCount;
    }

    unsigned char nMultiClass = m_nClassCount - 1;
    if (nMultiClass < m_nClassCount)
    {
        m_aClassInfo[nMultiClass].m_nClass = nClass;
    }

    nMultiClass = m_nClassCount - 1;
    if (nMultiClass < m_nClassCount)
    {
        m_aClassInfo[nMultiClass].m_nLevel = nLevel;
    }

    if (m_pCreature != NULL)
    {
        m_pCreature->UpdatePureGoodEvilPowers();
    }
}
// PC 0x005EE4C0
int CSWSCreatureStats::CheckPowerSuitability(int nPowerSuitability, int nFilter)
{
    int bSuitable = 1;
    int nMaskedFilter = nFilter & 0xF000;

    if (nMaskedFilter != 0xF000 &&
        nMaskedFilter != (nPowerSuitability & 0xF000))
    {
        bSuitable = 0;
    }

    nMaskedFilter = nFilter & 0x0F00;
    if (nMaskedFilter != 0x0F00 &&
        nMaskedFilter != (nPowerSuitability & 0x0F00))
    {
        bSuitable = 0;
    }

    nMaskedFilter = nFilter & 0x00F0;
    if (nMaskedFilter != 0x00F0 &&
        nMaskedFilter != (nPowerSuitability & 0x00F0))
    {
        bSuitable = 0;
    }

    nMaskedFilter = nFilter & 0x000F;
    if (nMaskedFilter != 0x000F &&
        nMaskedFilter != (nPowerSuitability & 0x000F))
    {
        bSuitable = 0;
    }

    return bSuitable;
}
// PC 0x005EE540
int CSWSCreatureStats::CheckInclusionMatch(int nType, int nRow, int nExclusion)
{
    int bMatch = 1;
    int nRowExclusion = 0;
    if (nType == CScriptTalent::TALENT_TYPE_FEAT)
    {
        bool bRead;
        {
            CExoString sColumn("Exclusion");
#ifdef _XBOX
            CTwoDimArrays* pArraysForFeat = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pArraysForFeat->m_pFeat);
            C2DA* pTable = pArraysForFeat->m_pFeat;
#else
            C2DA* pTable = g_pRules->Get2DArrays()->m_pFeat;
#endif
            bRead = pTable->GetINTEntry(nRow, sColumn, &nRowExclusion) != 0;
        }
        if (bRead && nRowExclusion && (nRowExclusion | nExclusion))
        {
            bMatch = 0;
        }
    }
    else if (nType == CScriptTalent::TALENT_TYPE_SPELL)
    {
        bool bRead;
        {
            CExoString sColumn("Exclusion");
#ifdef _XBOX
            CTwoDimArrays* pArraysForSpells = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pArraysForSpells->m_pSpells);
            C2DA* pTable = pArraysForSpells->m_pSpells;
#else
            C2DA* pTable = g_pRules->Get2DArrays()->m_pSpells;
#endif
            bRead = pTable->GetINTEntry(nRow, sColumn, &nRowExclusion) != 0;
        }
        if (bRead && nRowExclusion && (nRowExclusion | nExclusion))
        {
            bMatch = 0;
        }
    }
    return bMatch;
}
// PC 0x005EE630
unsigned char CSWSCreatureStats::GetSpellGainWithBonus(unsigned char nMultiClass,
                                                      unsigned char nSpellLevel)
{
    unsigned char nBonus = 0;
    char nAbilityModifier = 0;
    unsigned char nClass = GetClass(nMultiClass);
    CSWClass* pClass = &g_pRules->m_pClasses[nClass];

    if (pClass)
    {
        if (CSWClass::IsJedi((int)nClass))
        {
            nAbilityModifier = m_nCharismaModifier;
        }

        if (nSpellLevel > 0 && nAbilityModifier > 0 &&
            nAbilityModifier >= nSpellLevel)
        {
            nBonus = max(0, nAbilityModifier - nSpellLevel + 4) / 4;
        }

        unsigned char nSpellGain = pClass->GetSpellGain(
            GetClassLevel(nMultiClass, 0), nSpellLevel);
        if (nSpellGain != 0xFF)
        {
            return nSpellGain + nBonus;
        }
    }

    return 0;
}
// PC 0x005EE720
unsigned char CSWSCreatureStats::GetLevel(int bExcludeNegativeLevels)
{
    int nLevel = 0;

    for (int nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
    {
        nLevel += m_pCreature->m_pStats->GetClassLevel(
            (unsigned char)nClassIndex, bExcludeNegativeLevels);
    }

    CSWSCreature* pCreature = m_pCreature;
    if (pCreature != NULL && pCreature->m_pStats->m_bIsPC == 0 && pCreature->m_bInParty == 0 &&
        pCreature->m_c250 != 0)
    {
        int nAutoBalance = pCreature->m_c250;
        if (nAutoBalance != 0)
        {
            C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
            float fLevelMult;
            pAutoBalance->GetFLOATEntry(nAutoBalance, "LevelMult", &fLevelMult);
            char nPCLevel = pCreature->m_c251;
            if (nPCLevel == 0)
            {
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(
                    "G_PC_LEVEL", nPCLevel);
            }
            if (nPCLevel < 1)
            {
                nPCLevel = 1;
            }
            int nBalancedLevel = (int)(nPCLevel * fLevelMult) + nLevel - 1;
            if (nBalancedLevel < 0)
            {
                nBalancedLevel = 1;
            }
            return nBalancedLevel;
        }
    }

    return nLevel;
}
// PC 0x005ED2A0
int CSWSCreatureStats::GetIsJedi()
{
    for (int nClassIndex = 0; nClassIndex < m_nClassCount; nClassIndex++)
    {
        int nClass = GetClass((unsigned char)nClassIndex);
        if (nClass == CSWClass::CCLASS_JEDIGUARDIAN ||
            nClass == CSWClass::CCLASS_JEDICONSULAR ||
            nClass == CSWClass::CCLASS_JEDISENTINEL ||
            nClass == CSWClass::CCLASS_JEDIWATCHMAN ||
            nClass == CSWClass::CCLASS_JEDIMASTER ||
            nClass == CSWClass::CCLASS_JEDIWEAPONMASTER ||
            nClass == CSWClass::CCLASS_SITHLORD ||
            nClass == CSWClass::CCLASS_SITHASSASSIN ||
            nClass == CSWClass::CCLASS_SITHMARAUDER)
        {
            return 1;
        }
    }

    return 0;
}
// PC 0x005EE8E0
int CSWSCreatureStats::GetBaseAttackBonus()
{
    int nBonus = 0;

    for (int nClassIndex = 0; nClassIndex < m_nClassCount; ++nClassIndex)
    {
        unsigned char nLevel = GetClassLevel((unsigned char)nClassIndex, 0);
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass((unsigned char)nClassIndex)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetAttackBonus(nLevel);
        }
    }

    if (m_pCreature->m_bInParty == 0)
    {
        int nAutoBalance = m_pCreature->m_c250;
        if (nAutoBalance != 0)
        {
            C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
            float fToHitMult;
            pAutoBalance->GetFLOATEntry(nAutoBalance, "ToHitMult", &fToHitMult);
            char nLevel = m_pCreature->m_c251;
            if ((unsigned char)nLevel < 1)
            {
                nLevel = 1;
            }
            nBonus += (int)(nLevel * fToHitMult);
        }
    }

    if (m_nBaseAttackBonus == 0)
    {
        return nBonus;
    }
    return m_nBaseAttackBonus;
}

float CSWSCreatureStats::GetStatById(int nStatId)
{
    switch (nStatId)
    {
    case STAT_AGE:
        return (float)m_nAge;
    case STAT_CHALLENGE_RATING:
        return m_fChallengeRating;
    case STAT_FORTITUDE_SAVE:
        return (float)m_nFortSaveBonus;
    case STAT_REFLEX_SAVE:
        return (float)m_nReflexSaveBonus;
    case STAT_WILL_SAVE:
        return (float)m_nWillSaveBonus;
    case STAT_STRENGTH:
        return (float)m_nStrengthBase;
    case STAT_DEXTERITY:
        return (float)m_nDexterityBase;
    case STAT_CONSTITUTION:
        return (float)m_nConstitutionBase;
    case STAT_INTELLIGENCE:
        return (float)m_nIntelligenceBase;
    case STAT_WISDOM:
        return (float)m_nWisdomBase;
    case STAT_CHARISMA:
        return (float)m_nCharismaBase;
    case STAT_BASE_ATTACK_BONUS:
        return (float)GetBaseAttackBonus();
    case STAT_FORCE_RESISTANCE:
        return (float)GetForceResistance();
    }
    return 0.0f;
}
// PC 0x005EEA40
CSWLevelStats* CSWSCreatureStats::GetLevelStats(unsigned char nLevel)
{
    if (!m_bIsPC)
        return NULL;
    else
    {
        if (nLevel >= m_lstLevelStats.num)
            return NULL;
        return m_lstLevelStats.element[nLevel];
    }
}
// PC 0x005EFFE0
int CSWSCreatureStats::FeatAcquired(unsigned short nFeat,
                                  CExoArrayList<unsigned short>* pList)
{
    if (nFeat == CSWFeat::FEAT_INVALID)
    {
        return 0;
    }
    if (m_lstFeats.Contains(nFeat) || m_lstBonusFeats.Contains(nFeat))
    {
        return 1;
    }
    if (pList != NULL)
    {
        for (unsigned short i = 0; i < pList->num; ++i)
        {
            if (nFeat == (*pList)[i])
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x005F0070
int CSWSCreatureStats::HasSpellPrereq(CSWSpell* pSpell,
                                    CExoArrayList<unsigned long>* pList, int bNextLevel)
{
    unsigned long nSpell;
    for (int i = 0; (nSpell = pSpell->m_aMasterSpells[i]) != CSWSpell::SPELL_INVALID; ++i)
    {
        if (!HasSpell(0, nSpell, 1) && pList->IndexOf(nSpell) == -1)
        {
            return 0;
        }
    }
    for (int i = 0; pSpell->m_aPrerequisites[i] != CSWSpell::SPELL_INVALID; ++i)
    {
        unsigned long nSpell = pSpell->m_aPrerequisites[i];
        if (!HasSpell(0, nSpell, 1) && pList->IndexOf(nSpell) == -1)
        {
            return 0;
        }
    }
    if (bNextLevel)
    {
        if (GetLevel(0) + 1 < pSpell->GetSpellLevel(GetClass(m_nClassCount - 1)))
        {
            return 0;
        }
    }
    else if (GetLevel(0) < pSpell->GetSpellLevel(GetClass(m_nClassCount - 1)))
    {
        return 0;
    }
    return 1;
}
// PC 0x005F01E0
int CSWSCreatureStats::HasFeatPrereq(CSWFeat* pFeat,
                                   CExoArrayList<unsigned short>* pList)
{
    if (pFeat->m_nPrereqFeat1 != CSWFeat::FEAT_INVALID &&
        !FeatAcquired(pFeat->m_nPrereqFeat1, pList))
    {
        return 0;
    }
    if (pFeat->m_nPrereqFeat2 != CSWFeat::FEAT_INVALID &&
        !FeatAcquired(pFeat->m_nPrereqFeat2, pList))
    {
        return 0;
    }

    int bHasOrPrereq = 0;
    int bAcquiredOrPrereq = 0;
    for (int i = 0; !bAcquiredOrPrereq && i < CSWFeat::MAX_OR_PREREQ_FEATS; ++i)
    {
        if (pFeat->GetOrPrereqFeat(i) != CSWFeat::FEAT_INVALID)
        {
            bHasOrPrereq = 1;
            if (FeatAcquired(pFeat->GetOrPrereqFeat(i), pList))
            {
                bAcquiredOrPrereq = 1;
            }
        }
    }
    if (bHasOrPrereq && !bAcquiredOrPrereq)
    {
        return 0;
    }
    return 1;
}

static int IsActivatePowerProperty(CSWSItem* pItem, int nProperty, int nPower, int bAllowCharges);
// PC 0x005F0290
int CSWSCreatureStats::GetHasPowerFromItem(int nPower)
{
    if (m_pCreature->m_pInventory != NULL)
    {
        for (unsigned int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
        {
            CSWInventory* pInventory = m_pCreature->m_pInventory;
            CSWSItem* pItem = pInventory->GetItemInSlot(1 << nSlot);
            if (pItem != NULL && pItem->GetPropertyByTypeExists(
                CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
            {
                for (int nProperty = 0; nProperty < pItem->m_nActiveProperties; ++nProperty)
                {
                    if (IsActivatePowerProperty(pItem, nProperty, nPower, 0))
                    {
                        return 1;
                    }
                }
            }
        }
    }

    CItemRepository* pRepository = m_pCreature->GetItemRepository(1);
    if (pRepository != NULL)
    {
        for (unsigned int nItem = 0; nItem < (unsigned int)pRepository->m_lstItems.num; ++nItem)
        {
            CSWSItem* pItem = pRepository->ItemListGetItem(nItem);
            if (pItem != NULL &&
                g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nEquipableSlots == 0 &&
                pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
            {
                for (int nProperty = 0; nProperty < pItem->m_nActiveProperties; ++nProperty)
                {
                    if (IsActivatePowerProperty(pItem, nProperty, nPower, 1))
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x005F0450
int CSWSCreatureStats::GetItemDetailsForPower(int nPower,
    unsigned long& oidItem, int& nItemProperty)
{
    CSWSItem* pItem;
    int nProperty;
    int bFound = 0;
    if (m_pCreature->m_pInventory != NULL)
    {
        for (unsigned int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
        {
            CSWInventory* pInventory = m_pCreature->m_pInventory;
            pItem = pInventory->GetItemInSlot(1 << nSlot);
            if (pItem != NULL && pItem->GetPropertyByTypeExists(
                CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
            {
                for (nProperty = 0; nProperty < pItem->m_nActiveProperties; ++nProperty)
                {
                    if (IsActivatePowerProperty(pItem, nProperty, nPower, 0))
                    {
                        bFound = 1;
                        break;
                    }
                }
                if (bFound)
                {
                    break;
                }
            }
        }
    }

    if (!bFound)
    {
        CItemRepository* pRepository = m_pCreature->GetItemRepository(1);
        if (pRepository != NULL)
        {
            for (unsigned int nItem = 0; nItem < (unsigned int)pRepository->m_lstItems.num; ++nItem)
            {
                pItem = pRepository->ItemListGetItem(nItem);
                if (pItem != NULL &&
                    g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nEquipableSlots == 0 &&
                    pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
                {
                    for (nProperty = 0; nProperty < pItem->m_nActiveProperties; ++nProperty)
                    {
                        if (IsActivatePowerProperty(pItem, nProperty, nPower, 1))
                        {
                            bFound = 1;
                            break;
                        }
                    }
                    if (bFound)
                    {
                        break;
                    }
                }
            }
        }
    }
    if (!bFound)
    {
        return 0;
    }

    oidItem = pItem->GetId();
    nItemProperty = nProperty;
    return 1;
}

static int IsActivatePowerProperty(CSWSItem* pItem, int nProperty, int nPower, int bAllowCharges)
{
    if (pItem->GetActiveProperty(nProperty)->m_nPropertyType ==
            CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM &&
        ((unsigned char)pItem->GetActiveProperty(nProperty)->m_bUseable != 0 ||
         (bAllowCharges && pItem->m_nCharges > 0)))
    {
        CSWItemProperty* pProperty = pItem->GetActiveProperty(nProperty);
        if ((pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
             ((CSWSItem::UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & pItem->m_nUpgrades) != 0) &&
            pItem->GetActiveProperty(nProperty)->m_nSubtype == nPower)
        {
            return 1;
        }
    }
    return 0;
}

static inline void NwscreaturestatsTrace2()
{
    NwscreaturestatsTrace1();
}

static inline void NwscreaturestatsTrace1()
{
}


