#ifndef __NWSITEM_H__
#define __NWSITEM_H__

class C2DA;
class CResGFF;
class CResRef;
class CResStruct;
class CSWSArea;
class CSWSCreature;
class CSWCItem;
class CItemRepository;
#include "exobase.h"
#include "nwitem.h"
#include "nwsobject.h"

class CSWItemProperty
{
public:

    enum ImmunitySubtype
    {
        IMMUNITY_SUBTYPE_SNEAK_ATTACK = 0,
        IMMUNITY_SUBTYPE_NEGATIVE_LEVEL = 1,
        IMMUNITY_SUBTYPE_MIND_SPELLS = 2,
        IMMUNITY_SUBTYPE_POISON = 3,
        IMMUNITY_SUBTYPE_DISEASE = 4,
        IMMUNITY_SUBTYPE_FEAR = 5,
        IMMUNITY_SUBTYPE_KNOCKDOWN = 6,
        IMMUNITY_SUBTYPE_PARALYSIS = 7,
        IMMUNITY_SUBTYPE_CRITICAL_HITS = 8,
        IMMUNITY_SUBTYPE_DEATH = 9
    };

    enum { UPGRADE_TYPE_NONE = 0xFF };

    enum { SUBTYPE_NONE = 0xFFFF, COST_VALUE_NONE = 0xFFFF, PARAM1_VALUE_NONE = 0xFF };

    enum DamageSubtype
    {
        DAMAGE_SUBTYPE_BLUDGEONING = 0,
        DAMAGE_SUBTYPE_PIERCING = 1,
        DAMAGE_SUBTYPE_SLASHING = 2
    };

    enum DamageBonusSubtype
    {
        DAMAGE_BONUS_SUBTYPE_BLUDGEONING = 0,
        DAMAGE_BONUS_SUBTYPE_PIERCING = 1,
        DAMAGE_BONUS_SUBTYPE_SLASHING = 2
    };

    enum
    {
        ITEM_PROPERTY_ABILITY_BONUS = 0,
        ITEM_PROPERTY_AC_BONUS = 1,
        ITEM_PROPERTY_AC_BONUS_VS_ALIGNMENT_GROUP = 2,
        ITEM_PROPERTY_AC_BONUS_VS_DAMAGE_TYPE = 3,
        ITEM_PROPERTY_AC_BONUS_VS_RACIAL_GROUP = 4,
        ITEM_PROPERTY_ENHANCEMENT_BONUS = 5,
        ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP = 6,
        ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP = 7,
        ITEM_PROPERTY_BONUS_FEAT = 9,
        ITEM_PROPERTY_ATTACK_PENALTY = 8,
        ITEM_PROPERTY_ACTIVATE_ITEM = 10,
        ITEM_PROPERTY_DAMAGE_BONUS = 11,
        ITEM_PROPERTY_DAMAGE_BONUS_VS_ALIGNMENT_GROUP = 12,
        ITEM_PROPERTY_DAMAGE_BONUS_VS_RACIAL_GROUP = 13,
        ITEM_PROPERTY_IMMUNITY_DAMAGE_TYPE = 14,
        ITEM_PROPERTY_DECREASED_DAMAGE = 15,
        ITEM_PROPERTY_DAMAGE_REDUCTION = 16,
        ITEM_PROPERTY_DAMAGE_RESISTANCE = 17,
        ITEM_PROPERTY_DECREASED_AC = 20,
        ITEM_PROPERTY_DAMAGE_VULNERABILITY = 18,
        ITEM_PROPERTY_DECREASED_ABILITY_SCORE = 19,
        ITEM_PROPERTY_DECREASED_SKILL_MODIFIER = 21,
        ITEM_PROPERTY_IMMUNITY = 24,
        ITEM_PROPERTY_IMPROVED_FORCE_RESISTANCE = 25,
        ITEM_PROPERTY_IMPROVED_SAVING_THROW = 26,
        ITEM_PROPERTY_IMPROVED_SAVING_THROW_SPECIFIC = 27,
        ITEM_PROPERTY_KEEN = 28,
        ITEM_PROPERTY_LIGHT = 29,
        ITEM_PROPERTY_MIGHTY = 30,
        ITEM_PROPERTY_NO_DAMAGE = 31,
        ITEM_PROPERTY_ON_HIT_PROPERTIES = 32,
        ITEM_PROPERTY_SECURITY_SPIKE = 37,
        ITEM_PROPERTY_DECREASED_SAVING_THROWS = 33,
        ITEM_PROPERTY_DECREASED_SAVING_THROWS_SPECIFIC = 34,
        ITEM_PROPERTY_REGENERATION = 35,
        ITEM_PROPERTY_SKILL_BONUS = 36,
        ITEM_PROPERTY_ATTACK_BONUS = 38,
        ITEM_PROPERTY_ATTACK_BONUS_VS_ALIGNMENT_GROUP = 39,
        ITEM_PROPERTY_ATTACK_BONUS_VS_RACIAL_GROUP = 40,
        ITEM_PROPERTY_DECREASED_ATTACK_MODIFIER = 41,
        ITEM_PROPERTY_USE_LIMITATION_ALIGNMENT_GROUP = 43,
        ITEM_PROPERTY_USE_LIMITATION_CLASS = 44,
        ITEM_PROPERTY_USE_LIMITATION_RACIAL_TYPE = 45,

