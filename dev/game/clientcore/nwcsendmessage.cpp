#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwmessage.h"
#include "gameobjectarray.h"

#include "netlayer.h"
#include "exobase.h"
#include "nwclass.h"
#include "swguiingame.h"
#include "nwccreaturestats.h"
#include "nwccreature.h"
#include "clientexoappinternal.h"

char cheatreturnvalue[256];

int g_bShowCheatResult = 1;
CExoString g_returnCode;

char* dm_setrace(char* szArgs)
{
    unsigned short nRace;
    if (stricmp(szArgs, "human") == 0)
    {
        nRace = 6;
    }
    else if (stricmp(szArgs, "halforc") == 0)
    {
        nRace = 5;
    }
    else if (stricmp(szArgs, "elf") == 0)
    {
        nRace = 1;
    }
    else if (stricmp(szArgs, "halfling") == 0)
    {
        nRace = 3;
    }
    else if (stricmp(szArgs, "dwarf") == 0)
    {
        nRace = 0;
    }
    else if (stricmp(szArgs, "gnome") == 0)
    {
        nRace = 2;
    }
    else
    {
        nRace = (unsigned short)atoi(szArgs);
    }

    CExoString sLabel;
    g_pRules->m_p2DArrays->m_pAppearance->GetCExoStringEntry(nRace, CExoString("LABEL"),
                                                             &sLabel);
    char* szResult = "Failure";
    if (sLabel.GetLength() >= 1)
    {
        CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pCreature != NULL)
        {
            pCreature->GetStats()->m_nRace = nRace;
            szResult = "Success";
        }
    }
    return szResult;
}

#ifndef _XBOX
char* setvideomode(char* szArgs)
{
    int nWidth = 800;
    int nHeight = 600;
    int nBitsPerPixel = 32;
    int nRefreshRate = 60;
    sscanf(szArgs, "%i %i %i %i", &nWidth, &nHeight, &nBitsPerPixel, &nRefreshRate);

    DEVMODE cDevMode;
    memset(&cDevMode, 0, sizeof(cDevMode));
    cDevMode.dmSize = sizeof(cDevMode);
    for (unsigned long nMode = 0; EnumDisplaySettings(NULL, nMode, &cDevMode); nMode++)
    {
        if (cDevMode.dmPelsWidth == (unsigned long)nWidth &&
            cDevMode.dmPelsHeight == (unsigned long)nHeight &&
            cDevMode.dmBitsPerPel == (unsigned long)nBitsPerPixel &&
            cDevMode.dmDisplayFrequency == (unsigned long)nRefreshRate &&
            (g_pAppManager->m_pClientExoApp->GetAllowHighMonitorFrequency() != 0 ||
             (float)cDevMode.dmDisplayFrequency <=
                 (float)CClientExoAppInternal::VIDEO_MAX_STANDARD_REFRESH_RATE))
        {
            g_pAppManager->m_pClientExoApp->SetVideoMode(nMode);
            return "Success";
        }
        memset(&cDevMode, 0, sizeof(cDevMode));
        cDevMode.dmSize = sizeof(cDevMode);
    }
    return "Failure";
}
#endif

int equal(const Vector& vA, const Vector& vB);
// PC 0x00677410  Xbox 0x00050490
int CSWCMessage::SendPlayerToServerMessage(unsigned char nMajor,
                                           unsigned char nMinor,
                                           unsigned char* pData,
                                           unsigned long nSize)
{
    unsigned long nLength;
    unsigned char* pMessage;
    int nResult;

    if (nSize != 0)
    {
        nLength = nSize;
        pMessage = pData;
    }
    else
    {
        nLength = 3;
        pMessage = new unsigned char[3];
    }

    if (pMessage == NULL)
    {
        return 0;
    }

    pMessage[0] = 'p';
    pMessage[1] = nMajor;
    pMessage[2] = nMinor;
    nResult =
        g_pAppManager->m_pClientExoApp->GetNetLayer()->SendMessageToPlayer(
            CNetLayer::PLAYERID_SERVER, pMessage, nLength, 0);
    if (nSize == 0)
    {
        delete[] pMessage;
    }
    return nResult;
}
// PC 0x00677490  Xbox 0x00050510
int CSWCMessage::SendPlayerToServerModule_ModuleLoaded()
{
    return SendPlayerToServerMessage(3, 2, NULL, 0);
}
// PC 0x006774E0  Xbox 0x00050560
#ifdef _XBOX
int CSWCMessage::SendPlayerToServerModule_SaveGame(unsigned long nSlot,
                                                   const CExoString& sName)
