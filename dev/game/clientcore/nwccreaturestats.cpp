#include "exobase.h"

#include "exoresman.h"
#include "nwupdatestats.h"
#include "nwccreaturestats.h"
#include "nwscreaturestats.h"
#include "nwportrait.h"
#include "nwspell.h"
#include "nwclass.h"
#include "clientexoappinternal.h"
#include "exotlktable.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwccreature.h"
#include "nwcitem.h"
#include "nwsitem.h"
#include "nwsvartable.h"
#include <math.h>
#include <string.h>

static inline void NwccreaturestatsTrace3();
static inline void NwccreaturestatsTrace2();
static inline void NwccreaturestatsTrace1();
// PC 0x00700340
CSWCCreatureStats::CSWCCreatureStats(CSWCCreature* pCreature)
{
    m_sFirstName = "";
    m_sLastName = "";
    m_nRace = CSWRace::CRACE_HUMAN;
    m_nStartingPackage = 0;
    m_nAge = 0;
    m_sSubRace = "";
    m_nSubraceIndex = 0;
    m_cConversation = "";
    m_nGender = 0;
    m_nClassCount = 3;
    SetClass(0, CSWClass::CCLASS_SOLDIER);
    SetClassLevel(0, 1);
    SetClass(1, CSWClass::CCLASS_INVALID);
    SetClassLevel(1, 0);
    m_nClassCount = 1;
    m_nStrength = DEFAULT_ABILITY_SCORE;
    m_nDexterity = DEFAULT_ABILITY_SCORE;
    m_nIntelligence = DEFAULT_ABILITY_SCORE;
    m_nWisdom = DEFAULT_ABILITY_SCORE;
    m_nCharisma = DEFAULT_ABILITY_SCORE;
    m_nConstitution = DEFAULT_ABILITY_SCORE;
    m_aAbilities[ABILITY_STRENGTH] = 0;
    m_aAbilities[ABILITY_DEXTERITY] = 0;
    m_aAbilities[ABILITY_INTELLIGENCE] = 0;
    m_aAbilities[ABILITY_WISDOM] = 0;
    m_aAbilities[ABILITY_CHARISMA] = 0;
    m_aAbilities[ABILITY_CONSTITUTION] = 0;
    m_nDexterityBonus = 0;
    m_aUnknown41[0] = 0;
    m_aUnknown41[1] = 0;
    m_aUnknown41[2] = 0;
    m_aUnknown41[3] = 0;
    m_aUnknown41[4] = 0;
    m_aUnknown41[5] = 0;
    m_nDefenseBase = 0;
    m_nHitPointsBase = 0;
    m_nHitPointBonus = 0;
    m_nMaxHitPoints = 0;
    m_nCurrentHitPoints = 0;
    m_aUnknown52[0] = 0;
    m_fUnknown54 = 6.0f;
    m_nExperience = 0;
    m_nFortitudeSave = 0;
    m_nWillSave = 0;
    m_nReflexSave = 0;
    m_nGoodEvilAlignment = NEUTRAL_GOOD_EVIL_ALIGNMENT;
    m_nBaseCNPCAlignment = -1;
    m_nUnknown60 = 0;
    m_nUnknown64 = 0;
    m_nAppearance = m_nRace;
    m_pSkillRanksTotal = new char[g_pRules->m_nNumSkills];
    m_pSkillRanks = new char[g_pRules->m_nNumSkills];
    memset(m_pSkillRanksTotal, 0, g_pRules->m_nNumSkills);
    memset(m_pSkillRanks, 0, g_pRules->m_nNumSkills);
    m_pCombatInformation = new CCombatInformation;
    m_pCreature = pCreature;
    m_bOwnsCombatInformation = 0;
    m_nPortraitId = PORTRAIT_ID_RESERVED_FFFF;
    m_nMaxForcePoints = 0;
    m_nForcePointsBase = 0;
    m_nCurrentForcePoints = 0;
    m_nForcePointBonus = 0;
    m_nColorSkin = 0;
    m_nColorHair = 0;
    m_nColorTattoo1 = 0;
    m_nColorTattoo2 = 0;
    m_nPhenotype = 0;
    m_nAppearanceHead = 0;
    m_nTail = 0;
    m_nWings = 0;
    m_nDuplicatingHead = 0xFF;
    m_bUseBackupHead = 0;
}
// PC 0x006FB590
CSWCStats_SpellLikeAbility::CSWCStats_SpellLikeAbility()
{
    m_nSpell = 0;
    m_nCasterLevel = 1;
    m_bReady = 0;
}
// PC 0x006FB5B0
void CSWCCreatureStats::ClearSkills()
{
    memset(m_pSkillRanksTotal, 0, g_pRules->m_nNumSkills);
    memset(m_pSkillRanks, 0, g_pRules->m_nNumSkills);
}
// PC 0x006FB600
char CSWCCreatureStats::CalcStatModifier(unsigned char nScore)
{
    return (int)floor(nScore * 0.5) - 5;
}
// PC 0x006FB630
CExoString CSWCCreatureStats::GetRaceString()
{
    if (m_sSubRace == CExoString(""))
    {
        return g_pRules->m_pRaces[m_nRace].GetNameText();
    }

    return m_sSubRace;
}
// PC 0x006FD630
short CSWCCreatureStats::GetArmorClass(int nType)
{
    short nWisdomBonus = 0;
    if (nType == ARMOR_CLASS_TYPE_1 && HasSpell(0, CSWSpell::FORCE_POWER_BATTLE_PRECOGNITION))
    {
        CSWSCreature* pServerCreature = m_pCreature->GetServerCreature();
        if (pServerCreature != NULL)
        {
            CSWSItem* pArmor = pServerCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
            CSWBaseItem* pBaseItem;
            if (pArmor == NULL ||
                ((pBaseItem = pArmor->GetBaseItem()) != NULL && pBaseItem->m_nBaseAC == 0))
            {
                if ((char)GetWISMod() > 0)
                {
                    nWisdomBonus = (char)GetWISMod();
                }
            }
        }
    }
    return m_nDefenseBase + nWisdomBonus;
}
// PC 0x006FB6F0
unsigned char CSWCCreatureStats::GetClass(unsigned char nIndex)
{
    if (nIndex >= 2)
    {
        return 0xFF;
    }
    return m_aClassInfo[nIndex].m_nClass;
}

inline unsigned char CSWCCreatureStats::GetClassLeveledUpIn()
{
    return GetClass(m_nClassCount - 1);
}
// PC 0x006FB710
unsigned char CSWCCreatureStats::GetClassLevel(unsigned char nIndex)
{
    if (nIndex >= 2)
    {
        return 0;
    }
    return m_aClassInfo[nIndex].m_nLevel;
}
// PC 0x006FB730
void CSWCCreatureStats::SetClass(unsigned char nIndex, unsigned char nClass)
{
    if (nIndex < 2)
    {
        m_aClassInfo[nIndex].m_nClass = nClass;
    }
}
// PC 0x006FB750
void CSWCCreatureStats::SetClassLevel(unsigned char nIndex,
                                      unsigned char nLevel)
{
    if (nIndex < 2)
    {
        m_aClassInfo[nIndex].m_nLevel = nLevel;
    }
}
// PC 0x006FB770
char CSWCCreatureStats::GetSkillRank(unsigned short nSkill)
{
    if (nSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }
    return m_pSkillRanksTotal[nSkill];
}
// PC 0x006FD870
int CSWCCreatureStats::GetCanUseSkill(unsigned short nSkill)
{
    if (nSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }

    CSWSkill* pSkill = &g_pRules->m_pSkills[nSkill];
    if (nSkill == CSWSkill::SKILL_STEALTH &&
        (HasSpell(0, CSWSpell::FORCE_POWER_FORCE_CAMOUFLAGE) ||
         HasSpell(0, CSWSpell::FORCE_POWER_IMPROVED_FORCE_CAMOUFLAGE) ||
         HasSpell(0, CSWSpell::FORCE_POWER_MASTER_FORCE_CAMOUFLAGE) ||
         HasFeat(CSWFeat::FEAT_PERSONAL_CLOAKING_SHIELD)))
    {
        return 1;
    }

    if (pSkill->m_bUntrained)
    {
        for (unsigned char i = 0; i < m_nClassCount; i++)
        {
            if (g_pRules->m_pClasses[GetClass(i)].IsSkillUseable(nSkill))
            {
                return 1;
            }
        }
        return 0;
    }

    return GetSkillRankBase(nSkill) > 0;
}
// PC 0x006FB7A0
void CSWCLevelUpStats::SetINTBase(unsigned char nValue)
{
    m_aAbilities[3] = nValue;
    m_nIntelligence =
        nValue + g_pRules->m_pRaces[m_nRace].m_nIntelligenceAdjust;
}
// PC 0x006FB7D0
void CSWCLevelUpStats::SetCHABase(unsigned char nValue)
{
    m_aAbilities[5] = nValue;
    m_nCharisma = nValue + g_pRules->m_pRaces[m_nRace].m_nCharismaAdjust;
}
// PC 0x006FCB70
int CSWCLevelUpStats::GetSkillPointCostForSkillRank(unsigned short nSkill,
                                                    int bIncrease)
{
    int bUseable = 0;
    int bClassSkill = 0;
    int nCost = 0;

    CSWSkill* pSkill = &g_pRules->m_pSkills[nSkill];
    if (pSkill == NULL)
    {
        return 0;
    }

    if (pSkill->m_bAllClassesCanUse)
    {
        bUseable = 1;
    }

    for (int i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL && pClass->IsSkillUseable(nSkill))
        {
            bUseable = 1;
            if (pClass->IsSkillClassSkill(nSkill) || HasClassSkillFeat(nSkill))
            {
                bClassSkill = 1;
                break;
            }
        }
    }

    if (bUseable)
    {
        nCost = bClassSkill ? 1 : 2;
        if (!bIncrease)
        {
            nCost = -nCost;
        }
    }

    return nCost;
}
// PC 0x006FB800
int CSWCLevelUpStats::GetBaseAttackBonus()
{
    unsigned char nBonus = 0;
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        nBonus +=
            g_pRules->m_pClasses[GetClass(i)].GetAttackBonus(GetClassLevel(i));
    }
    return nBonus;
}
// PC 0x006FB890
short CSWCCreatureStats::GetMaxHitPoints()
{
    return m_nMaxHitPoints;
}
// PC 0x00700680
CSWCLevelUpStats::CSWCLevelUpStats(CSWCCreature* pCreature)
    : CSWCCreatureStats(pCreature)
{
    m_nCreationStage = 30;
    m_nSkillPoints = 0;
    SetSTRBase(8);
    SetDEXBase(8, 0);
    SetINTBase(8);
    SetWISBase(8);
    SetCHABase(8);
    SetCONBase(8);
}
// PC 0x006FE8A0
CSWCCreatureStats::~CSWCCreatureStats()
{
    if (m_pSkillRanksTotal != NULL)
    {
        delete[] m_pSkillRanksTotal;
        m_pSkillRanksTotal = NULL;
    }
    if (m_pSkillRanks != NULL)
    {
        delete[] m_pSkillRanks;
        m_pSkillRanks = NULL;
    }
    ClearFeats();
    if (m_pCombatInformation != NULL)
    {
        delete m_pCombatInformation;
        m_pCombatInformation = NULL;
    }
}
// PC 0x006FEA00
void CSWCCreatureStats::AddKnownSpell(unsigned char nClass,
                                      unsigned long nSpell)
{
    if (nClass < 2)
    {
        if (g_pRules->m_pSpellArray->GetSpell(nSpell) != NULL)
        {
            m_aClassInfo[nClass].AddKnownSpell(0, nSpell);
        }
    }
}
// PC 0x006FB8A0
void CSWCCreatureStats::SetCombatInformation(CCombatInformation* pInfo)
{
    if (m_pCombatInformation != NULL)
    {
        delete m_pCombatInformation;
    }
    if (m_bOwnsCombatInformation == 0)
    {
        m_bOwnsCombatInformation = 1;
    }
    m_pCombatInformation = pInfo;
}
// PC 0x006FB8F0
int CSWCCreatureStats::CalculateNumAttacks()
{
    int nBonus = 0;
    for (int i = 0; i < m_nClassCount; i++)
    {
        nBonus +=
            (unsigned char)g_pRules->m_pClasses[GetClass((unsigned char)i)]
                .GetAttackBonus(GetClassLevel((unsigned char)i));
    }
    int nAttacks = (nBonus + 4) / 5;
    if (nAttacks != 0)
    {
        return nAttacks;
    }
    return 1;
}

