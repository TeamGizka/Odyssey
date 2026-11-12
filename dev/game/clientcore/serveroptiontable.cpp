#include "serverinfo.h"

static char* CMessageServerAdmin_Echo = "Echo";
static char* CMessageServerAdmin_ServerStatusCategory = "ServerStatus";
static char* CMessageServerAdmin_ServerStatus_GetStatus = "GetStatus";
static char* CMessageServerAdmin_ServerStatus_ModuleLoaded = "ModuleLoaded";
static char* CMessageServerAdmin_ServerStatus_Idle = "Idle";
static char* CMessageServerAdmin_ServerStatus_ModuleRunning = "ModuleRunning";
static char* CMessageServerAdmin_ServerStatus_GetModuleList = "GetModuleList";
static char* CMessageServerAdmin_ServerStatus_ModuleList = "ModuleList";
static char* CMessageServerAdmin_ServerStatus_GetCharactervaultList =
    "GetCharactervaultList";
static char* CMessageServerAdmin_ServerStatus_CharactervaultList =
    "CharactervaultList";
static char* CMessageServerAdmin_ServerStatus_GetPlayerList = "GetPlayerList";
static char* CMessageServerAdmin_ServerStatus_PlayerList = "PlayerList";
static char* CMessageServerAdmin_ServerStatus_ShutDown = "ShutDown";
static char* CMessageServerAdmin_ServerStatus_Reset = "Reset";
static char* CMessageServerAdmin_ServerStatus_ServerError = "ServerError";
static char* CMessageServerAdmin_Module = "Module";
static char* CMessageServerAdmin_Module_Load = "Load";
static char* CMessageServerAdmin_Module_Run = "Run";
static char* CMessageServerAdmin_Module_Save = "Save";
static char* CMessageServerAdmin_Module_GetSavegameList = "GetSavegameList";
static char* CMessageServerAdmin_Module_SavegameList = "SavegameList";
static char* CMessageServerAdmin_Module_LoadSavegame = "LoadSavegame";
static char* CMessageServerAdmin_JoiningRestrictions = "JoiningRestrictions";
static char* CMessageServerAdmin_JoiningRestrictions_AllowLocal = "AllowLocal";
static char* CMessageServerAdmin_JoiningRestrictions_AllowServer =
    "AllowServer";
static char* CMessageServerAdmin_JoiningRestrictions_AllowVault = "AllowVault";
static char* CMessageServerAdmin_JoiningRestrictions_FixedParties =
    "FixedParties";
static char* CMessageServerAdmin_JoiningRestrictions_AllowFighter =
    "AllowFighter";
static char* CMessageServerAdmin_JoiningRestrictions_AllowBarbarian =
    "AllowBarbarian";
static char* CMessageServerAdmin_JoiningRestrictions_AllowRanger =
    "AllowRanger";
static char* CMessageServerAdmin_JoiningRestrictions_AllowPaladin =
    "AllowPaladin";
static char* CMessageServerAdmin_JoiningRestrictions_AllowWizard =
    "AllowWizard";
static char* CMessageServerAdmin_JoiningRestrictions_AllowSorcerer =
    "AllowSorcerer";
static char* CMessageServerAdmin_JoiningRestrictions_AllowCleric =
    "AllowCleric";
static char* CMessageServerAdmin_JoiningRestrictions_AllowDruid = "AllowDruid";
static char* CMessageServerAdmin_JoiningRestrictions_AllowRogue = "AllowRogue";
static char* CMessageServerAdmin_JoiningRestrictions_AllowBard = "AllowBard";
static char* CMessageServerAdmin_JoiningRestrictions_AllowMonk = "AllowMonk";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHuman = "AllowHuman";
static char* CMessageServerAdmin_JoiningRestrictions_AllowElf = "AllowElf";
static char* CMessageServerAdmin_JoiningRestrictions_AllowDwarf = "AllowDwarf";
static char* CMessageServerAdmin_JoiningRestrictions_AllowGnome = "AllowGnome";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHalfling =
    "AllowHalfling";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHalfOrc =
    "AllowHalfOrc";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHalfElf =
    "AllowHalfElf";