#else
int CSWCMessage::SendPlayerToServerModule_SaveGame(unsigned long nSlot,
                                                   const CExoString& sName,
                                                   const CExoString& sPath)
#endif
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(0x80, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nSlot, 32);
    WriteCExoString(sName, 32);
#ifndef _XBOX
    WriteCExoString(sPath, 32);
#endif
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(3, 4, pBuffer, nSize);
}
// PC 0x00677560  Xbox 0x000505D0
int CSWCMessage::SendPlayerToServerModule_LoadGame(unsigned long nSlot,
                                                   const CExoString& sName,
                                                   const CExoString& sPath)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(0x80, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nSlot, 32);
    WriteCExoString(sName, 32);
    WriteCExoString(sPath, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(3, 5, pBuffer, nSize);
}
// PC 0x006775E0  Xbox 0x00050650
int CSWCMessage::SendPlayerToServerStatus_RequestStatus()
{
    return SendPlayerToServerMessage(1, 0, NULL, 0);
}
// PC 0x00677630  Xbox 0x00050710
int CSWCMessage::SendPlayerToServerGuiContainer_Close(
    unsigned long oidContainer, int bTakeAll)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidContainer);
    WriteBOOL(bTakeAll);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(0x19, 2, pBuffer, nSize);
}
// PC 0x006776A0  Xbox 0x00050780
int CSWCMessage::SendPlayerToServerInput_CastSpell(
    unsigned long nSpell, unsigned char nMultiClass, unsigned char nDomainLevel,
    unsigned char nMetaMagic, int bCheat, const Vector& vTarget,
    unsigned long oidTarget)
{
    float fLength;
    unsigned char nFlags = 0;
    unsigned long nSize = 4;
    unsigned char* pBuffer;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    if (oidTarget != OBJECT_INVALID)
    {
        nFlags = 1;
    }
    fLength = vTarget.m_fX * vTarget.m_fX + vTarget.m_fY * vTarget.m_fY +
              vTarget.m_fZ * vTarget.m_fZ;
    if (fLength >= 0.00001f)
    {
        nFlags += 2;
    }
    if (bCheat != 0)
    {
        nFlags += 4;
    }
    WriteDWORD(nSpell, 32);
    WriteBYTE(nMultiClass, 8);
    WriteBYTE(nDomainLevel, 8);
    WriteBYTE(nMetaMagic, 8);
    WriteBYTE(nFlags, 8);
    if (oidTarget != OBJECT_INVALID)
    {
        WriteOBJECTIDClient(oidTarget);
    }
    if (fLength >= 0.00001f)
    {
        WriteFLOAT(vTarget.m_fX, 1.0f, 32);
        WriteFLOAT(vTarget.m_fY, 1.0f, 32);
        WriteFLOAT(vTarget.m_fZ + 0.3f, 1.0f, 32);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 0x12, pBuffer, nSize);
}
// PC 0x00677800  Xbox 0x000508F0
int CSWCMessage::SendPlayerToServerInput_TogglePauseRequest()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp != NULL)
    {
        pServerExoApp->TogglePauseState(2);
    }
    return 1;
}
// PC 0x00677820  Xbox 0x00050910
int CSWCMessage::SendPlayerToServerJournalMessage_QuestScreenOpen()
{
    unsigned char* pMessage = new unsigned char[3];
    if (pMessage == 0)
    {
        return 0;
    }
    pMessage[0] = 112;
    pMessage[1] = 28;
    pMessage[2] = 10;
    int nResult =
        g_pAppManager->m_pClientExoApp->GetNetLayer()->SendMessageToPlayer(
            CNetLayer::PLAYERID_SERVER, pMessage, 3, 0);
    delete[] pMessage;
    return nResult;
}
// PC 0x00677870  Xbox 0x00050960
int CSWCMessage::SendPlayerToServerInput_GiveItem(unsigned long oidItem,
                                                  unsigned long oidTarget,
                                                  unsigned long oidOwner,
                                                  int nStack)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(0x10, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidItem);
    WriteOBJECTIDClient(oidTarget);
    WriteOBJECTIDClient(oidOwner);
    WriteINT(nStack, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 0x24, pBuffer, nSize);
}
// PC 0x006778F0  Xbox 0x00051230
int CSWCMessage::SendPlayerToServerArea_AreaLoaded()
{
    return SendPlayerToServerMessage(4, 3, NULL, 0);
}
// PC 0x00677940  Xbox 0x000512D0
int CSWCMessage::SendPlayerToServerCheat_Resurrect()
{
    CResRef cSound("HEAL_PLAYER");

    g_pExoSound->PlayOneShotSound(cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
    return SendPlayerToServerMessage(15, 2, NULL, 0);
}
// PC 0x006779D0
int CSWCMessage::SendPlayerToServerCheat_Invulnerability()
{
    return SendPlayerToServerMessage(15, 4, NULL, 0);
}
// PC 0x00677A20  Xbox 0x00051450
int CSWCMessage::SendPlayerToServerGuiInventory_Status(int nStatus)
{
    unsigned char* pBuffer;
    unsigned long nSize = 1;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nStatus == 1, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(13, 1, pBuffer, nSize);
}
// PC 0x00677A90  Xbox 0x000514C0
int CSWCMessage::SendPlayerToServerInput_Attack(unsigned long oidTarget,
                                                unsigned short nAction)
{
    unsigned char* pBuffer = NULL;
    unsigned long nSize = 6;

    CreateWriteMessage(6, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidTarget);
    WriteWORD(nAction, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 2, pBuffer, nSize);
}
// PC 0x00677B10  Xbox 0x00051540
int CSWCMessage::SendPlayerToServerInput_UseSkill(unsigned char nSkill,
                                                  unsigned char nUseType,
                                                  unsigned long oidTarget,
                                                  const Vector& vTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 18;

    CreateWriteMessage(18, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nSkill, 8);
    WriteBYTE(nUseType, 8);
    WriteOBJECTIDClient(oidTarget);
    WriteFLOAT(vTarget.m_fX, 1.0f, 32);
    WriteFLOAT(vTarget.m_fY, 1.0f, 32);
    WriteFLOAT(vTarget.m_fZ, 1.0f, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 7, pBuffer, nSize);
}
// PC 0x00677BD0  Xbox 0x00051600
int CSWCMessage::SendPlayerToServerInput_UseItem(unsigned long oidItem,
                                                 unsigned char nPropertyIndex,
                                                 unsigned char nSubIndex,
                                                 unsigned long oidTarget,
                                                 const Vector& vTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 21;

    CreateWriteMessage(21, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidItem);
    WriteBYTE(nPropertyIndex, 8);
    if (nSubIndex != 0xFF)
    {
        WriteBOOL(1);
        WriteBYTE(nSubIndex, 8);
    }
    else
    {
        WriteBOOL(0);
    }
    if (oidTarget != OBJECT_INVALID)
    {
        WriteBOOL(1);
        WriteOBJECTIDClient(oidTarget);
    }
    else
    {
        WriteBOOL(0);
    }
    Vector vZero;
    if (!equal(vTarget, vZero))
    {
        WriteBOOL(1);
        WriteFLOAT(vTarget.m_fX, 1.0f, 32);
        WriteFLOAT(vTarget.m_fY, 1.0f, 32);
        WriteFLOAT(vTarget.m_fZ, 1.0f, 32);
    }
    else
    {
        WriteBOOL(0);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 9, pBuffer, nSize);
}
// PC 0x00677D10  Xbox 0x00051730
int CSWCMessage::SendPlayerToServerInput_UseObject(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 11, pBuffer, nSize);
}

