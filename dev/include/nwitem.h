#ifndef __NWITEM_H__
#define __NWITEM_H__

#include <string.h>
#include "exobase.h"
#include "exoresman.h"

class CSWBaseItem;

enum ItemDamageType
{
    DAMAGE_TYPE_BLUDGEONING = 1,
    DAMAGE_TYPE_PIERCING = 2,
    DAMAGE_TYPE_SLASHING = 4,

    DAMAGE_TYPE_PHYSICAL_MASK = DAMAGE_TYPE_BLUDGEONING | DAMAGE_TYPE_PIERCING | DAMAGE_TYPE_SLASHING,
    DAMAGE_TYPE_UNIVERSAL = 8,
    DAMAGE_TYPE_ACID = 16,
    DAMAGE_TYPE_COLD = 32,
    DAMAGE_TYPE_LIGHT_SIDE = 64,
    DAMAGE_TYPE_ELECTRICAL = 128,
    DAMAGE_TYPE_FIRE = 256,
    DAMAGE_TYPE_DARK_SIDE = 512,
    DAMAGE_TYPE_SONIC = 1024,
    DAMAGE_TYPE_ION = 2048,
    DAMAGE_TYPE_BLASTER = 4096,

    DAMAGE_TYPE_POISON = 8192
};

class CSWItem
{
public:

    static const int DEFAULT_BASE_ITEM = 30;

    CSWItem();
    virtual ~CSWItem();

    CSWBaseItem* GetBaseItem();

    int GetBaseItemType()
    {
        return m_nBaseItem;
    }

    unsigned char m_aUnknown04[6];
    unsigned char m_aUnmapped0A[2];
    int m_nBaseItem;
};

class CSWBaseItemArray
{
public:
    int m_nNumBaseItems;
    CSWBaseItem* m_pBaseItems;
    CSWBaseItemArray();
    CSWBaseItem* GetBaseItem(int) const;
    void Load();
    ~CSWBaseItemArray();
};

#define BASEITEM_MODEL_TYPE_BODY_VARIATION 1

class CSWBaseItem
{
public:

    unsigned char GetNumDice() { return m_nNumDice; }
    unsigned char GetDieToRoll() { return m_nDieToRoll; }

    enum GroundRotation
    {
        GROUND_ROTATION_ROLL = 1,
        GROUND_ROTATION_PITCH = 2
    };

    enum ItemType
    {

        ITEM_TYPE_CREDITS = 23,
        ITEM_TYPE_PAZAAK_CARD = 42,

        ITEM_TYPE_EQUIPPED_ACTIVATION_12 = 12,
        ITEM_TYPE_EQUIPPED_ACTIVATION_13 = 13,

        ITEM_TYPE_NO_DEX_LIMIT_DISPLAY = 15,

        ITEM_TYPE_UNARMED_DAMAGE_19 = 19,
        ITEM_TYPE_EQUIPPED_ACTIVATION_20 = 20,

        ITEM_TYPE_STIMULANT = 25,
        ITEM_TYPE_MINE = 28,

        ITEM_TYPE_STUN_BATON = 0,
        ITEM_TYPE_BLASTER_RIFLE = 4,

        ITEM_TYPE_HOSTILE_USABLE = 6,

        ITEM_TYPE_MEDPAC = 26,

        ITEM_TYPE_SPIKE = 27,
        ITEM_TYPE_ARMOR_FIRST = 31,
        ITEM_TYPE_ARMOR_LAST = 38,

        ITEM_TYPE_POWERED_HAND_39 = 39,
        ITEM_TYPE_POWERED_HAND_40 = 40,
        ITEM_TYPE_POWERED_HAND_41 = 41,

        ITEM_TYPE_PAZAAK_SIDE_DECK = 43,

        ITEM_TYPE_STEALTH_DEVICE = 44,
        ITEM_TYPE_REPAIR_KIT = 45,

        ITEM_TYPE_NO_PROPERTY_DESCRIPTION = 46,
        ITEM_TYPE_SQUAD_MEDPAC = 47
    };

    enum { ITEM_TYPE_TORCH = 29 };

    enum WeaponWield
    {
        WEAPON_WIELD_ONE_HANDED_1 = 1,