static char* CMessageServerAdmin_JoiningRestrictions_MaxPlayers = "MaxPlayers";
static char* CMessageServerAdmin_JoiningRestrictions_MaxParties = "MaxParties";
static char* CMessageServerAdmin_JoiningRestrictions_MaxItemPoints =
    "MaxItemPoints";
static char* CMessageServerAdmin_JoiningRestrictions_MaxStatTotal =
    "MaxStatTotal";
static char* CMessageServerAdmin_JoiningRestrictions_MinLevel = "MinLevel";
static char* CMessageServerAdmin_JoiningRestrictions_MaxLevel = "MaxLevel";
static char* CMessageServerAdmin_PlayOptions = "PlayOptions";
static char* CMessageServerAdmin_PlayOptions_AllKillable = "AllKillable";
static char* CMessageServerAdmin_PlayOptions_NonPartyKillable =
    "NonPartyKillable";
static char* CMessageServerAdmin_PlayOptions_RequireResurrection =
    "RequireResurrection";
static char* CMessageServerAdmin_PlayOptions_LoseStolenItems =
    "LoseStolenItems";
static char* CMessageServerAdmin_PlayOptions_LoseItems = "LoseItems";
static char* CMessageServerAdmin_PlayOptions_LoseExp = "LoseExp";
static char* CMessageServerAdmin_PlayOptions_LoseGold = "LoseGold";
static char* CMessageServerAdmin_PlayOptions_LoseGoldNum = "LoseGoldNum";
static char* CMessageServerAdmin_PlayOptions_LoseExpNum = "LoseExpNum";
static char* CMessageServerAdmin_PlayOptions_LoseItemsNum = "LoseItemsNum";
static char* CMessageServerAdmin_PlayOptions_GetPVP = "GetPVP";
static char* CMessageServerAdmin_PlayOptions_SetPVP = "SetPVP";
static char* CMessageServerAdmin_PlayOptions_PVP_Full = "full";
static char* CMessageServerAdmin_PlayOptions_PVP_Party = "party";
static char* CMessageServerAdmin_PlayOptions_PVP_None = "none";
static char* CMessageServerAdmin_PlayOptions_GetPauseAndPlay = "PauseAndPlay";
static char* CMessageServerAdmin_ResetOption = "ResetOption";
static char* CMessageServerAdmin_ResetOption_OnVacant = "OnVacant";
static char* CMessageServerAdmin_ResetOption_OnVictory = "OnVictory";
static char* CMessageServerAdmin_ResetOption_OnFailure = "OnFailure";
static char* CMessageServerAdmin_ResetOption_OnDeath = "OnDeath";
static char* CMessageServerAdmin_ResetOption_OnPartyDeath = "OnPartyDeath";
static char* CMessageServerAdmin_AutoSave = "AutoSave";
static char* CMessageServerAdmin_AutoSave_AtIntervals = "AtIntervals";
static char* CMessageServerAdmin_AutoSave_AtSavePoints = "AtSavePoints";
static char* CMessageServerAdmin_AutoSave_OnPlayerChange = "OnPlayerChange";
static char* CMessageServerAdmin_AutoSave_Interval = "Interval";
static char* CMessageServerAdmin_Difficulty = "Difficulty";
static char* CMessageServerAdmin_Control = "Control";
static char* CMessageServerAdmin_Control_Boot = "Boot";
static char* CMessageServerAdmin_Control_Ban = "Ban";
static char* CMessageServerAdmin_Control_ChangeSAPassword = "SAPass";
static char* CMessageServerAdmin_Control_ChangeGMPassword = "GMPass";
static char* CMessageServerAdmin_Control_Say = "Say";
// PC 0x005D5AE0
int CServerInfo::FindOptionIndex(CExoString const& sCategory,
                                 CExoString const& sName)
{
    int nIndex = 1;
    while (strcmp(sCategory.CStr(), m_aOptions[nIndex].pszCategory) != 0 ||
           strcmp(sName.CStr(), m_aOptions[nIndex].pszName) != 0)
    {
        if (++nIndex >= 50)
        {
            return 0;
        }
    }
    return nIndex;
}
// PC 0x005D5B80
void CServerInfo::SetDifficultyLevel(int nDifficultyLevel)
{
    if (nDifficultyLevel > 4)
    {
        nDifficultyLevel = 4;
    }
    else if (nDifficultyLevel < 0)
    {
        nDifficultyLevel = 0;
    }
    m_nDifficultyLevel = nDifficultyLevel;
}