        ITEM_PROPERTY_ON_MONSTER_HIT = 48,
        ITEM_PROPERTY_TRAP = 46,
        ITEM_PROPERTY_MASSIVE_CRITICALS = 49,

        ITEM_PROPERTY_USE_LIMITATION_FEAT = 57,
        ITEM_PROPERTY_TRUE_SEEING = 47,
        ITEM_PROPERTY_FREEDOM_OF_MOVEMENT = 50,
        ITEM_PROPERTY_MONSTER_DAMAGE = 51,
        ITEM_PROPERTY_COMPUTER_SPIKE = 53,
        ITEM_PROPERTY_REGENERATION_FORCE_POINTS = 54,
        ITEM_PROPERTY_BLASTER_BOLT_DEFLECT_INCREASE = 55,
        ITEM_PROPERTY_BLASTER_BOLT_DEFLECT_DECREASE = 56,
        ITEM_PROPERTY_USE_LIMITATION_GENDER = 60,
        ITEM_PROPERTY_USE_LIMITATION_SUBRACE = 61,
        ITEM_PROPERTY_USE_LIMITATION_PC = 62,
        ITEM_PROPERTY_MAX_DEXTERITY_BONUS = 68,
        ITEM_PROPERTY_USE_LIMITATION_ATTRIBUTE = 69
    };

    enum OnHitSubtype
    {
        ON_HIT_SLEEP = 0,
        ON_HIT_STUN = 1,
        ON_HIT_HOLD = 2,
        ON_HIT_CONFUSION = 3,
        ON_HIT_FEAR = 4,
        ON_HIT_SLOW = 5,
        ON_HIT_ABILITY_DAMAGE = 6,
        ON_HIT_POISON = 7,
        ON_HIT_SLAY_RACIAL_GROUP = 8,
        ON_HIT_SLAY_ALIGNMENT_GROUP = 9,
        ON_HIT_INSTANT_DEATH = 10,
        ON_HIT_KNOCKDOWN = 11
    };

    enum
    {
        ACTIVATE_ITEM_SINGLE_USE = 1,
        ACTIVATE_ITEM_5_CHARGES = 2,
        ACTIVATE_ITEM_4_CHARGES = 3,
        ACTIVATE_ITEM_3_CHARGES = 4,
        ACTIVATE_ITEM_2_CHARGES = 5,
        ACTIVATE_ITEM_1_CHARGE = 6,
        ACTIVATE_ITEM_UNLIMITED = 7,
        ACTIVATE_ITEM_1_USE_PER_DAY = 8,
        ACTIVATE_ITEM_2_USES_PER_DAY = 9,
        ACTIVATE_ITEM_3_USES_PER_DAY = 10,
        ACTIVATE_ITEM_4_USES_PER_DAY = 11,
        ACTIVATE_ITEM_5_USES_PER_DAY = 12,
        ACTIVATE_ITEM_UNLIMITED_ALT = 13
    };

