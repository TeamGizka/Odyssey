#include "exobase.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwsarea.h"
#include "nwsplayer.h"
#include "nwmessage.h"
#include "nwsambientsound.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "exoresman.h"

const int CSWSAmbientSound::BATTLE_DELAY = 5000;
// PC 0x005741D0 GOG 0x00574300 Xbox 0x00282590
CSWSAmbientSound::CSWSAmbientSound(unsigned long oidArea)
{
    m_oidArea = oidArea;
}
// PC 0x00574220 GOG 0x00574350 Xbox 0x002825E0
int CSWSAmbientSound::Load(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    m_nMusicDelay =
        pRes->ReadFieldINT(pStruct, "MusicDelay", bSuccess, m_nMusicDelay);
    m_nMusicDayTrack =
        pRes->ReadFieldINT(pStruct, "MusicDay", bSuccess, m_nMusicDayTrack);
    m_nMusicNightTrack =
        pRes->ReadFieldINT(pStruct, "MusicNight", bSuccess, m_nMusicNightTrack);
    m_nBattleMusicTrack = pRes->ReadFieldINT(pStruct, "MusicBattle", bSuccess,
                                             m_nBattleMusicTrack);
    m_nAmbientDayTrack = pRes->ReadFieldINT(pStruct, "AmbientSndDay", bSuccess,
                                            m_nAmbientDayTrack);
    m_nAmbientNightTrack = pRes->ReadFieldINT(pStruct, "AmbientSndNight",
                                              bSuccess, m_nAmbientNightTrack);
    m_nAmbientDayVolume = pRes->ReadFieldINT(pStruct, "AmbientSndDayVol",
                                             bSuccess, m_nAmbientDayVolume);
    m_nAmbientNightVolume = pRes->ReadFieldINT(pStruct, "AmbientSndNitVol",
                                               bSuccess, m_nAmbientNightVolume);
    return bSuccess;
}

