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

#ifndef _XBOX
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
#endif
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
// PC 0x006CCAE0
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
// PC 0x006CCB60
int CSWCMessage::SendPlayerToServerModule_ModuleLoaded()
{
    return SendPlayerToServerMessage(3, 2, NULL, 0);
}
// PC 0x006CCBB0
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
// PC 0x006CCC30
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
// PC 0x006CCCB0
int CSWCMessage::SendPlayerToServerStatus_RequestStatus()
{
    return SendPlayerToServerMessage(1, 0, NULL, 0);
}
// PC 0x006CCD00
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
// PC 0x006CCD70
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
// PC 0x006CCED0
int CSWCMessage::SendPlayerToServerInput_TogglePauseRequest()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp != NULL)
    {
        pServerExoApp->TogglePauseState(2);
    }
    return 1;
}
// PC 0x006CCEF0
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
// PC 0x006CCF40
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
// PC 0x006CCFC0
int CSWCMessage::SendPlayerToServerArea_AreaLoaded()
{
    return SendPlayerToServerMessage(4, 3, NULL, 0);
}
#ifndef _XBOX
int CSWCMessage::SendPlayerToServerCheat_Resurrect()  // PC 0x006CD010
{
    CResRef cSound("HEAL_PLAYER");

    g_pExoSound->PlayOneShotSound(cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
    return SendPlayerToServerMessage(15, 2, NULL, 0);
}
#endif
int CSWCMessage::SendPlayerToServerCheat_Invulnerability()  // PC 0x006CD0A0
{
    return SendPlayerToServerMessage(15, 4, NULL, 0);
}
// PC 0x006CD0F0
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
// PC 0x006CD160
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
// PC 0x006CD1E0
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
// PC 0x006CD2A0
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
// PC 0x006CD450
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
// PC 0x006CD4B0
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
// PC 0x006CD520
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
// PC 0x006CD5F0
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
// PC 0x006CD640
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
// PC 0x006CD6D0
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
// PC 0x006CD740
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
// PC 0x006CD7D0
int CSWCMessage::SendPlayerToServerLogin_LoginIFOCharacter()
{
    return SendPlayerToServerMessage(2, 15, NULL, 0);
}
// PC 0x006CD820
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
// PC 0x006CDA30
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

#ifdef _XBOX

int CSWCMessage::SendPlayerToServerInventory_Unequip(unsigned long oidItem, unsigned long oidRepository)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidItem);
    WriteOBJECTIDClient(oidRepository);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 7, pBuffer, nSize);
}
#endif

#ifdef _XBOX

#include "aurconsole.h"
#include "nwrules.h"

char* NetProfile(char* szArgs)
{
    if (g_pAppManager->m_pClientExoApp->GetSWCMessage() == NULL)
        sprintf(cheatreturnvalue, "Failure");
    return cheatreturnvalue;
}
ConsoleFunc ConFNetProfile("NetProfile", NetProfile);

#define XBOX_DICE_COMMAND(nSides)                                                \
    char* d##nSides(char* szArgs)                                                \
    {                                                                            \
        int nDice = atoi(szArgs);                                                \
        if (nDice < 1)                                                           \
            nDice = 1;                                                           \
        int nRoll = g_pRules->RollDice((unsigned char)nDice, nSides);            \
        sprintf(cheatreturnvalue, "Roll Dice %dd%d: %d", nDice, nSides, nRoll);  \
        return cheatreturnvalue;                                                 \
    }                                                                            \
    ConsoleFunc ConFd##nSides("d" #nSides, d##nSides);
XBOX_DICE_COMMAND(2)
XBOX_DICE_COMMAND(3)
XBOX_DICE_COMMAND(4)
XBOX_DICE_COMMAND(6)
XBOX_DICE_COMMAND(8)
XBOX_DICE_COMMAND(10)
XBOX_DICE_COMMAND(12)
XBOX_DICE_COMMAND(20)
XBOX_DICE_COMMAND(100)

int CSWCMessage::SendPlayerToServerCheat_ComputeSafeLocation()
{
    return SendPlayerToServerMessage(15, 32, NULL, 0);
}

int CSWCMessage::SendPlayerToServerCheat_PlayVisualEffect(unsigned long oidTarget,
                                                          unsigned short nVisualEffect,
                                                          float fDuration,
                                                          unsigned long oidSource,
                                                          unsigned char nType)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(15, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidTarget);
    WriteSHORT(nVisualEffect, 16);
    WriteFLOAT(fDuration, 1.0f, 32);
    WriteOBJECTIDClient(oidSource);
    WriteCHAR(nType, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 10, pBuffer, nSize);
}