    enum
    {
        ACTIVATE_ITEM_RECHARGE_1_MINUTE = 14,
        ACTIVATE_ITEM_RECHARGE_2_MINUTES = 15,
        ACTIVATE_ITEM_RECHARGE_3_MINUTES = 16,
        ACTIVATE_ITEM_RECHARGE_4_MINUTES = 17,
        ACTIVATE_ITEM_RECHARGE_5_MINUTES = 18
    };

    unsigned short m_nPropertyType;
    unsigned short GetPropertyType();
    unsigned short m_nSubtype;
    unsigned short GetSubtype();
    unsigned char m_nCostTable;
    unsigned char m_aUnmapped05[1];
    unsigned short m_nCostValue;
    unsigned short GetCostValue();
    unsigned char m_nParam1;
    unsigned char m_nParam1Value;
    unsigned char m_nChanceAppear;
    unsigned char m_aUnmapped0B[1];
    int m_bUseable;
    unsigned char m_nUsesPerDay;
    unsigned char m_nUpgradeType;
    unsigned short m_n12;

    unsigned long m_nLastUseDay;
    unsigned long m_nLastUseTime;
};

inline unsigned short CSWItemProperty::GetCostValue()
{
    return m_nCostValue;
}

class CSWSItem : public CSWItem, public CSWSObject
{
public:

    enum { DEFAULT_CHARGES = 50 };

    enum { MAX_TRACKED_ACTIVE_PROPERTIES = 8, ACTIVE_PROPERTY_MASK = 1 };

    static const unsigned long ACTIVATE_ITEM_RECHARGE_INTERVAL = 60000;

    static const int UPGRADE_PROPERTY_MASK = 1;

    enum UpgradableType
    {
        UPGRADABLE_TYPE_NONE = 0,
        UPGRADABLE_TYPE_LIGHTSABER = 1,
        UPGRADABLE_TYPE_BLASTER = 2,
        UPGRADABLE_TYPE_MELEE = 3,
        UPGRADABLE_TYPE_ARMOR = 4
    };

    CSWSItem(unsigned long oidItem);
    virtual ~CSWSItem();
    virtual CSWSItem* AsSWSItem() { return this; }  // PC 0x0069CC00
    virtual void AIUpdate();
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    virtual CExoLocString* GetFirstName()  // PC 0x0056E8E0
    {
        return &m_sLocalizedName;
    }
    void SetNumActiveProperties(int nNumProperties);
    void SetNumPassiveProperties(int nNumProperties);