// Dead code: not in the PC executable
void CSWSAmbientSound::CacheSound(int nTrack, int bMusic)
{
    CExoString sResource;
    CExoString sFileName;
    C2DA* pTable = bMusic ? g_pRules->m_p2DArrays->m_pAmbientMusic
                          : g_pRules->m_p2DArrays->m_pAmbientSound;

    if (pTable->GetCExoStringEntry(nTrack, CExoString("Resource"), &sResource))
    {
        sFileName.Format("HD0:STREAMMUSIC\\%s", sResource.CStr());
        g_pExoResMan->CacheResource(sFileName, RESTYPE_WAV);
    }
}
// PC 0x00574310 GOG 0x00574440 Xbox 0x002826D0
int CSWSAmbientSound::Save(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldINT(pStruct, m_nMusicDelay, "MusicDelay");
    pRes->WriteFieldINT(pStruct, m_nMusicDayTrack, "MusicDay");
    pRes->WriteFieldINT(pStruct, m_nMusicNightTrack, "MusicNight");
    pRes->WriteFieldINT(pStruct, m_nBattleMusicTrack, "MusicBattle");
    pRes->WriteFieldINT(pStruct, m_nAmbientDayTrack, "AmbientSndDay");
    pRes->WriteFieldINT(pStruct, m_nAmbientNightTrack, "AmbientSndNight");
    pRes->WriteFieldINT(pStruct, m_nAmbientDayVolume, "AmbientSndDayVol");
    return pRes->WriteFieldINT(pStruct, m_nAmbientNightVolume,
                               "AmbientSndNitVol");
}
// PC 0x005743B0 GOG 0x005744E0 Xbox 0x00282770
void CSWSAmbientSound::PackIntoMessage(CSWSMessage* pMessage)
{
    pMessage->WriteBOOL(m_bMusicEnabled);
    pMessage->WriteBOOL(m_bBattleMusic);
    pMessage->WriteBOOL(m_bAmbientPlaying);
    pMessage->WriteINT(m_nMusicDelay, 32);
    pMessage->WriteINT(m_nMusicDayTrack, 32);
    pMessage->WriteINT(m_nMusicNightTrack, 32);
    pMessage->WriteINT(m_nBattleMusicTrack, 32);
    pMessage->WriteINT(m_nAmbientDayTrack, 32);
    pMessage->WriteINT(m_nAmbientNightTrack, 32);
    pMessage->WriteBYTE(m_nAmbientDayVolume, 8);
    pMessage->WriteBYTE(m_nAmbientNightVolume, 8);
}
// PC 0x00574450 GOG 0x00574580 Xbox 0x00282810
int CSWSAmbientSound::GetPlayersInArea(CExoArrayList<unsigned long>* pPlayerList)
{
    CExoLinkedList<CSWSClient>* pPlayers;
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;
    CSWSCreature* pCreature;

    pPlayerList->SetSize(0);
    pPlayers = g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayers != NULL)
    {
        pNode = pPlayers->GetHeadPos();
        while (pNode != NULL)
        {
            pPlayer = (CSWSPlayer*)pPlayers->GetAtPos(pNode);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
            {
                pCreature = pPlayer->GetGameObject()->AsSWSCreature();
                if (pCreature != NULL && pCreature->GetArea() != NULL &&
                    pCreature->GetArea()->GetId() == m_oidArea)
                {
                    pPlayerList->Add(pPlayer->m_nPlayerId);
                }
            }
            pPlayers->GetNext(pNode);
        }
    }
    return pPlayerList->num;
}
// PC 0x00574530 GOG 0x00574660 Xbox 0x002828F0
void CSWSAmbientSound::PlayMusic(int bPlay)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;

    m_bMusicEnabled = bPlay;
    nPlayerCount = GetPlayersInArea(&lstPlayers);
    for (i = 0; i < nPlayerCount; i++)
    {
        pMessage->SendServerToPlayerAmbientMusicPlay(
            lstPlayers[i], bPlay);
    }
}
// PC 0x005745D0 GOG 0x00574700 Xbox 0x00282960
void CSWSAmbientSound::SetMusicDelay(int nDelay)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (m_nMusicDelay != nDelay)
    {
        m_nMusicDelay = nDelay;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientMusicSetDelay(
                lstPlayers[i], nDelay);
        }
    }
}
// PC 0x00574670 GOG 0x005747A0 Xbox 0x002829E0
void CSWSAmbientSound::SetMusicDayTrack(int nTrack,
                                        int bStreamingMusicForMovie)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    m_nMusicDayTrack = nTrack;
    nPlayerCount = GetPlayersInArea(&lstPlayers);
    for (i = 0; i < nPlayerCount; i++)
    {
        pMessage->SendServerToPlayerAmbientMusicChangeTrack(
            lstPlayers[i], 1, nTrack, bStreamingMusicForMovie);
    }
}
// PC 0x00574720 GOG 0x00574850 Xbox 0x00282A60
void CSWSAmbientSound::SetMusicNightTrack(int nTrack,
                                          int bStreamingMusicForMovie)
{
    int i;
    int nPlayerCount;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSMessage* pMessage = pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (m_nMusicNightTrack != nTrack)
    {
        m_nMusicNightTrack = nTrack;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientMusicChangeTrack(
                lstPlayers[i], 0, nTrack, bStreamingMusicForMovie);
        }
    }
}
// PC 0x005747E0 GOG 0x00574910 Xbox 0x00282AE0
void CSWSAmbientSound::PlayBattleMusic(int bPlay)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (m_bBattleMusic != bPlay)
    {
        m_bBattleMusic = bPlay;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientBattleMusicPlay(
                lstPlayers[i], bPlay);
        }
    }
}
// PC 0x00574880 GOG 0x005749B0 Xbox 0x00282B60
void CSWSAmbientSound::SetBattleMusicTrack(int nTrack)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (m_nBattleMusicTrack != nTrack)
    {
        m_nBattleMusicTrack = nTrack;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientBattleMusicChange(
                lstPlayers[i], nTrack);
        }
    }
}
// PC 0x00574920 GOG 0x00574A50 Xbox 0x00282BE0
void CSWSAmbientSound::PlayAmbientSound(int bPlay)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    m_bAmbientPlaying = bPlay;
    nPlayerCount = GetPlayersInArea(&lstPlayers);
    for (i = 0; i < nPlayerCount; i++)
    {
        pMessage->SendServerToPlayerAmbientSoundLoopPlay(lstPlayers[i],
                                                         bPlay);
    }
}
// PC 0x005749C0 GOG 0x00574AF0 Xbox 0x00282C50
void CSWSAmbientSound::SetAmbientDayTrack(int nTrack)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (m_nAmbientDayTrack != nTrack)
    {
        m_nAmbientDayTrack = nTrack;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientSoundLoopChange(
                lstPlayers[i], 1, nTrack);
        }
    }
}
// PC 0x00574A70 GOG 0x00574BA0 Xbox 0x00282CD0
void CSWSAmbientSound::SetAmbientNightTrack(int nTrack)
{
    int i;
    int nPlayerCount;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSMessage* pMessage = pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (m_nAmbientNightTrack != nTrack)
    {
        m_nAmbientNightTrack = nTrack;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientSoundLoopChange(
                lstPlayers[i], 0, nTrack);
        }
    }
}
// PC 0x00574B20 GOG 0x00574C50 Xbox 0x00282D50
void CSWSAmbientSound::SetAmbientDayVolume(int nVolume)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (nVolume >= 0 && nVolume <= 100 && m_nAmbientDayVolume != nVolume)
    {
        m_nAmbientDayVolume = nVolume;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientSoundVolumeChange(
                lstPlayers[i], 1, nVolume);
        }
    }
}
// PC 0x00574BD0 GOG 0x00574D00 Xbox 0x00282DE0
void CSWSAmbientSound::SetAmbientNightVolume(int nVolume)
{
    int i;
    int nPlayerCount;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CExoArrayList<unsigned long> lstPlayers;
    if (nVolume >= 0 && nVolume <= 100 && m_nAmbientNightVolume != nVolume)
    {
        m_nAmbientNightVolume = nVolume;
        nPlayerCount = GetPlayersInArea(&lstPlayers);
        for (i = 0; i < nPlayerCount; i++)
        {
            pMessage->SendServerToPlayerAmbientSoundVolumeChange(
                lstPlayers[i], 0, nVolume);
        }
    }
}