CExoString CSWCCreatureStats::GetAlignmentInfo()
{
    CExoString sInfo;
    STR_RES strRes;

    if (m_nGoodEvilAlignment <= 40)
        g_pTlkTable->Fetch(STRREF_ALIGNMENT_DARK, strRes, CTlkTable::GENDER_MALE);
    else if (m_nGoodEvilAlignment < 60)
        g_pTlkTable->Fetch(STRREF_ALIGNMENT_NEUTRAL, strRes, CTlkTable::GENDER_MALE);
    else
        g_pTlkTable->Fetch(STRREF_ALIGNMENT_LIGHT, strRes, CTlkTable::GENDER_MALE);

    sInfo.Format("%s (%d)", strRes.m_sString.CStr(), m_nGoodEvilAlignment);
    return sInfo;
}
// PC 0x006FB990
void CSWCCreatureStats::SetPortraitId(unsigned short nPortraitId)
{
    CSWPortrait cPortrait;
    if (cPortrait.SetPortraitId(nPortraitId) != 0)
    {
        m_cPortrait.CResRef::operator=(cPortrait.GetPortrait(0));
    }
    m_nPortraitId = nPortraitId;
}
// PC 0x006FBA20
int CSWCCreatureStats::CheckCustomPortrait(CResRef* pResRef, int bFemale)
{
    CSWPortrait cPortrait;

    cPortrait.CResRef::operator=(*pResRef);
    if (g_pExoResMan->Exists(cPortrait.GetPortrait(3), RESTYPE_TGA, (unsigned long*)0) != 0)
    {
        return 1;
    }

    if (bFemale != 0)
    {
        *pResRef = "po_hu_f_99_";
    }
    else
    {
        *pResRef = "po_hu_m_99_";
    }
    return 0;
}

CExoString CSWCCreatureStats::GetFullName()
{
    CExoString* pName = &m_sFirstName;
    if (pName->GetLength() > 0 && m_sLastName.GetLength() > 0)
    {
        return *pName + CExoString(" ") + m_sLastName;
    }
    if (pName->GetLength() > 0)
    {
        return *pName;
    }
    pName = &m_sLastName;
    if (pName->GetLength() > 0)
    {
        return *pName;
    }
    return CExoString("");
}
// PC 0x006FBAF0
CExoString CSWCCreatureStats::GetFirstName()
{
    CExoString sFirstName(m_sFirstName);
    if (m_sFirstName.GetLength() > 0)
    {
        int nSpace = m_sFirstName.Find(' ', 0);
        if (nSpace != -1)
        {
            sFirstName = m_sFirstName.SubString(0, nSpace);
        }
    }
    return sFirstName;
}
// PC 0x006FBBC0
CExoString CSWCCreatureStats::GetLastName()
{
    CExoString sLastName;
    if (m_sFirstName.GetLength() > 0)
    {
        int nSpace = m_sFirstName.Find(' ', 0);
        if (nSpace != -1)
        {
            sLastName = m_sFirstName.SubString(nSpace + 1, -1);
        }
    }
    return sLastName;
}
// PC 0x006FBC90
float CSWCCreatureStats::GetMinSpellRange(const CExoString& sRange)
{
    float fRange;
    if (sRange == "T" || sRange == "P")
    {
        fRange = g_pRules->m_afSecondaryRange[CSWRules::RANGE_TOUCH];
    }
    else if (sRange == "S")
    {
        fRange = g_pRules->m_afSecondaryRange[CSWRules::RANGE_SHORT];
    }
    else if (sRange == "M")
    {
        fRange = g_pRules->m_afSecondaryRange[CSWRules::RANGE_MEDIUM];
    }
    else if (sRange == "L")
    {
        fRange = g_pRules->m_afSecondaryRange[CSWRules::RANGE_LONG];
    }
    else if (sRange == "W")
    {
        fRange = g_pRules->m_afSecondaryRange[CSWRules::RANGE_WEAPON];
    }
    return fRange;
}
// PC 0x006FBD40
CResRef CSWCCreatureStats::GetPortrait(int bAlternate)
{
    CExoString sResRef;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pPortraits = p2DArrays->m_pPortrait;
    if (pPortraits == NULL)
    {
        return m_cPortrait;
    }
    if (bAlternate == 0)
    {
        return m_cPortrait;
    }
    sResRef = "";
    if (m_nGoodEvilAlignment <= 10)
    {
        if (pPortraits->GetCExoStringEntry(m_nPortraitId, "BaseResRefVVVE", &sResRef) == 0)
        {
            return m_cPortrait;
        }
    }
    else if (m_nGoodEvilAlignment <= 20)
    {
        if (pPortraits->GetCExoStringEntry(m_nPortraitId, "BaseResRefVVE", &sResRef) == 0)
        {
            return m_cPortrait;
        }
    }
    else if (m_nGoodEvilAlignment <= 30)
    {
        if (pPortraits->GetCExoStringEntry(m_nPortraitId, "BaseResRefVE", &sResRef) == 0)
        {
            return m_cPortrait;
        }
    }
    else if (m_nGoodEvilAlignment <= 40)
    {
        if (pPortraits->GetCExoStringEntry(m_nPortraitId, "BaseResRefE", &sResRef) == 0)
        {
            return m_cPortrait;
        }
    }
    else
    {
        return m_cPortrait;
    }
    return CResRef(sResRef);
}

int CSWCCreatureStats::GetBaseAttackBonus()
{
    if (m_bOwnsCombatInformation == 1)
    {
        return m_nUnknown64;
    }

    unsigned char nBonus = 0;
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetAttackBonus(GetClassLevel(i));
        }
    }
    return nBonus;
}

int CSWCCreatureStats::GetCombatModeAttackModifier(CSWCCreature* pCreature)
{
    if (pCreature == NULL)
    {
        return 0;
    }
    if (pCreature->m_nUnknown19C == 1)
    {
        return -5;
    }
    if (pCreature->m_nUnknown1A0 == 1)
    {
        return -10;
    }
    if (pCreature->m_nUnknown1A8 == 1)
    {
        return -2;
    }
    if (pCreature->m_nUnknown1AC == 1)
    {
        return -2;
    }
    return 0;
}

