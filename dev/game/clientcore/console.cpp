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
// PC 0x00656580
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
        CExoString sFile(".\\swkotor2.ini");

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
// PC 0x006566F0
void heal()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetSWCMessage()
            ->SendPlayerToServerCheat_Resurrect();
    }
}
// PC 0x00656710
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
// PC 0x006567A0
void setcomputeruse(char* szValue)
{
    CheatSetSkill(0, szValue);
}
// PC 0x006567B0
void setdemolitions(char* szValue)
{
    CheatSetSkill(1, szValue);
}
// PC 0x006567C0
void setstealth(char* szValue)
{
    CheatSetSkill(2, szValue);
}
// PC 0x006567D0
void setawareness(char* szValue)
{
    CheatSetSkill(3, szValue);
}
// PC 0x006567E0
void setpersuade(char* szValue)
{
    CheatSetSkill(4, szValue);
}
// PC 0x006567F0
void setrepair(char* szValue)
{
    CheatSetSkill(5, szValue);
}
// PC 0x00656800
void setsecurity(char* szValue)
{
    CheatSetSkill(6, szValue);
}
// PC 0x00656810
void settreatinjury(char* szValue)
{
    CheatSetSkill(7, szValue);
}
// PC 0x00656820
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
// PC 0x00656920
void setstrength(char* szValue)
{
    CheatSetAbility(0, szValue);
}
// PC 0x00656930
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
// PC 0x006569C0
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
// PC 0x00656A60
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
// PC 0x00656AF0
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
// PC 0x00656B80
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
// PC 0x00656C10
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
// PC 0x00656C70
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
// PC 0x00656CB0
void invulnerability()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetSWCMessage()
            ->SendPlayerToServerCheat_Invulnerability();
    }
}
// PC 0x00656CD0
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
// PC 0x00656D10
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
// PC 0x00656D50
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
// PC 0x00656DC0
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
// PC 0x00656E40
void revealmap()
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_bShowFogOfWar = g_bShowFogOfWar == 0;
    }
}
// PC 0x00656E60
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
// PC 0x00656EE0
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
// PC 0x00656F60
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
// PC 0x00656FF0
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
// PC 0x00657080
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
// PC 0x00657110
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
// PC 0x006571A0
void givesitharmour()
{
    if (GetCheatsTurnedOn() != 0)
    {
        CResRef cResRef("ptar_sitharmor");
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;

        pClient->CreatureAcquireItem(pClient->GetPlayerCreatureId(), cResRef);
    }
}
// PC 0x006571E0
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
// PC 0x006572B0
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
// PC 0x006572F0
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
// PC 0x00657420
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

#ifndef _XBOX
void dancedancemalak()  // PC 0x00657510
{
    if (GetCheatsTurnedOn() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bDanceDanceMalak = 1;
    }
}
#endif
// PC 0x00657540
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

void ToggleMovieNotification()
{
    g_bMovieNotification = !g_bMovieNotification;
}

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
#ifndef _XBOX
ConsoleFunc ConFdancedancemalak("dancedancemalak", dancedancemalak);
#endif
#ifdef _XBOX
ConsoleFunc ConFToggleMovieNotification("ToggleMovieNotification", ToggleMovieNotification);
ConsoleFunc ConFToggleLiveContentIgnore("ToggleLiveContentIgnore", ToggleLiveContentIgnore);
#endif

#ifdef _XBOX

#include "worldtimer.h"
#include "exoinput.h"
#include "twodimarray.h"

extern int g_bEnableScreenCapture;

char* fps(char* szArgs)
{
    static float fWindow = 2.0f;
    static char szFPS[64];
    static float fLastLow;
    static float fAverageFrameTime;
    static float fFrames;
    static float fTotalFrameTime;
    static float fLow;

    g_fFPS = 1000000.0f / (int)g_pAppManager->m_pClientExoApp->GetWorldTimer()->GetSnapshotTimeDifference();
    float fFrameTime = 1.0f / g_fFPS;
    fWindow += fFrameTime;
    if (fWindow > 2.0f)
    {
        fWindow -= 2.0f;
        fAverageFrameTime = fTotalFrameTime / fFrames;
        fLastLow = fLow;
        fLow = g_fFPS;
        fTotalFrameTime = 0.0f;
        fFrames = 0.0f;
    }
    if (fWindow < -2.0f)
    {
        fWindow = -2.0f;
    }
    if (g_fFPS <= fLow)
    {
        fLow = g_fFPS;
    }
    fTotalFrameTime += fFrameTime;
    fFrames += 1.0f;
    sprintf(szFPS, "%04.1f (%04.1f) [%04.1f] %01.5f", g_fFPS, fLastLow > fLow ? fLow : fLastLow,
            1.0f / fAverageFrameTime, fFrameTime);
    return szFPS;
}