        WIELD_DOUBLE_WEAPON = 3,

        WIELD_SIZE_UNRESTRICTED = 8,
        WEAPON_WIELD_ONE_HANDED_2 = 2,
        WEAPON_WIELD_ONE_HANDED_4 = 4,

        WEAPON_WIELD_5 = 5,
        WEAPON_WIELD_6 = 6,

        WEAPON_WIELD_THROWN = 7
    };

    enum { WEAPON_SIZE_TWO_HANDED = 4 };

    enum DroidOrHuman
    {
        DROID_OR_HUMAN_ANY = 0,
        DROID_OR_HUMAN_HUMAN = 1,
        DROID_OR_HUMAN_DROID = 2
    };

    enum { DEX_BONUS_UNLIMITED = 0xFF };
    int m_nName;
    int m_nEquipableSlots;
    char m_nWeaponWield;

    unsigned char m_nWeaponType;
    unsigned char m_aUnmapped0A[0xC - 0xA];
    int m_nDamageFlags;
    int m_nMinRange;
    int m_nMaxRange;
    unsigned char m_bUnknown18;
    unsigned char m_nChargesStarting;
    unsigned char m_nRangedWeapon;
    unsigned char m_nWeaponSize;
    unsigned char m_nNumDice;
    unsigned char m_nDieToRoll;
    unsigned char m_nCritThreat;
    unsigned char m_nCritHitMult;
    float m_fBaseCost;
    unsigned short m_nMaxStackSize;
    unsigned char m_aUnmapped26[2];
    int m_nDescription;
    unsigned char m_nMinProps;
    unsigned char m_nMaxProps;
    unsigned char m_nPropColumn;
    unsigned char m_nStorePanel;
    unsigned char m_nStorePanelSort;
    unsigned char m_aUnmapped31[3];
    float m_fMaxAttackRange;
    float m_fPrefAttackDist;
    char m_sItemClass[12];
    int m_nPartEnvMap;
    CResRef m_resDefaultIcon;
    CResRef m_resDefaultModel;
    int m_bCanRotateIcon;
    int m_bIsContainer;
    int m_nGenderSpecific;
    unsigned char m_nInvSoundType;
    unsigned char m_aUnmapped79[3];
    unsigned short* m_pRequiredFeats;
    unsigned char m_nRequiredFeatCount;
    unsigned char m_aUnmapped81[3];
    int m_nBaseItemStatRef;
    unsigned char m_nRotateOnGround;
    unsigned char m_nTenthLBS;
    unsigned char m_nBaseAC;
    unsigned char m_nACEnchantType;
    unsigned char m_nWeaponMatType;
    unsigned char m_nArmorCheckPen;
    unsigned char m_nAmmunitionType;
    unsigned char m_aUnmapped8F[0x90 - 0x8F];
    int m_bPoweredItem;
    CExoString m_sPoweredSnd;
    CExoString m_sPowerUpSnd;
    CExoString m_sPowerDownSnd;
    unsigned char m_nItemType;
    unsigned char m_nDexBonus;
    unsigned char m_aUnmappedAE[1];
    unsigned char m_nBodyVar;
    unsigned short m_nSpecFeat;
    unsigned short m_nFocFeat;
    unsigned char m_nDroidOrHuman;
    unsigned char m_aUnmappedB5[3];
    int m_nDenySubrace;
    CExoString m_sArmorType;
    float m_fCostMultiplier;

    CSWBaseItem();
    ~CSWBaseItem();
    CResRef GetIconResRef(short nVariation, char cPrefix);
    short GetRequiredFeat(unsigned char nIndex);

    int GetRequiredFeatCount();
    CResRef GetModelResRef(short nVariation, char cPrefix);
    int GetIsTwoHanded();
    int GetIsOneHanded();
    CExoString GetNameText();

    void SetNameStrRef(int value) { m_nName = value; }
    void SetEquipableSlots(int value) { m_nEquipableSlots = value; }