CExoString CSWCCreatureStats::GetAttackBonusString(CSWCCreature* pCreature, int nAttack,
                                                   int bOffHand)
{
    STR_RES strRes;
    CExoString sResult = "";

    if (m_pCombatInformation == NULL)
    {
        return sResult;
    }

    CCombatInformationNode* pNode = m_pCombatInformation->m_lstAttacks[nAttack];
    if (pNode->m_nWeaponWield != CCombatInformationNode::WEAPON_MISC &&
        pNode->m_nWeaponWield != CCombatInformationNode::WEAPON_ONHAND &&
        pNode->m_nWeaponWield != CCombatInformationNode::WEAPON_OFFHAND &&
        pNode->m_nWeaponWield != CCombatInformationNode::WEAPON_UNARMED)
    {
        return sResult;
    }
    if (pNode->m_nVersusRace == g_pRules->m_nNumRaces && pNode->m_nVersusGoodEvil == 0)
    {
        return sResult;
    }
    if (bOffHand)
    {
        if (pNode->m_nWeaponWield == CCombatInformationNode::WEAPON_ONHAND)
        {
            return "";
        }
    }
    else if (pNode->m_nWeaponWield == CCombatInformationNode::WEAPON_OFFHAND)
    {
        return "";
    }
    if (pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND) != OBJECT_INVALID &&
        pNode->m_nWeaponWield == CCombatInformationNode::WEAPON_UNARMED)
    {
        return "";
    }

    int nNumAttacks;
    char nAttackMod;
    if (bOffHand)
    {
        nNumAttacks = m_pCombatInformation != NULL &&
                      m_pCombatInformation->m_nOffHandWeaponEquipped == 1;
        nAttackMod = m_pCombatInformation->m_nOffHandAttackMod;
    }
    else
    {
        nNumAttacks = m_pCombatInformation->m_nNumAttacks;
        nAttackMod = m_pCombatInformation->m_nOnHandAttackMod;
    }
    int nModifier = pNode->m_nModifier + nAttackMod + GetCombatModeAttackModifier(pCreature);

    for (int i = 0; i < nNumAttacks; i++)
    {
        if (i != 0)
        {
            sResult = sResult + "/";
        }
        else
        {
            sResult = "    ";
        }

        int nBonus = 0;
        if (m_pCombatInformation != NULL)
        {
            nBonus = m_nUnknown60 ? GetBaseAttackBonus() - 3 * i : GetBaseAttackBonus() - 5 * i;
        }
        if (nBonus + nModifier >= 0)
        {
            sResult = sResult + "+" + CExoString(nBonus + nModifier);
        }
        else
        {
            sResult = sResult + "-" + CExoString(abs(nBonus + nModifier));
        }
    }

    int bVersus = FALSE;
    if (pNode->m_nVersusGoodEvil != 0)
    {
        g_pTlkTable->Fetch(STRREF_ATTACK_BONUS_VERSUS, strRes, CTlkTable::GENDER_MALE);
        sResult = sResult + " " + strRes.m_sString + " ";
        bVersus = TRUE;
        switch (pNode->m_nVersusGoodEvil)
        {
        case 2:
            g_pTlkTable->Fetch(STRREF_ALIGNMENT_LIGHT, strRes, CTlkTable::GENDER_MALE);
            break;
        case 3:
            g_pTlkTable->Fetch(STRREF_ALIGNMENT_DARK, strRes, CTlkTable::GENDER_MALE);
            break;
        case 1:
            g_pTlkTable->Fetch(STRREF_ALIGNMENT_NEUTRAL, strRes, CTlkTable::GENDER_MALE);
            break;
        }
        sResult = sResult + strRes.m_sString;
    }
    if (pNode->m_nVersusRace != g_pRules->m_nNumRaces)
    {
        if (bVersus)
        {
            sResult = sResult + " ";
        }
        else
        {
            g_pTlkTable->Fetch(STRREF_ATTACK_BONUS_VERSUS, strRes, CTlkTable::GENDER_MALE);
            sResult = sResult + " " + strRes.m_sString + " ";
        }
        g_pTlkTable->Fetch(g_pRules->m_pRaces[pNode->m_nVersusRace].m_nNamePluralStrRef,
                           strRes, CTlkTable::GENDER_MALE);
        sResult = sResult + strRes.m_sString;
    }
    return sResult;
}

CExoString CSWCCreatureStats::GetGeneralDamageBonusString(CSWCCreature* pCreature,
                                                          int bOffHand)
{
    CExoString sResult = "";

    if (m_pCombatInformation != NULL && pCreature != NULL)
    {
        unsigned char nDice = m_pCombatInformation->m_nUnarmedDamageDice;
        unsigned char nDie = m_pCombatInformation->m_nUnarmedDamageDie;
        char nDamageMod;
        unsigned long nSlot = bOffHand ? INVENTORY_SLOT_LEFTHAND : INVENTORY_SLOT_RIGHTHAND;
        if (m_pCombatInformation->m_nDamageDie != 0)
        {
            nDice = m_pCombatInformation->m_nDamageDice;
            nDie = m_pCombatInformation->m_nDamageDie;
        }
        else
        {
            CSWCItem* pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
                pCreature->GetEquippedItemID(nSlot));
            CSWBaseItem* pBaseItem;
            if (pItem != NULL &&
                (pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->m_nBaseItem)) != NULL)
            {
                nDice = pBaseItem->m_nNumDice;
                nDie = pBaseItem->m_nDieToRoll;
            }
        }
        if (bOffHand)
        {
            nDamageMod = m_pCombatInformation->m_nOffHandDamageMod;
        }
        else
        {
            nDamageMod = m_pCombatInformation->m_nOnHandDamageMod;
        }

        int nModifier = nDamageMod + GetCombatModeDamageModifier(pCreature);
        sResult = CExoString(nDice) + "-" + CExoString(nDice * nDie);
        if (nModifier > 0)
        {
            sResult = sResult + " + " + CExoString(nModifier);
        }
        else if (nModifier < 0)
        {
            sResult = sResult + " - " + CExoString(-nModifier);
        }
    }
    return sResult;
}

int CSWCCreatureStats::GetCombatModeDamageModifier(CSWCCreature* pCreature)
{
    if (pCreature->m_nUnknown19C == 1)
    {
        return 5;
    }
    if (pCreature->m_nUnknown1A0 == 1)
    {
        return 10;
    }
    return 0;
}
// PC 0x006FBFC0
inline char CSWCCreatureStats::GetSTRMod()
{
    NwccreaturestatsTrace1();
    return (int)floor(m_nStrength * 0.5) - 5;
}
// PC 0x006FBFF0
inline char CSWCCreatureStats::GetDEXMod()
{
    NwccreaturestatsTrace1();
    return CalcStatModifier(m_nDexterity) + m_nDexterityBonus;
}
// PC 0x006FC030
inline int CSWCCreatureStats::GetCONMod()
{
    NwccreaturestatsTrace1();
    return (int)floor(m_nConstitution * 0.5) - 5;
}
// PC 0x006FC060
inline int CSWCCreatureStats::GetINTMod()
{
    NwccreaturestatsTrace1();
    return (int)floor(m_nIntelligence * 0.5) - 5;
}
// PC 0x006FC090
inline int CSWCCreatureStats::GetWISMod()
{
    NwccreaturestatsTrace1();
    return (int)floor(m_nWisdom * 0.5) - 5;
}
// PC 0x006FC0C0
inline int CSWCCreatureStats::GetCHAMod()
{
    NwccreaturestatsTrace1();
    return (int)floor(m_nCharisma * 0.5) - 5;
}
// PC 0x006FC0F0
inline char CSWCCreatureStats::GetBaseINTMod()
{
    NwccreaturestatsTrace1();
    return (int)floor(m_aAbilities[3] * 0.5) - 5;
}
// PC 0x006FC120
inline int CSWCCreatureStats::FeatAcquired(unsigned short nFeat)
{
    NwccreaturestatsTrace1();
    int nLeft = m_lstFeats.num;
    int nCount = 0;
    if (nLeft > 0)
    {
        unsigned short* pFeat = m_lstFeats.element;
        do
        {
            if (*pFeat == nFeat)
            {
                nCount++;
            }
            pFeat++;
            nLeft--;
        } while (nLeft != 0);
    }
    if (nCount == 0)
    {
        nLeft = m_lstBonusFeats.num;
        int nBonusCount = 0;
        if (nLeft > 0)
        {
            unsigned short* pFeat = m_lstBonusFeats.element;
            do
            {
                if (*pFeat == nFeat)
                {
                    nBonusCount++;
                }
                pFeat++;
                nLeft--;
            } while (nLeft != 0);
        }
        if (nBonusCount != 0)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}
// PC 0x006FC190
void CSWCCreatureStats::ClearFeats()
{
    NwccreaturestatsTrace1();
    while (m_lstFeats.num != 0)
    {
        m_lstFeats.DelIndex(m_lstFeats.num - 1);
    }
    while (m_lstBonusFeats.num != 0)
    {
        m_lstBonusFeats.DelIndex(0);
    }
    while (m_lstFeatUses.num != 0)
    {
        delete m_lstFeatUses.element[0];
        m_lstFeatUses.DelIndex(0);
    }
}
// PC 0x006FC2A0
unsigned char CSWCCreatureStats::GetLevel()
{
    NwccreaturestatsTrace1();
    int nLevel = 0;
    for (int i = 0; i < m_nClassCount; i++)
    {
        nLevel += GetClassLevel((unsigned char)i);
    }
    if (m_pCreature != NULL)
    {
        CSWSCreature* pServerCreature = m_pCreature->GetServerCreature();
        if (pServerCreature != NULL && !pServerCreature->m_pStats->GetIsPC() &&
            !pServerCreature->m_bInParty && pServerCreature->m_c250 != 0)
        {
            int nAutoBalanceRow = pServerCreature->m_c250;
            if (nAutoBalanceRow != 0)
            {
                C2DA* pAutoBalance = g_pRules->Get2DArrays()->m_pAutoBalance;
                float fLevelMult;
                {
                    CExoString sColumn("LevelMult");
                    pAutoBalance->GetFLOATEntry(nAutoBalanceRow, sColumn, &fLevelMult);
                }
                char nPCLevel = pServerCreature->m_c251;
                if (nPCLevel == 0)
                {
                    CExoString sVariable("G_PC_LEVEL");
                    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(
                        sVariable, nPCLevel);
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
    }
    return nLevel;
}
// PC 0x006FC440
CExoString CSWCCreatureStats::GetClassesString(char cSeparator)
{
    NwccreaturestatsTrace1();
    CExoString sClasses;
    char szSeparator[2];
    szSeparator[0] = 0;
    szSeparator[1] = 0;

    for (int i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL)
        {
            sClasses = sClasses + szSeparator + pClass->GetNameText() +
                       " (" + GetClassLevel(i) + ")";
            szSeparator[0] = cSeparator;
        }
    }

    return sClasses;
}
// PC 0x006FC6B0
int CSWCCreatureStats::HasClassSkillFeat(unsigned short nSkill)
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
        return FeatAcquired(nFeat);
    }
    return 0;
}
// PC 0x006FC730
unsigned short CSWCCreatureStats::GetFeat(unsigned short nIndex)
{
    NwccreaturestatsTrace1();
    if (nIndex >= m_lstFeats.num)
    {
        return 0xFFFF;
    }
    return m_lstFeats.element[nIndex];
}
// PC 0x006FD6B0
void CSWCCreatureStats::SetFeatRemainingUses(unsigned short nFeat,
                                             unsigned char nUses)
{
    NwccreaturestatsTrace2();
    for (int i = 0; i < m_lstFeatUses.num; i++)
    {
        if (m_lstFeatUses.element[i]->m_nFeat == nFeat)
        {
            m_lstFeatUses.element[i]->m_nUses = nUses;
            return;
        }
    }
    CFeatUseListEntry* pUse = new CFeatUseListEntry;
    pUse->m_nFeat = nFeat;
    pUse->m_nUses = nUses;
    m_lstFeatUses.Add(pUse);
}
// PC 0x006FD7A0
void CSWCCreatureStats::ClearKnownSpells()
{
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        m_aClassInfo[i].ClearKnownSpells();
    }
}
// PC 0x006FD800
void CSWCCreatureStats::ClearKnownSpells(unsigned char nClass)
{
    if (nClass < 2)
    {
        m_aClassInfo[nClass].ClearKnownSpells();
    }
}
// PC 0x006FD820
unsigned long CSWCCreatureStats::GetKnownSpell(unsigned char nClass,
                                               unsigned char nLevel,
                                               unsigned char nIndex)
{
    NwccreaturestatsTrace2();
    if (nClass >= 2)
    {
        return 0xFFFFFFFF;
    }
    return m_aClassInfo[nClass].GetKnownSpell(nLevel, nIndex);
}

