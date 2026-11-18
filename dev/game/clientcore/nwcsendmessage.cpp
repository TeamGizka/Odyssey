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
// Dead code: not in the PC executable
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
// Dead code: not in the PC executable
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
            (float)cDevMode.dmDisplayFrequency <=
                (float)CClientExoAppInternal::VIDEO_MAX_STANDARD_REFRESH_RATE)
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
// PC 0x006CCAE0 GOG 0x006CC7E0 Xbox 0x00050F90
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerChat_Party(const CExoString& sMessage)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sMessage.GetLength() + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sMessage, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(9, 6, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerChat_Shout(const CExoString& sMessage)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sMessage.GetLength() + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sMessage, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(9, 2, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerChat_Talk(const CExoString& sMessage)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sMessage.GetLength() + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sMessage, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(9, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerChat_Tell(unsigned long nPlayerId, const CExoString& sMessage)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sMessage.GetLength() + 12, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(0);
    WriteDWORD(nPlayerId, 32);
    WriteCExoString(sMessage, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(9, 4, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerChat_TellName(const CExoString& sName,
                                                 const CExoString& sMessage)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sName.GetLength() + sMessage.GetLength() + 12, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(1);
    WriteCExoString(sName, 32);
    WriteCExoString(sMessage, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(9, 4, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerChat_Whisper(const CExoString& sMessage)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sMessage.GetLength() + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sMessage, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(9, 3, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_MoveToArea(const CExoString& sArea,
                                                    const Vector& vPosition)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(64, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sArea, 32);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_PlayerPathfindRule(unsigned char nRule)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(64, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nRule, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 25, pBuffer, nSize);
}
// PC 0x006CCB60 GOG 0x006CC860 Xbox 0x00051010
int CSWCMessage::SendPlayerToServerModule_ModuleLoaded()
{
    return SendPlayerToServerMessage(3, 2, NULL, 0);
}
// PC 0x006CCBB0 GOG 0x006CC8B0
#ifdef _XBOX
int CSWCMessage::SendPlayerToServerModule_SaveGame(unsigned long nSlot,  // Xbox 0x00051060
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
// PC 0x006CCC30 GOG 0x006CC930 Xbox 0x000510D0
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerModule_RunModule()
{
    return SendPlayerToServerMessage(3, 7, NULL, 0);
}
// PC 0x006CCCB0 GOG 0x006CC9B0 Xbox 0x00051150
int CSWCMessage::SendPlayerToServerStatus_RequestStatus()
{
    return SendPlayerToServerMessage(1, 0, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerUpdate_AppearancePartVariation(unsigned long oidObject,
                                                                  unsigned char nPart,
                                                                  unsigned char nVariation)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(3, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('V', 8);
    WriteOBJECTIDClient(oidObject);
    WriteBYTE(nPart, 8);
    WriteBYTE(nVariation, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerUpdate_AppearanceHeadVariation(unsigned long oidObject,
                                                                  unsigned char nHeadVariation)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(2, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('H', 8);
    WriteOBJECTIDClient(oidObject);
    WriteBYTE(nHeadVariation, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerUpdate_AppearanceGender(unsigned long oidObject,
                                                           unsigned char nGender)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(2, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('G', 8);
    WriteOBJECTIDClient(oidObject);
    WriteBYTE(nGender, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}
// PC 0x006CCD00 GOG 0x006CCA00 Xbox 0x000511A0
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_AssociateCommand(unsigned long oidObject, int nCommand)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    WriteINT(nCommand, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 27, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerSpellMemorize(unsigned char nA, unsigned long oidObject,
                                                 unsigned char nB, unsigned char nC,
                                                 unsigned char nD)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nA, 8);
    WriteDWORD(oidObject, 32);
    WriteBYTE(nB, 8);
    WriteBYTE(nC, 8);
    WriteBYTE(nD, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 16, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInventory_LearnScroll(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 20;

    CreateWriteMessage(20, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 13, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerSpellUnMemorize(unsigned char nA, unsigned char nB,
                                                   unsigned char nC)
{
    unsigned char* pBuffer;
    unsigned long nSize = 3;

    CreateWriteMessage(3, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nA, 8);
    WriteBYTE(nB, 8);
    WriteBYTE(nC, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 17, pBuffer, nSize);
}
// PC 0x006CCD70 GOG 0x006CCA70 Xbox 0x00051210
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_CastSpellLikeAbility(unsigned long oidObject,
                                                              unsigned char nA,
                                                              const Vector& vPosition,
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
    fLength = vPosition.m_fX * vPosition.m_fX + vPosition.m_fY * vPosition.m_fY +
              vPosition.m_fZ * vPosition.m_fZ;
    if (fLength >= 0.00001f)
    {
        nFlags += 2;
    }
    WriteDWORD(oidObject, 32);
    WriteBYTE(nA, 8);
    WriteBYTE(nFlags, 8);
    if (oidTarget != OBJECT_INVALID)
    {
        WriteOBJECTIDClient(oidTarget);
    }
    if (fLength >= 0.00001f)
    {
        WriteFLOAT(vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(vPosition.m_fZ + 0.3f, 1.0f, 32);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 35, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_CancelAction(unsigned short nGroupId,
                                                      unsigned short nActionId)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteWORD(nGroupId, 16);
    WriteWORD(nActionId, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 20, pBuffer, nSize);
}
// PC 0x006CCED0 GOG 0x006CCBD0 Xbox 0x00051380
int CSWCMessage::SendPlayerToServerInput_TogglePauseRequest()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp != NULL)
    {
        pServerExoApp->TogglePauseState(2);
    }
    return 1;
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_SetPauseRequest(int bPause)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bPause);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 25, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_ExportRequest()
{
    return SendPlayerToServerMessage(6, 31, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerStore_Close()
{
    return SendPlayerToServerMessage(7, 5, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_MergeItem(unsigned long oidObject, unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    WriteOBJECTIDClient(oidTarget);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 22, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_SplitItem(unsigned long oidItem, int nCount)
{
    unsigned char* pBuffer;
    unsigned long nSize = 6;

    CreateWriteMessage(6, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidItem);
    WriteWORD(nCount, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 23, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_CancelDisguise(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 30, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerGold_TransferGold(unsigned long oidTarget,
                                                     const Vector& vPosition, int nAmount)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    if (oidTarget == OBJECT_INVALID)
    {
        nSize = 22;
    }
    else
    {
        nSize = 10;
    }
    CreateWriteMessage(nSize, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidTarget);
    WriteINT(nAmount, 32);
    if (oidTarget == OBJECT_INVALID)
    {
        WriteFLOAT(vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(8, 1, pBuffer, nSize);
}
// PC 0x006CCEF0 GOG 0x006CCBF0 Xbox 0x000513A0
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerJournalMessage_QuestScreenClosed()
{
    unsigned char* pMessage = new unsigned char[3];
    if (pMessage == 0)
    {
        return 0;
    }
    pMessage[0] = 112;
    pMessage[1] = 28;
    pMessage[2] = 11;
    int nResult =
        g_pAppManager->m_pClientExoApp->GetNetLayer()->SendMessageToPlayer(
            CNetLayer::PLAYERID_SERVER, pMessage, 3, 0);
    delete[] pMessage;
    return nResult;
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerJournalMessage_RequestAdd(const CExoString& sString,
                                                             const CExoString& sStringB)
{
    unsigned char* pBuffer;
    unsigned long nSize = sString.GetLength() + sStringB.GetLength() + 4;

    CreateWriteMessage(nSize, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sString, 32);
    WriteCExoString(sStringB, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(28, 13, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerJournalMessage_RequestDelete(int nIndex, int bStrRef)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nIndex, 32);
    WriteBOOL(bStrRef);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(28, 14, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerGuiQuickbar_SetButton_Item(unsigned char nA,
                                                              unsigned long oidObject, int nB,
                                                              unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(10, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nA, 8);
    WriteBYTE(1, 8);
    WriteOBJECTIDClient(oidObject);
    WriteINT(nB, 32);
    if (oidTarget == CGameObjectArray::INVALIDOBJID)
    {
        WriteBOOL(0);
    }
    else
    {
        WriteBOOL(1);
        WriteOBJECTIDClient(oidTarget);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(30, 2, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerGuiQuickbar_SetButton_Spell(unsigned char nA, unsigned char nB,
                                                               unsigned long oidObject,
                                                               unsigned char nC, unsigned char nD)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nA, 8);
    WriteBYTE(2, 8);
    WriteBYTE(nB, 8);
    WriteDWORD(oidObject, 32);
    WriteBYTE(nD, 8);
    WriteBYTE(nC, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(30, 2, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerGuiQuickbar_SetButton_SpellLikeAbility(unsigned char nA,
                                                                          unsigned long oidObject,
                                                                          unsigned char nB)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(7, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nA, 8);
    WriteBYTE(44, 8);
    WriteDWORD(oidObject, 32);
    WriteBYTE(nB, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(30, 2, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheatLevelUp()
{
    return SendPlayerToServerMessage(15, 18, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerMapPinAdded(const Vector& vPosition, const CExoString& sString)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sString.GetLength() + 12, RECIPIENT_UNSPECIFIED, 1);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    WriteCExoString(sString, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(32, 2, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerMapPinRemoved(int nMapPin)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nMapPin, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(32, 3, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerMapPinChanged(const Vector& vPosition, const CExoString& sNote,
                                                 int nMapPin)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sNote.GetLength() + 16, RECIPIENT_UNSPECIFIED, 1);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    WriteCExoString(sNote, 32);
    WriteINT(nMapPin, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(32, 5, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_GetObjectDebugInfo(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 21, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerGuiPlayerDeath_Respawn()
{
    return SendPlayerToServerMessage(37, 1, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCharacterDownloadGimme()
{
    return SendPlayerToServerMessage(43, 1, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerShutDownServer(int bSave)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bSave);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(47, 0, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerPlayModuleCharacterList_Start()
{
    return SendPlayerToServerMessage(49, 1, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerPlayModuleCharacterList_Stop()
{
    return SendPlayerToServerMessage(49, 2, NULL, 0);
}
// PC 0x006CCF40 GOG 0x006CCC40 Xbox 0x000513F0
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
// PC 0x006CCFC0 GOG 0x006CCCC0 Xbox 0x00051470
int CSWCMessage::SendPlayerToServerArea_AreaLoaded()
{
    return SendPlayerToServerMessage(4, 3, NULL, 0);
}
#ifndef _XBOX
int CSWCMessage::SendPlayerToServerCheat_Resurrect()  // PC 0x006CD010 GOG 0x006CCD10
{
    CResRef cSound("HEAL_PLAYER");

    g_pExoSound->PlayOneShotSound(cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
    return SendPlayerToServerMessage(15, 2, NULL, 0);
}
#endif
// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_ShowServerMem()
{
    return SendPlayerToServerMessage(15, 17, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_Kill(unsigned long oidTarget, unsigned long oidKiller)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidTarget);
    WriteOBJECTIDClient(oidKiller);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 7, pBuffer, nSize);
}
int CSWCMessage::SendPlayerToServerCheat_Invulnerability()  // PC 0x006CD0A0 GOG 0x006CCDA0
{
    return SendPlayerToServerMessage(15, 4, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_ToggleCombatDebugging(int bEnable)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bEnable);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 9, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_ToggleMovementSpeedDebugging(int bEnable)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bEnable);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 22, pBuffer, nSize);
}
// PC 0x006CD0F0 GOG 0x006CCDF0 Xbox 0x000514C0
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerGuiCharacterSheet_Status(char nPanel, unsigned long oidCreature)
{
    unsigned char* pBuffer;
    unsigned long nSize = 5;

    CreateWriteMessage(5, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR(nPanel, 8);
    WriteOBJECTIDClient(oidCreature);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(21, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_AlwaysRun(int bAlwaysRun)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bAlwaysRun);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 26, pBuffer, nSize);
}
// PC 0x006CD160 GOG 0x006CCE60 Xbox 0x00051530
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_UseFeat(unsigned short nFeat, unsigned short nAction,
                                                 unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 18;

    CreateWriteMessage(18, RECIPIENT_UNSPECIFIED, 1);
    WriteWORD(nFeat, 16);
    WriteWORD(nAction, 16);
    WriteOBJECTIDClient(oidTarget);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 6, pBuffer, nSize);
}
// PC 0x006CD1E0 GOG 0x006CCEE0 Xbox 0x000515B0
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
// PC 0x006CD2A0 GOG 0x006CCFA0 Xbox 0x00051670
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
// PC 0x006CD3E0 GOG 0x006CD0E0 Xbox 0x000517A0
int CSWCMessage::SendPlayerToServerInput_SwitchWeapons(unsigned long oidCreature,
                                                       int nWeaponSet)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidCreature);
    WriteOBJECTIDClient(nWeaponSet);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 38, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_Stance(char nStance, unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 5;

    CreateWriteMessage(5, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR(nStance, 8);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 39, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_ToggleMode(unsigned char nMode, unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 1;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nMode, 8);
    if (nMode == 5)
    {
        WriteOBJECTIDClient(oidTarget);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 10, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_Examine(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 5, pBuffer, nSize);
}
// PC 0x006CD450 GOG 0x006CD150 Xbox 0x00051810
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

// Dead code: not in the PC executable
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
// PC 0x006CD4B0 GOG 0x006CD1B0 Xbox 0x00051870
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
// PC 0x006CD520 GOG 0x006CD220 Xbox 0x000518E0
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_LockObject(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 14, pBuffer, nSize);
}
// PC 0x006CD590 GOG 0x006CD290 Xbox 0x00051950
int CSWCMessage::SendPlayerToServerInput_DoorSaber(unsigned long oidDoor)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidDoor);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 37, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_PlayAnimation(unsigned long oidObject,
                                                       const Vector& vPosition,
                                                       unsigned short nAnimation)
{
    unsigned char* pBuffer;
    unsigned long nSize = 18;

    CreateWriteMessage(18, RECIPIENT_UNSPECIFIED, 1);
    WriteWORD(nAnimation, 16);
    WriteOBJECTIDClient(oidObject);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 4, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_DriveControl(const Vector& vPosition,
                                                      unsigned short nDirection,
                                                      unsigned char nDriveMode,
                                                      unsigned char nFlags)
{
    unsigned char* pBuffer;
    unsigned long nSize = 11;

    CreateWriteMessage(11, RECIPIENT_UNSPECIFIED, 1);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteWORD(nDirection, 12);
    WriteBYTE(nDriveMode, 8);
    WriteBYTE(nFlags, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 29, pBuffer, nSize);
}
// PC 0x006CD5F0 GOG 0x006CD2F0 Xbox 0x000519B0
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_CancelGuiTimingEvent()
{
    return SendPlayerToServerMessage(6, 34, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_WalkToWayPoint(unsigned long oidArea, float fPositionX,
                                                        float fPositionY, float fPositionZ,
                                                        unsigned char nMovementRate, int bRun,
                                                        int bUsePath, unsigned char nClientMoving,
                                                        unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 18;

    CreateWriteMessage(18, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidArea);
    WriteFLOAT(fPositionX, 1.0f, 32);
    WriteFLOAT(fPositionY, 1.0f, 32);
    WriteFLOAT(fPositionZ, 1.0f, 32);
    WriteBYTE(nMovementRate, 8);
    WriteBOOL(bRun);
    WriteBOOL(bUsePath);
    WriteBYTE(nClientMoving, 8);
    WriteOBJECTIDClient(oidTarget);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_TurnOnSpot(const Vector& vPosition)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(6, 28, pBuffer, nSize);
}
// PC 0x006CD640 GOG 0x006CD340 Xbox 0x00051A00
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_Rest()
{
    return SendPlayerToServerMessage(6, 13, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInput_StopDragMode()
{
    return SendPlayerToServerMessage(6, 15, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInventory_Drop(unsigned long oidItem, float fPositionX,
                                                  float fPositionY, float fPositionZ)
{
    unsigned char* pBuffer;
    unsigned long nSize = 16;

    CreateWriteMessage(16, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidItem);
    WriteFLOAT(fPositionX, 1.0f, 32);
    WriteFLOAT(fPositionY, 1.0f, 32);
    WriteFLOAT(fPositionZ, 1.0f, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 3, pBuffer, nSize);
}
// PC 0x006CD6D0 GOG 0x006CD3D0 Xbox 0x00051A90
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInventory_EquipToggle(unsigned long oidObject,
                                                         unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (oidTarget == CGameObjectArray::INVALIDOBJID)
    {
        WriteBOOL(0);
    }
    else
    {
        WriteBOOL(1);
        WriteOBJECTIDClient(oidTarget);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 11, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInventory_Pickup(unsigned long oidObject,
                                                    unsigned long oidTarget)
{
    unsigned char* pBuffer;
    unsigned long nSize = 8;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    WriteOBJECTIDClient(oidTarget);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 5, pBuffer, nSize);
}
extern int g_bDemoMode;

// PC 0x006CD740 GOG 0x006CD440 Xbox 0x00051B70
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerLogin_LoginServerCharacter(const CResRef& cResRef, int nA,
                                                              int nB)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(16, RECIPIENT_UNSPECIFIED, 1);
    WriteCResRef(cResRef, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    if (nA != 0)
    {
        return SendPlayerToServerMessage(2, 14, pBuffer, nSize);
    }
    if (nB != 0)
    {
        return SendPlayerToServerMessage(2, 17, pBuffer, nSize);
    }
    return SendPlayerToServerMessage(2, 2, pBuffer, nSize);
}
// PC 0x006CD7D0 GOG 0x006CD4D0 Xbox 0x00051C00
int CSWCMessage::SendPlayerToServerLogin_LoginIFOCharacter()
{
    return SendPlayerToServerMessage(2, 15, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerLogin_CharacterResponse(int bSuccess)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bSuccess);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(2, 11, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServer_RequestServerCharacterList()
{
    return SendPlayerToServerMessage(17, 1, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServer_RequestCharacterListUpdate(unsigned char nResponse,
                                                               const CResRef& cResRef)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(17, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nResponse, 8);
    WriteCResRef(cResRef, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(17, 3, pBuffer, nSize);
}
// PC 0x006CD820 GOG 0x006CD520 Xbox 0x00051C50
int CSWCMessage::SendPlayerToServerLogin_LoginLocalCharacter(
    const CResRef& cResRef, int bNewCharacter)
{
    unsigned long nSize;
    unsigned char* pData;
    int nResult;
    CExoString sFileName;
    CExoFile* pFile;

    cResRef.CopyToString(sFileName);
    if (sFileName.CompareNoCase("test") != 0 || g_bDemoMode)
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
// PC 0x006CDA30 GOG 0x006CD730 Xbox 0x00051DF0
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

int CSWCMessage::SendPlayerToServerInventory_Unequip(unsigned long oidItem, unsigned long oidRepository)  // Xbox 0x00051B00
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerInventory_Use(unsigned long oidObject)
{
    unsigned char* pBuffer;
    unsigned long nSize = 4;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(12, 12, pBuffer, nSize);
}

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
#endif

// Dead code: not in the PC executable
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_PonyRide()
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(1);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 12, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_RainOfCows()
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(1);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 13, pBuffer, nSize);
}
#ifdef _XBOX

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
#endif

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_ToggleFloatyEffects()
{
    return SendPlayerToServerMessage(15, 15, NULL, 0);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_SummonNasty(unsigned long oidArea, unsigned char nCount,
                                                     const CResRef& cResRef)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(21, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidArea);
    WriteBYTE(nCount, 8);
    WriteCResRef(cResRef, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 14, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_CastSpell(unsigned long nSpellId, const Vector& vTarget,
                                                   unsigned long oidTarget)
{
    float fLength;
    unsigned char nFlags = 0;
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(20, RECIPIENT_UNSPECIFIED, 1);
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
    WriteDWORD(nSpellId, 32);
    WriteBYTE(nFlags, 8);
    if (oidTarget != OBJECT_INVALID)
    {
        WriteOBJECTIDClient(oidTarget);
    }
    if (fLength >= 0.00001f)
    {
        WriteFLOAT(vTarget.m_fX, 1.0f, 32);
        WriteFLOAT(vTarget.m_fY, 1.0f, 32);
        WriteFLOAT(vTarget.m_fZ, 1.0f, 32);
    }
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 3, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCheat_RunScript(const CExoString& sScript)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sScript.GetLength() + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sScript, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(15, 8, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerDialog_Reply(unsigned long oidObject, unsigned long nReplyIndex,
                                                unsigned char bEscape, unsigned long nEntryId)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(9, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDClient(oidObject);
    WriteDWORD(nReplyIndex, 32);
    WriteBYTE(bEscape, 8);
    WriteDWORD(nEntryId, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(20, 3, pBuffer, nSize);
}

// Dead code: not in the PC executable
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
#ifdef _XBOX
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
        int nYear = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeYear;
        int nMonth = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeMonth;
        int nDay = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeDay;
        int nHour = g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeHour;
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

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerUpdate_AppearancePhenoType(unsigned long oidObject,
                                                              unsigned char nPhenoType)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(2, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('T', 8);
    WriteOBJECTIDClient(oidObject);
    WriteBYTE(nPhenoType, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerUpdate_AppearancePLTColor(unsigned long oidObject,
                                                             unsigned char nLayer,
                                                             unsigned char nColor)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(3, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('L', 8);
    WriteOBJECTIDClient(oidObject);
    WriteBYTE(nLayer, 8);
    WriteBYTE(nColor, 8);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerQuickChat(unsigned short nQuickChat)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(6, RECIPIENT_UNSPECIFIED, 1);
    WriteWORD(nQuickChat, 16);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(22, 0, pBuffer, nSize);
}

// Dead code: not in the PC executable
int CSWCMessage::SendPlayerToServerCreature_Add(unsigned long oidObject, const Vector& vPosition,
                                                const CExoString& sString)
{
    unsigned char* pBuffer;
    unsigned long nSize;

    CreateWriteMessage(sString.GetLength() + 17, RECIPIENT_UNSPECIFIED, 1);
    WriteCHAR('C', 8);
    WriteOBJECTIDClient(oidObject);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    WriteCExoString(sString, 32);
    if (GetWriteMessage(&pBuffer, &nSize) == 0)
    {
        return 0;
    }
    return SendPlayerToServerMessage(5, 1, pBuffer, nSize);
}
