#ifndef __NWSPELL_H__
#define __NWSPELL_H__

#include "exobase.h"
#include "exoresman.h"

class CSWSpell
{
public:
    static const int USERTYPE_NONE;
    static const int USERTYPE_FORCEPOWER;
    static const int USERTYPE_MONSTERPOWER;
    static const int USERTYPE_FEAT;
    static const int USERTYPE_ITEM;
    static const int USERTYPE_MASTER;

    enum ProjectileSpawnPoint
    {
        PROJECTILE_SPAWN_HAND = 1,
        PROJECTILE_SPAWN_HEAD = 2,
        PROJECTILE_SPAWN_RIGHT_HAND = 3
    };

    enum SubRadialType
    {
        SUBRADIAL_TYPE_HOSTILE_FORCE_POWER = 1,
        SUBRADIAL_TYPE_SELF_FORCE_POWER = 3
    };

    static const int SUBRADIAL_TYPE_SCALE = 1000;

    enum PowerPriority
    {
        POWER_PRIORITY_BASE = 0,
        POWER_PRIORITY_FIRST_UPGRADE = 1,
        POWER_PRIORITY_SECOND_UPGRADE = 2
    };

    enum ProjectileType
    {
        PROJECTILE_TYPE_DOUBLE_SPEED = 1,
        PROJECTILE_TYPE_FIXED_TIME = 3,
        PROJECTILE_TYPE_FAST = 5,
        PROJECTILE_TYPE_DELAYED = 6,
        PROJECTILE_TYPE_DISTANCE_SPEED = 7,
        PROJECTILE_TYPE_SLOW = 8
    };

    enum ForcePowerID
    {
        FORCE_POWER_LIGHT_SABER_THROW_ADVANCED = 4,
        FORCE_POWER_AFFECT_MIND = 6,
        FORCE_POWER_DROID_DISABLE = 12,
        FORCE_POWER_DROID_DESTROY = 13,
        FORCE_POWER_DOMINATE = 14,
        FORCE_POWER_DROID_STUN = 47,
        FORCE_POWER_LIGHT_SABER_THROW = 49,
        FORCE_POWER_CRUSH_OPPOSITION_I = 144,
        FORCE_POWER_CRUSH_OPPOSITION_II = 145,
        FORCE_POWER_CRUSH_OPPOSITION_III = 146,
        FORCE_POWER_CRUSH_OPPOSITION_IV = 147,
        FORCE_POWER_CRUSH_OPPOSITION_V = 148,
        FORCE_POWER_CRUSH_OPPOSITION_VI = 149,
        FORCE_POWER_FORCE_CAMOUFLAGE = 156,
        FORCE_POWER_IMPROVED_FORCE_CAMOUFLAGE = 157,
        FORCE_POWER_MASTER_FORCE_CAMOUFLAGE = 158,
        FORCE_POWER_FURY = 164,
        FORCE_POWER_IMPROVED_FURY = 165,
        FORCE_POWER_MASTER_FURY = 166,
        FORCE_POWER_INSPIRE_FOLLOWERS_I = 167,
        FORCE_POWER_INSPIRE_FOLLOWERS_II = 168,
        FORCE_POWER_INSPIRE_FOLLOWERS_III = 169,
        FORCE_POWER_INSPIRE_FOLLOWERS_IV = 170,
        FORCE_POWER_INSPIRE_FOLLOWERS_V = 171,
        FORCE_POWER_INSPIRE_FOLLOWERS_VI = 172,
        FORCE_POWER_FORCE_SIGHT = 176,
        FORCE_POWER_FORCE_CRUSH = 177,
        FORCE_POWER_PRECOGNITION = 178,
        FORCE_POWER_FORCE_ENLIGHTENMENT = 180,
        FORCE_POWER_MIND_TRICK = 181,
        FORCE_POWER_BEAST_TRICK = 182,
        FORCE_POWER_BEAST_CONFUSION = 184,
        FORCE_POWER_CONFUSION = 200,
        FORCE_POWER_DROID_TRICK = 201,
        FORM_SABER_I_SHII_CHO = 258,
        FORM_SABER_II_MAKASHI = 259,
        FORM_SABER_III_SORESU = 260,
        FORM_SABER_IV_ATARU = 261,
        FORM_SABER_V_SHIEN = 262,
        FORM_SABER_VI_NIMAN = 263,
        FORM_SABER_VII_JUYO = 264,
        FORM_FORCE_I_FOCUS = 265,
        FORM_FORCE_III_AFFINITY = 267,
        FORCE_POWER_DROID_CONFUSION = 269, ITEM_ABILITY_GRENADE_LAUNCHED_FRAGMENTATION = 246,
        FORCE_POWER_BREATH_CONTROL = 270,
        SPELL_WOOKIEE_RAGE_I = 271,
        SPELL_WOOKIEE_RAGE_II = 272,
        SPELL_WOOKIEE_RAGE_III = 273,
        FORCE_POWER_FORCE_REPULSION = 162,
        FORCE_POWER_FORCE_REDIRECTION = 163
    };

