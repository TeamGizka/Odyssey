#include "exobase.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "clientexoapp.h"
#include "partytables.h"
#include "nwmessage.h"
#include "nwsobject.h"
#include "nwscreaturestats.h"
#include "nwsplayer.h"
#include "nwclass.h"
#include "exoresman.h"
#include "clientoptions.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "aurcamera.h"
#include "aurbehavior.h"
#include "aurconsole.h"
#include "guiingame.h"

#include <stdio.h>
#include <string.h>

float g_fFPS = 1.0f;
int g_bEnableLiveContentIgnore = 0;

int g_bProcessModuleMemUsage = 0;
char* g_szProcessModuleMemName = 0;
// PC 0x0060A300
int GetCheatsTurnedOn()
{
    int nCheats = 0;
    CExoIni cIni;
    CExoString sValue;

    if (g_pAppManager->m_pServerExoApp == NULL)
    {
        return 0;
    }

    int bRead;
    {
        CExoString sKey("EnableCheats");
        CExoString sSection("Game Options");
        CExoString sFile(".\\swkotor.ini");

        bRead = cIni.ReadIniEntry(sValue, sFile, sSection, sKey);
    }
    if (bRead)
    {
        nCheats = sValue.AsINT();
        if (nCheats != 0)
        {
#ifndef _XBOX
            g_pAppManager->m_pServerExoApp->GetPartyTable()->m_bCheatUsed = 1;
#endif
        }
    }
    return nCheats == 1;
}
// PC 0x0060A460
void heal()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetSWCMessage()
            ->SendPlayerToServerCheat_Resurrect();
    }
}
// PC 0x0060A480
void CheatSetSkill(int nSkill, char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                pCreature->m_pStats->SetSkillRank(nSkill, nValue);
            }
        }
    }
}
// PC 0x0060A510
void setcomputeruse(char* szValue)
{
    CheatSetSkill(0, szValue);
}
// PC 0x0060A520
void setdemolitions(char* szValue)
{
    CheatSetSkill(1, szValue);
}
// PC 0x0060A530
void setstealth(char* szValue)
{
    CheatSetSkill(2, szValue);
}
// PC 0x0060A540
void setawareness(char* szValue)
{
    CheatSetSkill(3, szValue);
}
// PC 0x0060A550
void setpersuade(char* szValue)
{
    CheatSetSkill(4, szValue);
}
// PC 0x0060A560
void setrepair(char* szValue)
{
    CheatSetSkill(5, szValue);
}
// PC 0x0060A570
void setsecurity(char* szValue)
{
    CheatSetSkill(6, szValue);
}
// PC 0x0060A580
void settreatinjury(char* szValue)
{
    CheatSetSkill(7, szValue);
}
// PC 0x0060A590
void CheatSetAbility(int nAbility, char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    switch (nAbility)
                    {
                    case 0:
                        pStats->SetSTRBase(nValue);
                        break;
                    case 1:
                        pStats->SetDEXBase(nValue);
                        break;
                    case 2:
                        pStats->SetCONBase(nValue, 1);
                        break;
                    case 3:
                        pStats->SetINTBase(nValue);
                        break;
                    case 4:
                        pStats->SetWISBase(nValue);
                        break;
                    case 5:
                        pStats->SetCHABase(nValue);
                        break;
                    }
                }
            }
        }
    }
}
// PC 0x0060A690
void setstrength(char* szValue)
{
    CheatSetAbility(0, szValue);
}
// PC 0x0060A6A0
void setdexterity(char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    pStats->SetDEXBase(nValue);
                }
            }
        }
    }
}
// PC 0x0060A730
void setconstitution(char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    pStats->SetCONBase(nValue, 1);
                }
            }
        }
    }
}
// PC 0x0060A7D0
void setintelligence(char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    pStats->SetINTBase(nValue);
                }
            }
        }
    }
}
// PC 0x0060A860
void setwisdom(char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    pStats->SetWISBase(nValue);
                }
            }
        }
    }
}
// PC 0x0060A8F0
void setcharisma(char* szValue)
{
    int nValue = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nValue);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            if (nValue < 0)
            {
                nValue = 0;
            }
            else if (nValue > 99)
            {
                nValue = 99;
            }
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    pStats->SetCHABase(nValue);
                }
            }
        }
    }
}
// PC 0x0060A980
void addexp(char* szValue)
{
    int nExperience = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nExperience);
        if (nExperience > 0)
        {
            CSWPartyTable* pPartyTable =
                g_pAppManager->m_pServerExoApp->GetPartyTable();
            if (pPartyTable != NULL)
            {
                pPartyTable->DistributeExperience(nExperience, 1);
            }
        }
    }
}
// PC 0x0060A9E0
void turbo()
{
    if (GetCheatsTurnedOn() != 0)
    {
        int nFlags =
            g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCheatFlags;

        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCheatFlags =
            nFlags == 0;
    }
}
// PC 0x0060AA20
void invulnerability()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetSWCMessage()
            ->SendPlayerToServerCheat_Invulnerability();
    }
}
// PC 0x0060AA40
void restartminigame()
{
    if (GetCheatsTurnedOn() != 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo->m_pMiniGame !=
            NULL)
        {
            g_pAppManager->m_pClientExoApp->GetModule()
                ->m_pInfo->RestartMiniGame();
        }
    }
}
// PC 0x0060AA80
void bright()
{
    if (GetCheatsTurnedOn() != 0)
    {
        if (g_pAppManager != NULL)
        {
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            if (pClient != NULL)
            {
                CSWCModule* pModule = pClient->GetModule();
                if (pModule != NULL)
                {
                    CSWCArea* pArea = pModule->m_pInfo;
                    if (pArea != NULL)
                    {
                        pArea->ForceBrightSunAndMoon();
                    }
                }
            }
        }
    }
}
// PC 0x0060AAC0
void addlightside(int nAmount)
{
    if (GetCheatsTurnedOn() != 0)
    {
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                short nShift = max(-100, min(100, nAmount));
                pCreature->m_pStats->ModifyAlignment(nShift);
            }
        }
    }
}
// PC 0x0060AB30
void adddarkside(int nAmount)
{
    if (GetCheatsTurnedOn() != 0)
    {
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                short nShift = min(100, max(-100, -nAmount));
                pCreature->m_pStats->ModifyAlignment(nShift);
            }
        }
    }
}
// PC 0x0060ABB0
void revealmap()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_bShowFogOfWar = g_bShowFogOfWar == 0;
    }
}
// PC 0x0060ABD0
void givecredits(char* szValue)
{
    int nCredits = 0;

    sscanf(szValue, "%d", &nCredits);
    if (GetCheatsTurnedOn() != 0)
    {
        if (nCredits > 0)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId());
            if (pCreature != NULL)
            {
                int nGold = nCredits;
                if (nCredits <= 99)
                {
                    nGold = 99;
                }
                pCreature->SetGold(nGold + pCreature->GetGold());
            }
        }
    }
}
// PC 0x0060AC50
void giveitem(char* szValue)
{
    if (GetCheatsTurnedOn() != 0)
    {
        char szResRef[256];
        char* pszSpace = strchr(szValue, ' ');

        if (pszSpace != NULL)
        {
            *pszSpace = 0;
            strcpy(szResRef, szValue);
        }
        else
        {
            strcpy(szResRef, szValue);
        }
        CResRef cResRef(szResRef);
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

        pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(), cResRef);
    }
}
// PC 0x0060ACD0
void givemed(char* szValue)
{
    int nCount = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nCount);
        if (nCount == 0 || nCount > 99)
        {
            nCount = 99;
        }
        for (int i = 0; i < nCount; i++)
        {
            CResRef cResRef("g_i_medeqpmnt02");
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

            pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(),
                                         cResRef);
        }
    }
}
// PC 0x0060AD60
void giverepair(char* szValue)
{
    int nCount = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nCount);
        if (nCount == 0 || nCount > 99)
        {
            nCount = 99;
        }
        for (int i = 0; i < nCount; i++)
        {
            CResRef cResRef("g_i_drdrepeqp002");
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

            pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(),
                                         cResRef);
        }
    }
}
// PC 0x0060ADF0
void givecomspikes(char* szValue)
{
    int nCount = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nCount);
        if (nCount == 0 || nCount > 99)
        {
            nCount = 99;
        }
        for (int i = 0; i < nCount; i++)
        {
            CResRef cResRef("g_i_progspike01");
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

            pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(),
                                         cResRef);
        }
    }
}
// PC 0x0060AE80
void givesecspikes(char* szValue)
{
    int nCount = 0;

    if (GetCheatsTurnedOn() != 0)
    {
        sscanf(szValue, "%d", &nCount);
        if (nCount == 0 || nCount > 99)
        {
            nCount = 99;
        }
        for (int i = 0; i < nCount; i++)
        {
            CResRef cResRef("g_i_secspike01");
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

            pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(),
                                         cResRef);
        }
    }
}
// PC 0x0060AF10
void givesitharmour()
{
    if (GetCheatsTurnedOn() != 0)
    {
        CResRef cResRef("ptar_sitharmor");
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

        pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(), cResRef);
    }
}
// PC 0x0060AF50
void warp(char* szValue)
{
    if (GetCheatsTurnedOn() != 0)
    {
        char szModule[256];
        char* pszSpace = strchr(szValue, ' ');

        if (pszSpace != NULL)
        {
            *pszSpace = 0;
            strcpy(szModule, szValue);
        }
        else
        {
            strcpy(szModule, szValue);
        }
        {
            CExoString sModule(szModule);
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;

            pServer->SetMoveToModuleString(sModule);
        }
        g_pAppManager->m_pServerExoApp->SetMoveToModulePending(1);
    }
}
// PC 0x0060B020
void infiniteuses()
{
    if (GetCheatsTurnedOn() != 0)
    {
        int nFlags =
            g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDebugFlags;

        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDebugFlags =
            nFlags == 0;
    }
}
// PC 0x0060B060
void whereami()
{
    if (GetCheatsTurnedOn() != 0)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
            pServer->GetPlayerCreatureId());
        if (pCreature != NULL)
        {
            CExoString sMessage;
            if (g_pAppManager->m_pClientExoApp->GetModule() != NULL)
            {
                CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
                sMessage.Format("Module: %s",
                    pModule->m_sModuleName.CStr());
                AurPostString(sMessage.CStr(), 3, 3, 10.0f);
            }
            const Vector& vPosition = pCreature->m_vPosition;
            sMessage.Format("Position: (%3.2f, %3.2f, %3.2f)",
                (vPosition.m_fX), (vPosition.m_fY), (vPosition.m_fZ));
            AurPostString(sMessage.CStr(), 3, 4, 10.0f);
        }
    }
}
// PC 0x0060B190
void addlevel()
{
    if (GetCheatsTurnedOn() != 0)
    {
        int bJedi = 0;
        CSWSPlayer* pPlayer =
            g_pAppManager->m_pServerExoApp->GetClientObjectByPlayerId(0, 1);
        if (pPlayer != NULL)
        {
            unsigned long oidCreature = pPlayer->m_oidPCObject;
            CSWSCreature* pCreature =
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                    oidCreature);
            if (pCreature != NULL)
            {
                for (int i = 0; i < pCreature->m_pStats->m_nClassCount; i++)
                {
                    if (CSWClass::IsJedi(int(pCreature->m_pStats->GetClass(i))))
                    {
                        bJedi = 1;
                    }
                }
                if (bJedi || pCreature->m_pStats->GetLevel(0) != 15)
                {
                    if (pCreature->m_pStats->GetLevel(0) != 20)
                    {
                        CSWSCreatureStats* pStats = pCreature->m_pStats;
                        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;

                        pServer->GetPartyTable()->DistributeExperience(
                            pStats->GetExpNeededForLevelUp(), 1);
                    }
                }
            }
        }
    }
}
// PC 0x0060B270
void dancedancemalak()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bDanceDanceMalak = 1;
    }
}
// PC 0x0060B2A0  Xbox 0x0003AC00
int lockcamera(char* szValue)
{
    static int bLockcamera = 1;
    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();

    if (pModule != NULL)
    {
        if (pModule->m_pModuleCamera != NULL)
        {
            CAurBehavior* pBehavior = pModule->m_pModuleCamera->GetBehavior(-1);
            if (pBehavior != 0)
            {
                CAurBehaviorCameraFollow* pFollow =
                    pBehavior->AsBehaviorCameraFollow();
                if (pFollow != NULL && szValue != NULL)
                {
                    switch (*szValue)
                    {
                    case '0':
                        bLockcamera = 0;
                        pFollow->m_fClampDistance = 0.0f;
                        pFollow->m_fClampPitch = 0.0f;
                        break;
                    case '1':
                        bLockcamera = 1;
                        pFollow->m_fClampDistance = 1.0f;
                        pFollow->m_fClampPitch = 1.0f;
                        break;
                    }
                }
            }
        }
    }
    return bLockcamera;
}

