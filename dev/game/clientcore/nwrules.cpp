#include <stdlib.h>
#include <string.h>

#include "nwrules.h"
#include "nwfeat.h"
#include "nwrace.h"
#include "nwclass.h"
#include "resgff.h"
#include "twodimarray.h"
#include "nwitem.h"
#include "nwspell.h"
#include "nwskill.h"

static const int RULES_SPECIAL_RANGE = 5;
static const int RULES_SPECIAL_RANGE_ROW = 19;
static const int RULES_MAX_LEVEL = 50;

#define RULES_RANGES_TABLE "Ranges"
#define RULES_EXPERIENCE_TABLE "EXPTABLE"
// PC 0x005D6300
CCombatInformationNode::CCombatInformationNode()
{
    m_nModifier = 0;
    m_nModifierType = 0;
    m_nWeaponWield = 0;
    m_nVersusGoodEvil = 0;
    m_nVersusRace = g_pRules->m_nNumRaces;
}
// PC 0x005D7E20
CCombatInformation::~CCombatInformation()
{
    m_nUnarmedDamageDie = 3;
    m_nNumAttacks = 0;
    m_nOnHandAttackMod = 0;
    m_nOnHandDamageMod = 0;
    m_nOffHandAttackMod = 0;
    m_nOffHandDamageMod = 0;
    m_nOffHandWeaponEquipped = 0;
    m_nForceResistance = 0;
    m_nArcaneSpellFailure = 0;
    m_nArmorCheckPenalty = 0;
    m_nUnarmedDamageDice = 1;
    m_nOnHandCriticalRange = 0;
    m_nOnHandCriticalMultiplier = 0;
    m_nOffHandCriticalRange = 0;
    m_nOffHandCriticalMultiplier = 2;

    while (m_lstAttacks.num > 0)
    {
        CCombatInformationNode* pNode = m_lstAttacks.element[0];
        m_lstAttacks.DelIndex(0);
        if (pNode != 0)
        {
            delete pNode;
        }
    }

    while (m_lstDamage.num > 0)
    {
        CCombatInformationNode* pNode = m_lstDamage.element[0];
        m_lstDamage.DelIndex(0);
        if (pNode != 0)
        {
            delete pNode;
        }
    }
}
// PC 0x005D7F50
int CCombatInformation::SaveData(CResGFF* pGFF, CResStruct* pStruct)
{
    CResGFF* pCombatGFF = pGFF;
    CResStruct cCombatStruct;
    CResList cList;
    int i;
    int j;

    pCombatGFF->AddStructToStruct(&cCombatStruct, pStruct, "CombatInfo",
                                  0xCAAA);
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nNumAttacks, "NumAttacks");
    pCombatGFF->WriteFieldCHAR(&cCombatStruct, m_nOnHandAttackMod,
                               "OnHandAttackMod");
    pCombatGFF->WriteFieldCHAR(&cCombatStruct, m_nOnHandDamageMod,
                               "OnHandDamageMod");
    pCombatGFF->WriteFieldCHAR(&cCombatStruct, m_nOffHandAttackMod,
                               "OffHandAttackMod");
    pCombatGFF->WriteFieldCHAR(&cCombatStruct, m_nOffHandDamageMod,
                               "OffHandDamageMod");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nForceResistance,
                               "ForceResistance");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nArcaneSpellFailure,
                               "ArcaneSpellFail");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nArmorCheckPenalty,
                               "ArmorCheckPen");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nUnarmedDamageDice,
                               "UnarmedDamDice");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nUnarmedDamageDie,
                               "UnarmedDamDie");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nOnHandCriticalRange,
                               "OnHandCritRng");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nOnHandCriticalMultiplier,
                               "OnHandCritMult");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nOffHandWeaponEquipped,
                               "OffHandWeaponEq");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nOffHandCriticalRange,
                               "OffHandCritRng");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nOffHandCriticalMultiplier,
                               "OffHandCritMult");
    pCombatGFF->WriteFieldDWORD(&cCombatStruct, m_nLeftEquipment, "LeftEquip");
    pCombatGFF->WriteFieldDWORD(&cCombatStruct, m_nRightEquipment,
                                "RightEquip");
    pCombatGFF->WriteFieldCExoString(&cCombatStruct, m_sLeftString,
                                     "LeftString");
    pCombatGFF->WriteFieldCExoString(&cCombatStruct, m_sRightString,
                                     "RightString");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nDamageDice, "DamageDice");
    pCombatGFF->WriteFieldBYTE(&cCombatStruct, m_nDamageDie, "DamageDie");

    pCombatGFF->AddList(&cList, &cCombatStruct, "AttackList");
    for (i = 0; i < m_lstAttacks.num; i++)
    {
        CResStruct cAttackStruct;
        pCombatGFF->AddListElement(&cAttackStruct, &cList, 0xAAEE);
        pCombatGFF->WriteFieldCHAR(
            &cAttackStruct, m_lstAttacks[i]->m_nModifier, "Modifier");
        pCombatGFF->WriteFieldBYTE(&cAttackStruct,
                                   m_lstAttacks[i]->m_nWeaponWield,
                                   "WeaponWield");
        pCombatGFF->WriteFieldBYTE(&cAttackStruct,
                                   m_lstAttacks[i]->m_nVersusGoodEvil,
                                   "VersusGoodEvil");
        pCombatGFF->WriteFieldBYTE(&cAttackStruct,
                                   m_lstAttacks[i]->m_nVersusRace,
                                   "VersusRace");
    }

    pCombatGFF->AddList(&cList, &cCombatStruct, "DamageList");
    for (j = 0; j < m_lstDamage.num; j++)
    {
        CResStruct cDamageStruct;
        pCombatGFF->AddListElement(&cDamageStruct, &cList, 0xDDEE);
        pCombatGFF->WriteFieldCHAR(
            &cDamageStruct, m_lstDamage[j]->m_nModifier, "Modifier");
        pCombatGFF->WriteFieldBYTE(&cDamageStruct,
                                   m_lstDamage[j]->m_nModifierType,
                                   "ModifierType");
        pCombatGFF->WriteFieldBYTE(&cDamageStruct,
                                   m_lstDamage[j]->m_nWeaponWield,
                                   "WeaponWield");
        pCombatGFF->WriteFieldBYTE(&cDamageStruct,
                                   m_lstDamage[j]->m_nVersusGoodEvil,
                                   "VersusGoodEvil");
        pCombatGFF->WriteFieldBYTE(&cDamageStruct,
                                   m_lstDamage[j]->m_nVersusRace,
                                   "VersusRace");
    }

    return 1;
}
// PC 0x005D82F0
int CCombatInformation::operator!=(CCombatInformation& cInfo)
{
    if (m_nNumAttacks != cInfo.m_nNumAttacks ||
        m_nOnHandAttackMod != cInfo.m_nOnHandAttackMod ||
        m_nOnHandDamageMod != cInfo.m_nOnHandDamageMod ||
        m_nOffHandAttackMod != cInfo.m_nOffHandAttackMod ||
        m_nOffHandDamageMod != cInfo.m_nOffHandDamageMod ||
        m_nOffHandWeaponEquipped != cInfo.m_nOffHandWeaponEquipped ||
        m_nForceResistance != cInfo.m_nForceResistance ||
        m_nArcaneSpellFailure != cInfo.m_nArcaneSpellFailure ||
        m_nArmorCheckPenalty != cInfo.m_nArmorCheckPenalty ||
        m_nUnarmedDamageDice != cInfo.m_nUnarmedDamageDice ||
        m_nUnarmedDamageDie != cInfo.m_nUnarmedDamageDie ||
        m_nOnHandCriticalRange != cInfo.m_nOnHandCriticalRange ||
        m_nOnHandCriticalMultiplier != cInfo.m_nOnHandCriticalMultiplier ||
        m_nOffHandCriticalRange != cInfo.m_nOffHandCriticalRange ||
        m_nOffHandCriticalMultiplier != cInfo.m_nOffHandCriticalMultiplier ||
        m_nLeftEquipment != cInfo.m_nLeftEquipment ||
        m_nRightEquipment != cInfo.m_nRightEquipment ||
        m_lstAttacks.num != cInfo.m_lstAttacks.num ||
        m_lstDamage.num != cInfo.m_lstDamage.num)
    {
        return 1;
    }

    for (int i = 0; i < m_lstAttacks.num; i++)
    {
        CCombatInformationNode* pNode = m_lstAttacks[i];
        CCombatInformationNode* pOther = cInfo.m_lstAttacks[i];
        if (*pNode != *pOther)
        {
            return 1;
        }
    }

    for (int i = 0; i < m_lstDamage.num; i++)
    {
        CCombatInformationNode* pNode = m_lstDamage[i];
        CCombatInformationNode* pOther = cInfo.m_lstDamage[i];
        if (*pNode != *pOther)
        {
            return 1;
        }
    }

    return 0;
}
// PC 0x005D9F00
CSWRules::CSWRules()
{
    m_pSpellArray = 0;
    m_nNumFeats = 0;
    m_pFeats = 0;
    m_pFeatIndices = 0;
    m_pBaseItems = 0;
    memset(m_anExperience, 0, sizeof(m_anExperience));
    m_nNumClasses = 0;
    m_pClasses = 0;
    m_nNumRaces = 0;
    m_pRaces = 0;
    m_nNumSkills = 0;
    m_pSkills = 0;
    m_pMasterFeatStrRefs = 0;
    m_pMasterFeatDescriptionStrRefs = 0;
    m_pMasterFeatIcons = 0;

    C2DA* pRanges = new C2DA(RULES_RANGES_TABLE, 0);
    pRanges->Load2DArray();
    float fRange;
    for (int i = 0; i < 6; i++)
    {
        if (i != RULES_SPECIAL_RANGE)
        {
            m_afPrimaryRange[i] = 0.0f;
            if (pRanges->GetFLOATEntry(i, "PrimaryRange", &fRange))
            {
                m_afPrimaryRange[i] = fRange;
            }
            m_afSecondaryRange[i] = 0.0f;
            if (pRanges->GetFLOATEntry(i, "SecondaryRange", &fRange))
            {
                m_afSecondaryRange[i] = fRange;
            }
        }
        else
        {
            m_afPrimaryRange[RULES_SPECIAL_RANGE] = 0.0f;
            if (pRanges->GetFLOATEntry(RULES_SPECIAL_RANGE_ROW, "PrimaryRange", &fRange))
            {
                m_afPrimaryRange[RULES_SPECIAL_RANGE] = fRange;
            }
            m_afSecondaryRange[RULES_SPECIAL_RANGE] = 0.0f;
            if (pRanges->GetFLOATEntry(RULES_SPECIAL_RANGE_ROW, "SecondaryRange", &fRange))
            {
                m_afSecondaryRange[RULES_SPECIAL_RANGE] = fRange;
            }
        }
    }
    delete pRanges;

    m_p2DArrays = new CTwoDimArrays;
#ifdef _XBOX
    EXOXBOXASSERT(m_p2DArrays->Load2DArrays());
#else
    m_p2DArrays->Load2DArrays();
#endif
    m_pSpellArray = new CSWSpellArray;
    m_pSpellArray->Load();
    LoadFeatInfo();
    m_pBaseItems = new CSWBaseItemArray;
    m_pBaseItems->Load();

    C2DA* pExperience = new C2DA(RULES_EXPERIENCE_TABLE, 0);
#ifdef _XBOX
    EXOXBOXASSERT(pExperience->Load2DArray());
#else
    pExperience->Load2DArray();
#endif
    int nExperience;
    for (int i = 0; i <= RULES_MAX_LEVEL; i++)
    {
        pExperience->GetINTEntry(i, "XP", &nExperience);
        m_anExperience[i] = nExperience;
    }
    delete pExperience;

    LoadSkillInfo();
    LoadClassInfo();
    LoadRaceInfo();
    LoadDifficultyInfo();
}
// PC 0x005D6320
int CCombatInformationNode::operator!=(CCombatInformationNode& cNode)
{
    return m_nModifier != cNode.m_nModifier ||
           m_nModifierType != cNode.m_nModifierType ||
           m_nWeaponWield != cNode.m_nWeaponWield ||
           m_nVersusGoodEvil != cNode.m_nVersusGoodEvil ||
           m_nVersusRace != cNode.m_nVersusRace;
}
// PC 0x005D6360
void CSWRules::LoadClassInfo()
{
    C2DA* p2DA = new C2DA("CLASSES", 0);
    p2DA->Load2DArray();

    int nNumRows = p2DA->m_nNumRows;
    m_nNumClasses = nNumRows;
    m_pClasses = new CSWClass[m_nNumClasses];

    for (int i = 0; i < m_nNumClasses; i++)
    {
        CExoString sValue;
        int nStrRef;
        int nDie;
        int nSkillPointBase;
        int nValue;
        int bFound;

        p2DA->GetINTEntry(i, "Name", &nStrRef);
        m_pClasses[i].SetNameStrRef(nStrRef);
        p2DA->GetINTEntry(i, "SkillPointBase", &nSkillPointBase);
        m_pClasses[i].SetSkillPointBase(nSkillPointBase);
        p2DA->GetINTEntry(i, "Lower", &nStrRef);
        m_pClasses[i].SetNameLowerStrRef(nStrRef);
        p2DA->GetINTEntry(i, "Plural", &nStrRef);
        m_pClasses[i].SetNamePluralStrRef(nStrRef);
        p2DA->GetINTEntry(i, "Description", &nStrRef);
        m_pClasses[i].SetDescriptionStrRef(nStrRef);
        p2DA->GetCExoStringEntry(i, "Icon", &sValue);
        m_pClasses[i].SetIcon(sValue);
        p2DA->GetINTEntry(i, "HitDie", &nDie);
        m_pClasses[i].SetHitDie(nDie);
        p2DA->GetINTEntry(i, "ForceDie", &nDie);
        m_pClasses[i].SetForceDie(nDie);

        bFound = p2DA->GetCExoStringEntry(i, "AttackBonusTable", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadAttackBonusTable(sValue);
        }
        bFound = p2DA->GetCExoStringEntry(i, "FeatsTable", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadFeatsTable(sValue, this);
        }
        bFound = p2DA->GetCExoStringEntry(i, "SavingThrowTable", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadSavingThrowTable(sValue);
        }
        bFound = p2DA->GetCExoStringEntry(i, "SkillsTable", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadSkillsTable(sValue);
        }
        bFound = p2DA->GetCExoStringEntry(i, "SpellGainTable", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadSpellGainTable(sValue);
        }
        bFound = p2DA->GetCExoStringEntry(i, "SpellKnownTable", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadSpellKnownTable(sValue);
        }
        bFound = p2DA->GetCExoStringEntry(i, "FeatGain", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadFeatGain(sValue);
        }
        bFound = p2DA->GetCExoStringEntry(i, "ArmorClassColumn", &sValue);
        if (bFound)
        {
            m_pClasses[i].LoadArmorClassTable(sValue);
        }
        m_pClasses[i].LoadSpellsTable();

        bFound = p2DA->GetINTEntry(i, "PlayerClass", &nValue);
        if (bFound)
        {
            m_pClasses[i].SetPlayerClass(nValue != 0);
        }
        bFound = p2DA->GetINTEntry(i, "SpellCaster", &nValue);
        if (bFound)
        {
            m_pClasses[i].SetSpellCaster(nValue != 0);
        }
        bFound = p2DA->GetINTEntry(i, "AlignRestrict", &nValue);
        if (bFound)
        {
            m_pClasses[i].SetAlignRestrict(nValue);
        }
        bFound = p2DA->GetINTEntry(i, "AlignRstrctType", &nValue);
        if (bFound)
        {
            m_pClasses[i].SetAlignRestrictType(nValue);
        }

        bFound = p2DA->GetCExoStringEntry(i, "PrimaryAbil", &sValue);
        if (bFound)
        {
            if (sValue == "STR")
            {
                m_pClasses[i].SetPrimaryAbility(ABILITY_STRENGTH);
            }
            else if (sValue == "DEX")
            {
                m_pClasses[i].SetPrimaryAbility(ABILITY_DEXTERITY);
            }
            else if (sValue == "CON")
            {
                m_pClasses[i].SetPrimaryAbility(ABILITY_CONSTITUTION);
            }
            else if (sValue == "INT")
            {
                m_pClasses[i].SetPrimaryAbility(ABILITY_INTELLIGENCE);
            }
            else if (sValue == "WIS")
            {
                m_pClasses[i].SetPrimaryAbility(ABILITY_WISDOM);
            }
            else if (sValue == "CHA")
            {
                m_pClasses[i].SetPrimaryAbility(ABILITY_CHARISMA);
            }
        }

        p2DA->GetINTEntry(i, "Str", &nValue);
        m_pClasses[i].SetStrength(nValue);
        p2DA->GetINTEntry(i, "Dex", &nValue);
        m_pClasses[i].SetDexterity(nValue);
        p2DA->GetINTEntry(i, "Con", &nValue);
        m_pClasses[i].SetConstitution(nValue);
        p2DA->GetINTEntry(i, "Int", &nValue);
        m_pClasses[i].SetIntelligence(nValue);
        p2DA->GetINTEntry(i, "Wis", &nValue);
        m_pClasses[i].SetWisdom(nValue);
        p2DA->GetINTEntry(i, "Cha", &nValue);
        m_pClasses[i].SetCharisma(nValue);

        CExoString sColumn;
        for (int nLevel = 1; nLevel <= 20; nLevel++)
        {
            sColumn.Format("EffCRLvl%02d", nLevel);
            p2DA->GetINTEntry(i, sColumn.CStr(), &nValue);
            m_pClasses[i].SetEffectiveCRLevel(nLevel, nValue);
        }
    }

    delete p2DA;
}
// PC 0x005D6EE0
void CSWRules::LoadRaceInfo()
{
    C2DA* p2DA = new C2DA("RacialTypes", 0);
    p2DA->Load2DArray();

    int nNumRows = p2DA->m_nNumRows;
    m_nNumRaces = nNumRows;
    m_pRaces = new CSWRace[m_nNumRaces];

    for (int i = 0; i < m_nNumRaces; i++)
    {
        CExoString sFeatsTable;
        int nStrRef;
        int nValue;
        int bFound;

        p2DA->GetINTEntry(i, "Name", &nStrRef);
        m_pRaces[i].SetNameStrRef(nStrRef);
        p2DA->GetINTEntry(i, "ConverName", &nStrRef);
        m_pRaces[i].SetConverNameStrRef(nStrRef);
        p2DA->GetINTEntry(i, "ConverNameLower", &nStrRef);
        m_pRaces[i].SetConverNameLowerStrRef(nStrRef);
        p2DA->GetINTEntry(i, "NamePlural", &nStrRef);
        m_pRaces[i].SetNamePluralStrRef(nStrRef);
        p2DA->GetINTEntry(i, "Description", &nStrRef);
        m_pRaces[i].SetDescriptionStrRef(nStrRef);
        bFound = p2DA->GetINTEntry(i, "Biography", &nStrRef);
        if (bFound)
        {
            m_pRaces[i].SetBiographyStrRef(nStrRef);
        }

        bFound = p2DA->GetINTEntry(i, "Favored", &nValue);
        if (bFound)
        {
            m_pRaces[i].SetFavoredClass(nValue);
        }
        bFound = p2DA->GetINTEntry(i, "PlayerRace", &nValue);
        if (bFound)
        {
            m_pRaces[i].SetPlayerRace(nValue != 0);
        }

        p2DA->GetINTEntry(i, "STRAdjust", &nValue);
        m_pRaces[i].SetStrengthAdjust(nValue);
        p2DA->GetINTEntry(i, "DEXAdjust", &nValue);
        m_pRaces[i].SetDexterityAdjust(nValue);
        p2DA->GetINTEntry(i, "INTAdjust", &nValue);
        m_pRaces[i].SetIntelligenceAdjust(nValue);
        p2DA->GetINTEntry(i, "CHAAdjust", &nValue);
        m_pRaces[i].SetCharismaAdjust(nValue);
        p2DA->GetINTEntry(i, "WISAdjust", &nValue);
        m_pRaces[i].SetWisdomAdjust(nValue);
        p2DA->GetINTEntry(i, "CONAdjust", &nValue);
        m_pRaces[i].SetConstitutionAdjust(nValue);
        p2DA->GetINTEntry(i, "Endurance", &nValue);
        m_pRaces[i].SetEnduranceAdjust(nValue);
        p2DA->GetINTEntry(i, "Age", &nValue);
        m_pRaces[i].SetAge(nValue);
        p2DA->GetINTEntry(i, "Appearance", &nValue);
        m_pRaces[i].SetAppearance(nValue);

        bFound = p2DA->GetCExoStringEntry(i, "FeatsTable", &sFeatsTable);
        if (bFound)
        {
            m_pRaces[i].LoadFeatsTable(sFeatsTable);
        }
    }

    delete p2DA;
}
// PC 0x005D8460
CSWRules::~CSWRules()
{
    if (m_p2DArrays != 0)
    {
        m_p2DArrays->UnLoad2DArrays();
        delete m_p2DArrays;
        m_p2DArrays = 0;
    }
    if (m_pBaseItems != 0)
    {
        delete m_pBaseItems;
        m_pBaseItems = 0;
    }
    if (m_pSpellArray != 0)
    {
        delete m_pSpellArray;
        m_pSpellArray = 0;
    }
    if (m_pFeats != 0)
    {
        delete[] m_pFeats;
        m_pFeats = 0;
    }
    if (m_pFeatIndices != 0)
    {
        delete[] m_pFeatIndices;
        m_pFeatIndices = 0;
    }
    if (m_pClasses != 0)
    {
        delete[] m_pClasses;
        m_pClasses = 0;
    }
    if (m_pRaces != 0)
    {
        delete[] m_pRaces;
        m_pRaces = 0;
    }
    if (m_pSkills != 0)
    {
        delete[] m_pSkills;
        m_pSkills = 0;
    }
    if (m_pMasterFeatStrRefs != 0)
    {
        delete[] m_pMasterFeatStrRefs;
        m_pMasterFeatStrRefs = 0;
    }
    if (m_pMasterFeatDescriptionStrRefs != 0)
    {
        delete[] m_pMasterFeatDescriptionStrRefs;
        m_pMasterFeatDescriptionStrRefs = 0;
    }
    if (m_pMasterFeatIcons != 0)
    {
        delete[] m_pMasterFeatIcons;
        m_pMasterFeatIcons = 0;
    }
}
// PC 0x005D75B0
void CSWRules::LoadSkillInfo()
{
    C2DA* p2DA = new C2DA("Skills", 0);
    p2DA->Load2DArray();

    int nNumRows = p2DA->m_nNumRows;
    m_nNumSkills = nNumRows;
    m_pSkills = new CSWSkill[m_nNumSkills];

    for (int i = 0; i < m_nNumSkills; i++)
    {
        CExoString sValue;
        int nValue;

        p2DA->GetINTEntry(i, "Name", &nValue);
        m_pSkills[i].SetNameStrRef(nValue);
        p2DA->GetINTEntry(i, "Description", &nValue);
        m_pSkills[i].SetDescriptionStrRef(nValue);
        p2DA->GetCExoStringEntry(i, "Icon", &sValue);
        m_pSkills[i].SetIcon(sValue);
        p2DA->GetINTEntry(i, "Untrained", &nValue);
        m_pSkills[i].SetUntrained(nValue);
        p2DA->GetCExoStringEntry(i, "KeyAbility", &sValue);
        m_pSkills[i].SetKeyAbility(ABILITY_STRENGTH);
        if (sValue == "STR")
        {
            m_pSkills[i].SetKeyAbility(ABILITY_STRENGTH);
        }
        else if (sValue == "DEX")
        {
            m_pSkills[i].SetKeyAbility(ABILITY_DEXTERITY);
        }
        else if (sValue == "CON")
        {
            m_pSkills[i].SetKeyAbility(ABILITY_CONSTITUTION);
        }
        else if (sValue == "INT")
        {
            m_pSkills[i].SetKeyAbility(ABILITY_INTELLIGENCE);
        }
        else if (sValue == "WIS")
        {
            m_pSkills[i].SetKeyAbility(ABILITY_WISDOM);
        }
        else if (sValue == "CHA")
        {
            m_pSkills[i].SetKeyAbility(ABILITY_CHARISMA);
        }
        p2DA->GetINTEntry(i, "ArmorCheckPenalty", &nValue);
        m_pSkills[i].SetArmorCheckPenalty(nValue);
        p2DA->GetINTEntry(i, "AllClassesCanUse", &nValue);
        m_pSkills[i].SetAllClassesCanUse(nValue);
        p2DA->GetINTEntry(i, "Category", &nValue);
        m_pSkills[i].SetCategory(nValue);
        p2DA->GetINTEntry(i, "MaxCR", &nValue);
        m_pSkills[i].SetMaxCR(nValue);
        p2DA->GetINTEntry(i, "HostileSkill", &nValue);
        m_pSkills[i].SetHostileSkill(nValue);
        p2DA->GetINTEntry(i, "NPCCanUse", &nValue);
        m_pSkills[i].SetNPCCanUse(nValue);
        p2DA->GetINTEntry(i, "DroidCanUse", &nValue);
        m_pSkills[i].SetDroidCanUse(nValue);
    }

    delete p2DA;
}
// PC 0x005D7B80
unsigned short CSWRules::RollDice(unsigned char nDice, unsigned char nDie)
{
    int nRolls = nDice;
    unsigned short nResult = 0;

    if (nRolls > 0)
    {
        do
        {
            nResult += rand() % nDie + 1;
            --nRolls;
        } while (nRolls != 0);
    }

    return nResult;
}
// PC 0x005D7BB0
unsigned char CSWRules::GetMetaMagicLevelCost(unsigned char nMetaMagic)
{
    switch (nMetaMagic)
    {
    case 1:
        return 2;
    case 2:
    case 16:
    case 32:
        return 1;
    case 4:
        return 3;
    case 8:
        return 4;
    default:
        return 0;
    }
}
// PC 0x005D7C20
CSWFeat* CSWRules::GetFeat(unsigned short nFeat)
{
    if (nFeat < m_nNumFeats)
    {
        CSWFeat* pFeat = &m_pFeats[nFeat];
        if (pFeat->m_bAvailable)
        {
            return pFeat;
        }
    }
    return 0;
}
// PC 0x005D7C50
int CSWRules::GetDifficultyOption(int nOption, int nDifficulty)
{
    return m_aDifficultyOptions[nDifficulty * 6 + nOption];
}
// PC 0x005D7C70
void CSWRules::LoadDifficultyInfo()
{
    C2DA* p2DA = new C2DA("DiffSettings", 0);
    int nResult;

    p2DA->Load2DArray();
    int nDifficulty = 1;
    do
    {
        int nOption = 0;
        do
        {
            m_aDifficultyOptions[(nDifficulty - 1) * 6 + nOption] =
                p2DA->GetINTEntry(nOption, nDifficulty, &nResult) ? nResult : 0;
            nOption++;
        } while (nOption < 6);
        nDifficulty++;
    } while (nDifficulty <= 5);
    delete p2DA;
}
// PC 0x005D8600
void CSWRules::LoadFeatInfo()
{
    C2DA* p2DA = new C2DA("Feat", 0);
#ifdef _XBOX
    EXOXBOXASSERT(p2DA->Load2DArray());
#else
    p2DA->Load2DArray();
#endif

    int nNumRows = p2DA->m_nNumRows;
    m_nNumFeats = nNumRows;
    m_pFeats = new CSWFeat[m_nNumFeats];
    m_pFeatIndices = new unsigned short[m_nNumFeats];
    m_nNumFeatEntries = 0;

    for (int i = 0; i < m_nNumFeats; i++)
    {
        CExoString sIcon;
        int nStrRef;
        int nValue;

        CExoString sNameColumn("Name");
        if (p2DA->GetColumnIndex(sNameColumn) == -1)
        {
            sNameColumn = "Feat";
        }

        if (p2DA->GetINTEntry(i, sNameColumn, &nStrRef))
        {
            m_pFeatIndices[m_nNumFeatEntries] = i;
            m_nNumFeatEntries++;

            m_pFeats[i].SetNameStrRef(nStrRef);
            p2DA->GetINTEntry(i, "Description", &nStrRef);
            m_pFeats[i].SetDescriptionStrRef(nStrRef);

            if (p2DA->GetCExoStringEntry(i, "Icon", &sIcon))
            {
                CResRef cIcon((char*)sIcon.CStr());
                m_pFeats[i].SetIcon(cIcon);
            }

            p2DA->GetINTEntry(i, "Category", &nValue);
            m_pFeats[i].SetCategory(nValue);
            p2DA->GetINTEntry(i, "MaxCR", &nValue);
            m_pFeats[i].SetMaxCR(nValue);
            p2DA->GetINTEntry(i, "MinAttackBonus", &nValue);
            m_pFeats[i].SetMinAttackBonus(nValue);
            p2DA->GetINTEntry(i, "MinCharLevel", &nValue);
            m_pFeats[i].SetMinCharLevel(nValue);
            p2DA->GetINTEntry(i, "MinSTR", &nValue);
            m_pFeats[i].SetMinStrength(nValue);
            p2DA->GetINTEntry(i, "MinDEX", &nValue);
            m_pFeats[i].SetMinDexterity(nValue);
            p2DA->GetINTEntry(i, "MinINT", &nValue);
            m_pFeats[i].SetMinIntelligence(nValue);
            p2DA->GetINTEntry(i, "MinWIS", &nValue);
            m_pFeats[i].SetMinWisdom(nValue);

            if (p2DA->GetINTEntry(i, "PrereqFeat1", &nValue))
            {
                m_pFeats[i].SetPrereqFeat1(nValue);
            }
            if (p2DA->GetINTEntry(i, "PrereqFeat2", &nValue))
            {
                m_pFeats[i].SetPrereqFeat2(nValue);
            }

            CExoString sOrReqFeat;
            for (int nOrReq = 0; nOrReq < CSWFeat::MAX_OR_PREREQ_FEATS; nOrReq++)
            {
                sOrReqFeat = "OrReqFeat";
                sOrReqFeat = sOrReqFeat + nOrReq;
                if (p2DA->GetINTEntry(i, sOrReqFeat, &nValue))
                {
                    m_pFeats[i].SetOrPrereqFeat(nOrReq, nValue);
                }
            }

            if (p2DA->GetINTEntry(i, "REQSKILL", &nValue))
            {
                m_pFeats[i].SetRequiredSkill(nValue);
            }
            if (p2DA->GetINTEntry(i, "SUCCESSOR", &nValue))
            {
                m_pFeats[i].SetSuccessor(nValue);
            }
            if (p2DA->GetINTEntry(i, "HANDMAIDEN", &nValue))
            {
                m_pFeats[i].m_bHandmaiden = nValue;
            }
            if (p2DA->GetINTEntry(i, "BAODUR", &nValue))
            {
                m_pFeats[i].m_bBaoDur = nValue;
            }
            if (p2DA->GetINTEntry(i, "HANHARR", &nValue))
            {
                m_pFeats[i].m_bHanharr = nValue;
            }
            if (p2DA->GetINTEntry(i, "HK47", &nValue))
            {
                m_pFeats[i].m_bHK47 = nValue;
            }
            if (p2DA->GetINTEntry(i, "G0T0", &nValue))
            {
                m_pFeats[i].m_bG0T0 = nValue;
            }
            if (p2DA->GetINTEntry(i, "ATTON", &nValue))
            {
                m_pFeats[i].m_bAtton = nValue;
            }
            if (p2DA->GetINTEntry(i, "KREIA", &nValue))
            {
                m_pFeats[i].m_bKreia = nValue;
            }

            p2DA->GetINTEntry(i, "GainMultiple", &nValue);
            m_pFeats[i].SetGainMultiple(nValue == 1);

            p2DA->GetINTEntry(i, "MinSpellLvl", &nValue);
            m_pFeats[i].SetMinSpellLevel(0);
            p2DA->GetINTEntry(i, "AllClassesCanUse", &nValue);
            m_pFeats[i].SetAllClassesCanUse(nValue);

            if (p2DA->GetINTEntry(i, "SpellId", &nValue))
            {
                m_pFeats[i].SetSpellId(nValue);
            }
            if (p2DA->GetINTEntry(i, "UsesPerDay", &nValue))
            {
                m_pFeats[i].SetUsesPerDay(nValue);
            }
            if (p2DA->GetINTEntry(i, "MASTERFEAT", &nValue))
            {
                m_pFeats[i].SetMasterFeat(nValue);
            }
            else
            {
                m_pFeats[i].SetMasterFeat(CSWFeat::MASTER_FEAT_NONE);
            }
            if (p2DA->GetINTEntry(i, "TargetSelf", &nValue))
            {
                m_pFeats[i].SetTargetSelf(nValue);
            }

            p2DA->GetINTEntry(i, "HostileFeat", &nValue);
            m_pFeats[i].SetHostileFeat(nValue);
        }
        else
        {
            m_pFeats[i].SetAvailable(0);
        }
    }

    delete p2DA;

    p2DA = new C2DA("MasterFeats", 0);
#ifdef _XBOX
    EXOXBOXASSERT(p2DA->Load2DArray());
#else
    p2DA->Load2DArray();
#endif

    nNumRows = p2DA->m_nNumRows;
    m_nNumMasterFeats = nNumRows;
    m_pMasterFeatStrRefs = new unsigned long[m_nNumMasterFeats];
    m_pMasterFeatDescriptionStrRefs = new unsigned long[m_nNumMasterFeats];
    m_pMasterFeatIcons = new CResRef[m_nNumMasterFeats];

    int nStrRef;
    CExoString sIcon;
    for (int j = 0; j < m_nNumMasterFeats; j++)
    {
        p2DA->GetINTEntry(j, "STRREF", &nStrRef);
        m_pMasterFeatStrRefs[j] = nStrRef;
        p2DA->GetINTEntry(j, "DESCRIPTION", &nStrRef);
        m_pMasterFeatDescriptionStrRefs[j] = nStrRef;
        p2DA->GetCExoStringEntry(j, "ICON", &sIcon);
        m_pMasterFeatIcons[j] = (char*)sIcon.CStr();
    }

    delete p2DA;
}
// PC 0x005D7D30
int CSWRules::GetWeightedDamageAmount(int nMinimum, int nMaximum, int nCritical,
                                      int nMultiplier, int nChance)
{
    int nResult;

    if (nCritical == 1)
    {
        if (nChance != 0)
        {
            if (nChance * nMaximum / 100 == nMaximum)
            {
                return nMaximum;
            }
            nResult = nMinimum +
                      nMaximum * (nChance + rand() % (100 - nChance)) / 100;
            return (nResult < nMaximum) ? nResult : nMaximum;
        }
        return nMinimum;
    }
    if (nMultiplier != 1 || nChance == 0)
    {
        return nMinimum;
    }
    if (nChance < 100)
    {
        nResult = nMinimum - nMaximum * (100 - nChance) / 100;
        if (nResult >= 1)
        {
            return nResult;
        }
        return 1;
    }
    if (nChance == 100)
    {
        return nMinimum;
    }
    return nMinimum + nMaximum * (rand() % (nChance - 100)) / 100;
}
// PC 0x005D9530
CCombatInformation::CCombatInformation()
{
    m_nNumAttacks = 0;
    m_nOnHandAttackMod = 0;
    m_nOnHandDamageMod = 0;
    m_nOffHandAttackMod = 0;
    m_nOffHandDamageMod = 0;
    m_nOffHandWeaponEquipped = 0;
    m_nForceResistance = 0;
    m_nArcaneSpellFailure = 0;
    m_nArmorCheckPenalty = 0;
    m_nUnarmedDamageDice = 1;
    m_nUnarmedDamageDie = 3;
    m_nOnHandCriticalRange = 0;
    m_nOnHandCriticalMultiplier = 2;
    m_nOffHandCriticalRange = 0;
    m_nOffHandCriticalMultiplier = 2;
    m_nRightEquipment = OBJECT_INVALID;
    m_nLeftEquipment = OBJECT_INVALID;
    m_sLeftString = "";
    m_sRightString = "";
    m_nDamageDice = 0;
    m_nDamageDie = 0;
}
// PC 0x005D9600
int CCombatInformation::LoadData(CResGFF* pGFF, CResStruct* pStruct)
{
    CResStruct cCombatStruct;
    CResStruct cListElement;
    int bSuccess;
    CResList cList;
    int i;

    if (!pGFF->GetStructFromStruct(&cCombatStruct, pStruct, "CombatInfo"))
    {
        return 0;
    }

    m_nNumAttacks = pGFF->ReadFieldBYTE(&cCombatStruct, "NumAttacks", bSuccess,
                                        GetNumAttacks());
    m_nOnHandAttackMod = pGFF->ReadFieldCHAR(&cCombatStruct, "OnHandAttackMod",
                                             bSuccess, GetOnHandAttackMod());
    m_nOnHandDamageMod = pGFF->ReadFieldCHAR(&cCombatStruct, "OnHandDamageMod",
                                             bSuccess, GetOnHandDamageMod());
    m_nOffHandAttackMod = pGFF->ReadFieldCHAR(
        &cCombatStruct, "OffHandAttackMod", bSuccess, GetOffHandAttackMod());
    m_nOffHandDamageMod = pGFF->ReadFieldCHAR(
        &cCombatStruct, "OffHandDamageMod", bSuccess, GetOffHandDamageMod());
    m_nForceResistance = pGFF->ReadFieldBYTE(&cCombatStruct, "ForceResistance",
                                             bSuccess, GetForceResistance());
    pGFF->ReadFieldBYTE(&cCombatStruct, "ArcaneSpellFail", bSuccess,
                        GetArcaneSpellFailure());
    m_nArmorCheckPenalty = pGFF->ReadFieldBYTE(
        &cCombatStruct, "ArmorCheckPen", bSuccess, GetArmorCheckPenalty());
    m_nUnarmedDamageDice = pGFF->ReadFieldBYTE(
        &cCombatStruct, "UnarmedDamDice", bSuccess, GetUnarmedDamageDice());
    m_nUnarmedDamageDie = pGFF->ReadFieldBYTE(&cCombatStruct, "UnarmedDamDie",
                                              bSuccess, GetUnarmedDamageDie());
    m_nOnHandCriticalRange = pGFF->ReadFieldBYTE(
        &cCombatStruct, "OnHandCritRng", bSuccess, GetOnHandCriticalRange());
    m_nOnHandCriticalMultiplier =
        pGFF->ReadFieldBYTE(&cCombatStruct, "OnHandCritMult", bSuccess,
                            GetOnHandCriticalMultiplier());
    m_nOffHandWeaponEquipped =
        pGFF->ReadFieldBYTE(&cCombatStruct, "OffHandWeaponEq", bSuccess,
                            GetOffHandWeaponEquipped());
    m_nOffHandCriticalRange = pGFF->ReadFieldBYTE(
        &cCombatStruct, "OffHandCritRng", bSuccess, GetOffHandCriticalRange());
    m_nOffHandCriticalMultiplier =
        pGFF->ReadFieldBYTE(&cCombatStruct, "OffHandCritMult", bSuccess,
                            GetOffHandCriticalMultiplier());
    m_nLeftEquipment = pGFF->ReadFieldDWORD(&cCombatStruct, "LeftEquip",
                                            bSuccess, GetLeftEquipment());
    m_nRightEquipment = pGFF->ReadFieldDWORD(&cCombatStruct, "RightEquip",
                                             bSuccess, GetRightEquipment());
    m_sLeftString =
        pGFF->ReadFieldCExoString(&cCombatStruct, "LeftString", bSuccess, "");
    m_sRightString =
        pGFF->ReadFieldCExoString(&cCombatStruct, "RightString", bSuccess, "");
    m_nDamageDice =
        pGFF->ReadFieldBYTE(&cCombatStruct, "DamageDice", bSuccess, 0);
    m_nDamageDie =
        pGFF->ReadFieldBYTE(&cCombatStruct, "DamageDie", bSuccess, 0);

    if (pGFF->GetList(&cList, pStruct, "AttackList") == 1)
    {
        for (i = 0; i < (int)pGFF->GetListCount(&cList); i++)
        {
            pGFF->GetListElement(&cListElement, &cList, i);
            CCombatInformationNode* pNode = new CCombatInformationNode;
            pNode->m_nModifier =
                pGFF->ReadFieldCHAR(&cListElement, "Modifier", bSuccess, 0);
            pNode->m_nWeaponWield =
                pGFF->ReadFieldBYTE(&cListElement, "WeaponWield", bSuccess, 0);
            pNode->m_nVersusGoodEvil = pGFF->ReadFieldBYTE(
                &cListElement, "VersusGoodEvil", bSuccess, 0);
            pNode->m_nVersusRace = pGFF->ReadFieldBYTE(
                &cListElement, "VersusRace", bSuccess, g_pRules->GetNumRaces());
            m_lstAttacks.Add(pNode);
        }
    }

    if (pGFF->GetList(&cList, pStruct, "DamageList") == 1)
    {
        for (i = 0; i < (int)pGFF->GetListCount(&cList); i++)
        {
            pGFF->GetListElement(&cListElement, &cList, i);
            CCombatInformationNode* pNode = new CCombatInformationNode;
            pNode->m_nModifier =
                pGFF->ReadFieldCHAR(&cListElement, "Modifier", bSuccess, 0);
            pNode->m_nModifierType =
                pGFF->ReadFieldBYTE(&cListElement, "ModifierType", bSuccess, 0);
            pNode->m_nWeaponWield =
                pGFF->ReadFieldBYTE(&cListElement, "WeaponWield", bSuccess, 0);
            pNode->m_nVersusGoodEvil = pGFF->ReadFieldBYTE(
                &cListElement, "VersusGoodEvil", bSuccess, 0);
            pNode->m_nVersusRace = pGFF->ReadFieldBYTE(
                &cListElement, "VersusRace", bSuccess, g_pRules->GetNumRaces());
            m_lstDamage.Add(pNode);
        }
    }

    return 1;
}
// PC 0x005D9CB0
CCombatInformation& CCombatInformation::operator=(CCombatInformation& cInfo)
{
    if (this == &cInfo)
    {
        return *this;
    }

    m_nNumAttacks = cInfo.m_nNumAttacks;
    m_nOnHandAttackMod = cInfo.m_nOnHandAttackMod;
    m_nOnHandDamageMod = cInfo.m_nOnHandDamageMod;
    m_nOffHandAttackMod = cInfo.m_nOffHandAttackMod;
    m_nOffHandDamageMod = cInfo.m_nOffHandDamageMod;
    m_nOffHandWeaponEquipped = cInfo.m_nOffHandWeaponEquipped;
    m_nForceResistance = cInfo.m_nForceResistance;
    m_nArcaneSpellFailure = cInfo.m_nArcaneSpellFailure;
    m_nArmorCheckPenalty = cInfo.m_nArmorCheckPenalty;
    m_nUnarmedDamageDice = cInfo.m_nUnarmedDamageDice;
    m_nUnarmedDamageDie = cInfo.m_nUnarmedDamageDie;
    m_nOnHandCriticalRange = cInfo.m_nOnHandCriticalRange;
    m_nOnHandCriticalMultiplier = cInfo.m_nOnHandCriticalMultiplier;
    m_nOffHandCriticalRange = cInfo.m_nOffHandCriticalRange;
    m_nOffHandCriticalMultiplier = cInfo.m_nOffHandCriticalMultiplier;
    m_nLeftEquipment = cInfo.m_nLeftEquipment;
    m_nRightEquipment = cInfo.m_nRightEquipment;

    CExoArrayList<CCombatInformationNode*>* pAttackList = &m_lstAttacks;

    if (pAttackList->element != 0)
    {
        delete pAttackList->element;
        pAttackList->element = 0;
        pAttackList->array_size = 0;
    }
    pAttackList->num = 0;
    if (m_lstDamage.element != 0)
    {
        delete m_lstDamage.element;
        m_lstDamage.element = 0;
        m_lstDamage.array_size = 0;
    }
    m_lstDamage.num = 0;

    for (int i = 0; i < cInfo.m_lstAttacks.num; i++)
    {
        CCombatInformationNode* pNode = new CCombatInformationNode;
        *pNode = *cInfo.m_lstAttacks.element[i];
        if (pAttackList->num == pAttackList->array_size)
        {
            pAttackList->Allocate(pAttackList->array_size != 0
                                      ? pAttackList->array_size * 2
                                      : 16);
        }
        pAttackList->element[pAttackList->num++] = pNode;
    }
    for (int i = 0; i < cInfo.m_lstDamage.num; i++)
    {
        CCombatInformationNode* pNode = new CCombatInformationNode;
        *pNode = *cInfo.m_lstDamage.element[i];
        if (m_lstDamage.num == m_lstDamage.array_size)
        {
            m_lstDamage.Allocate(
                m_lstDamage.array_size != 0 ? m_lstDamage.array_size * 2 : 16);
        }
        m_lstDamage.element[m_lstDamage.num++] = pNode;
    }

    return *this;
}