    static const unsigned char USER_TYPE_FORCE_POWER = 1;

    static const unsigned char USER_TYPE_LEVEL_UP_ALTERNATE = 5;

    static const int SPELL_INVALID = -1;

    enum { SPELL_LEVEL_INVALID = 255 };

    enum { ITEM_TARGETING_PERSONAL = 1,
           ITEM_TARGETING_PLACEABLE = 2, ITEM_TARGETING_UNKNOWN_3 = 3, FORCE_POWER_BATTLE_PRECOGNITION = 179 };

    enum { SPELL_KNOCK = 93 };

    enum { ALIGNMENT_ROW_NEUTRAL = 5 };

    enum
    {
        FORM_FORCE_II_POTENCY = 266,
        FORM_FORCE_IV_MASTERY = 268,
        FORCE_BODY_COST_PERCENT_0 = 50,
        FORCE_BODY_COST_PERCENT_1 = 40,
        FORCE_BODY_COST_PERCENT_2 = 30
    };

    int m_n00;
    CExoString m_sLabel;
    unsigned long m_nNameStrRef;
    unsigned long m_nDescriptionStrRef;
    CResRef m_resIcon;
    CExoString m_sRange;
    CExoString m_sUnused28;
    short m_nTargetType;
    CExoString m_sImpactScript;
    unsigned char m_nGuardianLevel;
    unsigned char m_nConsularLevel;
    unsigned char m_nSentinelLevel;
    unsigned char m_nInnateLevel;
    unsigned char m_nWeaponMasterLevel;
    unsigned char m_nJediMasterLevel;
    unsigned char m_nWatchmanLevel;
    unsigned char m_nMarauderLevel;
    unsigned char m_nSithLordLevel;
    unsigned char m_nAssassinLevel;
    unsigned char GetInnateLevel();
    unsigned char GetUserType();
    int m_nConjureTime;
    short m_nConjureAnim;
    CResRef m_resConjureHeadVisual;
    CResRef m_resConjureHandVisual;
    CResRef m_resConjureGroundVisual;
    CResRef m_resConjureSoundMale;
    CResRef m_resConjureSoundFemale;
    CResRef m_resConjureSoundVFX;
    short m_nCastAnim;
    int m_nCastTime;
    CResRef m_resCastHeadVisual;
    CResRef m_resCastHandVisual;
    CResRef m_resCastGroundVisual;
    CResRef m_resCastSound;
    int m_nCatchTime;
    short m_nCatchAnim;
    int m_nProjectile;
    CResRef m_resProjectileModel;
    int m_nProjectileType;
    unsigned char m_nProjectileSpawnPoint;
    CResRef m_resProjectileSound;
    unsigned char m_nProjectileOrientation;
    unsigned char m_bUnknown11E;
    int m_nUnknown120;
    int m_nCategory;
    int m_nMaxCR;
    int* m_pSubRadialSpells;
    unsigned char m_nSubRadialSpellCount;
    int m_nUnknown134;
    int m_nCounter1;
    int m_nCounter2;
    unsigned char m_nUserType;
    int m_nAltMessage;
    int m_bHostileSetting;
    short m_nFeatID;
    unsigned char m_nForcePoints;
    char m_cGoodEvil;
    CExoString m_sPrerequisites;
    CExoString m_sMasterSpells;
    int m_aPrerequisites[5];
    int m_aMasterSpells[5];
    int m_nExclusion;
    int m_nForbidItemMask;
    int m_nRequireItemMask;
    int m_nFormMask;
    unsigned char m_nItemTargeting;

