#include <string.h>

#include "exobase.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "netlayer.h"
#include "nwmessage.h"

static char* CMessageServerAdmin_Echo = "Echo";
static char* CMessageServerAdmin_ServerStatus = "ServerStatus";
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
// PC 0x005DA2F0 GOG 0x005DA360
int CSWSMessage::SendServerToServerAdminMessage(
    unsigned long nPlayerId, const CExoString& sMessage)
{
    unsigned long nSize = sMessage.GetLength() + 1;
    unsigned char* pMessage = new unsigned char[nSize];

    if (pMessage == NULL)
    {
        return 0;
    }

    pMessage[0] = 'S';
    memcpy(pMessage + 1, sMessage.CStr(), sMessage.GetLength());
    int nResult = g_pAppManager->m_pServerExoApp->GetNetLayer()
                      ->SendMessageToPlayer(nPlayerId, pMessage, nSize, 0);
    delete[] pMessage;
    return nResult;
}
// PC 0x005DA3A0 GOG 0x005DA410
void CSWSMessage::SendServerToServerAdminServerStatus(unsigned long nPlayerId)
{
    CExoString sMessage;
    CExoString sModuleName;
    unsigned short nServerMode =
        g_pAppManager->m_pServerExoApp->GetServerMode();

    if (nServerMode == 0)
    {
        sMessage.Format("%s.%s", CMessageServerAdmin_ServerStatus,
                        CMessageServerAdmin_ServerStatus_Idle);
        SendServerToServerAdminMessage(nPlayerId, sMessage);
    }

    if (nServerMode != 0)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        sModuleName = pServerExoApp->GetModuleName();
        if (nServerMode == 1)
        {
            sMessage.Format("%s.%s %s", CMessageServerAdmin_ServerStatus,
                            CMessageServerAdmin_ServerStatus_ModuleLoaded,
                            sModuleName.CStr());
            SendServerToServerAdminMessage(nPlayerId, sMessage);
        }
        else if (nServerMode == 2)
        {
            sMessage.Format("%s.%s %s", CMessageServerAdmin_ServerStatus,
                            CMessageServerAdmin_ServerStatus_ModuleRunning,
                            sModuleName.CStr());
            SendServerToServerAdminMessage(nPlayerId, sMessage);
        }
    }
}
// PC 0x005DA500 GOG 0x005DA570
void CSWSMessage::SendServerToServerAdminPlayerList(unsigned long nPlayerId)
{
    CExoString sMessage;
    sMessage.Format("%s.%s ", CMessageServerAdmin_ServerStatus,
                    CMessageServerAdmin_ServerStatus_PlayerList);
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    sMessage = sMessage + pServerExoApp->GetPlayerListString();
    SendServerToServerAdminMessage(nPlayerId, sMessage);
}
// PC 0x005DA5D0 GOG 0x005DA640
void CSWSMessage::SendServerToServerAdminModuleList(unsigned long nPlayerId)
{
    CExoString sFileName;
    CExoString sModuleList;
    CExoString sModuleName;
    CExoArrayList<CExoString> lstModules;

    {
        CExoString sDirectory("MODULES:");
        g_pExoBase->GetDirectoryList(&lstModules, sDirectory, RESTYPE_MOD, 0, 0);
    }

    int i;
    for (i = 0; i < lstModules.num; i++)
    {
        sFileName = lstModules[i];
        sFileName = sFileName.SubString(0, sFileName.GetLength() - 4);
        sModuleList = sModuleList + "\n";
        sModuleList = sModuleList + sFileName;
        sFileName = CExoString("MODULES:") + sFileName;
        sModuleList = sModuleList + ":";
        sModuleList = sModuleList + sFileName.CStr();
    }

    {
        CExoString sDirectory("NWMFILES:");
        g_pExoBase->GetDirectoryList(&lstModules, sDirectory, RESTYPE_NWM, 0, 0);
    }

    for (; i < lstModules.num; i++)
    {
        sFileName = lstModules[i];
        sFileName = sFileName.SubString(0, sFileName.GetLength() - 4);
        sModuleList = sModuleList + "\n";
        sModuleList = sModuleList + sFileName;
        sFileName = CExoString("NWMFILES:") + sFileName;
        sModuleList = sModuleList + ":";
        sModuleList = sModuleList + sFileName.CStr();
    }

    sModuleList = CExoString(lstModules.num) + sModuleList;
    sModuleList = CExoString("ServerStatus.ModuleList ") + sModuleList;
    SendServerToServerAdminMessage(nPlayerId, sModuleList.CStr());
}