int CSWCMessage::SendPlayerToServerCheat_DebugMode(int bDebugMode)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bDebugMode);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 21, pBuffer, nSize);
}

char* dm_heal(char* szArgs)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
        return cheatreturnvalue;
    }
    pMessage->SendPlayerToServerCheat_Resurrect();
    sprintf(cheatreturnvalue, "Success");
    return cheatreturnvalue;
}
ConsoleFunc ConFdm_heal("dm_heal", dm_heal);

char* makesafe(char* szArgs)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
        return cheatreturnvalue;
    }
    pMessage->SendPlayerToServerCheat_ComputeSafeLocation();
    sprintf(cheatreturnvalue, "Success");
    return cheatreturnvalue;
}
ConsoleFunc ConFmakesafe("makesafe", makesafe);

char* DebugMode(char* szArgs)
{
    int bDebugMode = atoi(szArgs);
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    pMessage->SendPlayerToServerCheat_DebugMode(bDebugMode);
    sprintf(cheatreturnvalue, "Success");
    return cheatreturnvalue;
}
ConsoleFunc ConFDebugMode("DebugMode", DebugMode);

char* visualeffect(char* szArgs)
{
    CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pGui == NULL || pMessage == NULL)
    {
        return "Failure: NULL pointers";
    }
    unsigned long oidSource = g_pAppManager->m_pClientExoApp->GetPlayerCreatureId();
    unsigned long oidTarget = g_pAppManager->m_pClientExoApp->GetLastTarget();
    if (oidTarget == OBJECT_INVALID || oidSource == OBJECT_INVALID)
    {
        return "Failure: Missing target or source";
    }
    int nVisualEffect = 0;
    float fDuration = 0.0f;
    sscanf(szArgs, "%i %f", &nVisualEffect, &fDuration);
    CExoString sType;
    g_pRules->Get2DArrays()->GetVisualEffect()->GetCExoStringEntry(CExoString(nVisualEffect), "Type_FD",
                                                               &sType);
    if (sType.CompareNoCase("F"))
    {
        fDuration = 0.0f;
    }
    else if (sType.CompareNoCase("D") && fDuration <= 0.0f)
    {
        fDuration = 1.0f;
    }
    pMessage->SendPlayerToServerCheat_PlayVisualEffect(oidTarget, (unsigned short)nVisualEffect,
                                                       fDuration, oidSource, 0);
    return "Success";
}
ConsoleFunc ConFvisualeffect("visualeffect", visualeffect);
#endif

#ifdef _XBOX

#include "nwcarea.h"
#include "nwcmodule.h"
#include "nwsobject.h"
#include "nwscreaturestats.h"
#include "exotlktable.h"