    CSWSpell();
    ~CSWSpell();

    unsigned char GetSpellLevel(unsigned char nClass);
    int GetPriority(int nIndex)  // PC 0x006FB4D0 GOG 0x006FB0C0
    {
        return GetSubRadialSpell(nIndex) -
               10 * ((unsigned int)GetSubRadialSpell(nIndex) / 10);
    }
    int GetSlot(int nIndex)  // PC 0x006FB510 GOG 0x006FB100
    {
        return (GetSubRadialSpell(nIndex) -
                (int)(1000 * ((unsigned int)GetSubRadialSpell(nIndex) / 1000))) /
               10;
    }
    int GetSubRadialSpell(unsigned char nIndex);

    void SetLabel(const CExoString& value) { m_sLabel = value; }
    void SetIcon(const CResRef& value) { m_resIcon = value; }
    void SetRange(const CExoString& value) { m_sRange = value; }
    void SetImpactScript(const CExoString& value) { m_sImpactScript = value; }
    void SetConjureSoundMale(const CResRef& value) { m_resConjureSoundMale = value; }
    void SetConjureSoundFemale(const CResRef& value) { m_resConjureSoundFemale = value; }
    void SetConjureSoundVFX(const CResRef& value) { m_resConjureSoundVFX = value; }
    void SetConjureHeadVisual(const CResRef& value) { m_resConjureHeadVisual = value; }
    void SetConjureHandVisual(const CResRef& value) { m_resConjureHandVisual = value; }
    void SetConjureGroundVisual(const CResRef& value) { m_resConjureGroundVisual = value; }
    void SetCastHeadVisual(const CResRef& value) { m_resCastHeadVisual = value; }
    void SetCastHandVisual(const CResRef& value) { m_resCastHandVisual = value; }
    void SetCastSound(const CResRef& value) { m_resCastSound = value; }
    void SetCastGroundVisual(const CResRef& value) { m_resCastGroundVisual = value; }
    void SetProjectileModel(const CResRef& value) { m_resProjectileModel = value; }
    void SetProjectileSound(const CResRef& value) { m_resProjectileSound = value; }

    void SetPrerequisites(const CExoString& sPrerequisites);
    void SetMasterSpells(const CExoString& sMasterSpells);
    int GetAdjustedForcePoints(unsigned long nLevel, int bAdjust, int nRoomForceRating,
                               int nForm, char nCharismaModifier);

    unsigned long GetNameStrRef() { return m_nNameStrRef; }
    unsigned long GetDescriptionStrRef() { return m_nDescriptionStrRef; }
    unsigned char GetForcePoints() { return m_nForcePoints; }
    CExoString GetSpellNameText();

    void SetSubRadialSpellCount(unsigned char nCount)
    {
        if (m_pSubRadialSpells != 0)
        {
            delete m_pSubRadialSpells;
            m_pSubRadialSpells = 0;
        }
        if (nCount > 0)
        {
            m_pSubRadialSpells = new int[nCount];
            if (m_pSubRadialSpells == 0)
            {
                nCount = 0;
            }
        }
        m_nSubRadialSpellCount = nCount;
    }
};

class CSWSpellArray
{
public:
    int m_nNumSpells;
    CSWSpell* m_pSpells;

    CSWSpellArray();
    ~CSWSpellArray();

    CSWSpell* GetSpell(int nIndex) const;

    int GetNumSpells() const
    {
        return m_nNumSpells;
    }
    void Load();
};

#endif