int CSWCMessage::SendPlayerToServerParty_Action(unsigned long oidTarget,
                                                int nAction)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidTarget);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    switch (nAction)
    {
    case 0:
        return SendPlayerToServerMessage(0x0E, 6, pBuffer, nSize);
    case 1:
        return SendPlayerToServerMessage(0x0E, 7, pBuffer, nSize);
    case 3:
        return SendPlayerToServerMessage(0x0E, 8, pBuffer, nSize);
    case 4:
        return SendPlayerToServerMessage(0x0E, 9, pBuffer, nSize);
    case 5:
        return SendPlayerToServerMessage(0x0E, 10, pBuffer, nSize);
    case 6:
        return SendPlayerToServerMessage(0x0E, 11, pBuffer, nSize);
    case 7:
        return SendPlayerToServerMessage(0x0E, 12, pBuffer, nSize);
    case 8:
        return SendPlayerToServerMessage(0x0E, 2, NULL, 0);
    }
    return 0;
}
// PC 0x00677D70  Xbox 0x00051790
int CSWCMessage::SendPlayerToServerInput_ChangeDoorState(unsigned long oidDoor,
                                                         unsigned short nState)
{
    unsigned char* pBuffer;
    unsigned long nSize = 6;

    CreateWriteMessage(6, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidDoor);
    WriteWORD(nState, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 3, pBuffer, nSize);
}
// PC 0x00677DE0  Xbox 0x00051800
int CSWCMessage::SendPlayerToServerInput_UnlockObject(unsigned long oidObject,
                                                      unsigned long oidKey)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    WriteOBJECTIDClient(oidKey);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 12, pBuffer, nSize);
}
// PC 0x00677E50  Xbox 0x00051870
int CSWCMessage::SendPlayerToServerInput_AbortDriveControl()
{
    unsigned char* pMessage = new unsigned char[3];
    if (pMessage == 0)
    {
        return 0;
    }
    pMessage[0] = 112;
    pMessage[1] = 6;
    pMessage[2] = 33;
    int nResult =
        g_pAppManager->m_pClientExoApp->GetNetLayer()->SendMessageToPlayer(
            CNetLayer::PLAYERID_SERVER, pMessage, 3, 0);
    delete[] pMessage;
    return nResult;
}
// PC 0x00677EA0  Xbox 0x000518C0
int CSWCMessage::SendPlayerToServerInput_Dialog(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState ==
        0)
    {
        nSize = 4;
        CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDClient(oidObject);
        if (GetWriteMessage(&pBuffer, &nSize) == 0)
        {
            return 0;
        }
        return SendPlayerToServerMessage(6, 8, pBuffer, nSize);
    }
    return 1;
}
// PC 0x00677F30  Xbox 0x00051950
int CSWCMessage::SendPlayerToServerInventory_Equip(unsigned long oidItem,
                                                   unsigned long nSlot)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidItem);
    WriteDWORD(nSlot, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 1, pBuffer, nSize);
}
// PC 0x00677FA0  Xbox 0x00051BE0
int CSWCMessage::SendPlayerToServerLogin_LoginLocalCharacter(
    unsigned char* pData, unsigned long nSize, int bNewCharacter)
{
    unsigned long nLength;
    unsigned char* pBuffer;
    int nResult;

    nLength = nSize;
    CreateWriteMessage(nSize + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nLength, 32);
    WriteVOIDPtr(pData, nLength);
    nResult = GetWriteMessage(&pBuffer, &nSize);
    if (nResult == 0)
    {
        return nResult;
    }
    if (bNewCharacter == 0)
    {
        return SendPlayerToServerMessage(2, 1, pBuffer, nSize);
    }
    return SendPlayerToServerMessage(2, 0x13, pBuffer, nSize);
}
// PC 0x00678030  Xbox 0x00051C70
int CSWCMessage::SendPlayerToServerLogin_LoginIFOCharacter()
{
    return SendPlayerToServerMessage(2, 15, NULL, 0);
}
// PC 0x00678080  Xbox 0x00052760
int CSWCMessage::SendPlayerToServerLogin_LoginLocalCharacter(
    const CResRef& cResRef, int bNewCharacter)
{
    unsigned long nSize;
    unsigned char* pData;
    int nResult;
    CExoString sFileName;
    CExoFile* pFile;

    cResRef.CopyToString(sFileName);
    if (sFileName.CompareNoCase("test") != 0)
    {
        sFileName = CExoString("LOCALVAULT:") + sFileName;
    }
    else
    {
        sFileName = CExoString("TEMP:") + sFileName;
    }

    pFile = new CExoFile(sFileName, RESTYPE_BIC, "rb");
    if (pFile->FileOpened() == 0)
    {
        delete pFile;
        return 0;
    }

    nSize = pFile->GetSize();
    pData = new unsigned char[nSize + 4];
    pFile->Read(pData, 1, nSize);
    delete pFile;
    nResult = SendPlayerToServerLogin_LoginLocalCharacter(pData, nSize,
                                                          bNewCharacter);
    delete[] pData;
    return nResult;
}