int CSWCMessage::SendPlayerToServerCheat_Resurrect()
{
    g_pExoSound->PlayOneShotSound("HEAL_PLAYER", CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
    return SendPlayerToServerMessage(15, 2, NULL, 0);
}

int CSWCMessage::SendPlayerToServerUpdate_AppearanceRace(unsigned long oidCreature, unsigned short nRace)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(3, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('R', 8);
    WriteOBJECTIDClient(oidCreature);
    WriteWORD(nRace, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

int CSWCMessage::SendPlayerToServerItem_Add(unsigned long oidArea, const Vector& vPosition,
                                            const CResRef& cResRef, unsigned char nA,
                                            unsigned char nB, unsigned char nC)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(0x24, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('A', 8);
    WriteOBJECTIDClient(oidArea);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    WriteBYTE(nA, 8);
    WriteBYTE(nB, 8);
    WriteBYTE(nC, 8);
    WriteCResRef(cResRef, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

int CSWCMessage::SendPlayerToServerParty_Add(unsigned long oidArea, const Vector& vPosition,
                                             const CExoString& sTemplate)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sTemplate.GetLength() + 0x11, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('M', 8);
    WriteOBJECTIDClient(oidArea);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    WriteCExoString(sTemplate, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

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
    else if (stricmp(szArgs, "halfelf") == 0)
    {
        nRace = 4;
    }
    else
    {
        nRace = (unsigned short)atoi(szArgs);
    }

    CExoString sLabel;
    g_pRules->Get2DArrays()->GetAppearance()->GetCExoStringEntry(nRace, "LABEL", &sLabel);
    if (sLabel.GetLength() < 1)
    {
        return "Failure";
    }
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage->SendPlayerToServerUpdate_AppearanceRace(pCreature->GetId(), nRace) == 0)
    {
        return "Failure";
    }
    return "Success";
}

char* dm_gettime(char* szArgs)
{
    char szLine[128];

    if (g_pAppManager->m_pClientExoApp->GetModule() != NULL)
    {
        unsigned char nMinutesPerHour = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeOfDayHour;
        int nYear = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeA4;
        int nMonth = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeA8;
        int nDay = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeAC;
        int nHour = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeB0;
        unsigned char nState = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeOfDayState;

        sprintf(szLine, "MinutesPerHour: %i", nMinutesPerHour);
        AurPostString(szLine, 0, 4, 5.0f);
        sprintf(szLine, "Year: %i", nYear);
        AurPostString(szLine, 0, 5, 5.0f);
        sprintf(szLine, "Month: %i", nMonth);
        AurPostString(szLine, 0, 6, 5.0f);
        sprintf(szLine, "Day: %i", nDay);
        AurPostString(szLine, 0, 7, 5.0f);
        sprintf(szLine, "Hour: %i", nHour);
        AurPostString(szLine, 0, 8, 5.0f);
        switch (nState)
        {
        case 1:
            sprintf(szLine, "TimeOfDayState: Day");
            break;
        case 2:
            sprintf(szLine, "TimeOfDayState: Night");
            break;
        case 3:
            sprintf(szLine, "TimeOfDayState: Dawn");
            break;
        case 4:
            sprintf(szLine, "TimeOfDayState: Dusk");
            break;
        default:
            sprintf(szLine, "TimeOfDayState: Uh oh...");
            break;
        }
        AurPostString(szLine, 0, 9, 5.0f);
        return "Success";
    }
    return "Failure";
}

char* giveforcepowers(char* szArgs)
{
    CExoIni cIni;
    CExoString sValue;
    CExoString sKey;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(pServerExoApp->GetPlayerCreatureId());
    int bFound = 0;

    for (int i = 1; i < 1000; i++)
    {
        sKey.Format("power%d", i);
        int bRead = cIni.ReadIniEntry(sValue, "d:\\forcepowers.ini", "Force Powers", sKey);
        if (bRead != 1)
        {
            break;
        }
        bFound = 1;
        unsigned long nSpell = sValue.AsINT();
        if (!pCreature->m_pStats->HasSpell(0, nSpell, 1))
        {
            pCreature->AddPrerequisiteSpells(nSpell);
        }
    }
    if (bFound)
    {
        sprintf(cheatreturnvalue, "Success");
    }
    else
    {
        sprintf(cheatreturnvalue, "Failed: no entries");
    }
    return cheatreturnvalue;
}

char* spawnitem(char* szArgs)
{
    char szResRef[256];

    CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    if (pPlayer == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
        return cheatreturnvalue;
    }
    CSWCArea* pArea = pPlayer->m_pArea;
    if (pArea == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
        return cheatreturnvalue;
    }
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
        return cheatreturnvalue;
    }

    char* pSpace = strchr(szArgs, ' ');
    if (pSpace != NULL)
    {
        *pSpace = 0;
        strcpy(szResRef, szArgs);
        szArgs = pSpace + 1;
    }
    else
    {
        strcpy(szResRef, szArgs);
    }

    Vector vPosition = g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_vPosition;
    Vector vOrientation = g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_vOrientation;
    Vector vSpawn = vPosition + normalize(vOrientation);

    pSpace = strchr(szArgs, ' ');
    if (pSpace != NULL)
    {
        *pSpace = 0;
        vSpawn.m_fX = (float)atof(szArgs);
        szArgs = pSpace + 1;
        pSpace = strchr(szArgs, ' ');
        if (pSpace != NULL)
        {
            *pSpace = 0;
            vSpawn.m_fY = (float)atof(szArgs);
            szArgs = pSpace + 1;
            if (strlen(szArgs) != 0)
            {
                vSpawn.m_fZ = (float)atof(szArgs);
            }
            else
            {
                vSpawn.m_fZ = 0.2f;
            }
        }
    }

    pMessage->SendPlayerToServerItem_Add(pArea->GetId(), vSpawn, szResRef, 0, 0, 0);
    sprintf(cheatreturnvalue, "Success");
    return cheatreturnvalue;
}

char* spawnweapon(char* szArgs)
{
    char szResRef[256];

    CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    CSWCArea* pArea;
    if (pPlayer == NULL || (pArea = pPlayer->m_pArea) == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
        return cheatreturnvalue;
    }
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage == NULL)
    {
        sprintf(cheatreturnvalue, "Failure");
    }
    else
    {
        char* pSpace = strchr(szArgs, ' ');
        if (pSpace != NULL)
        {
            *pSpace = 0;
            strcpy(szResRef, szArgs);
            szArgs = pSpace + 1;
        }
        else
        {
            strcpy(szResRef, szArgs);
        }

        Vector vPosition = g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_vPosition;
        Vector vOrientation = g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_vOrientation;
        Vector vSpawn = normalize(vOrientation) + vPosition;

        unsigned char nA = 0;
        unsigned char nB = 0;
        unsigned char nC = 0;
        pSpace = strchr(szArgs, ' ');
        if (pSpace != NULL)
        {
            *pSpace = 0;
            nA = (unsigned char)atoi(szArgs);
            szArgs = pSpace + 1;
            pSpace = strchr(szArgs, ' ');
            if (pSpace != NULL)
            {
                *pSpace = 0;
                nB = (unsigned char)atoi(szArgs);
                nC = (unsigned char)atoi(pSpace + 1);
            }
        }

        pMessage->SendPlayerToServerItem_Add(pArea->GetId(), vSpawn, szResRef, nA, nB, nC);
        sprintf(cheatreturnvalue, "Success");
    }
    return cheatreturnvalue;
}

char* displaystrref(char* szArgs)
{
    unsigned long nFirst = 0;
    unsigned long nLast = 0;
    unsigned long nGender = (unsigned long)-1;
    STR_RES strRes;
    CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();

    sscanf(szArgs, "%u %u %u", &nFirst, &nLast, &nGender);
    if (nGender == (unsigned long)-1)
    {
        return "";
    }
    if (nLast < nFirst)
    {
        nLast = nFirst;
    }
    g_returnCode = szArgs;
    for (unsigned long nStrRef = nFirst; nStrRef <= nLast; nStrRef++)
    {
        g_pTlkTable->Fetch(nStrRef, strRes, (unsigned char)nGender);
        pGui->AppendToMsgBuffer(strRes.m_sString, 0x10000000, 0);
    }
    g_pExoSound->PlayOneShotSound(strRes.m_resSound, 0, 0, 0, 0.0f, 0.0f);
    return g_returnCode.CStr();
}

char* addpartymember(char* szArgs)
{
    CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    if (pPlayer == NULL)
    {
        sprintf(cheatreturnvalue, "Failure - Player creture is NULL");
        return cheatreturnvalue;
    }
    CSWCArea* pArea = pPlayer->m_pArea;
    if (pArea == NULL)
    {
        sprintf(cheatreturnvalue, "Failure - Area is NULL");
        return cheatreturnvalue;
    }
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage == NULL)
    {
        sprintf(cheatreturnvalue, "Failure - Message is NULL");
        return cheatreturnvalue;
    }

    Vector vPosition = pPlayer->m_vPosition;
    float fX = rand() % 100 < 50 ? (rand() % 100) * 0.01f + 1.0f : -1.0f - (rand() % 100) * 0.01f;
    float fY = rand() % 100 < 50 ? (rand() % 100) * 0.01f + 1.0f : -1.0f - (rand() % 100) * 0.01f;
    vPosition = Vector(vPosition.m_fX + fX, vPosition.m_fY + fY, vPosition.m_fZ);
    vPosition.m_fZ += 0.2f;
    pMessage->SendPlayerToServerParty_Add(pArea->GetId(), vPosition, "PARTYMEMBER");
    sprintf(cheatreturnvalue, "Success");
    return cheatreturnvalue;
}

ConsoleFunc ConFspawnitem("spawnitem", spawnitem);
ConsoleFunc ConFspawnweapon("spawnweapon", spawnweapon);
ConsoleFunc ConFdm_setrace("dm_setrace", dm_setrace);
ConsoleFunc ConFdm_gettime("dm_gettime", dm_gettime);
ConsoleFunc ConFdisplaystrref("displaystrref", displaystrref);
ConsoleFunc ConFaddpartymember("addpartymember", addpartymember);
ConsoleFunc ConFgiveforcepowers("giveforcepowers", giveforcepowers);
#endif