    int GetEquipableSlots() { return m_nEquipableSlots; }
    void SetCanRotateIcon(int value) { m_bCanRotateIcon = value; }
    void SetModelType(unsigned char value) { m_bUnknown18 = value; }
    void SetGenderSpecific(unsigned char value) { m_nGenderSpecific = value; }
    void SetPartEnvMap(int value) { m_nPartEnvMap = value; }
    void SetDefaultModel(const CResRef& value) { m_resDefaultModel = value; }
    void SetDefaultIcon(const CResRef& value) { m_resDefaultIcon = value; }
    void SetIsContainer(int value) { m_bIsContainer = value; }
    void SetWeaponWield(char value) { m_nWeaponWield = value; }
    void SetWeaponType(unsigned char value) { m_nWeaponType = value; }
    void SetDamageFlags(int value) { m_nDamageFlags = value; }
    void SetWeaponSize(unsigned char value) { m_nWeaponSize = value; }

    unsigned char GetWeaponSize() { return m_nWeaponSize; }
    void SetRangedWeapon(unsigned char value) { m_nRangedWeapon = value; }
    void SetMaxAttackRange(float value) { m_fMaxAttackRange = value; }
    void SetPrefAttackDist(float value) { m_fPrefAttackDist = value; }
    void SetMinRange(int value) { m_nMinRange = value; }
    void SetMaxRange(int value) { m_nMaxRange = value; }
    void SetNumDice(unsigned char value) { m_nNumDice = value; }
    void SetDieToRoll(unsigned char value) { m_nDieToRoll = value; }
    unsigned char GetCritThreat();
    unsigned char GetCritHitMult();
    void SetCritThreat(unsigned char value) { m_nCritThreat = value; }
    void SetCritHitMult(unsigned char value) { m_nCritHitMult = value; }
    void SetBaseCost(float value) { m_fBaseCost = value; }
    void SetMaxStackSize(unsigned short value) { m_nMaxStackSize = value; }

    unsigned short GetMaxStackSize() { return m_nMaxStackSize; }
    void SetCostMultiplier(float value) { m_fCostMultiplier = value; }
    void SetDescription(int value) { m_nDescription = value; }
    void SetInvSoundType(unsigned char value) { m_nInvSoundType = value; }
    void SetMaxProps(unsigned char value) { m_nMaxProps = value; }
    void SetMinProps(unsigned char value) { m_nMinProps = value; }
    void SetPropColumn(unsigned char value) { m_nPropColumn = value; }
    void SetArmorCheckPen(unsigned char value) { m_nArmorCheckPen = value; }
    void SetChargesStarting(unsigned char value) { m_nChargesStarting = value; }
    void SetBaseItemStatRef(int value) { m_nBaseItemStatRef = value; }
    void SetStorePanel(unsigned char value) { m_nStorePanel = value; }
    void SetStorePanelSort(unsigned char value) { m_nStorePanelSort = value; }

    unsigned char GetStorePanelSort() { return m_nStorePanelSort; }
    void SetRotateOnGround(unsigned char value) { m_nRotateOnGround = value; }
    void SetTenthLBS(unsigned char value) { m_nTenthLBS = value; }
    void SetBaseAC(unsigned char value) { m_nBaseAC = value; }
    void SetACEnchantType(unsigned char value) { m_nACEnchantType = value; }
    void SetWeaponMatType(unsigned char value) { m_nWeaponMatType = value; }
    void SetAmmunitionType(unsigned char value) { m_nAmmunitionType = value; }
    void SetPoweredItem(int value) { m_bPoweredItem = value; }
    void SetPoweredSnd(const CExoString& value) { m_sPoweredSnd = value; }
    void SetPowerDownSnd(const CExoString& value) { m_sPowerDownSnd = value; }
    void SetPowerUpSnd(const CExoString& value) { m_sPowerUpSnd = value; }
    void SetItemType(unsigned char value) { m_nItemType = value; }

    unsigned char GetItemType() { return m_nItemType; }
    void SetDroidOrHuman(unsigned char value) { m_nDroidOrHuman = value; }
    void SetBodyVar(unsigned char value) { m_nBodyVar = value; }
    void SetSpecFeat(unsigned short value) { m_nSpecFeat = value; }
    void SetFocFeat(unsigned short value) { m_nFocFeat = value; }
    void SetDenySubrace(int value) { m_nDenySubrace = value; }
    void SetArmorType(const CExoString& value) { m_sArmorType = value; }
    void SetDexBonus(char value) { m_nDexBonus = value; }
    void SetItemClass(const char* sClass)
    {
        strncpy(m_sItemClass, sClass, 11);
    }