    int ActiveProperty(unsigned short nPropertyType);
    unsigned long GetDamageFlags();
    void GetPropertyDamageRange(unsigned short nCostValue,
                                int& nMinDamage, int& nMaxDamage);
    CResRef GetIcon();
    CExoString GetKeyedPropertyString(unsigned char nUpgradeIndex);
    void AddProperty(unsigned short nPropertyType, unsigned short nSubtype,
                     unsigned short nCostValue, unsigned char nParam1Value,
                     CExoArrayList<CExoString>& aStrings);
    void SetIdentified(int bIdentified);
    void OpenInventory(unsigned long oidOpener);
    void CloseInventory(unsigned long oidCloser, int bUpdatePlayer);
    void CloseItemForAllPlayers();
    CExoString GetPropertyDescription();
    CExoString GetPropertiesOnlyDescription();
    int GetCost();
    unsigned char GetMinEquipLevel();
    CSWCItem* GetClientItem();
    void GetDamageTypeString(unsigned long nDamageFlags, CExoString& sString);
    void AddWeaponSizeProperties(CExoString& sString);
    void AddFeatRequirements(CExoString& sString);
    void AddDamageProperties(CExoString& sString);
    void AddRangeProperties(CExoString& sString);
    void AddCriticalThreatProperties(CExoString& sString);
    void AddOnHitProperties(CExoString& sString);
    void AddAttackModifierProperties(CExoString& sString);
    void AddAttackBonusStrings(int nPropertyType, CExoString& sString);
    void AddDefenceProperties(CExoString& sString);
    void AddMiscellaneousProperties(CExoString& sString);
    void AddGenderRequirements(CExoString& sString);
    void AddSubraceRequirements(CExoString& sString);
    void AddPCRequirements(CExoString& sString);
    void AddAttributeRequirements(CExoString& sString);
    void AddCNPCRequirements(CExoString& sString);
    enum { GUI_STRREF_GENDER_REQUIRED = 47995 };
    enum { GUI_STRREF_SUBRACE_REQUIRED = 48013 };
    enum { GUI_STRREF_PC_REQUIRED = 48026 };
    enum { GUI_STRREF_ATTRIBUTE_REQUIRED = 117597 };
    enum { GUI_STRREF_STRENGTH = 117591, GUI_STRREF_DEXTERITY = 117592,
           GUI_STRREF_CONSTITUTION = 117593, GUI_STRREF_INTELLIGENCE = 117594,
           GUI_STRREF_WISDOM = 117595, GUI_STRREF_CHARISMA = 117596 };
    enum { ATTRIBUTE_REQUIREMENT_COST_TABLE = 26 };
    enum { TLK_STRREF_ATTACK_MODIFIER_SUFFIX = 114160 };
    enum { GUI_STRREF_CNPC_REQUIRED = 130590, GUI_STRREF_CNPC_REQUIREMENT = 102045 };
    CSWItemProperty* GetActiveProperty(int nIndex);
    void DecrementUses(int nPropertyIndex, unsigned long oidUser);
    CSWItemProperty* GetPassiveProperty(int nIndex);
    int GetWeight();
    void PowerItem(int bPower, int bAnimate, int bSilent);
    int IsLightsaber();
    CSWSItem* SplitItem(int nCount);
    int CopyItem(CSWSItem* pItem);
    int CompareItem(CSWSItem* pItem);
    int AcquireItem(CSWSItem** ppItem, unsigned long oidPossessor,
                    int bDisplayFeedback);
    int MergeItem(CSWSItem* pItem);
    int SaveContainerItems(CResGFF* pRes, CResStruct* pStruct);
    void ReadContainerItemsFromGff(CResGFF* pRes, CResStruct* pStruct);
    int LoadFromTemplate(const CResRef& cResRef);
    void LoadDataFromGff(CResGFF* pRes, CResStruct* pStruct);
    unsigned char GetUpgradableType(C2DA* pTable);
    void AddUpgrade(CSWSItem* pUpgrade, unsigned long nSlot);
    void RemoveUpgrade(CSWSItem* pUpgrade, unsigned long nSlot);
    void RemoveFromArea();
    int AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                  int bLoadingGame);
    int GetPropertyByTypeExists(unsigned short nPropertyType,
                                unsigned short nSubtype);
    int GetExistsHigherValuedProperty(unsigned short nPropertyType,
                                      unsigned short nSubtype,
                                      unsigned short nCostValue);
    int GetPropertyByType(CSWItemProperty** ppProperty,
                          unsigned short nPropertyType,
                          unsigned short nSubtype);

    void GetPropertyChanceDurationDC(unsigned short nCostValue,
                                    unsigned char nParam1Value,
                                    int& nEffectChance, int& nDurationRounds,
                                    int& nDC);
    void GetPropertyStrings(unsigned short nPropertyType,
                            unsigned short nSubtype,
                            unsigned short nCostValue,
                            unsigned char nParam1Value,
                            CExoString& sProperty, CExoString& sSubtype,
                            CExoString& sCost, CExoString& sParam1,
                            int bPropertiesOnly = 0);

    unsigned char m_nUsableActivePropertyMask;
    unsigned char m_aActivePropertyUsesLeft[8];
    unsigned char m_aUnmapped241[3];

    int m_nBaseAC;
    int m_nActiveProperties;
    int GetActiveProperties();
    int m_nPassiveProperties;
    int GetPassiveProperties();
    CSWItemProperty* m_pActiveProperties;
    CSWItemProperty* m_pPassiveProperties;
    int m_nCharges;
    int m_nMaxCharges;
    int m_nAddCost;
    char m_nModelVariation;

    char GetModelVariation();
    char m_nBodyVariation;
    char m_nTextureVar;
    char m_aUnmapped267[1];
    unsigned long GetPossessor() { return m_oidPossessor; }
    unsigned long m_oidPossessor;
    CItemRepository* m_pRepository;
    CExoLocString m_sDescIdentified;
    CExoLocString m_sDescription;
    CExoLocString m_sLocalizedName;
    unsigned long m_bIdentified : 1;
    unsigned long m_bInventoryOpen : 1;
    unsigned long m_bInfinite : 1;
    unsigned long m_bDropable : 1;
    unsigned long m_bPickpocketable : 1;
    unsigned long m_bStolen : 1;
    unsigned long m_bNonEquippable : 1;
    unsigned long m_bNewItem : 1;
    unsigned long m_bDeleting : 1;
    unsigned long m_nUnknown289Rest : 23;
    // PC 0x0050B670
    void SetDropable(int bDropable)
    {
        m_bDropable = bDropable;
    }
    void SetStolen(int bStolen);
    void SetPickpocketable(int bPickpocketable);
    void SetNonEquippable(int bNonEquippable);
    void SetNewItem(int bNewItem);
    int GetUpgrades();
    // PC 0x0050B6A0
    int GetIsUpgradeActive(unsigned char nUpgradeType)
    {
        return nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            ((UPGRADE_PROPERTY_MASK << nUpgradeType) & m_nUpgrades) != 0;
    }

    int GetStackSize() { return m_nStackSize; }
    int m_nStackSize;
    int m_nWeight;
    int m_nUpgrades;

    unsigned short m_nInventoryIndex;
    unsigned char m_aUnmapped29A[2];
    int m_n2DC;
    int m_n2E0;
    int m_n2E4;
    int m_n2E8;
    int m_n2EC;
    int m_nUpgradeLevel;
    int m_anUpgradeSlot[6];

    int SaveItem(CResGFF* pRes, CResStruct* pStruct);
    int LoadItem(CResGFF* pRes, CResStruct* pStruct);
    void GetSortedPropertyStrings(CExoArrayList<CExoString>& aStrings,
                                  CExoString& sString);
    void SetPossessor(unsigned long oidPossessor, int bFlag1, int bFlag2,
                      unsigned long oidRepository);

    void ApplyItemProperties(CSWSCreature* pCreature, unsigned long nSlot,
                             int bLoading);
    void RemoveItemProperties(CSWSCreature* pCreature, unsigned long nSlot);
    int ComputeArmorClass();
    int SaveItemProperties(CResGFF* pResGFF, CResStruct* pResStruct);
    int RestoreUsedActiveProperties(int bRestoreAll);
    unsigned short GetUsedActivePropertyUsesLeft(unsigned char nProperty);
    void UpdateUsedActiveProperties();
    void InitRepository(unsigned long oidOwner);

