#include "clientexoappinternal.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "netlayer.h"
#include "serverinfo.h"
#include <stdio.h>
#include <stdlib.h>

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
// PC 0x006754D0  Xbox 0x00070080
int CClientMsgSysAdmin::GetTokenPair(unsigned char* pData, unsigned long nSize,
                                    CExoString& sCategory, CExoString& sName,
                                    CExoString& sValue)
{
    unsigned long nPos = 0;
    int nChar;
    for (; nPos < nSize; ++nPos)
    {
        nChar = pData[nPos];
        if (nChar == '.')
            break;
        if (nChar == ' ')
        {
            sCategory = "";
            sName = "";
            sValue = "";
            return 0;
        }
    }
    sCategory = CExoString((const char*)pData, nPos);
    if (nChar == '.')
    {
        ++nPos;
        unsigned long nStart = nPos;
        for (; nPos < nSize; ++nPos)
        {
            nChar = pData[nPos];
            if (nChar == ' ')
                break;
            if (nChar == '.')
            {
                sName = "";
                sValue = "";
                return 0;
            }
        }
        sName = CExoString((const char*)pData + nStart, nPos - nStart);
        if (nChar == ' ')
        {
            ++nPos;
            if (nPos < nSize)
            {
                sValue = CExoString((const char*)pData + nPos, nSize - nPos);
                return 3;
            }
            sValue = "";
            return 0;
        }
        if (nPos > nStart)
        {
            sValue = "";
            return 2;
        }
        sValue = "";
        return 0;
    }
    if (nPos > 0)
    {
        sName = "";
        sValue = "";
        return 1;
    }
    sName = "";
    sValue = "";
    return 0;
}
// PC 0x00675690  Xbox 0x00070270
int CClientMsgSysAdmin::SendSysAdminToServerMessage(const CExoString& sMessage)
{
    unsigned long nLength = sMessage.GetLength() + 1;
    unsigned char* pData = new unsigned char[nLength];
    if (pData == NULL)
    {
        return 0;
    }
    pData[0] = 's';
    memcpy(pData + 1, sMessage.CStr(), sMessage.GetLength());
    return g_pAppManager->m_pClientExoApp->GetNetLayer()->SendMessageToPlayer(
        CNetLayer::PLAYERID_SERVER, pData, nLength, 0);
}
// PC 0x00675710  Xbox 0x000702F0
int CClientMsgSysAdmin::HandleServerToSysAdminMessage(unsigned char* pData,
                                                      unsigned long nSize)
{
    CExoString sMessage;
    if (pData[0] != 'S')
    {
        return 0;
    }
    ++pData;
    --nSize;
    sMessage = CExoString((const char*)pData, nSize);

    CExoString sCategory;
    CExoString sName;
    CExoString sValue;
    int nTokens = GetTokenPair(pData, nSize, sCategory, sName, sValue);

    if (nTokens >= 1 && sCategory == CMessageServerAdmin_ServerStatusCategory)
    {
        if (nTokens >= 2)
        {
            if (sName == CMessageServerAdmin_ServerStatus_Idle)
            {
                g_pAppManager->GetClientExoApp()->GetServerInfo()->m_nServerMode = 0;
                g_pAppManager->GetClientExoApp()->GetServerInfo()
                    ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_SERVER_MODE;
                printf(":: Server mode: Idle.\n");
                return 1;
            }
            if (sName == CMessageServerAdmin_ServerStatus_ModuleLoaded)
            {
                g_pAppManager->GetClientExoApp()->GetServerInfo()->m_nServerMode = 1;
                g_pAppManager->GetClientExoApp()->GetServerInfo()
                    ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_SERVER_MODE;
                printf(":: Server mode: Module Loaded.\n");
                g_pAppManager->GetClientExoApp()->GetServerInfo()->SetModuleName(sValue);
                return 1;
            }
            if (sName == CMessageServerAdmin_ServerStatus_ModuleRunning)
            {
                g_pAppManager->GetClientExoApp()->GetServerInfo()->m_nServerMode = 2;
                g_pAppManager->GetClientExoApp()->GetServerInfo()
                    ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_SERVER_MODE;
                printf(":: Server mode: Module Running.\n");
                g_pAppManager->GetClientExoApp()->GetServerInfo()->SetModuleName(sValue);
                return 1;
            }
            if (sName == CMessageServerAdmin_ServerStatus_ModuleList)
            {
                g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sModuleList = sValue;
                g_pAppManager->GetClientExoApp()->GetServerInfo()
                    ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_MODULE_LIST;
                printf(":: Server module list: ");
                printf(g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sModuleList.CStr());
                printf("\n");
                return 1;
            }
            if (sName == "GetBannedList")
            {
                return 1;
            }
            if (sName == CMessageServerAdmin_ServerStatus_CharactervaultList)
            {
                g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sVaultList = sValue;
                g_pAppManager->GetClientExoApp()->GetServerInfo()
                    ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_VAULT_LIST;
                printf(":: Server character vault list: %s.\n",
                       g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sVaultList.CStr());
                return 1;
            }
            if (sName == CMessageServerAdmin_ServerStatus_PlayerList)
            {
                g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sPlayerList = sValue;
                g_pAppManager->GetClientExoApp()->GetServerInfo()
                    ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_PLAYER_LIST;
                printf(":: Server player list ::\n%s\n",
                       g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sPlayerList.CStr());
                return 1;
            }
        }
        if (sName == CMessageServerAdmin_ServerStatus_ModuleLoaded)
        {
            g_pAppManager->GetClientExoApp()->GetServerInfo()->SetModuleName(sValue);
            g_pAppManager->GetClientExoApp()->GetServerInfo()
                ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_MODULE_LIST;
            CExoString sReply;
            sReply.Format("%s.%s", CMessageServerAdmin_Module,
                          CMessageServerAdmin_Module_Run);
            SendSysAdminToServerMessage(sReply);
            return 1;
        }
    }

    if (nTokens >= 2 && sCategory == CMessageServerAdmin_Module &&
        sName == CMessageServerAdmin_Module_SavegameList)
    {
        g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sSavegameList = sValue;
        g_pAppManager->GetClientExoApp()->GetServerInfo()
            ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_SAVEGAME_LIST;
        printf(":: Module savegame list: %s.\n",
               g_pAppManager->GetClientExoApp()->GetServerInfo()->m_sSavegameList.CStr());
        return 1;
    }

    int bResult = 0;
    CServerInfo* pServerInfo = g_pAppManager->GetClientExoApp()->GetServerInfo();
    while (nTokens == 3)
    {
        int nOption =
            g_pAppManager->GetClientExoApp()->GetServerInfo()->FindOptionIndex(
                sCategory, sName);
        if (nOption == 0)
        {
            break;
        }
        g_pAppManager->GetClientExoApp()->GetServerInfo()
            ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_OPTIONS;
        SServerOption* pOption = &pServerInfo->m_aOptions[nOption];
        switch (pOption->nType)
        {
        case SServerOption::OPTION_BOOLEAN:
            *pOption->pnValue = sValue[0] == '1';
            break;
        case SServerOption::OPTION_INTEGER:
            *pOption->pnValue = atoi(sValue.CStr());
            break;
        }
        bResult = 1;

        int nEnd = 0;
        while (sValue[nEnd] != '\0' && sValue[nEnd] != '\n')
        {
            ++nEnd;
        }
        printf(":: %s.%s %s\n", sCategory.CStr(), sName.CStr(),
               sValue.Left(nEnd).CStr());
        if (sValue[nEnd] != '\n')
        {
            break;
        }
        unsigned long nRemaining = sValue.GetLength() - nEnd - 1;
        nTokens = GetTokenPair((unsigned char*)sValue.CStr() + nEnd + 1,
                               nRemaining, sCategory, sName, sValue);
    }
    return bResult;
}