inline unsigned long CSWCCreatureStats_ClassInfo::GetKnownSpell(unsigned char nLevel,
                                                                 unsigned char nIndex)
{
    if (nLevel >= SPELL_LEVEL_COUNT)
    {
        return 0xFFFFFFFF;
    }
    if (nIndex >= m_lstKnownSpells[nLevel].num)
    {
        return 0xFFFFFFFF;
    }
    return m_lstKnownSpells[nLevel][nIndex];
}
// PC 0x006FD970
CSWCCreatureStats_ClassInfo::~CSWCCreatureStats_ClassInfo()
{
    ClearKnownSpells();
}
// PC 0x006FC760
CExoString CSWCCreatureStats::GetAlignmentString(int nAlignment)
{
    NwccreaturestatsTrace1();
    STR_RES strRes;

    unsigned long nStrRef;
    switch (GetSimpleAlignmentGoodEvil())
    {
    case CSWSCreatureStats::ALIGNMENT_GOOD:
        nStrRef = 0;
        break;
    case CSWSCreatureStats::ALIGNMENT_EVIL:
        nStrRef = 0;
        break;
    default:
        nStrRef = 0;
        break;
    }

    g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x006FC820
unsigned short CSWCCreatureStats::GetNumberKnownSpells(unsigned char nClass,
                                                       unsigned char nLevel)
{
    NwccreaturestatsTrace1();
    if (nClass >= 2)
    {
        return 0;
    }
    return m_aClassInfo[nClass].m_lstKnownSpells[nLevel].num;
}
// PC 0x006FC850
unsigned char CSWCCreatureStats::GetSpellsPerDayLeft(unsigned char nClass,
                                                     unsigned char nLevel)
{
    NwccreaturestatsTrace1();
    if (nClass >= 2)
    {
        return 0;
    }

    if (nLevel >= 1)
    {
        return 0;
    }
    return m_aClassInfo[nClass].m_aSpellsPerDayLeft[nLevel];
}
// PC 0x005F0720
void CSWCCreatureStats_ClassInfo::AddKnownSpell(unsigned char nLevel,
                                               unsigned long nSpell)
{
    NwccreaturestatsTrace2();
    if (nLevel < 1)
    {
        if (m_lstKnownSpells[nLevel].IndexOf(nSpell) == -1)
        {
            m_lstKnownSpells[nLevel].Add(nSpell);
        }
    }
}
// PC 0x006FC880
char CSWCCreatureStats::GetSkillModifier(unsigned short nSkill)
{
    switch (g_pRules->m_pSkills[nSkill].m_nKeyAbility)
    {
    case 0:
        return GetSTRMod();
    case 1:
        return GetDEXMod();
    case 2:
        return GetCONMod();
    case 3:
        return GetINTMod();
    case 4:
        return GetWISMod();
    case 5:
        return GetCHAMod();
    }
    return 0;
}
// PC 0x006FC900
void CSWCCreatureStats_ClassInfo::ClearKnownSpells()
{
    NwccreaturestatsTrace1();
    for (int i = m_lstKnownSpells[0].num - 1; i >= 0; i--)
    {
        m_lstKnownSpells[0].DelIndex(i);
    }
}
// PC 0x006FD9E0
void CSWCLevelUpStats::SetCONBase(unsigned char nValue)
{
    m_aAbilities[2] = nValue;
    m_nConstitution =
        nValue + g_pRules->m_pRaces[m_nRace].m_nConstitutionAdjust;
    if (GetLevel() == 1 && m_nExperience == 0)
    {
        unsigned char nIndex = m_nClassCount - 1;
        if (&g_pRules->m_pClasses[GetClass(nIndex)] != NULL)
        {
            short nBase = m_nHitPointsBase;
            short nHitPoints = nBase + (char)GetCONMod();
            if (nHitPoints < 1)
            {
                nHitPoints = 1;
            }
            m_nMaxHitPoints = nHitPoints;
            m_nCurrentHitPoints = nHitPoints;
        }
    }
    SetFortSavingThrow();
}
// PC 0x006FDA80
void CSWCLevelUpStats::SetWISBase(unsigned char nValue)
{
    m_aAbilities[4] = nValue;
    m_nWisdom = nValue + g_pRules->m_pRaces[m_nRace].m_nWisdomAdjust;
    SetWillSavingThrow();
}
// PC 0x006FC930
void CSWCLevelUpStats::SetSkillRankBase(unsigned short nSkill, char nRank)
{
    if (nSkill < g_pRules->m_nNumSkills)
    {
        CSWCCreatureStats::SetSkillRankBase(nSkill, nRank);
        if (!g_pRules->m_pSkills[nSkill].m_bUntrained &&
            GetSkillRankBase(nSkill) == 0)
        {
            SetSkillRank(nSkill, 0);
        }
        else
        {
            char nModifier;
            switch (g_pRules->m_pSkills[nSkill].m_nKeyAbility)
            {
            case CSWSkill::KEY_ABILITY_STRENGTH:
                nModifier = GetSTRMod();
                break;
            case CSWSkill::KEY_ABILITY_DEXTERITY:
                nModifier = GetDEXMod();
                break;
            case CSWSkill::KEY_ABILITY_CONSTITUTION:
                nModifier = GetCONMod();
                break;
            case CSWSkill::KEY_ABILITY_INTELLIGENCE:
                nModifier = GetINTMod();
                break;
            case CSWSkill::KEY_ABILITY_WISDOM:
                nModifier = GetWISMod();
                break;
            case CSWSkill::KEY_ABILITY_CHARISMA:
                nModifier = GetCHAMod();
                break;
            }
            SetSkillRank(nSkill, GetSkillRankBase(nSkill) + nModifier);
        }
    }
}
// PC 0x006FCA50
void CSWCLevelUpStats::AddSkillPointsPerLevel()
{
    CSWClass* pClass = &g_pRules->m_pClasses[GetClass(m_nClassCount - 1)];
    if (pClass != NULL)
    {
        if (GetLevel() == 1)
        {
            m_nSkillPoints = FIRST_LEVEL_SKILL_POINT_MULTIPLIER *
                ((pClass->GetSkillPointBase() + GetBaseINTMod()) >
                 MIN_SKILL_POINTS_PER_LEVEL ?
                 (pClass->GetSkillPointBase() + GetBaseINTMod()) :
                 MIN_SKILL_POINTS_PER_LEVEL);
        }
        else
        {
            m_nSkillPoints +=
                ((pClass->GetSkillPointBase() + GetBaseINTMod()) >
                 MIN_SKILL_POINTS_PER_LEVEL ?
                 (pClass->GetSkillPointBase() + GetBaseINTMod()) :
                 MIN_SKILL_POINTS_PER_LEVEL);
        }
    }
}
// PC 0x006FCC70
void CSWCLevelUpStats::SetFortSavingThrow()
{
    char nBonus = 0;
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetFortSaveBonus(GetClassLevel(i));
        }
    }
    m_nFortitudeSave = nBonus + GetCONMod();
}
// PC 0x006FCD20
void CSWCLevelUpStats::SetWillSavingThrow()
{
    char nBonus = 0;
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetWillSaveBonus(GetClassLevel(i));
        }
    }
    m_nWillSave = nBonus + GetWISMod();
}
// PC 0x006FCDD0
void CSWCLevelUpStats::SetReflexSavingThrow()
{
    char nBonus = 0;
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL)
        {
            nBonus += pClass->GetRefSaveBonus(GetClassLevel(i));
        }
    }
    m_nReflexSave = nBonus + GetDEXMod();
}
// PC 0x006FCE90
int CSWCLevelUpStats::IncreaseSkillRank(unsigned short nSkill)
{
    int bUseable = 0;
    int bClassSkill = 0;
    CSWSkill* pSkill = &g_pRules->m_pSkills[nSkill];
    if (pSkill == NULL)
    {
        return 0;
    }

    if (pSkill->m_bAllClassesCanUse)
    {
        bUseable = 1;
    }

    for (int i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL && pClass->IsSkillUseable(nSkill))
        {
            bUseable = 1;
            if (pClass->IsSkillClassSkill(nSkill) || HasClassSkillFeat(nSkill))
            {
                bClassSkill = 1;
                break;
            }
        }
    }

    if (!bClassSkill)
    {
        if (!bUseable || m_nSkillPoints < CROSS_CLASS_SKILL_POINT_COST)
            return 0;
    }
    else if (m_nSkillPoints < CLASS_SKILL_POINT_COST)
    {
        return 0;
    }

    unsigned char nMaxRank = GetLevel() + SKILL_RANK_LEVEL_BONUS;
    if (!bClassSkill)
    {
        nMaxRank /= 2;
    }
    if (GetSkillRankBase(nSkill) >= nMaxRank)
    {
        return 0;
    }

    SetSkillRankBase(nSkill, GetSkillRankBase(nSkill) + 1);
    if (bClassSkill)
    {
        m_nSkillPoints--;
    }
    else
    {
        m_nSkillPoints -= 2;
    }
    return 1;
}
// PC 0x006FD010
void CSWCLevelUpStats::DecreaseSkillRank(unsigned short nSkill,
                                       unsigned char nMinimum)
{
    int bUseable = 0;
    int bClassSkill = 0;
    CSWSkill* pSkill = &g_pRules->m_pSkills[nSkill];
    if (pSkill == NULL)
    {
        return;
    }

    if (pSkill->m_bAllClassesCanUse)
    {
        bUseable = 1;
    }

    for (int i = 0; i < m_nClassCount; i++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(i)];
        if (pClass != NULL && pClass->IsSkillUseable(nSkill))
        {
            bUseable = 1;
            if (pClass->IsSkillClassSkill(nSkill) || HasClassSkillFeat(nSkill))
            {
                bClassSkill = 1;
                break;
            }
        }
    }

    if (bUseable && GetSkillRankBase(nSkill) != nMinimum)
    {
        SetSkillRankBase(nSkill, GetSkillRankBase(nSkill) - 1);
        if (bClassSkill)
        {
            m_nSkillPoints++;
        }
        else
        {
            m_nSkillPoints += 2;
        }
    }
}
// PC 0x006FD150
void CSWCLevelUpStats::CalcNumberFeats(unsigned char& nGeneral,
                                       unsigned char& nBonus)
{
    NwccreaturestatsTrace1();
    CSWClass* pClass = &g_pRules->m_pClasses[GetClassLeveledUpIn()];
    unsigned char nLevel = GetClassLevel(m_nClassCount - 1);

    nGeneral = nLevel - 1 < 50 ? pClass->m_aFeatGain[nLevel - 1] : 0;
    nBonus = nLevel - 1 < 50 ? pClass->m_aBonusFeatGain[nLevel - 1] : 0;
}
// PC 0x006FD1E0
unsigned char CSWCLevelUpStats::ComputeNumberKnownSpellsLeft()
{
    NwccreaturestatsTrace1();
    unsigned char nClass = GetClass(m_nClassCount - 1);
    CSWClass* pClass = &g_pRules->m_pClasses[nClass];
    if (pClass == NULL)
    {
        return 0;
    }
    if (nClass != CSWClass::CCLASS_JEDICONSULAR &&
        nClass != CSWClass::CCLASS_JEDIGUARDIAN &&
        nClass != CSWClass::CCLASS_JEDISENTINEL &&
        nClass != CSWClass::CCLASS_JEDIWATCHMAN &&
        nClass != CSWClass::CCLASS_JEDIWEAPONMASTER &&
        nClass != CSWClass::CCLASS_JEDIMASTER &&
        nClass != CSWClass::CCLASS_SITHASSASSIN &&
        nClass != CSWClass::CCLASS_SITHMARAUDER &&
        nClass != CSWClass::CCLASS_SITHLORD)
    {
        return 0;
    }
    unsigned char nSpells = pClass->GetSpellGain(GetClassLevel(m_nClassCount - 1), 0);
    if (nSpells == CSWClass::SPELL_GAIN_INVALID)
    {
        return 0;
    }
    return nSpells;
}
// PC 0x006FD2B0
void CSWCLevelUpStats::SetNormalBonusFlags(unsigned short nFeat, int& bNormal,
                                           int& bBonus)
{
    NwccreaturestatsTrace1();
    bNormal = 0;
    bBonus = 0;
    if (g_pRules->GetFeat(nFeat) != NULL)
    {
        unsigned char nIndex = m_nClassCount - 1;
        CSWClass* pClass = &g_pRules->m_pClasses[GetClass(nIndex)];
        if (pClass != NULL)
        {
            if (pClass->IsNormalFeat(nFeat) != 0)
            {
                bNormal = 1;
            }
            if (pClass->IsBonusFeat(nFeat) != 0)
            {
                bBonus = 1;
            }
        }
    }
}
// PC 0x006FD340
void CSWCLevelUpStats::AddDefaultPackageSkills(int nClass, int bAllSkills)
{
    CSWClass* pClass = &g_pRules->m_pClasses[(unsigned char)nClass];
    int nSkillPoints = m_nSkillPoints;
    int nNumSkills = g_pRules->m_nNumSkills;

    for (int i = 1; nSkillPoints > 0 && i <= nNumSkills; i++)
    {
        int nSkill = pClass->GetRecommendedSkill(i);
        if (nSkill != -1)
        {
            if (!bAllSkills)
            {
                CSWSkill* pSkill = &g_pRules->m_pSkills[(unsigned short)nSkill];
                int bDroidException = 0;
                if (nSkill == CSWSkill::SKILL_STEALTH)
                {
                    CSWCCreature* pCreature = m_pCreature;
                    if (pCreature->GetServerCreature()->GetTag().CompareNoCase("g0t0"))
                    {
                        bDroidException = 1;
                    }
                }
                if (!pSkill->m_bNPCCanUse)
                {
                    continue;
                }
                if (m_nRace == CSWRace::CRACE_DROID &&
                    !pSkill->m_bDroidCanUse && !bDroidException)
                {
                    continue;
                }
            }
            while (IncreaseSkillRank((unsigned short)nSkill))
            {
            }
            nSkillPoints = m_nSkillPoints;
        }
    }
}
// PC 0x006FEA50
void CSWCCreatureStats::RemoveKnownSpell(unsigned char nClass,
                                         unsigned long nSpell)
{
    NwccreaturestatsTrace3();
    if (nClass < 2)
    {
        if (g_pRules->m_pSpellArray->GetSpell(nSpell) != NULL)
        {
            m_aClassInfo[nClass].RemoveKnownSpell(0, nSpell);
        }
    }
}
// PC 0x006FEAC0
CSWCCreatureStats_ClassInfo::CSWCCreatureStats_ClassInfo()
{
    NwccreaturestatsTrace3();
    m_nClass = 0;
    m_nLevel = 0;
    m_nUnknown19 = 0;
    m_nUnknown1A = 0xFF;
    m_nUnknown1B = 0xFF;
    memset(m_aSpellsPerDayLeft, 0, sizeof(m_aSpellsPerDayLeft));
}

