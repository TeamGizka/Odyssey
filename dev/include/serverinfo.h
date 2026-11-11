#ifndef __SERVERINFO_H__
#define __SERVERINFO_H__

#include "exobase.h"

extern const char* g_aszServerOptionText[];

struct SServerOption
{
    enum { OPTION_UNUSED = 0, OPTION_BOOLEAN = 1, OPTION_INTEGER = 2 };

    SServerOption() {}
    SServerOption(const char* szCategory, const char* szName, int* pValue, int nValueType)
        : pszCategory(szCategory), pszName(szName), pnValue(pValue), nType(nValueType) {}

    const char* pszCategory;
    const char* pszName;
    int* pnValue;
    int nType;
};

class CPersistantWorldOptions
{
public:
    CPersistantWorldOptions()
        : bVaultCharsOnly(0), bSaveCharsInSaveGame(1) {}
    int bVaultCharsOnly;
    int bSaveCharsInSaveGame;
};

class CServerInfo
{
public:

    enum { GAME_DIFFICULTY_NORMAL = 2 };
    enum { DEFAULT_MAX_PLAYERS = 16 };

    enum
    {
        DATA_CHANGED_SERVER_MODE = 0x1,
        DATA_CHANGED_OPTIONS = 0x2,
        DATA_CHANGED_MODULE_LIST = 0x10,
        DATA_CHANGED_PLAYER_LIST = 0x20,
        DATA_CHANGED_SAVEGAME_LIST = 0x80,
        DATA_CHANGED_VAULT_LIST = 0x100
    };
    int m_nDataChangedFlags;
    short m_nServerMode;
    CExoString m_sModuleList;
    CExoString m_sPlayerList;
    CExoString m_sSavegameList;
    CExoString m_sVaultList;
#ifdef _XBOX

    CExoString m_sXboxUnmapped028;
#endif
    int m_bAllowLocal;
    int m_bAllowServer;
    int m_bAllowVault;
    int m_bFixedParties;
    int m_bAllowFighter;
    int m_bAllowBarbarian;
    int m_bAllowRanger;
    int m_bAllowPaladin;
    int m_bAllowWizard;
    int m_bAllowSorcerer;
    int m_bAllowCleric;
    int m_bAllowDruid;
    int m_bAllowRogue;
    int m_bAllowBard;
    int m_bAllowMonk;
    int m_bAllowHuman;
    int m_bAllowElf;
    int m_bAllowDwarf;
    int m_bAllowGnome;
    int m_bAllowHalfling;
    int m_bAllowHalfOrc;
    int m_bAllowHalfElf;
    int m_nMaxPlayers;
    int m_nMaxParties;
    int m_nMaxItemPoints;
    int m_nMaxStatTotal;
    int m_nMinLevel;
    int m_nMaxLevel;
    int m_nPVP;
    int m_bAllKillable;
    int m_bNonPartyKillable;
    int m_bRequireResurrection;
    int m_bLoseStolenItems;
    int m_bLoseItems;
    int m_bLoseExp;
    int m_bLoseGold;
    int m_nLoseGoldNum;
    int m_nLoseExpNum;
    int m_nLoseItemsNum;
    int m_bPauseAndPlay;
#ifdef _XBOX
    int m_nXboxUnmapped0D0;
    int m_nXboxUnmapped0D4;
#endif
    int m_bItemLevelRestrictions;
    int m_bResetOnVacant;
    int m_bResetOnVictory;
    int m_bResetOnFailure;
    int m_bResetOnDeath;
    int m_bResetOnPartyDeath;
    int m_bAutoSaveAtIntervals;
    int m_bAutoSaveAtSavePoints;
    int m_bAutoSaveOnPlayerChange;
    int m_nAutoSaveInterval;
    CPersistantWorldOptions m_PersistantWorldOptions;

    int m_bSuppressBaseServerVault;
    CExoString m_sModuleName;
#ifdef _XBOX

    short m_nXboxUnmapped114;
#endif
    unsigned int m_bPartyPicking : 1;

    unsigned int m_bConversationMultiPlayer : 1;
    unsigned int m_bConversationMultiPlayerViewOnly : 1;

#ifdef _XBOX
    int m_nXboxUnmapped11C;
#endif
    int m_nDifficultyLevel;
    SServerOption m_aOptions[50];
    ~CServerInfo()  // PC 0x004E9E50
    {
    }
    int FindOptionIndex(CExoString const&, CExoString const&);
    void SetDifficultyLevel(int);
    void SetModuleName(CExoString const&);
    CServerInfo();
};

#endif