char* MacroRecord(char* szArgs)
{
    if (*szArgs == 0)
    {
        szArgs = "inputmacro.default";
    }
    if (!g_pAppManager->m_pClientExoApp->GetCaptureEvents())
    {
        g_pAppManager->GetClientExoApp()->SetCaptureEvents(szArgs);
        return "Macro is recording...";
    }
    g_pAppManager->GetClientExoApp()->SetCaptureEvents("");
    return "Finished Macro recording";
}

char* MacroStop(char* szArgs)
{
    g_pAppManager->GetClientExoApp()->SetCaptureEvents("");
    return "Finished Macro recording";
}

char* MacroPlay(char* szArgs)
{
    if (g_pAppManager->m_pClientExoApp->GetCaptureEvents())
    {
        return "Ignoring that one...";
    }
    g_pAppManager->GetClientExoApp()->SetCaptureEventPlayBack(*szArgs ? szArgs : "inputmacro.default");
    g_pAppManager->m_pClientExoApp->SetCaptureEventPlayBack(5);
    g_bEnableScreenCapture = 0;
    return "Playing Macro...";
}

int keyboardmode(char* szArgs)
{
    if (szArgs != NULL)
    {
        switch (*szArgs)
        {
        case '0':
            g_pExoInput->KeyboardModeOff();
            break;
        case '1':
            g_pExoInput->KeyboardModeOn();
            break;
        }
    }
    return g_pExoInput->GetKeyboardMode() != 0;
}

char* PlayMovie(char* szArgs)
{
    if (*szArgs == 0)
    {
        return "usage: PlayMovie sMovieName";
    }
    g_pExoInput->ClearEvents();
    g_pAppManager->m_pClientExoApp->ResetDriveAcceleration();
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
    int nResult;
    g_pAppManager->GetClientExoApp()->PlayMovie(szArgs, nResult, 0, 0);
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    return "Playing movie...";
}

char* Binarize2DA(char* szArgs)
{
    static char szResult[256];
    CExoStringList* pList;
    char szFile[256];

    if (szArgs != NULL && *szArgs != 0)
    {
        CExoString* pName = new CExoString(szArgs);
        pList = new CExoStringList;
        pList->Add(pName);
    }
    else
    {
        pList = g_pExoResMan->GetResOfType(RESTYPE_2DA, 0);
    }

    int nBinarized = 0;
    int nSkipped = 0;
    for (int i = 0; i < pList->GetCount(); i++)
    {
        C2DA c2DA((*pList)[i]->CStr(), 0);
        if (c2DA.Load2DArray())
        {
            if (c2DA.m_bPacked)
            {
                nSkipped++;
            }
            else
            {
                sprintf(szFile, "logs\%s.2da", (*pList)[i]->CStr());
                if (c2DA.SaveBinary2da(szFile))
                {
                    nBinarized++;
                }
            }
        }
    }

    int nFailed = pList->GetCount() - nSkipped - nBinarized;
    if (nFailed == 0)
    {
        sprintf(szResult, "Binarized %d files to logs\; %d skipped", nBinarized, nSkipped);
    }
    else
    {
        sprintf(szResult, "**** %d files failed!  Binarized %d files to logs\; %d skipped", nFailed,
                nBinarized, nSkipped);
    }
    if (pList != NULL)
    {
        delete pList;
    }
    return szResult;
}

ConsoleFunc ConFfps("fps", fps);
ConsoleFunc ConFMacroRecord("MacroRecord", MacroRecord);
ConsoleFunc ConFMacroStop("MacroStop", MacroStop);
ConsoleFunc ConFMacroPlay("MacroPlay", MacroPlay);
ConsoleFunc ConFlockcamera("lockcamera", lockcamera);
ConsoleFunc ConFkeyboardmode("keyboardmode", keyboardmode);
ConsoleFunc ConFPlayMovie("PlayMovie", PlayMovie);
ConsoleFunc ConFAurSnapshot("AurSnapshot", AurSnapshot);
ConsoleFunc ConFBinarize2DA("Binarize2DA", Binarize2DA);
#endif