public:

    CExoLocString* GetDescription()
    {
        return &m_sDescription;
    }
    void SetWeight(int nWeight)
    {
        m_nWeight = nWeight;
    }
};

class CItemRepository
{
public:
    CItemRepository(unsigned long oidOwner, int bFlag);
    ~CItemRepository();
    void ClearNewItems(CExoArrayList<unsigned long>* pNewItems);
    int ClearItems();
    CSWSItem* ItemListGetItem(int nIndex);
    unsigned long ItemListGetItemObjectID(int nIndex);
    int CalculateContentsWeight();
    int SetParentId(unsigned long oidParent);
    int RemoveItem(CSWSItem* pItem);
    int AddItem(CSWSItem** ppItem, int bFlag1, int bFlag2, int bFlag3);
    int GetNumItems() { return m_lstItems.num; }
    int GetItemInRepository(CSWSItem* pItem, int bFlag);
    void InsertItem(CSWSItem* pItem, int nIndex);

    unsigned long m_oidOwner;
    unsigned long m_nNewItems;
    unsigned long m_bUnknown08 : 1;
    unsigned long m_nUnknown09 : 31;
    CExoArrayList<unsigned long> m_lstItems;
    int FindItemWithBaseItemType(unsigned char nItemType, int nIndex);
    int FindItemWithTag(const CExoString& sTag);
    int GetMatchingItemCount(CSWSItem* pItem);
};

#endif