void CSWCCreatureStats_ClassInfo::RemoveKnownSpell(unsigned char nLevel,
                                                   unsigned long nSpell)
{
    if (nLevel < SPELL_LEVEL_COUNT)
    {
        if (m_lstKnownSpells[nLevel].IndexOf(nSpell) >= 0)
        {
            m_lstKnownSpells[nLevel].Remove(nSpell);
        }
    }
}
// PC 0x006FEB40
void CSWCLevelUpStats::SetSTRBase(unsigned char nValue)
{
    m_aAbilities[0] = nValue;
    m_nStrength = nValue + g_pRules->m_pRaces[m_nRace].m_nStrengthAdjust;
    InitializeCombatInformation();
}
// PC 0x006FEB70
void CSWCLevelUpStats::SetDEXBase(unsigned char nValue, int bUpdate)
{
    int nOldArmor = m_nDefenseBase - (char)(m_nDexterityBonus +
                                            (int)floor(m_nDexterity * 0.5) - 5);
    unsigned short nRace = m_nRace;
    m_aAbilities[1] = nValue;
    m_nDexterity = nValue + g_pRules->m_pRaces[nRace].m_nDexterityAdjust;
    SetReflexSavingThrow();
    if (bUpdate != 0)
    {
        m_nDefenseBase = nOldArmor + (char)(m_nDexterityBonus +
                                            (int)floor(m_nDexterity * 0.5) - 5);
    }
    else
    {
        m_nDefenseBase =
            (char)(m_nDexterityBonus + (int)floor(m_nDexterity * 0.5) - 5) + 10;
    }
    InitializeCombatInformation();
}
// PC 0x006FEC60
void CSWCLevelUpStats::SetClass(unsigned char nIndex, unsigned char nClass)
{
    if (nIndex < 2)
    {
        m_aClassInfo[nIndex].m_nClass = nClass;
        SetReflexSavingThrow();
        SetWillSavingThrow();
        SetFortSavingThrow();
        InitializeCombatInformation();
    }
}
// PC 0x006FECA0
void CSWCLevelUpStats::SetClassLevel(unsigned char nIndex, unsigned char nLevel)
{
    if (nIndex < 2)
    {
        m_aClassInfo[nIndex].m_nLevel = nLevel;
        SetReflexSavingThrow();
        SetWillSavingThrow();
        SetFortSavingThrow();
        InitializeCombatInformation();
    }
}
// PC 0x006FECE0
int CSWCLevelUpStats::FeatRequirementsMet(unsigned short nFeat,
                                         CExoArrayList<unsigned short>* pList)
{
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
    if (pFeat == NULL)
    {
        return 0;
    }

    CSWClass* pLeveledClass = &g_pRules->m_pClasses[GetClassLeveledUpIn()];
    unsigned char nGrantedLevel = pLeveledClass->GetLevelFeatGranted(nFeat);
    if (nGrantedLevel != CSWClass::FEAT_GRANTED_LEVEL_INVALID &&
        nGrantedLevel > GetLevel())
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
                unsigned char nCharisma = m_aAbilities[ABILITY_CHARISMA];
                unsigned char nSpellLevel = pFeat->m_nMinSpellLevel;
                unsigned short nRace = m_nRace;
                unsigned char nClassLevel = GetClassLevel(i);
                if (pClass->GetSpellsKnownPerLevel(nClassLevel,
                        nSpellLevel, nClass, nRace, nCharisma) != 0)
                {
                    bSpellLevelMet = 1;
                }
            }
            i++;
        }
        if (bSpellLevelMet == 0)
        {
            return 0;
        }
    }

    unsigned char nMinCharLevel = pFeat->m_nMinCharLevel;
    if (nMinCharLevel > GetLevel())
    {
        return 0;
    }
    unsigned char nMinAttackBonus = pFeat->m_nMinAttackBonus;
    if (nMinAttackBonus > GetBaseAttackBonus())
    {
        return 0;
    }
    if (pFeat->m_nMinStrength > m_aAbilities[ABILITY_STRENGTH])
    {
        return 0;
    }
    if (pFeat->m_nMinDexterity > m_aAbilities[ABILITY_DEXTERITY])
    {
        return 0;
    }
    if (pFeat->m_nMinIntelligence > m_aAbilities[ABILITY_INTELLIGENCE])
    {
        return 0;
    }
    if (pFeat->m_nMinWisdom > m_aAbilities[ABILITY_WISDOM])
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
// PC 0x006FEEC0
void CSWCLevelUpStats::AddGrantedPowers(CExoArrayList<unsigned long>* pList)
{
    unsigned char nClassLevel = GetClassLevel(m_nClassCount - 1);
    unsigned char nJediLevel = 0;
    for (int i = 0; i < m_nClassCount; i++)
    {
        if (CSWClass::IsJedi(GetClass(i)))
        {
            nJediLevel += GetClassLevel(i);
        }
    }
    for (unsigned char j = 0; j < m_nClassCount; j++)
    {
        unsigned long nSpell = CSWSpell::SPELL_INVALID;
        switch (GetClass(j))
        {
        case CLASS_TYPE_JEDIGUARDIAN:
            switch (nJediLevel)
            {
            case 11:
                nSpell = CSWSpell::FORM_SABER_I_SHII_CHO;
                break;
            case 12:
                nSpell = CSWSpell::FORM_SABER_II_MAKASHI;
                break;
            case 13:
                nSpell = CSWSpell::FORM_SABER_III_SORESU;
                break;
            case 14:
                nSpell = CSWSpell::FORM_FORCE_I_FOCUS;
                break;
            }
            break;
        case CLASS_TYPE_JEDICONSULAR:
            switch (nJediLevel)
            {
            case 11:
                nSpell = CSWSpell::FORM_SABER_I_SHII_CHO;
                break;
            case 12:
                nSpell = CSWSpell::FORM_FORCE_I_FOCUS;
                break;
            case 13:
                nSpell = CSWSpell::FORM_SABER_II_MAKASHI;
                break;
            case 14:
                nSpell = CSWSpell::FORM_SABER_III_SORESU;
                break;
            }
            break;
        case CLASS_TYPE_JEDISENTINEL:
            switch (nJediLevel)
            {
            case 11:
                nSpell = CSWSpell::FORM_SABER_I_SHII_CHO;
                break;
            case 12:
                nSpell = CSWSpell::FORM_SABER_III_SORESU;
                break;
            case 13:
                nSpell = CSWSpell::FORM_FORCE_I_FOCUS;
                break;
            case 14:
                nSpell = CSWSpell::FORM_SABER_II_MAKASHI;
                break;
            }
            break;
        case CLASS_TYPE_JEDIWATCHMAN:
        case CLASS_TYPE_SITHASSASSIN:
            switch (nClassLevel)
            {
            case 1:
                nSpell = CSWSpell::FORCE_POWER_FORCE_CAMOUFLAGE;
                break;
            case 7:
                nSpell = CSWSpell::FORCE_POWER_IMPROVED_FORCE_CAMOUFLAGE;
                break;
            case 13:
                nSpell = CSWSpell::FORCE_POWER_MASTER_FORCE_CAMOUFLAGE;
                break;
            }
            break;
        case CLASS_TYPE_SITHLORD:
            switch (nClassLevel)
            {
            case 1:
                nSpell = CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_I;
                break;
            case 5:
                nSpell = CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_II;
                break;
            case 9:
                nSpell = CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_III;
                break;
            case 13:
                nSpell = CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_IV;
                break;
            case 17:
                nSpell = CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_V;
                break;
            }
            break;
        case CLASS_TYPE_JEDIMASTER:
            switch (nClassLevel)
            {
            case 1:
                nSpell = CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_I;
                break;
            case 5:
                nSpell = CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_II;
                break;
            case 9:
                nSpell = CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_III;
                break;
            case 13:
                nSpell = CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_IV;
                break;
            case 17:
                nSpell = CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_V;
                break;
            }
            break;
        case CLASS_TYPE_SITHMARAUDER:
            switch (nClassLevel)
            {
            case 1:
                nSpell = CSWSpell::FORCE_POWER_FURY;
                break;
            case 5:
                nSpell = CSWSpell::FORCE_POWER_IMPROVED_FURY;
                break;
            case 9:
                nSpell = CSWSpell::FORCE_POWER_MASTER_FURY;
                break;
            }
            break;
        }
        if (nSpell != CSWSpell::SPELL_INVALID)
        {
            AddKnownSpell(m_nClassCount - 1, nSpell);
            if (pList != NULL)
            {
                pList->Add(nSpell);
            }
        }
    }
}
// PC 0x006FF2A0
int CSWCLevelUpStats::CanChooseFeat(unsigned short nFeat, unsigned char nNormal,
                                  unsigned char nBonus,
                                  CExoArrayList<unsigned short>* pList)
{
    int bNormal = 0;
    int bBonus = 0;
    if (nFeat == CSWFeat::FEAT_COMPLEX_UNARMED_ANIMS || nFeat == CSWFeat::FEAT_MINE_IMMUNITY)
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
    if (GetTag().CompareNoCase("baodur") && pFeat->m_bBaoDur == 0xff)
    {
        return 0;
    }
    if (GetTag().CompareNoCase("kreia") && pFeat->m_bKreia == 0xff)
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
    if (!FeatRequirementsMet(nFeat, pList))
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
        if (!bNormal && bBonus && nBonusLeft)
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
        int bFeatAssigned = lstAssigned[m];
        if (!bFeatAssigned)
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
// PC 0x006FDAB0
int CSWCLevelUpStats::HasSpellPrereq(CSWSpell* pSpell,
                                    CExoArrayList<unsigned long>* pList)
{
    unsigned long nSpell = pSpell->m_aMasterSpells[0];
    if (nSpell != CSWSpell::SPELL_INVALID)
    {
        int* pMaster = pSpell->m_aMasterSpells;
        do
        {
            if (!HasSpell(0, nSpell) && pList->IndexOf(nSpell) == -1)
            {
                return 0;
            }
            nSpell = *++pMaster;
        } while (nSpell != CSWSpell::SPELL_INVALID);
    }
    nSpell = pSpell->m_aPrerequisites[0];
    if (nSpell != CSWSpell::SPELL_INVALID)
    {
        int* pPrereq = pSpell->m_aPrerequisites;
        do
        {
            if (!HasSpell(0, nSpell) && pList->IndexOf(nSpell) == -1)
            {
                return 0;
            }
            nSpell = *++pPrereq;
        } while (nSpell != CSWSpell::SPELL_INVALID);
    }
    unsigned char nSpellLevel = pSpell->GetSpellLevel(GetClass(m_nClassCount - 1));
    return GetLevel() >= nSpellLevel;
}
// PC 0x006FDBB0
int CSWCLevelUpStats::HasFeatPrereq(CSWFeat* pFeat,
                                   CExoArrayList<unsigned short>* pList)
{
    unsigned short nFeat = pFeat->m_nPrereqFeat1;
    if (nFeat != CSWFeat::FEAT_INVALID && FeatAcquired(nFeat, pList) == 0)
    {
        return 0;
    }
    nFeat = pFeat->m_nPrereqFeat2;
    if (nFeat != CSWFeat::FEAT_INVALID && FeatAcquired(nFeat, pList) == 0)
    {
        return 0;
    }

    int bHasOrPrereq = 0;
    int bOrPrereqAcquired = 0;
    int i = 0;
    do
    {
        if (i >= 5)
        {
            break;
        }
        nFeat = pFeat->GetOrPrereqFeat((unsigned char)i);
        if (nFeat != CSWFeat::FEAT_INVALID)
        {
            bHasOrPrereq = 1;
            if (FeatAcquired(nFeat, pList) != 0)
            {
                bOrPrereqAcquired = 1;
            }
        }
        i++;
    } while (bOrPrereqAcquired == 0);
    if (bHasOrPrereq != 0 && bOrPrereqAcquired == 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x006FDC60
void CSWCLevelUpStats::AddGrantedFeats(CExoArrayList<unsigned long>* pList,
                                       bool bPCGranted)
{
    int nTotalLevel = 0;
    for (int nClass = 0; nClass < GetNumClasses(); nClass++)
    {
        nTotalLevel += GetClassLevel(nClass);
    }
    int nJediLevel = 0;
    for (int nJediClass = 0; nJediClass < m_nClassCount; nJediClass++)
    {
        if (CSWClass::IsJedi(GetClass(nJediClass)))
        {
            nJediLevel += GetClassLevel(nJediClass);
        }
    }
    CSWRace* pRace = &g_pRules->m_pRaces[m_nRace];
    unsigned short nNumFeats;
    for (int i = 0; i < (nNumFeats = g_pRules->m_nNumFeats); i++)
    {
        CSWFeat* pFeat = g_pRules->GetFeat(i);
        if (pFeat != NULL)
        {
            if (GetLevel() == 1 && m_nExperience == 0 &&
                pRace->IsFirstLevelGrantedFeat(i))
            {
                m_lstFeats.AddUnique(i);
                if (pList != NULL)
                {
                    pList->Add(i);
                }
            }
            else
            {
                CSWClass* pClass =
                    &g_pRules->m_pClasses[GetClassLeveledUpIn()];
                unsigned char nGrantedLevel;
                if (pFeat != NULL && pClass != NULL)
                {
                    if (pClass->IsGrantedFeat(i, nGrantedLevel))
                    {
                        if (i >= CSWFeat::FEAT_UNARMED_SPECIALIST_I &&
                            i <= CSWFeat::FEAT_UNARMED_SPECIALIST_VIII)
                        {
                            if (CSWClass::IsJedi(GetClassLeveledUpIn()) &&
                                nGrantedLevel == nJediLevel)
                            {
                                m_lstFeats.AddUnique(i);
                                if (pList != NULL)
                                {
                                    pList->Add(i);
                                }
                            }
                        }
                        else if (nGrantedLevel ==
                                 GetClassLevel(m_nClassCount - 1))
                        {
                            if (GetTag().CompareNoCase("baodur") &&
                                pFeat->m_bBaoDur == 0xff)
                            {
                                continue;
                            }
                            if (GetTag().CompareNoCase("kreia") &&
                                pFeat->m_bKreia == 0xff)
                            {
                                continue;
                            }
                            m_lstFeats.AddUnique(i);
                            if (i != CSWFeat::FEAT_COMPLEX_UNARMED_ANIMS &&
                                i != CSWFeat::FEAT_MINE_IMMUNITY)
                            {
                                if (pList != NULL)
                                {
                                    pList->Add(i);
                                }
                            }
                        }
                    }
                    else if (pClass->IsPCGrantedFeat(i) && bPCGranted)
                    {
                        m_lstFeats.AddUnique(i);
                        if (pList != NULL)
                        {
                            pList->Add(i);
                        }
                    }
                    else if (GetTag().CompareNoCase("handmaiden"))
                    {
                        if (pFeat->m_bHandmaiden > 0 &&
                            pFeat->m_bHandmaiden == nTotalLevel)
                        {
                            m_lstFeats.AddUnique(i);
                            if (pList != NULL)
                            {
                                pList->Add(i);
                            }
                        }
                    }
                    else if (GetTag().CompareNoCase("baodur"))
                    {
                        if (pFeat->m_bBaoDur > 0 &&
                            pFeat->m_bBaoDur == nTotalLevel)
                        {
                            m_lstFeats.AddUnique(i);
                            if (pList != NULL)
                            {
                                pList->Add(i);
                            }
                        }
                    }
                    else if (GetTag().CompareNoCase("hanharr"))
                    {
                        if (pFeat->m_bHanharr > 0 &&
                            pFeat->m_bHanharr == nTotalLevel)
                        {
                            m_lstFeats.AddUnique(i);
                            if (pList != NULL)
                            {
                                pList->Add(i);
                            }
                        }
                    }
                    else if (GetTag().CompareNoCase("hk47"))
                    {
                        if (pFeat->m_bHK47 > 0 &&
                            pFeat->m_bHK47 == nTotalLevel)
                        {
                            m_lstFeats.AddUnique(i);
                            if (pList != NULL)
                            {
                                pList->Add(i);
                            }
                        }
                    }
                    else if (GetTag().CompareNoCase("g0t0"))
                    {
                        if (pFeat->m_bG0T0 > 0 &&
                            pFeat->m_bG0T0 == nTotalLevel)
                        {
                            m_lstFeats.AddUnique(i);
                            if (pList != NULL)
                            {
                                pList->Add(i);
                            }
                        }
                    }
                    else if (GetTag().CompareNoCase("atton"))
                    {
                        if (pFeat->m_bAtton > 0 &&
                            pFeat->m_bAtton == nTotalLevel)
                        {
                            m_lstFeats.AddUnique(i);
                            if (pList != NULL)
                            {
                                pList->Add(i);
                            }
                        }
                    }
                }
            }
        }
    }
    if (bPCGranted)
    {
        if (!HasFeat(CSWFeat::FEAT_WAR_VETERAN))
        {
            m_lstFeats.AddUnique(CSWFeat::FEAT_WAR_VETERAN);
            if (pList != NULL)
            {
                pList->Add(CSWFeat::FEAT_WAR_VETERAN);
            }
        }
    }
}
// PC 0x006FE4C0
void CSWCLevelUpStats::ClearPackage()
{
    m_nSkillPoints = 0;
    ClearSkills();
    UpdateAbilityModifiedSkillRanks();
    ClearFeats();
    ClearKnownSpells();
    AddGrantedFeats(NULL, false);
    while (m_lstInventoryRes.num != 0)
    {
        m_lstInventoryRes.DelIndex(m_lstInventoryRes.num - 1);
    }
}
// PC 0x006FE5A0
int CSWCLevelUpStats::CanChooseSpell(unsigned long nSpell, unsigned char nMax,
                                   CExoArrayList<unsigned long>* pList)
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
        if (HasSpell(0, nSpell))
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
        if (!HasSpellPrereq(pSpell, pList))
        {
            return 0;
        }
        return nMax - pList->num > 0;
    }
}
// PC 0x006FE690
unsigned char CSWCLevelUpStats::AddDefaultPackageSpells(
    CExoArrayList<unsigned long>* pList)
{
    unsigned char nSpells = ComputeNumberKnownSpellsLeft();
    unsigned char nClass = GetClassLeveledUpIn();
    int nChosen = 0;
    int nNumSpells = g_pRules->m_pSpellArray->GetNumSpells();
    CSWClass* pClass = &g_pRules->m_pClasses[nClass];
    int i = 1;

    if (pClass->m_bSpellCaster)
    {
        unsigned char nAlignment = GetSimpleAlignmentGoodEvil();
        int nSide = CSWClass::SPELL_RECOMMEND_LIGHT;
        if (nAlignment == CSWSCreatureStats::ALIGNMENT_EVIL)
        {
            nSide = CSWClass::SPELL_RECOMMEND_DARK;
        }
        if (nChosen < nSpells)
        {
            do
            {
                unsigned short nSpell;
                if (i > nNumSpells)
                {
                    break;
                }
                nSpell = (unsigned short)pClass->GetRecommendedSpell(nSide, i);
                if (nSpell != CSWSpell::SPELL_INVALID)
                {
                    if (CanChooseSpell(nSpell, nSpells, pList))
                    {
                        pList->Add(nSpell);
                        nChosen++;
                    }
                    i++;
                }
            } while (nChosen < nSpells);
        }
        return nSpells;
    }
    return nSpells;
}
// PC 0x006FE7C0
void CSWCCreatureStats::InitializeCombatInformation()
{
    CCombatInformation* pInfo = m_pCombatInformation;
    if (pInfo)
    {
        pInfo->m_nNumAttacks = CalculateNumAttacks();

        int nAttackMod = CalculateAttackModifier();
        if (pInfo)
        {
            pInfo->m_nOnHandAttackMod = nAttackMod;
        }
        pInfo->m_nOnHandDamageMod = GetSTRMod();
        pInfo->m_nUnarmedDamageDice = CalculateUnarmedDamageDice();
        pInfo->m_nUnarmedDamageDie = CalculateUnarmedDamageDie();
        pInfo->m_nOnHandCriticalRange = 1;
    }
}
// PC 0x006FD460
int CSWCLevelUpStats::FeatAcquired(unsigned short nFeat,
                                    CExoArrayList<unsigned short>* pList)
{
    if (nFeat == CSWFeat::FEAT_INVALID)
    {
        return 0;
    }
    if (CSWCCreatureStats::FeatAcquired(nFeat) != 0)
    {
        return 1;
    }
    if (pList != NULL)
    {
        for (unsigned short i = 0; i < pList->num; i++)
        {
            if (nFeat == pList->element[i])
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x006FD4C0
int CSWCCreatureStats::HasSpell(unsigned char nLevel, unsigned long nSpell)
{
    NwccreaturestatsTrace1();
    for (unsigned char i = 0; i < m_nClassCount; i++)
    {
        CSWCCreatureStats_ClassInfo* pInfo = &m_aClassInfo[i];
        for (int j = 0; j < (unsigned short)pInfo->m_lstKnownSpells[nLevel].num;
             j++)
        {
            if (pInfo->m_lstKnownSpells[nLevel].element[j] == nSpell)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x006FD540
int CSWCCreatureStats::HasFeat(unsigned short nFeat)
{
    NwccreaturestatsTrace1();
    int nCount = m_lstFeats.num;
    int nIndex = 0;
    if (nCount > 0)
    {
        unsigned short* pFeat = m_lstFeats.element;
        do
        {
            if (*pFeat == nFeat)
            {
                return 1;
            }
            nIndex++;
            pFeat++;
        } while (nIndex < nCount);
    }
    return 0;
}
// PC 0x006FD580
void CSWCCreatureStats::UpdateAbilityModifiedSkillRanks()
{
    for (unsigned short i = 0; i < g_pRules->m_nNumSkills; i++)
    {
        if (!g_pRules->m_pSkills[i].m_bUntrained &&
            GetSkillRankBase(i) == 0)
        {
            SetSkillRank(i, 0);
        }
        else
        {
            SetSkillRank(i, GetSkillRankBase(i) + GetSkillModifier(i));
        }
    }
}

unsigned char CSWCCreatureStats::CalculateUnarmedDamageDice()
{
    int nSizeCategory;
    g_pRules->m_p2DArrays->m_pAppearance->GetINTEntry(
        g_pRules->m_pRaces[m_nRace].m_nAppearance,
        CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY, &nSizeCategory);
    return 0;
}

unsigned char CSWCCreatureStats::CalculateUnarmedDamageDie()
{
    int nSizeCategory;
    g_pRules->m_p2DArrays->m_pAppearance->GetINTEntry(
        g_pRules->m_pRaces[m_nRace].m_nAppearance,
        CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY, &nSizeCategory);
    return 0;
}
// PC 0x006FF5F0
unsigned char CSWCLevelUpStats::AddDefaultPackageFeats(
    int nClass, CExoArrayList<unsigned short>* pList, int bNormal, int bBonus)
{
    unsigned char nNormal = 0;
    unsigned char nBonus = 0;
    CalcNumberFeats(nNormal, nBonus);
    if (!bNormal)
    {
        nNormal = 0;
    }
    if (!bBonus)
    {
        nBonus = 0;
    }

    unsigned char nFeatsLeft = nBonus + nNormal;
    CSWClass* pClass = &g_pRules->m_pClasses[(unsigned char)nClass];

    for (int i = 1, nNumFeats = g_pRules->m_nNumFeats;
         nFeatsLeft > 0 && i <= nNumFeats; i++)
    {
        unsigned short nFeat = (unsigned short)pClass->GetRecommendedFeat(i);
        if (nFeat != -1 && CanChooseFeat(nFeat, nNormal, nBonus, pList))
        {
            pList->Add(nFeat);
            nFeatsLeft--;
        }
    }
    return nFeatsLeft;
}

// PC 0x006FF6F0
void CSWCCreatureStats::GetMenuInfo(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions, unsigned long nType,
    unsigned long nForcePoints, int nExclusion, unsigned long nItemMask,
    float fDistance, int nRoomForceRating, int nForceBodyLevel, int nHitPoints,
    int bForms, int nForm, int bBeastTarget, char nCharismaModifier)
{
    if (g_pAppManager->m_pServerExoApp->GetModule()->GetArea()->GetPlayerRestrictMode() == 0)
    {
        CExoArrayList<unsigned long> lstKnownSpells;
        for (int nClass = 0; nClass < m_nClassCount; nClass++)
        {
            int nBase = lstKnownSpells.num;
            lstKnownSpells.SetSize(nBase + m_aClassInfo[nClass].GetNumberKnownSpells(0));
            for (int nKnown = 0; nKnown < m_aClassInfo[nClass].GetNumberKnownSpells(0); nKnown++)
            {
                lstKnownSpells[nBase + nKnown] = m_aClassInfo[nClass].m_lstKnownSpells[0][nKnown];
            }
        }

        CExoArrayList<CSWSpell*> lstSpells;
        CExoArrayList<short> lstPriorities;
        CExoArrayList<short> lstSlots;
        CExoArrayList<unsigned long> lstSpellIds;
        int nAlignment = m_nGoodEvilAlignment;

        for (int i = 0; i < lstKnownSpells.num; i++)
        {
            unsigned long nSpellId = lstKnownSpells[i];
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
            if (pSpell != NULL)
            {
                int bAvailable = 1;
                if (pSpell->m_nExclusion & nExclusion)
                {
                    bAvailable = 0;
                }
                if ((nSpellId == CSWSpell::FORCE_POWER_MIND_TRICK ||
                     nSpellId == CSWSpell::FORCE_POWER_CONFUSION) &&
                    bBeastTarget)
                {
                    bAvailable = 0;
                }
                if ((nSpellId == CSWSpell::FORCE_POWER_BEAST_TRICK ||
                     nSpellId == CSWSpell::FORCE_POWER_BEAST_CONFUSION) &&
                    !bBeastTarget)
                {
                    bAvailable = 0;
                }
                char nActive;
                if (nSpellId == CSWSpell::FORCE_POWER_CONFUSION)
                {
                    g_pAppManager->m_pServerExoApp->GetGlobalVariableTable()->GetValueNumber(
                        "000_Human_Conf_Active", nActive);
                    if (nActive > 0)
                    {
                        bAvailable = 0;
                    }
                }
                else if (nSpellId == CSWSpell::FORCE_POWER_BEAST_CONFUSION)
                {
                    g_pAppManager->m_pServerExoApp->GetGlobalVariableTable()->GetValueNumber(
                        "000_Beast_Conf_Active", nActive);
                    if (nActive > 0)
                    {
                        bAvailable = 0;
                    }
                }
                else if (nSpellId == CSWSpell::FORCE_POWER_DROID_CONFUSION)
                {
                    g_pAppManager->m_pServerExoApp->GetGlobalVariableTable()->GetValueNumber(
                        "000_Droid_Conf_Active", nActive);
                    if (nActive > 0)
                    {
                        bAvailable = 0;
                    }
                }
                else if (nSpellId >= CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_I &&
                         nSpellId <= CSWSpell::FORCE_POWER_CRUSH_OPPOSITION_VI)
                {
                    if (nAlignment > 40)
                    {
                        bAvailable = 0;
                    }
                }
                else if (nSpellId >= CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_I &&
                         nSpellId <= CSWSpell::FORCE_POWER_INSPIRE_FOLLOWERS_VI)
                {
                    if (nAlignment < 60)
                    {
                        bAvailable = 0;
                    }
                }
                if (bForms)
                {
                    if (nSpellId < CSWSpell::FORM_SABER_I_SHII_CHO ||
                        nSpellId > CSWSpell::FORM_FORCE_IV_MASTERY)
                    {
                        bAvailable = 0;
                    }
                }
                else
                {
                    if (nSpellId >= CSWSpell::FORM_SABER_I_SHII_CHO &&
                        nSpellId <= CSWSpell::FORM_FORCE_IV_MASTERY)
                    {
                        bAvailable = 0;
                    }
                }
                if (bAvailable)
                {
                    unsigned char nCount = pSpell->m_nSubRadialSpellCount;
                    for (int j = 0; j < nCount; j++)
                    {
                        short nPriority = pSpell->GetPriority(j);
                        short nSlot = pSpell->GetSlot(j);
                        if (pSpell->GetSubRadialSpell(j) / 1000 == nType)
                        {
                            int bInserted = 0;
                            for (int k = 0; k < lstSpells.num && !bInserted; k++)
                            {
                                if (nSlot == 17 && lstSlots[k] == 16)
                                {
                                    lstPriorities[k] = nPriority;
                                    lstSpells[k] = pSpell;
                                    lstSpellIds[k] = nSpellId;
                                    bInserted = 1;
                                    lstSlots[k] = 17;
                                }
                                else if (nSlot == 21 && lstSlots[k] == 20)
                                {
                                    lstPriorities[k] = nPriority;
                                    lstSpells[k] = pSpell;
                                    lstSpellIds[k] = nSpellId;
                                    bInserted = 1;
                                    lstSlots[k] = 21;
                                }
                                else if (lstSlots[k] == nSlot)
                                {
                                    bInserted = 1;
                                    if (lstPriorities[k] < nPriority)
                                    {
                                        lstPriorities[k] = nPriority;
                                        lstSpells[k] = pSpell;
                                        lstSpellIds[k] = nSpellId;
                                    }
                                }
                                if (lstSlots[k] > nSlot)
                                {
                                    bInserted = 1;
                                    lstPriorities.Insert(nPriority, k);
                                    lstSlots.Insert(nSlot, k);
                                    lstSpells.Insert(pSpell, k);
                                    lstSpellIds.Insert(nSpellId, k);
                                }
                            }
                            if (!bInserted)
                            {
                                lstSpells.Add(pSpell);
                                lstPriorities.Add(nPriority);
                                lstSlots.Add(nSlot);
                                lstSpellIds.Add(nSpellId);
                            }
                        }
                    }
                }
            }
        }

        int nAction = lstActions.num;
        for (int iSpell = 0; iSpell < lstSpells.num; nAction++, iSpell++)
        {
            CSWSpell* pSpell = lstSpells[iSpell];
            int bEnabled = 1;
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(pSpell->GetNameStrRef());
            lstActions[nAction].m_nAction =
                lstSpellIds[iSpell] | CSWGuiInterfaceAction::SPELL_ACTION_MASK;
            lstActions[nAction].m_resIcon = pSpell->m_resIcon;
            lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_NONE;
            if (nForceBodyLevel != -1)
            {
                int nPercent = 0;
                int nCost = pSpell->GetAdjustedForcePoints(nAlignment, 1, nRoomForceRating,
                                                           nForm, nCharismaModifier);
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
                int nHitPointCost = nCost * nPercent / 100;
                if (nCost - nHitPointCost > nForcePoints)
                {
                    bEnabled = 0;
                    lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_FORCE_POINTS;
                }
                else if (nHitPoints - nHitPointCost < 1)
                {
                    bEnabled = 0;
                    lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_HIT_POINTS;
                }
            }
            else if (pSpell->GetAdjustedForcePoints(nAlignment, 1, nRoomForceRating, nForm,
                                                    nCharismaModifier) > nForcePoints)
            {
                bEnabled = 0;
                lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_FORCE_POINTS;
            }
            float fRange = GetMinSpellRange(pSpell->m_sRange);
            if (fRange * fRange >= fDistance)
            {
                bEnabled = 0;
                lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_RANGE;
            }
            if (nItemMask & pSpell->m_nForbidItemMask)
            {
                bEnabled = 0;
                lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_FORBIDDEN_ITEM;
            }
            else if (bEnabled && (nItemMask & pSpell->m_nRequireItemMask) != pSpell->m_nRequireItemMask)
            {
                bEnabled = 0;
                lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_REQUIRED_ITEM;
            }
            lstActions[nAction].m_bEnabled = bEnabled;
        }
    }
}

int CSWCCreatureStats::CalculateAttackModifier()
{
    return GetSTRMod();
}

static inline void NwccreaturestatsTrace3()
{
    NwccreaturestatsTrace2();
}

static inline void NwccreaturestatsTrace2()
{
    NwccreaturestatsTrace1();
}

static inline void NwccreaturestatsTrace1()
{
}