void CServerInfo::SetModuleName(const CExoString& sModuleName)
{
    m_sModuleName = sModuleName;
    m_bPartyPicking = m_sModuleName == "ebo_m12aa" ||
                    m_sModuleName == "tar_m02af" ||
                    m_sModuleName == "ebo_m40ad";
}
// PC 0x005D5BB0
CServerInfo::CServerInfo()
{
    m_nDataChangedFlags = 0;
    m_nServerMode = 0;
    m_nMinLevel = 1;
    m_nMaxLevel = 50;
    m_bPauseAndPlay = 1;
    m_nPVP = 1;
    m_bPartyPicking = 0;
    m_bConversationMultiPlayer = 0;
    m_nDifficultyLevel = GAME_DIFFICULTY_NORMAL;

    m_aOptions[0] = SServerOption("Undefined", "UndefinedOption", NULL, SServerOption::OPTION_UNUSED);
    m_aOptions[1] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowLocal, &m_bAllowLocal, SServerOption::OPTION_BOOLEAN);
    m_aOptions[2] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowServer, &m_bAllowServer, SServerOption::OPTION_BOOLEAN);
    m_aOptions[3] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowVault, &m_bAllowVault, SServerOption::OPTION_BOOLEAN);
    m_aOptions[4] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_FixedParties, &m_bFixedParties, SServerOption::OPTION_BOOLEAN);
    m_aOptions[5] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowFighter, &m_bAllowFighter, SServerOption::OPTION_BOOLEAN);
    m_aOptions[6] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowBarbarian, &m_bAllowBarbarian, SServerOption::OPTION_BOOLEAN);
    m_aOptions[7] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowRanger, &m_bAllowRanger, SServerOption::OPTION_BOOLEAN);
    m_aOptions[8] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowPaladin, &m_bAllowPaladin, SServerOption::OPTION_BOOLEAN);
    m_aOptions[9] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowWizard, &m_bAllowWizard, SServerOption::OPTION_BOOLEAN);
    m_aOptions[10] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowSorcerer, &m_bAllowSorcerer, SServerOption::OPTION_BOOLEAN);
    m_aOptions[11] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowCleric, &m_bAllowCleric, SServerOption::OPTION_BOOLEAN);
    m_aOptions[12] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowDruid, &m_bAllowDruid, SServerOption::OPTION_BOOLEAN);
    m_aOptions[13] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowRogue, &m_bAllowRogue, SServerOption::OPTION_BOOLEAN);
    m_aOptions[14] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowBard, &m_bAllowBard, SServerOption::OPTION_BOOLEAN);
    m_aOptions[15] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowMonk, &m_bAllowMonk, SServerOption::OPTION_BOOLEAN);
    m_aOptions[16] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowHuman, &m_bAllowHuman, SServerOption::OPTION_BOOLEAN);
    m_aOptions[17] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowElf, &m_bAllowElf, SServerOption::OPTION_BOOLEAN);
    m_aOptions[18] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowDwarf, &m_bAllowDwarf, SServerOption::OPTION_BOOLEAN);
    m_aOptions[19] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowGnome, &m_bAllowGnome, SServerOption::OPTION_BOOLEAN);
    m_aOptions[20] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowHalfling, &m_bAllowHalfling, SServerOption::OPTION_BOOLEAN);
    m_aOptions[21] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowHalfOrc, &m_bAllowHalfOrc, SServerOption::OPTION_BOOLEAN);
    m_aOptions[22] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_AllowHalfElf, &m_bAllowHalfElf, SServerOption::OPTION_BOOLEAN);
    m_aOptions[23] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_MaxPlayers, &m_nMaxPlayers, SServerOption::OPTION_INTEGER);
    m_aOptions[24] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_MaxParties, &m_nMaxParties, SServerOption::OPTION_INTEGER);
    m_aOptions[25] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_MaxItemPoints, &m_nMaxItemPoints, SServerOption::OPTION_INTEGER);
    m_aOptions[26] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_MaxStatTotal, &m_nMaxStatTotal, SServerOption::OPTION_INTEGER);
    m_aOptions[27] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_MinLevel, &m_nMinLevel, SServerOption::OPTION_INTEGER);
    m_aOptions[28] = SServerOption(CMessageServerAdmin_JoiningRestrictions,
        CMessageServerAdmin_JoiningRestrictions_MaxLevel, &m_nMaxLevel, SServerOption::OPTION_INTEGER);
    m_aOptions[29] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_AllKillable, &m_bAllKillable, SServerOption::OPTION_BOOLEAN);
    m_aOptions[30] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_NonPartyKillable, &m_bNonPartyKillable, SServerOption::OPTION_BOOLEAN);
    m_aOptions[31] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_RequireResurrection, &m_bRequireResurrection, SServerOption::OPTION_BOOLEAN);
    m_aOptions[32] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseStolenItems, &m_bLoseStolenItems, SServerOption::OPTION_BOOLEAN);
    m_aOptions[33] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseItems, &m_bLoseItems, SServerOption::OPTION_BOOLEAN);
    m_aOptions[34] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseExp, &m_bLoseExp, SServerOption::OPTION_BOOLEAN);
    m_aOptions[35] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseGold, &m_bLoseGold, SServerOption::OPTION_BOOLEAN);
    m_aOptions[36] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseGoldNum, &m_nLoseGoldNum, SServerOption::OPTION_INTEGER);
    m_aOptions[37] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseExpNum, &m_nLoseExpNum, SServerOption::OPTION_INTEGER);
    m_aOptions[38] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_LoseItemsNum, &m_nLoseItemsNum, SServerOption::OPTION_INTEGER);
    m_aOptions[39] = SServerOption(CMessageServerAdmin_ResetOption,
        CMessageServerAdmin_ResetOption_OnVacant, &m_bResetOnVacant, SServerOption::OPTION_BOOLEAN);
    m_aOptions[40] = SServerOption(CMessageServerAdmin_ResetOption,
        CMessageServerAdmin_ResetOption_OnVictory, &m_bResetOnVictory, SServerOption::OPTION_BOOLEAN);
    m_aOptions[41] = SServerOption(CMessageServerAdmin_ResetOption,
        CMessageServerAdmin_ResetOption_OnFailure, &m_bResetOnFailure, SServerOption::OPTION_BOOLEAN);
    m_aOptions[42] = SServerOption(CMessageServerAdmin_ResetOption,
        CMessageServerAdmin_ResetOption_OnDeath, &m_bResetOnDeath, SServerOption::OPTION_BOOLEAN);
    m_aOptions[43] = SServerOption(CMessageServerAdmin_ResetOption,
        CMessageServerAdmin_ResetOption_OnPartyDeath, &m_bResetOnPartyDeath, SServerOption::OPTION_BOOLEAN);
    m_aOptions[44] = SServerOption(CMessageServerAdmin_AutoSave,
        CMessageServerAdmin_AutoSave_AtIntervals, &m_bAutoSaveAtIntervals, SServerOption::OPTION_BOOLEAN);
    m_aOptions[45] = SServerOption(CMessageServerAdmin_AutoSave,
        CMessageServerAdmin_AutoSave_AtSavePoints, &m_bAutoSaveAtSavePoints, SServerOption::OPTION_BOOLEAN);
    m_aOptions[46] = SServerOption(CMessageServerAdmin_AutoSave,
        CMessageServerAdmin_AutoSave_OnPlayerChange, &m_bAutoSaveOnPlayerChange, SServerOption::OPTION_BOOLEAN);
    m_aOptions[47] = SServerOption(CMessageServerAdmin_AutoSave,
        CMessageServerAdmin_AutoSave_Interval, &m_nAutoSaveInterval, SServerOption::OPTION_INTEGER);
    m_aOptions[48] = SServerOption("Undefined", "UndefinedOption", NULL, SServerOption::OPTION_UNUSED);
    m_aOptions[49] = SServerOption(CMessageServerAdmin_PlayOptions,
        CMessageServerAdmin_PlayOptions_GetPauseAndPlay, &m_bPauseAndPlay, SServerOption::OPTION_BOOLEAN);
}