static inline unsigned char LevelUpAbilityScore(CSWCLevelUpStats* pStats, int nAbility)
{
    return pStats->m_aAbilities[nAbility];
}
// PC 0x00678280  Xbox 0x00052960
int CSWCMessage::SendPlayerToServer_LevelUp(CSWCLevelUpStats* pOldStats,
                                            CSWCLevelUpStats* pNewStats)
{
    unsigned char* pBuffer;
    unsigned long nSize = 100;
    unsigned char nSkills;
    unsigned char nSkill;
    unsigned char nSpell;
    unsigned char nClass;
    unsigned char i;
    unsigned short j;
    unsigned long nSkillMask;
    int bFound;

    CreateWriteMessage(100, RECIPIENT_UNSPECIFIED, 1);
    CSWCLevelUpStats& rNewStats = *pNewStats;
    WriteBYTE(rNewStats.GetClass(rNewStats.m_nClassCount - 1), 8);
    int nLevel = pNewStats->GetLevel();
    if (nLevel % 4 == 0)
    {
        if (LevelUpAbilityScore(pNewStats, 0) > pOldStats->m_aAbilities[0])
        {
            WriteBYTE(0, 8);
        }
        else if (LevelUpAbilityScore(pNewStats, 1) > pOldStats->m_aAbilities[1])
        {
            WriteBYTE(1, 8);
        }
        else if (LevelUpAbilityScore(pNewStats, 2) > pOldStats->m_aAbilities[2])
        {
            WriteBYTE(2, 8);
        }
        else if (LevelUpAbilityScore(pNewStats, 3) > pOldStats->m_aAbilities[3])
        {
            WriteBYTE(3, 8);
        }
        else if (LevelUpAbilityScore(pNewStats, 4) > pOldStats->m_aAbilities[4])
        {
            WriteBYTE(4, 8);
        }
        else if (LevelUpAbilityScore(pNewStats, 5) > pOldStats->m_aAbilities[5])
        {
            WriteBYTE(5, 8);
        }
    }

    nSkillMask = 0;
    nSkill = 0;
    nSkills = g_pRules->m_nNumSkills;
    while (nSkill < nSkills)
    {
        if (pNewStats->GetSkillRank(nSkill) != pOldStats->GetSkillRank(nSkill))
        {
            nSkillMask |= 1 << nSkill;
            nSkill++;
        }
        else
        {
            nSkill++;
        }
    }
    WriteDWORD(nSkillMask, 32);
    for (nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
    {
        if ((1 << nSkill) & nSkillMask)
        {
            WriteCHAR(pNewStats->GetSkillRank(nSkill) -
                          pOldStats->GetSkillRank(nSkill),
                      8);
        }
    }

    WriteWORD(pNewStats->GetSkillPoints(), 16);
    WriteBYTE(pNewStats->GetNumFeats() - pOldStats->GetNumFeats(), 8);
    for (j = 0; j < (unsigned short)pNewStats->GetNumFeats(); j++)
    {
        unsigned short nFeat = pNewStats->GetFeat(j);
        int bAcquired = pOldStats->FeatAcquired(nFeat, NULL);
        if (bAcquired == 0)
        {
            WriteWORD(nFeat, 16);
        }
    }

    nClass = pNewStats->GetNumClasses() - 1;
    {
        CExoArrayList<unsigned long> lstGained;
        CExoArrayList<unsigned long> lstLost;
        for (nSpell = 0; nSpell < pNewStats->GetNumberKnownSpells(nClass, 0); nSpell++)
        {
            unsigned long nKnown = pNewStats->GetKnownSpell(nClass, 0, nSpell);
            bFound = 0;
            for (unsigned char nOther = 0; !bFound && nOther < pOldStats->GetNumberKnownSpells(nClass, 0);
                 nOther++)
            {
                if (nKnown == pOldStats->GetKnownSpell(nClass, 0, nOther))
                {
                    bFound = 1;
                }
            }
            if (bFound == 0)
            {
                lstGained.Add(nKnown);
            }
        }

        for (nSpell = 0; nSpell < pOldStats->GetNumberKnownSpells(nClass, 0);
             nSpell++)
        {
            unsigned long nKnown = pOldStats->GetKnownSpell(nClass, 0, nSpell);
            bFound = 0;
            for (unsigned char nOther = 0; !bFound; nOther++)
            {
                unsigned short nNewCount = pNewStats->GetNumberKnownSpells(nClass, 0);
                if (nOther >= nNewCount)
                    break;
                if (nKnown == pNewStats->GetKnownSpell(nClass, 0, nOther))
                {
                    bFound = 1;
                }
            }
            if (bFound == 0)
            {
                lstLost.Add(nKnown);
            }
        }

        WriteBYTE(lstGained.num, 8);
        for (i = 0; i < lstGained.num; i++)
        {
            WriteDWORD(lstGained.element[i], 32);
        }
        WriteBYTE(lstLost.num, 8);
        for (i = 0; i < lstLost.num; i++)
        {
            WriteDWORD(lstLost.element[i], 32);
        }
    }

    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(29, 0, pBuffer, nSize);
}