    static const int INVALID_STAT_REF = -1;
    static const unsigned char INVALID_AC_ENCHANT = 255;

    void SetRequiredFeatCount(unsigned char nCount)
    {
        if (m_pRequiredFeats != NULL)
        {
            delete[] m_pRequiredFeats;
            m_pRequiredFeats = NULL;
        }
        m_pRequiredFeats = new unsigned short[nCount];
        if (m_pRequiredFeats == NULL)
        {
            nCount = 0;
        }
        m_nRequiredFeatCount = nCount;
    }

    void SetRequiredFeat(unsigned char nIndex, unsigned short nFeat)
    {
        if (nIndex < m_nRequiredFeatCount)
        {
            m_pRequiredFeats[nIndex] = nFeat;
        }
    }

};

#define INVENTORY_SLOT_INVALID (-1)

enum InventorySlotMask
{
    INVENTORY_SLOT_HEAD = 0x01,
    INVENTORY_SLOT_ARMOUR = 0x02,
    INVENTORY_SLOT_BOOTS = 0x04,
    INVENTORY_SLOT_CLOAK = 0x08,

    INVENTORY_SLOT_HANDS = 0x08,
    INVENTORY_SLOT_RIGHTHAND = 0x10,
    INVENTORY_SLOT_LEFTHAND = 0x20,
    INVENTORY_SLOT_LEFTRING = 0x40,
    INVENTORY_SLOT_RIGHTRING = 0x80,
    INVENTORY_SLOT_NECK = 0x100,
    INVENTORY_SLOT_BELT = 0x200,
    INVENTORY_SLOT_ARMS = 0x400,
    INVENTORY_SLOT_ARROWS = 0x800,
    INVENTORY_SLOT_BULLETS = 0x1000,
    INVENTORY_SLOT_BOLTS = 0x2000,
    INVENTORY_SLOT_CREATUREWEAPON1 = 0x4000,
    INVENTORY_SLOT_CREATUREWEAPON2 = 0x8000,
    INVENTORY_SLOT_CREATUREWEAPON3 = 0x10000,
    INVENTORY_SLOT_CREATUREARMOUR = 0x20000,
    INVENTORY_SLOT_RIGHTHAND2 = 0x40000,
    INVENTORY_SLOT_LEFTHAND2 = 0x80000
};

class CSWSItem;

class CSWInventory
{
public:

    enum { EQUIPMENT_SLOT_COUNT = 14 };

    enum { TOTAL_SLOT_COUNT = 20 };

    enum SlotIndex
    {
        SLOT_HEAD = 0,
        SLOT_ARMOUR = 1,
        SLOT_BOOTS = 2,
        SLOT_HANDS = 3,
        SLOT_RIGHTHAND = 4,
        SLOT_LEFTHAND = 5,
        SLOT_LEFTRING = 6,
        SLOT_RIGHTRING = 7,
        SLOT_NECK = 8,
        SLOT_BELT = 9,
        SLOT_ARMS = 10,
        SLOT_ARROWS = 11,
        SLOT_BULLETS = 12,
        SLOT_BOLTS = 13,
        SLOT_CREATUREWEAPON1 = 14,
        SLOT_CREATUREWEAPON2 = 15,
        SLOT_CREATUREWEAPON3 = 16,
        SLOT_CREATUREARMOUR = 17
    };

    CSWInventory();

    virtual ~CSWInventory();

    int GetItemInInventory(CSWSItem* pItem);
    unsigned long GetSlotFromItem(CSWSItem* pItem);
    unsigned long* GetSlotPointer(unsigned long nSlot);
    int RemoveItem(CSWSItem* pItem);
    void PutItemInSlot(unsigned long nSlot, CSWSItem* pItem);
    CSWSItem* GetItemInSlot(unsigned long nSlot);

    unsigned long m_oidSlot[20];
};

#endif
