#include <stdlib.h>
#include <string.h>

#include "exobase.h"
#include "appmanager.h"
#include "serverinfo.h"
#include "nwsclient.h"
#include "serverexoapp.h"
#include "netlayer.h"
#include "nwsplayer.h"
#include "nwmessage.h"

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
// PC 0x005A9960
int CSWSMessage::ParseGetString(unsigned char* pString, unsigned long nLength,
                                CExoString& sString, unsigned long nMaxLength)
{
    if (*pString == ' ' &&
        (nMaxLength == 0 || (nLength > 1 && nLength - 1 <= nMaxLength)))
    {
        sString = CExoString((char*)&pString[1], nLength - 1);
        return 1;
    }
    return 0;
}
// PC 0x005A9A00
int CSWSMessage::ParseToken(const char* szToken, unsigned char** ppString,
                            unsigned long& nLength, int bPeriod)
{
    size_t nTokenLength;
    size_t nTokenSize;

    nTokenLength = strlen(szToken);
    nTokenSize = nTokenLength;

    if (nTokenLength <= nLength &&
        strncmp((const char*)*ppString, szToken, nTokenLength) == 0)
    {
        if (bPeriod != 0)
        {
            if ((*ppString)[nTokenSize] == '.')
            {
                *ppString = &(*ppString)[nTokenSize + 1];
                nLength += -1 - nTokenSize;
                return 1;
            }
        }
        else
        {
            *ppString += nTokenSize;
            nLength -= nTokenSize;
            return 1;
        }
    }
    return 0;
}
// PC 0x005A9A80
static int GetTokenPair(unsigned char* pString, unsigned long nLength,
                        CExoString& sCategory, CExoString& sName,
                        CExoString& sValue)
{
    unsigned long nPos;
    int nChar;

    for (nPos = 0; nPos < nLength; nPos++)
    {
        nChar = pString[nPos];
        if (nChar == '.')
        {
            break;
        }
        if (nChar == ' ')
        {
            sCategory = "";
            sName = "";
            sValue = "";
            return 0;
        }
    }

    sCategory = CExoString((char*)pString, nPos);

    if (nChar == '.')
    {
        nPos++;
        unsigned long nStart = nPos;

        while (nPos < nLength)
        {
            nChar = pString[nPos];
            if (nChar == ' ')
            {
                break;
            }
            if (nChar == '.')
            {
                sName = "";
                sValue = "";
                return 0;
            }
            nPos++;
        }

        sName = CExoString((char*)&pString[nStart], nPos - nStart);

        if (nChar == ' ')
        {
            nPos++;
            if (nPos < nLength)
            {
                sValue = CExoString((char*)&pString[nPos], nLength - nPos);
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
// PC 0x005A9C50
int CSWSMessage::HandleServerAdminToServerMessage(unsigned long nPlayerId,
                                                  unsigned char* pMessage,
                                                  unsigned long nMessageLength)
{
    CExoString sParameter;

    if (pMessage[0] != 's')
    {
        return 0;
    }
    if (g_pAppManager->GetServerExoApp()
            ->GetNetLayer()
            ->GetPlayerInfo(nPlayerId)
            ->m_bAuthorized == 0)
    {
        return 0;
    }

    unsigned char* pString = pMessage + 1;
    unsigned long nLength = nMessageLength - 1;

    if (ParseToken(CMessageServerAdmin_Echo, &pString, nLength, 0))
    {
        SendServerToServerAdminMessage(
            nPlayerId,
            CExoString((char*)pMessage + strlen(CMessageServerAdmin_Echo) + 2,
                       nMessageLength - strlen(CMessageServerAdmin_Echo) - 2));
    }

    if (ParseToken(CMessageServerAdmin_ServerStatusCategory, &pString, nLength,
                   1))
    {
        if (ParseToken(CMessageServerAdmin_ServerStatus_GetStatus, &pString,
                       nLength, 0))
        {
            if (nLength != 0)
            {
                return 0;
            }
            SendServerToServerAdminServerStatus(nPlayerId);
            return 1;
        }
        else if (ParseToken(CMessageServerAdmin_ServerStatus_GetModuleList,
                            &pString, nLength, 0))
        {
            if (nLength != 0)
            {
                return 0;
            }
            SendServerToServerAdminModuleList(nPlayerId);
            return 1;
        }
        else if (ParseToken(CMessageServerAdmin_ServerStatus_GetPlayerList,
                            &pString, nLength, 0))
        {
            if (nLength != 0)
            {
                return 0;
            }
            SendServerToServerAdminPlayerList(nPlayerId);
            return 1;
        }
    }

    if (ParseToken(CMessageServerAdmin_Module, &pString, nLength, 1))
    {
        if (ParseToken(CMessageServerAdmin_Module_Load, &pString, nLength, 0))
        {
            if (nLength == 0)
            {
                return 0;
            }
            CExoString sModule;
            if (ParseGetString(pString, nLength, sModule, 256))
            {
                g_pAppManager->GetServerExoApp()->LoadModule(
                    sModule, g_pAppManager->GetServerExoApp()
                                 ->GetClientObjectByPlayerId(nPlayerId, 0)
                                 ->AsSWSPlayer());
            }
            return 1;
        }
        else if (ParseToken(CMessageServerAdmin_Module_Run, &pString, nLength,
                            0))
        {
            if (nLength != 0)
            {
                return 0;
            }
            g_pAppManager->GetServerExoApp()->RunModule();
            return 1;
        }
        else if (ParseToken(CMessageServerAdmin_Module_Save, &pString, nLength,
                            0))
        {
            if (nLength == 0)
            {
                return 0;
            }
            if (!ParseGetString(pString, nLength, sParameter, 256))
            {
                return 1;
            }

            CExoString sSaveName;
            CExoString sSaveDirectory;
            int nSpace = sParameter.Find(' ', 0);
            if (nSpace > 0)
            {
                int nSlot = sParameter.Left(nSpace).AsINT();
                sSaveName = sParameter.SubString(nSpace + 1, -1);
                if (nSlot != 1 && nSlot > 0 && sSaveName.GetLength() > 0)
                {
                    CExoString sSaveGame;
                    CExoArrayList<CExoString> aSaveGames;
                    g_pExoBase->GetDirectoryList(&aSaveGames, "SAVES:", 0xFFFF,
                                                 1, 0);
                    int bFound = 0;
                    int nSaves = aSaveGames.num;
                    for (int nSave = 0; nSave < nSaves; nSave++)
                    {
                        if (aSaveGames[nSave].AsINT() == nSlot)
                        {
                            bFound = 1;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        g_pAppManager->GetServerExoApp()->SaveGame(
                            nSlot, sSaveName, NULL, 0);
                    }
                }
            }
            return 1;
        }
    }

    if (ParseToken(CMessageServerAdmin_Control, &pString, nLength, 1))
    {
        if (ParseToken(CMessageServerAdmin_Control_Boot, &pString, nLength, 1))
        {
            if (nLength == 0)
            {
                return 0;
            }
            if (ParseGetString(pString, nLength, sParameter, 256))
            {
                sParameter.AsINT();
            }
        }
        else if (ParseToken(CMessageServerAdmin_Control_ChangeSAPassword,
                            &pString, nLength, 1))
        {
            if (nLength == 0)
            {
                return 0;
            }
            ParseGetString(pString, nLength, sParameter, 256);
        }
        else if (ParseToken(CMessageServerAdmin_Control_Say, &pString, nLength,
                            0))
        {
            if (nLength == 0)
            {
                return 0;
            }
            if (ParseGetString(pString, nLength, sParameter, 256))
            {
                CExoLinkedList<CSWSClient>* pList =
                    g_pAppManager->GetServerExoApp()->GetSysAdminList();
                for (int nList = 0; nList < 2; nList++)
                {
                    CExoLinkedListNode* pPos = pList->GetHeadPos();
                    while (pPos != NULL)
                    {
                        CSWSClient* pClient =
                            (CSWSClient*)g_pAppManager->GetServerExoApp()
                                ->GetPlayerList()
                                ->GetAtPos(pPos);
                        g_pAppManager->GetServerExoApp()
                            ->GetSWSMessage()
                            ->SendServerToPlayerChatMessage(
                                5, OBJECT_INVALID, sParameter,
                                pClient->GetPlayerId(), "");
                        g_pAppManager->GetServerExoApp()
                            ->GetPlayerList()
                            ->GetNext(pPos);
                    }
                    pList = g_pAppManager->GetServerExoApp()->GetPlayerList();
                }
            }
        }
    }

    CExoString sCategory;
    CExoString sName;
    CExoString sValue;
    int nTokens = GetTokenPair(pString, nLength, sCategory, sName, sValue);
    int bChanged = 0;
    CServerInfo* pServerInfo =
        g_pAppManager->GetServerExoApp()->GetServerInfo();

    while (nTokens == 3)
    {
        int nOption =
            g_pAppManager->GetServerExoApp()->GetServerInfo()->FindOptionIndex(
                sCategory, sName);
        if (nOption != 0)
        {
            g_pAppManager->GetServerExoApp()
                ->GetServerInfo()
                ->m_nDataChangedFlags |= CServerInfo::DATA_CHANGED_OPTIONS;
            SServerOption* pOption = &pServerInfo->m_aOptions[nOption];
            switch (pOption->nType)
            {
            case 1:
                *pOption->pnValue = sValue[0] == '1';
                break;
            case 2:
                *pOption->pnValue = atoi(sValue.CStr());
                break;
            }

            SendServerToServerAdminMessage(CNetLayer::PLAYERID_ALL_SERVERADMINS,
                                           sCategory + "." + sName + " " +
                                               sValue);
            bChanged = 1;

            int nPos;
            for (nPos = 0; sValue[nPos] != 0; nPos++)
            {
                if (sValue[nPos] == '\n')
                {
                    break;
                }
            }
            if (sValue[nPos] != '\n')
            {
                return 1;
            }

            int nRemaining = sValue.GetLength() - nPos - 1;
            nTokens = GetTokenPair((unsigned char*)sValue.CStr() + nPos + 1,
                                   nRemaining, sCategory, sName, sValue);
        }
        else
        {
            return bChanged;
        }
    }
    return 0;
}