#ifdef _XBOX

int g_bMovieNotification;
int g_bLiveContentIgnore;
// Xbox 0x0003ADA0
void ToggleMovieNotification()
{
    g_bMovieNotification = !g_bMovieNotification;
}
// Xbox 0x0003ADC0
void ToggleLiveContentIgnore()
{
    g_bLiveContentIgnore = !g_bLiveContentIgnore;
}
#endif

ConsoleFunc ConFheal("heal", heal);
ConsoleFunc ConFsetcomputeruse("setcomputeruse", setcomputeruse);
ConsoleFunc ConFsetdemolitions("setdemolitions", setdemolitions);
ConsoleFunc ConFsetstealth("setstealth", setstealth);
ConsoleFunc ConFsetawareness("setawareness", setawareness);
ConsoleFunc ConFsetpersuade("setpersuade", setpersuade);
ConsoleFunc ConFsetrepair("setrepair", setrepair);
ConsoleFunc ConFsetsecurity("setsecurity", setsecurity);
ConsoleFunc ConFsettreatinjury("settreatinjury", settreatinjury);
ConsoleFunc ConFsetstrength("setstrength", setstrength);
ConsoleFunc ConFsetdexterity("setdexterity", setdexterity);
ConsoleFunc ConFsetconstitution("setconstitution", setconstitution);
ConsoleFunc ConFsetintelligence("setintelligence", setintelligence);
ConsoleFunc ConFsetwisdom("setwisdom", setwisdom);
ConsoleFunc ConFsetcharisma("setcharisma", setcharisma);
ConsoleFunc ConFaddexp("addexp", addexp);
ConsoleFunc ConFturbo("turbo", turbo);
ConsoleFunc ConFinvulnerability("invulnerability", invulnerability);
ConsoleFunc ConFrestartminigame("restartminigame", restartminigame);
ConsoleFunc ConFbright("bright", bright);
ConsoleFunc ConFaddlightside("addlightside", addlightside);
ConsoleFunc ConFadddarkside("adddarkside", adddarkside);
ConsoleFunc ConFrevealmap("revealmap", revealmap);
ConsoleFunc ConFgivecredits("givecredits", givecredits);
ConsoleFunc ConFgiveitem("giveitem", giveitem);
ConsoleFunc ConFgivemed("givemed", givemed);
ConsoleFunc ConFgiverepair("giverepair", giverepair);
ConsoleFunc ConFgivecomspikes("givecomspikes", givecomspikes);
ConsoleFunc ConFgivesecspikes("givesecspikes", givesecspikes);
ConsoleFunc ConFgivesitharmour("givesitharmour", givesitharmour);
ConsoleFunc ConFwarp("warp", warp);
ConsoleFunc ConFinfiniteuses("infiniteuses", infiniteuses);
ConsoleFunc ConFwhereami("whereami", whereami);
ConsoleFunc ConFaddlevel("addlevel", addlevel);
ConsoleFunc ConFdancedancemalak("dancedancemalak", dancedancemalak);
#ifdef _XBOX
ConsoleFunc ConFToggleMovieNotification("ToggleMovieNotification", ToggleMovieNotification);
ConsoleFunc ConFToggleLiveContentIgnore("ToggleLiveContentIgnore", ToggleLiveContentIgnore);
#endif
