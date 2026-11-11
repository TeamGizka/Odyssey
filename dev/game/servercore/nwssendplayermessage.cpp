#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "gameobjectarray.h"
#include "netlayer.h"
#include "nwitem.h"
#include "nwrules.h"
#include "nwclass.h"
#include "serverexoapp.h"

template void CExoArrayList<long>::Allocate(int);
#include "swguiabilitieschargen.h"
#include "nwmessage.h"
#include "nwupdatestats.h"
#include "nwportrait.h"
#include "exotlktable.h"
#include "factionmanager.h"
#include "nwsparty.h"
#include "nwjournal.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "resgff.h"
#include "pathfindinfo.h"
#include "guiingame.h"
#include "serverinfo.h"
#include "nwscombatround.h"

class CFeatUseListEntry;
template unsigned short& CExoArrayList<unsigned short>::operator[](int);
template CFeatUseListEntry*& CExoArrayList<CFeatUseListEntry*>::operator[](int);
template int CExoArrayList<unsigned long>::Contains(unsigned long) const;

#include "nwcanimbase.h"

#include "serverexoappinternal.h"
#include "nwsarea.h"
#include "nwsdoor.h"
#include "nwsitem.h"
#include "nwsmodule.h"
#include "nwsplaceable.h"
#include "nwscreaturestats.h"
#include "nwsplayer.h"
#include "nwsplayerluo.h"
#include "nwsspellimpact.h"
#include "nwstrigger.h"
#include "nwfeat.h"
#include "nwspell.h"

static inline void NwssendplayermessageTrace1();
// PC 0x00566DA0  Xbox 0x00229DD0
void CSWSMessage::AddDoorAppearanceToMessage(CSWSDoor* pDoor)
{
    WriteBOOL(pDoor->m_bStatic);
    WriteBYTE(pDoor->m_nAppearanceType, 8);
    if (pDoor->m_nAppearanceType == 0)
    {
        WriteBYTE(pDoor->m_nGenericType, 8);
    }
}
// PC 0x00566DF0  Xbox 0x00229E20
void CSWSMessage::AddPlaceableAppearanceToMessage(CSWSPlaceable* pPlaceable)
{
    unsigned char nAppearance = pPlaceable->m_nAppearance;
    WriteBYTE(nAppearance, 8);
    int nAnimation = pPlaceable->m_nAnimation;
    WriteWORD(nAnimation, 16);
    int bPlot = pPlaceable->m_bPlotObject;
    WriteBOOL(bPlot);
    int bUseable = pPlaceable->m_bUseable;
    WriteBOOL(bUseable);
    int bTrapDisarmable = pPlaceable->m_bTrapDisarmable;
    WriteBOOL(bTrapDisarmable);
    int bLockable = pPlaceable->m_bLockable;
    WriteBOOL(bLockable);
    int bLocked = pPlaceable->m_bLocked;
    WriteBOOL(bLocked);
    int bUnknown = pPlaceable->m_n32C;
    WriteBOOL(bUnknown);

    CExoString sDescription;

    GetLocStringServer(-1, pPlaceable->m_lsDescription,
                       CExoLocString(), sDescription, 0);
    int bHasDescription = 0;
    if (!sDescription.IsEmpty())
    {
        bHasDescription = 1;
    }
    WriteBOOL(bHasDescription);
}
// PC 0x00566F20  Xbox 0x00229F50
void CSWSMessage::AddActiveItemPropertiesToMessage(CSWSItem* pItem,
                                                   CSWSCreature* pCreature)
{
    int bCanUseItem;
    int nActiveProperties;
    int i;
    CSWItemProperty* pActiveProperty;
    unsigned char* pUnknown;
    int j;

    if (pItem->GetBaseItem()->m_nItemType >= 31 &&
        pItem->GetBaseItem()->m_nItemType <= 38)
    {
        WriteSHORT(pItem->m_nBaseAC, 16);
    }
    WriteCExoLocStringServer(pItem->m_sLocalizedName, 0);
    WriteBOOL(1);
    WriteDWORD(pItem->GetCost(), 32);
    WriteDWORD(pItem->m_nStackSize, 32);
    if (pCreature != NULL)
    {
        bCanUseItem = pCreature->CanUseItem(pItem, 1);
    }
    else
    {
        bCanUseItem = 0;
    }
    WriteBOOL(bCanUseItem);
    WriteBOOL(pItem->m_bStolen != 0);
    nActiveProperties = pItem->m_nActiveProperties;
    WriteBYTE(nActiveProperties, 8);
    for (i = 0; i < nActiveProperties; i++)
    {
        pActiveProperty = pItem->GetActiveProperty(i);
        WriteWORD(pActiveProperty->m_nPropertyType, 16);
        pActiveProperty = pItem->GetActiveProperty(i);
        WriteWORD(pActiveProperty->m_nSubtype, 16);
        pActiveProperty = pItem->GetActiveProperty(i);
        WriteWORD(pActiveProperty->m_nCostValue, 16);
        pActiveProperty = pItem->GetActiveProperty(i);
        WriteBYTE(pActiveProperty->m_nParam1Value, 8);
    }
    WriteBYTE(pItem->m_nUsableActivePropertyMask, 8);
    WriteBYTE(0xFF, 8);
    pUnknown = pItem->m_aActivePropertyUsesLeft;
    for (j = 8; j != 0; j--)
    {
        WriteBYTE(*pUnknown++, 8);
    }
}
// PC 0x00570530  Xbox 0x00233800
void CSWSMessage::AssignVisualEffectLists(
    CExoArrayList<CLoopingVisualEffect*>* pDestination,
    CExoArrayList<CLoopingVisualEffect*>* pSource)
{
    for (int i = 0; i < pDestination->num; i++)
    {
        delete (*pDestination)[i];
        (*pDestination)[i] = NULL;
    }
    pDestination->SetSize(0);

    for (int i = 0; i < pSource->num; i++)
    {
        int bFound = 0;
        for (int j = 0; j < pDestination->num; j++)
        {
            if ((*pSource)[i]->m_nVisualEffectId ==
                (*pDestination)[j]->m_nVisualEffectId &&
                (!(*pSource)[i]->GetIsBeam() ||
                 ((*pSource)[i]->m_nVisualEffectId ==
                  (*pDestination)[j]->m_nVisualEffectId &&
                  (*pSource)[i]->m_oidCreator ==
                  (*pDestination)[j]->m_oidCreator &&
                  (*pSource)[i]->m_nBeamFlag ==
                  (*pDestination)[j]->m_nBeamFlag)))
            {
                bFound = 1;
            }
        }
        if (!bFound)
        {
            CLoopingVisualEffect* pEffect = new CLoopingVisualEffect;
            pEffect->m_nVisualEffectId = (*pSource)[i]->m_nVisualEffectId;
            pEffect->m_oidCreator = (*pSource)[i]->m_oidCreator;
            pEffect->m_nBeamFlag = (*pSource)[i]->m_nBeamFlag;
            pDestination->AddUnique(pEffect);
        }
    }
}
// PC 0x005706F0  Xbox 0x002339C0
void CSWSMessage::AssignCreatureLists(CExoArrayList<unsigned long>* pSource,
                                     CExoArrayList<unsigned long>* pDestination)
{
    pDestination->SetSize(0);
    for (int i = 0; i < pSource->num; i++)
    {
        pDestination->Add((*pSource)[i]);
    }
}
// PC 0x005707A0  Xbox 0x00233A70
void CSWSMessage::UpdateLastUpdateVisibilityList(
    CSWSCreature* pCreature, CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    CExoArrayList<CSWVisibilityNode*>* pLastVisibility =
        &pLastUpdateObject->m_lstVisibility;

    if (pLastVisibility->num > 0)
    {
        for (int i = pLastVisibility->num - 1; i >= 0; i--)
        {
            CSWVisibilityNode* pNode = (*pLastVisibility)[i];
            pLastVisibility->DelIndex(i);
            if (pNode != NULL)
            {
                delete pNode;
            }
        }
    }

    for (int j = 0; j < pCreature->m_lstVisibilityList.num; j++)
    {
        CSWVisibilityNode* pNode = pCreature->m_lstVisibilityList[j];
        CSWVisibilityNode* pNewNode = new CSWVisibilityNode;
        pNewNode->m_oidObject = pNode->m_oidObject;
        pNewNode->m_bHeard = pNode->m_bHeard;
        pNewNode->m_bSeen = pNode->m_bSeen;
        pNewNode->m_bInvisible = pNode->m_bInvisible;
        pLastVisibility->Add(pNewNode);
    }
}
// PC 0x00570930  Xbox 0x00233C00
int CSWSMessage::ComputeLastUpdate_GuiKnownSpells(CSWSCreature* pCreature,
    CSWSPlayerLastUpdateObject* pLastUpdate)
{
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    unsigned char nClass;
    unsigned char nLevel;
    for (nClass = 0; nClass < 2; nClass++)
    {
        unsigned char nCurrent[1];
        memset(nCurrent, 0, sizeof(nCurrent));
        for (nLevel = 0; nLevel < 1; nLevel++)
        {
            for (unsigned char nOld = 0;
                 nOld < (unsigned short)pLastUpdate->m_aKnownSpells[nClass].num;
                 nOld++)
            {
                unsigned long nSpell = pLastUpdate->GetKnownSpell(nClass, nLevel, nOld);
                if (nSpell == (unsigned long)pStats->GetKnownSpell(nClass, nLevel, nCurrent[nLevel]))
                    nCurrent[nLevel]++;
                else
                    pLastUpdate->m_aSpellsRemoved[nClass].Add(nSpell);
            }
        }
        for (nLevel = 0; nLevel < 1; nLevel++)
        {
            for (unsigned char nNew = nCurrent[nLevel];
                 nNew < pStats->GetNumberKnownSpells(nClass, nLevel); nNew++)
                pLastUpdate->m_aSpellsAdded[nClass].Add(
                    pStats->GetKnownSpell(nClass, nLevel, nNew));
        }
    }
    for (nClass = 0; nClass < 2; nClass++)
        if (pLastUpdate->m_aSpellsAdded[nClass].num != 0 ||
            pLastUpdate->m_aSpellsRemoved[nClass].num != 0)
            return 1;
    return 0;
}
// PC 0x00570BA0  Xbox 0x00233E70
void CSWSMessage::ComputeLastUpdate_StoreUpdateSpellLikeAbility(
    CSWSCreature* pCreature, CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    CExoArrayList<CSWSStats_SpellLikeAbility>* pLastAbilities =
        pLastUpdateObject->m_pExtraSpells;
    CExoArrayList<CSWSStats_SpellLikeAbility>* pAbilities =
        pStats->m_pSpellLikeAbilities;
    int nCount = pAbilities->num;
    if (pLastAbilities == NULL)
    {
        CExoArrayList<CSWSStats_SpellLikeAbility>* pNewAbilities =
            new CExoArrayList<CSWSStats_SpellLikeAbility>(0);
        pLastUpdateObject->m_pExtraSpells = pNewAbilities;
        pLastAbilities = pLastUpdateObject->m_pExtraSpells;
    }
    if (nCount != 0)
        pLastAbilities->SetSize(nCount);
    for (int nIndex = 0; nIndex < nCount; ++nIndex)
    {
        pLastAbilities->element[nIndex].m_nReady =
            pAbilities->element[nIndex].m_nReady;
        pLastAbilities->element[nIndex].m_nLevel =
            pAbilities->element[nIndex].m_nLevel;
        pLastAbilities->element[nIndex].m_nSpell =
            pAbilities->element[nIndex].m_nSpell;
    }
}
// PC 0x00570C60  Xbox 0x00233F30
int CSWSMessage::SendServerToPlayerUpdateCharResponse(
    CSWSPlayer* pPlayer, unsigned char nResponse, CResRef resFile)
{
    CSWSCreature* pCreature = NULL;
    CExoArrayList<CExoString> lstDirectories;
    CExoString sDirectory;
    CExoString sCharacter;
    CExoString sPlayerName;
    CResStruct cStruct;
    CResStruct cPlayerStruct;
    CResGFF* pGFF;

    switch (nResponse)
    {
    case UPDATE_CHARACTER_LOCAL:
        {
            pGFF = new CResGFF(RESTYPE_BIC, "BIC ", resFile);
            if (!pGFF->m_bLoaded)
            {
                delete pGFF;
                return 0;
            }
            pGFF->GetTopLevelStruct(&cStruct);
            pCreature = new CSWSCreature(OBJECT_INVALID, 1);
            CSWSCreatureStats* pLoadedStats = pCreature->m_pStats;
            int nReadResult = pLoadedStats->ReadStatsFromGff(
                    pGFF, &cStruct, &pCreature->m_cAppearanceInfo);
            if (nReadResult != 0)
            {
                delete pCreature;
                delete pGFF;
                return 0;
            }
            delete pGFF;
            break;
        }

    case UPDATE_CHARACTER_MODULE:
        {
            resFile.CopyToString(sCharacter);
            CResRef resModule("Module");
            pGFF = (CResGFF*)g_pExoResMan->GetResObject(resModule, RESTYPE_IFO);
            if (pGFF == NULL)
                return 0;
            pGFF->Demand();
            pGFF->GetTopLevelStruct(&cStruct);
            CResList lstPlayers;
            if (!pGFF->GetList(&lstPlayers, &cStruct, "Mod_PlayerList"))
            {
                pGFF->Release();
                g_pExoResMan->ReleaseResObject(pGFF);
                return 0;
            }
            if (!pGFF->GetListElement(&cPlayerStruct, &lstPlayers, sCharacter.AsINT()))
            {
                pGFF->Release();
                g_pExoResMan->ReleaseResObject(pGFF);
                return 0;
            }
            pCreature = new CSWSCreature(OBJECT_INVALID, 1);
            if (pCreature->m_pStats->ReadStatsFromGff(
                    pGFF, &cPlayerStruct, &pCreature->m_cAppearanceInfo))
            {
                delete pCreature;
                pGFF->Release();
                g_pExoResMan->ReleaseResObject(pGFF);
                return 0;
            }
            pGFF->Release();
            g_pExoResMan->ReleaseResObject(pGFF);
            break;
        }

    case UPDATE_CHARACTER_SERVER_VAULT:
        sPlayerName = pPlayer->GetPlayerName();
        g_pExoBase->GetDirectoryList(&lstDirectories, "SERVERVAULT:", RESTYPE_INVALID, 1, 0);
        if (!lstDirectories.Contains(sPlayerName))
            return 0;
        sDirectory.Format("SERVERVAULT:%s", sPlayerName.CStr());
        g_pExoResMan->AddResourceDirectory(sDirectory);
        pGFF = new CResGFF(RESTYPE_BIC, "BIC ", resFile);
        if (!pGFF->m_bLoaded)
        {
            delete pGFF;
            g_pExoResMan->RemoveResourceDirectory(sDirectory);
            return 0;
        }
        pGFF->GetTopLevelStruct(&cStruct);
        pCreature = new CSWSCreature(OBJECT_INVALID, 1);
        if (pCreature->m_pStats->ReadStatsFromGff(
                pGFF, &cStruct, &pCreature->m_cAppearanceInfo))
        {
            delete pCreature;
            delete pGFF;
            g_pExoResMan->RemoveResourceDirectory(sDirectory);
            return 0;
        }
        delete pGFF;
        g_pExoResMan->RemoveResourceDirectory(sDirectory);
        break;

    default:
        break;
    }

    if (pCreature == NULL)
        return 0;

    pGFF = new CResGFF;
    CResStruct* pStruct = new CResStruct;
    if (pGFF->CreateGFFFile(pStruct, "BIC ", "V2.0"))
    {
        CExoString sFile;
        CExoString sUnused;
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        pStats->SaveStats(pGFF, pStruct);
        sFile = "TEMP:temp_charfile";
        pGFF->WriteGFFFile(sFile, RESTYPE_BIC);
        g_pExoResMan->AddResourceDirectory("TEMP:");
        CExoFile* pFile = new CExoFile(sFile, RESTYPE_BIC, "rb");
        if (pFile->FileOpened())
        {
            int nFileSize = pFile->GetSize();
            unsigned char* pData = new unsigned char[nFileSize];
            pFile->Read(pData, 1, nFileSize);
            delete pFile;
            CreateWriteMessage(21, RECIPIENT_UNSPECIFIED, 1);
            WriteBYTE(nResponse, 8);
            WriteCResRef(resFile, 16);
            WriteDWORD(nFileSize, 32);
            WriteVOIDPtr(pData, nFileSize);
            g_pExoResMan->RemoveResourceDirectory("TEMP:");
            g_pExoResMan->RemoveFile(sFile, RESTYPE_BIC);
            delete[] pData;
        }
        else
        {
            delete pFile;
            delete pStruct;
            delete pGFF;
            return 0;
        }
    }
    else
    {
        delete pStruct;
        delete pGFF;
        return 0;
    }
    delete pStruct;
    delete pGFF;
    unsigned char* pMessage;
    unsigned long nMessageSize;
    if (GetWriteMessage(&pMessage, &nMessageSize))
        return SendServerToPlayerMessage(pPlayer->GetPlayerId(), MESSAGE_CHARACTER_LIST,
                                         UPDATE_CHARACTER_RESPONSE, pMessage, nMessageSize);
    return 0;
}
// PC 0x00571570  Xbox 0x00234850
int CSWSMessage::SendServerToPlayerChatMultiLangMessage(unsigned char nType,
    unsigned long oidSpeaker, CExoLocString& lsMessage,
    unsigned long oidTarget, unsigned char nGender,
    unsigned long* pnRecipients, unsigned long nRecipientCount,
    int bExcludeUnlisted, const CResRef& resSound,
    unsigned long oidSound, int nSoundVolume)
{
    Vector vSpeakerPosition;
    if (nType != CHAT_TYPE_MULTILANG)
    {
        return 0;
    }
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 0;
    }
    CGameObject* pObject;
    CSWSArea* pArea;
    int nResult;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidSpeaker, &pObject) == CGameObjectArray::SUCCESS)
    {
        CSWSObject* pSpeaker = (CSWSObject*)pObject;
        pArea = pSpeaker->GetArea();
        vSpeakerPosition = pSpeaker->m_vPosition;
        nResult = 1;
    }
    else
    {
        nResult = 0;
    }
    CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
    while (pPosition != NULL && nResult == 1)
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer->GetGameObject() != NULL)
        {
            int bListedRecipient = 0;
            int bExclude = bExcludeUnlisted;
            for (unsigned long i = 0; i < nRecipientCount; i++)
            {
                if (pnRecipients[i] == pPlayer->m_nPlayerId)
                {
                    bListedRecipient = 1;
                    bExclude = 0;
                    break;
                }
            }
            Vector vDifference = pPlayer->GetGameObject()->m_vPosition -
                vSpeakerPosition;
            if (!bExclude && pArea == pPlayer->GetGameObject()->GetArea() &&
                vDifference * vDifference <=
                CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE *
                CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE)
            {
                unsigned long nPlayerId = pPlayer->m_nPlayerId;
                nResult = SendServerToPlayerChatMultiLang_Helper(
                    nPlayerId, CHAT_TYPE_MULTILANG, oidSpeaker,
                    lsMessage, oidTarget, nGender, bListedRecipient,
                    resSound, oidSound, nSoundVolume);
            }
        }
        pPlayerList->GetNext(pPosition);
    }
    return nResult;
}
// PC 0x00571760  Xbox 0x00234A60
int CSWSMessage::SendServerToPlayerChatStrRefMessage(unsigned char nType,
    unsigned long oidSpeaker, unsigned long nStrRef)
{
    int nResult = 1;
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 0;
    }
    switch (nType)
    {
    case CHAT_STRREF_SHOUT:
        nResult &= SendServerToPlayerChat_StrRef(
            CNetLayer::PLAYERID_ALL_PLAYERS, oidSpeaker, nType, nStrRef);
        break;
    case CHAT_STRREF_TALK:
    case CHAT_STRREF_WHISPER:
        {
            CExoLinkedListNode* pPosition = NULL;
            Vector vSpeakerPosition;
            float fDistance;
            if (nType == CHAT_STRREF_TALK)
                fDistance = CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE;
            else
                fDistance = CSWSArea::MAX_OBJECT_CHATWHISPER_DISTANCE;
            CGameObject* pObject;
            CSWSArea* pArea;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidSpeaker, &pObject) == CGameObjectArray::SUCCESS &&
                pObject->AsSWSObject() != NULL)
            {
                pArea = pObject->AsSWSObject()->GetArea();
                vSpeakerPosition = pObject->AsSWSObject()->m_vPosition;
                pPosition = pPlayerList->GetHeadPos();
            }
            while (pPosition != NULL)
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
                if (pPlayer->GetGameObject() != NULL)
                {
                    Vector vDifference = pPlayer->GetGameObject()->m_vPosition -
                        vSpeakerPosition;
                    if (pArea == pPlayer->GetGameObject()->GetArea() &&
                        vDifference * vDifference <= fDistance * fDistance)
                    {
                        unsigned long nPlayerId = pPlayer->m_nPlayerId;
                        nResult &= SendServerToPlayerChat_StrRef(
                            nPlayerId, oidSpeaker, nType, nStrRef);
                    }
                }
                pPlayerList->GetNext(pPosition);
            }
        }
        break;
    default:
        nResult = 0;
        break;
    }
    return nResult;
}
// PC 0x00571960  Xbox 0x00234C80
int CSWSMessage::SendServerToPlayerChatMessage(unsigned char nType,
    unsigned long oidSpeaker, CExoString sMessage, unsigned long nPlayerId,
    const CExoString& sTargetName)
{
    CExoLinkedListNode* pPosition;
    CGameObject* pShoutObject;
    int nResult = 1;
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->GetServerExoApp()->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 0;
    }
    switch (nType)
    {
    case CHAT_SERVER_TELL:
        SendServerToPlayerChat_ServerTell(nPlayerId, sMessage);
        break;
    case CHAT_TELL:
        {
            CSWSPlayer* pSpeaker =
                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
                    oidSpeaker);
            if (pSpeaker != NULL && nPlayerId != pSpeaker->GetPlayerId())
            {
                nResult &= SendServerToPlayerChat_Tell(pSpeaker->GetPlayerId(),
                    oidSpeaker, sMessage);
            }
            nResult &= SendServerToPlayerChat_Tell(nPlayerId, oidSpeaker,
                sMessage);
        }
        break;
    case CHAT_SHOUT:
        {
            nResult &= SendServerToPlayerChat_Shout(
                CNetLayer::PLAYERID_ALL_PLAYERS, oidSpeaker, sMessage);
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidSpeaker, &pShoutObject) == CGameObjectArray::SUCCESS &&
                pShoutObject != NULL)
            {
                ((CSWSObject*)pShoutObject)->BroadcastDialog(sMessage,
                    CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE, 0);
            }
        }
        break;
    case CHAT_TALK:
        {
            Vector vSpeakerPosition;
            CGameObject* pObject;
            CSWSArea* pArea;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidSpeaker, &pObject) == CGameObjectArray::SUCCESS &&
                pObject != NULL)
            {
                pArea = ((CSWSObject*)pObject)->GetArea();
                vSpeakerPosition = ((CSWSObject*)pObject)->m_vPosition;
                ((CSWSObject*)pObject)->BroadcastDialog(sMessage,
                    CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE, 0);
            }
            else
            {
                nResult = 0;
            }
            pPosition = pPlayerList->GetHeadPos();
            while (pPosition != NULL && nResult == 1)
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
                if (pPlayer->GetGameObject() != NULL)
                {
                    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
                    Vector vDifference(
                        pPlayerObject->m_vPosition.m_fX - vSpeakerPosition.m_fX,
                        pPlayerObject->m_vPosition.m_fY - vSpeakerPosition.m_fY,
                        pPlayerObject->m_vPosition.m_fZ - vSpeakerPosition.m_fZ);
                    if (pArea == pPlayer->GetGameObject()->GetArea() &&
                        vDifference * vDifference <=
                        CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE *
                        CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE)
                    {
                        nResult &= SendServerToPlayerChat_Talk(
                            pPlayer->GetPlayerId(), oidSpeaker, sMessage);
                    }
                }
                pPlayerList->GetNext(pPosition);
            }
        }
        break;
    case CHAT_WHISPER:
        {
            Vector vSpeakerPosition;
            CGameObject* pObject;
            CSWSArea* pArea;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidSpeaker, &pObject) == CGameObjectArray::SUCCESS &&
                pObject->AsSWSCreature() != NULL)
            {
                pArea = pObject->AsSWSCreature()->GetArea();
                vSpeakerPosition = pObject->AsSWSCreature()->m_vPosition;
                ((CSWSObject*)pObject)->BroadcastDialog(sMessage,
                    CSWSArea::MAX_OBJECT_CHATWHISPER_DISTANCE, 0);
            }
            else
            {
                nResult = 0;
            }
            pPosition = pPlayerList->GetHeadPos();
            while (pPosition != NULL && nResult == 1)
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
                if (pPlayer->GetGameObject() != NULL)
                {
                    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
                    Vector vDifference(
                        pPlayerObject->m_vPosition.m_fX - vSpeakerPosition.m_fX,
                        pPlayerObject->m_vPosition.m_fY - vSpeakerPosition.m_fY,
                        pPlayerObject->m_vPosition.m_fZ - vSpeakerPosition.m_fZ);
                    if (pArea == pPlayer->GetGameObject()->GetArea() &&
                        vDifference * vDifference <=
                        CSWSArea::MAX_OBJECT_CHATWHISPER_DISTANCE *
                        CSWSArea::MAX_OBJECT_CHATWHISPER_DISTANCE)
                    {
                        nResult &= SendServerToPlayerChat_Whisper(
                            pPlayer->GetPlayerId(), oidSpeaker, sMessage);
                    }
                }
                pPlayerList->GetNext(pPosition);
            }
        }
        break;
    case CHAT_DIALOG:
        {
            CGameObject* pObject;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidSpeaker, &pObject) == CGameObjectArray::SUCCESS &&
                pObject != NULL)
            {
                ((CSWSObject*)pObject)->BroadcastDialog(sMessage,
                    CSWSArea::MAX_OBJECT_CHATTALKSILENT_DISTANCE, 1);
            }
        }
        break;
    case CHAT_DIALOG_SHOUT:
        {
            CGameObject* pObject;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidSpeaker, &pObject) == CGameObjectArray::SUCCESS &&
                pObject != NULL)
            {
                ((CSWSObject*)pObject)->BroadcastDialog(sMessage,
                    CSWSArea::MAX_OBJECT_CHATSHOUTSILENT_DISTANCE, 0);
            }
        }
        break;
    case CHAT_PARTY:
        {
            CSWSFaction* pFaction = g_pAppManager->GetServerExoApp()->
                GetClientObjectByObjectId(oidSpeaker)->GetGameObject()->
                AsSWSCreature()->GetFaction();
            if (pFaction != NULL)
            {
                pFaction->SendChatMessage(oidSpeaker, sMessage);
            }
        }
        break;
    default:
        nResult = 0;
        break;
    }
    return nResult;
}

void CSWSMessage::WriteGameObjUpdate_WriteInventorySlotDelete(
    CSWSCreature* pCreature, unsigned long nSlot)
{
    WriteCHAR(GAME_OBJECT_RECORD_DELETE, 8);
    WriteOBJECTIDServer(CGameObjectArray::INVALIDOBJID);
    WriteDWORD(nSlot, 32);
    if (nSlot == INVENTORY_SLOT_RIGHTHAND || nSlot == INVENTORY_SLOT_LEFTHAND)
    {
        WriteBOOL(pCreature->GetLastInstantEquip(nSlot));
    }
}
// PC 0x00571EC0  Xbox 0x00235220
int CSWSMessage::SendServerToPlayerQuickChatMessage(unsigned long oidSpeaker,
    unsigned short nQuickChat)
{
    int nResult = 1;
    CSWSArea* pArea = NULL;
    Vector vSpeakerPosition;
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->GetServerExoApp()->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 0;
    }
    CGameObject* pObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidSpeaker, &pObject) == CGameObjectArray::SUCCESS)
    {
        if (pObject->AsSWSCreature() != NULL)
        {
            pArea = pObject->AsSWSCreature()->GetArea();
            vSpeakerPosition = pObject->AsSWSCreature()->m_vPosition;
        }
        else
        {
            nResult = 0;
        }
    }
    else
    {
        nResult = 0;
    }
    CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
    while (pPosition != NULL && nResult == 1)
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer->GetGameObject() != NULL)
        {
            Vector vDifference = pPlayer->GetGameObject()->m_vPosition -
                vSpeakerPosition;
            if (pArea == pPlayer->GetGameObject()->GetArea() &&
                vDifference * vDifference <=
                CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE *
                CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE)
            {
                nResult &= SendServerToPlayerQuickChat(pPlayer->GetPlayerId(),
                    oidSpeaker, nQuickChat);
            }
        }
        pPlayerList->GetNext(pPosition);
    }
    return nResult;
}
// PC 0x00572070  Xbox 0x002353F0
void CSWSMessage::WriteGameObjUpdate_UpdateAppearance(
    CSWSObject* pObject, CLastUpdateObject* pLastUpdateObject,
    unsigned long nAppearanceMask)
{
    unsigned long oidObject = pObject->m_nId;
    if (pObject->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
    {
        return;
    }
    CSWSCreature* pCreature = (CSWSCreature*)pObject;
    WriteCHAR(GAME_OBJECT_RECORD_APPEARANCE, 8);
    WriteBYTE(CGameObject::OBJECT_TYPE_CREATURE, 8);
    WriteOBJECTIDServer(oidObject);
    WriteWORD(nAppearanceMask, 16);
    if (nAppearanceMask & APPEARANCE_UPDATE_NAME)
    {
        WriteCExoLocStringServer(pCreature->GetStats()->m_lsFirstName, 0);
        WriteCExoLocStringServer(pCreature->GetStats()->m_lsLastName, 0);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_TYPE)
    {
        WriteWORD(pCreature->m_cAppearanceInfo.m_nAppearance, 16);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART0)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart0, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART1)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart1, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART6)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart6, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_STATE)
    {
        WriteBOOL(pCreature->m_cAppearanceInfo.m_bUseBackupHead);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_VARIANT)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearanceHead, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART7)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart7, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART8)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart8, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART2)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart2, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART3)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart3, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART4)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart4, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_PART5)
    {
        WriteBYTE(pCreature->m_cAppearanceInfo.m_nAppearancePart5, 8);
    }
    if (nAppearanceMask & APPEARANCE_UPDATE_FLAGS)
    {
        unsigned short nSoundSetFile = pCreature->m_nSoundSetFile;
        WriteWORD(nSoundSetFile, 16);
    }

    unsigned char nSlotUpdates = 0;
    if (nAppearanceMask == APPEARANCE_UPDATE_ALL)
    {
        nSlotUpdates = 4;
        if (pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_ARMOUR) != NULL)
            nSlotUpdates++;
        if (pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_HEAD) != NULL)
            nSlotUpdates++;
        if (pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND) != NULL)
            nSlotUpdates++;
        if (pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL)
            nSlotUpdates++;
        WriteBYTE(nSlotUpdates, 8);
        WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_ARMOUR);
        WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_HEAD);
        WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_LEFTHAND);
        WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_RIGHTHAND);

        CSWSItem* pItem;
        pItem = pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
        if (pItem != NULL)
            WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem, INVENTORY_SLOT_ARMOUR);
        pItem = pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_HEAD);
        if (pItem != NULL)
            WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem, INVENTORY_SLOT_HEAD);
        pItem = pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        if (pItem != NULL)
            WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem, INVENTORY_SLOT_LEFTHAND);
        pItem = pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pItem != NULL)
            WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem, INVENTORY_SLOT_RIGHTHAND);
        pCreature->SetLastInstantEquip(INVENTORY_SLOT_RIGHTHAND, FALSE);
        pCreature->SetLastInstantEquip(INVENTORY_SLOT_LEFTHAND, FALSE);
    }
    else if (nAppearanceMask & APPEARANCE_UPDATE_EQUIPMENT)
    {
        CExoArrayList<unsigned long> lstSlotsToAdd;
        if (pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand != pLastUpdateObject->m_oidAppearanceLeftHand)
        {
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand == OBJECT_INVALID)
                nSlotUpdates++;
            else
                nSlotUpdates += 2;
        }
        if (pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand != pLastUpdateObject->m_oidAppearanceRightHand)
        {
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand == OBJECT_INVALID)
                nSlotUpdates++;
            else
                nSlotUpdates += 2;
        }
        if (pCreature->m_cAppearanceInfo.m_nAppearance != pLastUpdateObject->m_nAppearance ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart1 != pLastUpdateObject->m_nAppearancePart1 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart0 != pLastUpdateObject->m_nAppearancePart0 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart2 != pLastUpdateObject->m_nAppearancePart2 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart3 != pLastUpdateObject->m_nAppearancePart3 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart4 != pLastUpdateObject->m_nAppearancePart4 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart5 != pLastUpdateObject->m_nAppearancePart5)
        {
            nSlotUpdates += 2;
            if (pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_HEAD) != NULL)
                nSlotUpdates++;
            if (pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_ARMOUR) != NULL)
                nSlotUpdates++;
        }
        else
        {
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceArmour != pLastUpdateObject->m_oidAppearanceArmour)
            {
                if (pCreature->m_cAppearanceInfo.m_oidAppearanceArmour == OBJECT_INVALID)
                    nSlotUpdates++;
                else
                    nSlotUpdates += 2;
            }
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceHead != pLastUpdateObject->m_oidAppearanceHead)
            {
                if (pCreature->m_cAppearanceInfo.m_oidAppearanceHead == OBJECT_INVALID)
                    nSlotUpdates++;
                else
                    nSlotUpdates += 2;
            }
        }
        int bResetRightHand = FALSE;
        int bResetLeftHand = FALSE;
        WriteBYTE(nSlotUpdates, 8);
        if (pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand != pLastUpdateObject->m_oidAppearanceLeftHand)
        {
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand == OBJECT_INVALID)
            {
                WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_LEFTHAND);
            }
            else
            {
                WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_LEFTHAND);
                lstSlotsToAdd.Add(INVENTORY_SLOT_LEFTHAND);
            }
            bResetLeftHand = TRUE;
        }
        if (pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand != pLastUpdateObject->m_oidAppearanceRightHand)
        {
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand == OBJECT_INVALID)
            {
                WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_RIGHTHAND);
            }
            else
            {
                WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_RIGHTHAND);
                lstSlotsToAdd.Add(INVENTORY_SLOT_RIGHTHAND);
            }
            bResetRightHand = TRUE;
        }
        if (pCreature->m_cAppearanceInfo.m_nAppearance != pLastUpdateObject->m_nAppearance ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart1 != pLastUpdateObject->m_nAppearancePart1 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart0 != pLastUpdateObject->m_nAppearancePart0 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart2 != pLastUpdateObject->m_nAppearancePart2 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart3 != pLastUpdateObject->m_nAppearancePart3 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart4 != pLastUpdateObject->m_nAppearancePart4 ||
            pCreature->m_cAppearanceInfo.m_nAppearancePart5 != pLastUpdateObject->m_nAppearancePart5)
        {
            WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_ARMOUR);
            WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_HEAD);
            CSWSItem* pItem = pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_HEAD);
            if (pItem != NULL)
                WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem, INVENTORY_SLOT_HEAD);
            pItem = pCreature->GetInventory()->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
            if (pItem != NULL)
                WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem, INVENTORY_SLOT_ARMOUR);
        }
        else
        {
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceArmour != pLastUpdateObject->m_oidAppearanceArmour)
            {
                if (pCreature->m_cAppearanceInfo.m_oidAppearanceArmour == OBJECT_INVALID)
                {
                    WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_ARMOUR);
                }
                else
                {
                    WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_ARMOUR);
                    lstSlotsToAdd.Add(INVENTORY_SLOT_ARMOUR);
                }
            }
            if (pCreature->m_cAppearanceInfo.m_oidAppearanceHead != pLastUpdateObject->m_oidAppearanceHead)
            {
                if (pCreature->m_cAppearanceInfo.m_oidAppearanceHead == OBJECT_INVALID)
                {
                    WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_HEAD);
                }
                else
                {
                    WriteGameObjUpdate_WriteInventorySlotDelete(pCreature, INVENTORY_SLOT_HEAD);
                    lstSlotsToAdd.Add(INVENTORY_SLOT_HEAD);
                }
            }
        }
        for (int nSlot = 0; nSlot < lstSlotsToAdd.num; nSlot++)
        {
            CSWSItem* pItem = pCreature->GetInventory()->GetItemInSlot(lstSlotsToAdd[nSlot]);
            WriteGameObjUpdate_WriteInventorySlotAdd(pCreature, pItem,
                lstSlotsToAdd[nSlot]);
        }
        if (bResetRightHand)
            pCreature->SetLastInstantEquip(INVENTORY_SLOT_RIGHTHAND, FALSE);
        if (bResetLeftHand)
            pCreature->SetLastInstantEquip(INVENTORY_SLOT_LEFTHAND, FALSE);
    }
}
// PC 0x00572990  Xbox 0x00235D20
void CSWSMessage::WriteGameObjUpdate_MinorGUIPanels(CSWSPlayer* pPlayer)
{

    CExoArrayList<unsigned long> lstObjectIds;
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        unsigned long nUpdateRequired =
            pPlayer->m_pCharSheetGUI->ComputeCharacterSheetUpdateRequired(pPlayer);
        if (nUpdateRequired != 0)
        {
            WriteGameObjUpdate_CharacterSheet(pPlayer, nUpdateRequired);
        }
    }
}
// PC 0x00572A20  Xbox 0x00235DB0
void CSWSMessage::TestPlayerUpdateDifferences(CSWSPlayer* pPlayer,
    CSWSPlayerLastUpdateObject* pPlayerLastUpdate,
    CLastUpdateObject* pLastUpdateObject, unsigned long& nUpdateMask)
{
    CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
    if (pCreature != NULL)
    {
        if (pPlayerLastUpdate->m_nGuiSkills != ComputeLastUpdate_GuiSkills(pCreature))
            nUpdateMask |= PLAYER_UPDATE_GUI_SKILLS;
        if (pPlayerLastUpdate->m_nPlayerState != ComputeLastUpdate_PlayerState(pCreature))
            nUpdateMask |= PLAYER_UPDATE_STATE;
        if (ComputeLastUpdate_GuiKnownSpells(pCreature, pPlayerLastUpdate))
            nUpdateMask |= PLAYER_UPDATE_KNOWN_SPELLS;
        if (ComputeLastUpdate_GuiKnownSpellUses(pCreature, pPlayerLastUpdate))
            nUpdateMask |= PLAYER_UPDATE_KNOWN_SPELL_USES;
        if (ComputeLastUpdate_ActionQueue(pCreature, pPlayerLastUpdate))
            nUpdateMask |= PLAYER_UPDATE_ACTION_QUEUE;
        if (ComputeVisibilityLists(pCreature, pPlayerLastUpdate))
            nUpdateMask |= PLAYER_UPDATE_VISIBILITY;
        if (ComputeLastUpdate_GuiFeats(pCreature, pPlayerLastUpdate))
            nUpdateMask |= PLAYER_UPDATE_GUI_FEATS;

        if (pCreature->GetGold() != pPlayerLastUpdate->m_nGold ||
            pPlayerLastUpdate->m_nArmorClass != pCreature->GetArmorClass() ||
            pPlayerLastUpdate->m_nEncumbranceState != pCreature->m_nEncumbranceState ||
            pCreature->m_bComputeWeightCarried == 1 ||
            pPlayerLastUpdate->m_nExperience != pCreature->m_pStats->m_nExperience ||
            pPlayerLastUpdate->m_bCanLevelUp != pCreature->m_pStats->CanLevelUp() ||
            pPlayerLastUpdate->m_nLevel != pCreature->m_pStats->GetLevel(0))
            nUpdateMask |= PLAYER_UPDATE_CHARACTER;

        if (pLastUpdateObject != NULL && pCreature->m_pStats != NULL)
        {
            if (pCreature->m_pStats->m_nStrengthBase != pLastUpdateObject->m_nStrengthBase ||
                pCreature->m_pStats->m_nDexterityBase != pLastUpdateObject->m_nDexterityBase ||
                pCreature->m_pStats->m_nConstitutionBase != pLastUpdateObject->m_nConstitutionBase ||
                pCreature->m_pStats->m_nIntelligenceBase != pLastUpdateObject->m_nIntelligenceBase ||
                pCreature->m_pStats->m_nWisdomBase != pLastUpdateObject->m_nWisdomBase ||
                pCreature->m_pStats->m_nCharismaBase != pLastUpdateObject->m_nCharismaBase)
                nUpdateMask |= PLAYER_UPDATE_ABILITIES;

            if (pCreature->m_pStats->GetSTRStat() != pLastUpdateObject->m_nStrength ||
                pCreature->m_pStats->GetDEXStat() != pLastUpdateObject->m_nDexterity ||
                pCreature->m_pStats->GetCONStat() != pLastUpdateObject->m_nConstitution ||
                pCreature->m_pStats->GetINTStat() != pLastUpdateObject->m_nIntelligence ||
                pCreature->m_pStats->GetWISStat() != pLastUpdateObject->m_nWisdom ||
                pCreature->m_pStats->GetCHAStat() != pLastUpdateObject->m_nCharisma)
                nUpdateMask |= PLAYER_UPDATE_ABILITIES;

            for (int nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
            {
                if ((unsigned char)pCreature->m_pStats->GetSkillRank(nSkill, NULL, 1) !=
                    pLastUpdateObject->m_nSkillBase[nSkill])
                    nUpdateMask |= PLAYER_UPDATE_SKILLS;
                if ((unsigned char)pCreature->m_pStats->GetSkillRank(nSkill, NULL, 0) !=
                    pLastUpdateObject->m_nSkill[nSkill])
                    nUpdateMask |= PLAYER_UPDATE_SKILLS;
            }

            if (pCreature->GetPortrait() != pLastUpdateObject->m_resRef ||
                pCreature->GetPortraitId() != pLastUpdateObject->m_nPortraitId)
                nUpdateMask |= PLAYER_UPDATE_PORTRAIT;
            if (pLastUpdateObject->m_nBaseMaxHitPoints != (short)pCreature->m_nMaxHitPoints ||
                pLastUpdateObject->m_nCurrentHitPoints != pCreature->GetCurrentHitPoints(0) ||
                pLastUpdateObject->m_nTemporaryHitPoints != (short)pCreature->m_nTemporaryHitPoints ||
                pLastUpdateObject->m_nMaxHitPoints != pCreature->GetMaxHitPoints(1))
                nUpdateMask |= PLAYER_UPDATE_HIT_POINTS;
            if (pLastUpdateObject->m_nForcePoints != pCreature->m_pStats->m_nForcePoints ||
                pLastUpdateObject->m_nCurrentForcePoints !=
                    pCreature->m_pStats->GetCurrentForcePoints() ||
                pLastUpdateObject->m_nTemporaryForcePoints != pCreature->m_pStats->m_nTemporaryForcePoints ||
                pLastUpdateObject->m_nMaxForcePoints != pCreature->GetMaxForcePoints())
                nUpdateMask |= PLAYER_UPDATE_FORCE_POINTS;
        }
        if (ComputeLastUpdate_CompareSpellLikeAbility(pCreature, pPlayerLastUpdate))
            nUpdateMask |= PLAYER_UPDATE_SPELL_LIKE_ABILITIES;
    }
}

void CSWSMessage::AddAreaOfEffectObjectToMessage(CSWSAreaOfEffectObject* pObject)
{
    WriteINT(pObject->m_nAreaEffectId, 32);
    WriteOBJECTIDServer(pObject->m_oidTarget);
}
// PC 0x00572DC0  Xbox 0x00236150
void CSWSMessage::WriteGameObjUpdate_PlayerUpdate(CSWSPlayer* pPlayer,
    CSWSPlayerLastUpdateObject* pPlayerLastUpdate,
    CLastUpdateObject* pLastUpdateObject, unsigned long nUpdateMask)
{
    CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
    if (pCreature != NULL)
    {
        WriteCHAR(GAME_OBJECT_RECORD_PLAYER_INFO, 8);
        WriteOBJECTIDServer(pCreature->GetId());
        WriteDWORD(nUpdateMask, 32);
        if (nUpdateMask & PLAYER_UPDATE_CHARACTER)
        {
            WriteSHORT(pCreature->GetArmorClass(), 16);
            WriteDWORD(pCreature->GetGold(), 32);
            WriteINT(pCreature->GetEncumbranceState(), 32);
            WriteBOOL(pCreature->GetStats()->CanLevelUp());
            WriteDWORD(pCreature->GetStats()->GetExperience(), 32);
            if (pCreature->GetStats()->CanLevelUp() == 1)
            {
                unsigned short nSkillPoints = pCreature->GetStats()->m_nSkillPoints;
                WriteWORD(nSkillPoints, 16);
                for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
                    WriteCHAR(pCreature->GetStats()->GetSkillRank(nSkill, NULL, 1), 8);
                unsigned char nStartingPackage = pCreature->GetStats()->m_nStartingPackage;
                WriteBYTE(nStartingPackage, 8);
                WriteBYTE(pCreature->GetStats()->GetNumClasses() - 1, 8);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_GUI_SKILLS)
            WriteWORD(ComputeLastUpdate_GuiSkills(pCreature), 16);
        if (nUpdateMask & PLAYER_UPDATE_STATE)
        {
            unsigned short nPlayerState = ComputeLastUpdate_PlayerState(pCreature);
            WriteWORD(nPlayerState, 16);
        }
        if (nUpdateMask & PLAYER_UPDATE_RESERVED_CLASS_SPELL_MASKS)
        {
            unsigned short anSpellMasks[CSWSCreatureStats::MAX_CLASSES];
            unsigned char abClassChanged[CSWSCreatureStats::MAX_CLASSES];
            for (int i = 0; i < CSWSCreatureStats::MAX_CLASSES; i++)
            {
                abClassChanged[i] = 0;
                anSpellMasks[i] = 0;
            }
            unsigned char nChanged = 0;
            for (int i = 0; i < pCreature->GetStats()->GetNumClasses(); i++)
            {
                if (anSpellMasks[i] != 0)
                {
                    abClassChanged[i] = 1;
                    nChanged++;
                }
            }
            WriteBYTE(nChanged, 8);
            for (int i = 0; i < pCreature->GetStats()->GetNumClasses(); i++)
            {
                if (abClassChanged[i])
                {
                    WriteBYTE(i, 8);
                    WriteWORD(anSpellMasks[i], 16);
                    for (int nLevel = 1; nLevel < CSWSCreatureStats_ClassInfo::NUM_SPELL_LEVELS; nLevel++)
                    {
                        if (anSpellMasks[i] & (1 << nLevel))
                            WriteBYTE(pCreature->GetStats()->GetSpellsPerDayLeft(i, nLevel), 8);
                    }
                }
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_KNOWN_SPELLS)
        {
            for (int nClass = 0; nClass < CSWSCreatureStats::MAX_CLASSES; nClass++)
            {
                int nSpellsRemoved = pPlayerLastUpdate->m_aSpellsRemoved[nClass].num;
                WriteWORD(nSpellsRemoved, 16);
                for (unsigned char i = 0; i < pPlayerLastUpdate->m_aSpellsRemoved[nClass].num; i++)
                    WriteDWORD(pPlayerLastUpdate->m_aSpellsRemoved[nClass][i], 32);
                int nSpellsAdded = pPlayerLastUpdate->m_aSpellsAdded[nClass].num;
                WriteWORD(nSpellsAdded, 16);
                for (unsigned char i = 0; i < pPlayerLastUpdate->m_aSpellsAdded[nClass].num; i++)
                    WriteDWORD(pPlayerLastUpdate->m_aSpellsAdded[nClass][i], 32);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_RESERVED_SPELL_LISTS)
        {
            for (int nClass = 0; nClass < CSWSCreatureStats::MAX_CLASSES; nClass++)
            {
                WriteWORD(0, 16);
                WriteWORD(0, 16);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_KNOWN_SPELL_USES)
        {
            unsigned short anSpellMasks[CSWSCreatureStats::MAX_CLASSES];
            unsigned char abClassChanged[CSWSCreatureStats::MAX_CLASSES];
            for (int i = 0; i < CSWSCreatureStats::MAX_CLASSES; i++)
            {
                abClassChanged[i] = 0;
                anSpellMasks[i] = 0;
            }
            unsigned char nChanged = 0;
            for (int i = 0; i < pCreature->GetStats()->GetNumClasses(); i++)
            {
                if (CSWClass::IsJedi(pCreature->GetStats()->GetClass(i)))
                {
                    unsigned char nKnownUses = pPlayerLastUpdate->GetKnownSpellUsesLeft(i, 0);
                    if (nKnownUses != pCreature->GetStats()->GetSpellsPerDayLeft(i, 0))
                        anSpellMasks[i] |= PLAYER_SPELL_USES_LEVEL_ZERO;
                    if (anSpellMasks[i] != 0)
                    {
                        abClassChanged[i] = 1;
                        nChanged++;
                    }
                }
            }
            WriteBYTE(nChanged, 8);
            for (int i = 0; i < pCreature->GetStats()->GetNumClasses(); i++)
            {
                if (abClassChanged[i])
                {
                    WriteBYTE(i, 8);
                    unsigned short nSpellMask = anSpellMasks[i];
                    WriteWORD(nSpellMask, 16);
                    if (nSpellMask & PLAYER_SPELL_USES_LEVEL_ZERO)
                        WriteBYTE(pCreature->GetStats()->GetSpellsPerDayLeft(i, 0), 8);
                }
            }
            for (int i = 0; i < pCreature->GetStats()->GetNumClasses(); i++)
            {
                if (CSWClass::IsJedi(pCreature->GetStats()->GetClass(i)))
                {
                    unsigned char nUsesLeft = pCreature->GetStats()->GetSpellsPerDayLeft(i, 0);
                    pPlayerLastUpdate->SetKnownSpellUsesLeft(i, 0, nUsesLeft);
                }
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_GUI_FEATS)
        {
            CExoArrayList<unsigned short> lstFeats;
            CExoArrayList<unsigned char> lstChanges;
            int i;
            unsigned short nFeat;
            for (i = 0; (unsigned short)i < (unsigned short)pCreature->GetStats()->GetNumFeats(); i++)
            {
                nFeat = pCreature->GetStats()->GetFeat(i);
                int bFound = 0;
                int j = 0;
                if (pPlayerLastUpdate->m_lstFeats.num > 0)
                {
                    do
                    {
                        if (pPlayerLastUpdate->m_lstFeats[j] == nFeat)
                            bFound = 1;
                        j++;
                    } while (j < pPlayerLastUpdate->m_lstFeats.num);
                }
                if (!bFound)
                {
                    lstChanges.Add(PLAYER_FEAT_ADD);
                    lstFeats.Add(nFeat);
                }
            }
            for (i = 0; i < pPlayerLastUpdate->m_lstFeats.num; i++)
            {
                nFeat = pPlayerLastUpdate->m_lstFeats[i];
                int bFound = 0;
                for (unsigned short j = 0; j < (unsigned short)pCreature->GetStats()->GetNumFeats(); j++)
                    if (pCreature->GetStats()->GetFeat(j) == nFeat)
                        bFound = 1;
                if (!bFound)
                {
                    lstChanges.Add(PLAYER_FEAT_DELETE);
                    lstFeats.Add(nFeat);
                }
            }
            for (i = 0; (unsigned short)i < (unsigned short)pCreature->GetStats()->m_lstBonusFeats.num; i++)
            {
                nFeat = pCreature->GetStats()->GetBonusFeat(i);
                int bFound = 0;
                int j = 0;
                if (pPlayerLastUpdate->m_lstBonusFeats.num > 0)
                {
                    do
                    {
                        if (pPlayerLastUpdate->m_lstBonusFeats[j] == nFeat)
                            bFound = 1;
                        j++;
                    } while (j < pPlayerLastUpdate->m_lstBonusFeats.num);
                }
                if (!bFound)
                {
                    lstChanges.Add(PLAYER_BONUS_FEAT_ADD);
                    lstFeats.Add(nFeat);
                }
            }
            for (i = 0; i < pPlayerLastUpdate->m_lstBonusFeats.num; i++)
            {
                nFeat = pPlayerLastUpdate->m_lstBonusFeats[i];
                int bFound = 0;
                for (unsigned short j = 0; j < (unsigned short)pCreature->GetStats()->m_lstBonusFeats.num; j++)
                    if (pCreature->GetStats()->GetBonusFeat(j) == nFeat)
                        bFound = 1;
                if (!bFound)
                {
                    lstChanges.Add(PLAYER_BONUS_FEAT_DELETE);
                    lstFeats.Add(nFeat);
                }
            }
            WriteWORD(lstChanges.num, 16);
            for (i = 0; i < lstChanges.num; i++)
            {
                WriteCHAR(lstChanges[i], 8);
                WriteWORD(lstFeats[i], 16);
            }
            CExoArrayList<CFeatUseListEntry*>* pFeatUses = &pCreature->GetStats()->m_lstFeatUses;
            int nFeatUses = pFeatUses->num;
            WriteWORD(nFeatUses, 16);
            for (i = 0; i < pFeatUses->num; i++)
            {
                WriteWORD((*pFeatUses)[i]->m_nFeat, 16);
                WriteBYTE(pCreature->GetStats()->GetFeatRemainingUses((*pFeatUses)[i]->m_nFeat), 8);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_ACTION_QUEUE)
        {
            CExoArrayList<unsigned char> lstChanges;
            CExoArrayList<unsigned char> lstActions;
            int i;
            int j;
            CSWActionNode* pNode = pCreature->m_aActionQueue;
            for (i = 0; i < 10; i++)
            {
                CSWActionNode* pAction = &pPlayerLastUpdate->m_aActions[i];
                if (pAction->m_nActionId != 0xFFFF)
                {
                    int bFound = 0;
                    for (j = 0; j < 10; j++)
                    {
                        if (pNode[j].m_nGroupActionId == pAction->m_nGroupActionId &&
                            pNode[j].m_nActionId == pAction->m_nActionId &&
                            pNode[j].m_nActionType == pAction->m_nActionType)
                        {
                            bFound = 1;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        lstChanges.Add(PLAYER_ACTION_DELETE);
                        lstActions.Add(i);
                    }
                }
            }
            for (i = 0; i < 10; i++)
            {
                CSWActionNode* pAction = &pCreature->m_aActionQueue[i];
                if (pAction->m_nActionId != 0xFFFF)
                {
                    int bFound = 0;
                    for (j = 0; j < 10; j++)
                    {
                        if (pAction->m_nGroupActionId == pPlayerLastUpdate->m_aActions[j].m_nGroupActionId &&
                            pAction->m_nActionId == pPlayerLastUpdate->m_aActions[j].m_nActionId &&
                            pAction->m_nActionType == pPlayerLastUpdate->m_aActions[j].m_nActionType)
                        {
                            bFound = 1;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        if (pAction->m_nGroupActionId == ::ACTION_INVALID &&
                            pAction->m_nActionType != 0xFFFF)
                        {
                            lstChanges.Add(PLAYER_ACTION_SPELL);
                            lstActions.Add(i);
                        }
                        else
                        {
                            lstChanges.Add(PLAYER_ACTION_ADD);
                            lstActions.Add(i);
                        }
                    }
                }
            }
            WriteBYTE(lstChanges.num, 8);
            for (i = 0; i < lstChanges.num; i++)
            {
                int nAction = lstActions[i];
                WriteCHAR(lstChanges[i], 8);
                if (lstChanges[i] == PLAYER_ACTION_DELETE)
                {
                    if (pPlayerLastUpdate->m_aActions[nAction].m_nGroupActionId == ::ACTION_INVALID)
                    {
                        WriteWORD(pPlayerLastUpdate->m_aActions[nAction].m_nActionType, 16);
                        WriteWORD(pPlayerLastUpdate->m_aActions[nAction].m_nActionId, 16);
                    }
                    else
                    {
                        WriteWORD(pPlayerLastUpdate->m_aActions[nAction].m_nGroupActionId, 16);
                        WriteWORD(pPlayerLastUpdate->m_aActions[nAction].m_nActionId, 16);
                    }
                }
                else if (lstChanges[i] == PLAYER_ACTION_SPELL)
                {
                    CSWActionNode* pAction = &pCreature->m_aActionQueue[nAction];
                    WriteWORD(pAction->m_nActionType, 16);
                    WriteWORD(pAction->m_nActionId, 16);
                    WriteOBJECTIDServer(pAction->m_oidTarget);
                }
                else if (lstChanges[i] == PLAYER_ACTION_ADD)
                {
                    CSWActionNode* pAction = &pCreature->m_aActionQueue[nAction];
                    WriteWORD(pAction->m_nGroupActionId, 16);
                    WriteWORD(pAction->m_nActionId, 16);
                    if (pAction->m_nGroupActionId != ::ACTION_MOVETOPOINT &&
                        pAction->m_nGroupActionId != ::ACTION_DROPITEM)
                        WriteOBJECTIDServer(pAction->m_oidTarget);
                    if (pAction->m_nGroupActionId == ::ACTION_MOVETOPOINT ||
                        pAction->m_nGroupActionId == ::ACTION_CASTSPELL ||
                        pAction->m_nGroupActionId == ::ACTION_ITEMCASTSPELL ||
                        pAction->m_nGroupActionId == ::ACTION_DROPITEM ||
                        pAction->m_nGroupActionId == ::ACTION_SETTRAP)
                    {
                        WriteFLOAT(pAction->m_vLocation.m_fX, 1.0f, 32);
                        WriteFLOAT(pAction->m_vLocation.m_fY, 1.0f, 32);
                        WriteFLOAT(pAction->m_vLocation.m_fZ, 1.0f, 32);
                    }
                    if (pAction->m_nGroupActionId == ::ACTION_CASTSPELL)
                    {
                        unsigned long nParameter = pAction->m_nParameter;
                        WriteDWORD(nParameter, 32);
                    }
                    else if (pAction->m_nGroupActionId == ::ACTION_ITEMCASTSPELL)
                    {
                        unsigned long nParameter = pAction->m_nParameter;
                        WriteDWORD(nParameter, 32);
                    }
                }
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_VISIBILITY)
        {
            CExoArrayList<CSWVisibilityNode*> lstUpdates;
            CExoArrayList<CSWVisibilityNode*> lstDeletes;
            int bFound;
            int i;
            int j;
            CSWVisibilityNode* pNode;
            for (i = 0; i < pPlayerLastUpdate->m_lstVisibility.num; i++)
            {
                bFound = 0;
                pNode = pPlayerLastUpdate->m_lstVisibility[i];
                for (j = 0; j < pCreature->m_lstVisibilityList.num; j++)
                {
                    unsigned long oidCurrent = pCreature->m_lstVisibilityList[j]->m_oidObject;
                    if (pNode->m_oidObject == oidCurrent)
                    {
                        bFound = 1;
                        break;
                    }
                }
                if (!bFound)
                    lstDeletes.Add(pNode);
            }
            WriteDWORD(lstDeletes.num, 32);
            for (i = 0; i < lstDeletes.num; i++)
            {
                unsigned long oidDelete = lstDeletes[i]->m_oidObject;
                WriteOBJECTIDServer(oidDelete);
            }
            for (i = 0; i < pCreature->m_lstVisibilityList.num; i++)
            {
                bFound = 0;
                pNode = pCreature->m_lstVisibilityList[i];
                for (j = 0; j < pPlayerLastUpdate->m_lstVisibility.num; j++)
                {
                    unsigned long oidLast = pPlayerLastUpdate->m_lstVisibility[j]->m_oidObject;
                    if (pNode->m_oidObject == oidLast &&
                        pNode->m_bHeard == pPlayerLastUpdate->m_lstVisibility[j]->m_bHeard &&
                        pNode->m_bSeen == pPlayerLastUpdate->m_lstVisibility[j]->m_bSeen &&
                        pNode->m_bInvisible == pPlayerLastUpdate->m_lstVisibility[j]->m_bInvisible)
                    {
                        bFound = 1;
                        break;
                    }
                }
                if (!bFound)
                    lstUpdates.Add(pNode);
            }
            WriteDWORD(lstUpdates.num, 32);
            for (i = 0; i < lstUpdates.num; i++)
            {
                pNode = lstUpdates[i];
                unsigned long oidUpdate = pNode->m_oidObject;
                WriteOBJECTIDServer(oidUpdate);
                WriteBOOL(pNode->m_bHeard);
                WriteBOOL(pNode->m_bSeen);
                WriteBOOL(pNode->m_bInvisible);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_ABILITIES)
        {
            if (pLastUpdateObject != NULL)
            {
                if (pCreature->GetStats() != NULL)
                {
                    WriteBOOL(1);
                    WriteBYTE(pCreature->GetStats()->m_nStrengthBase, 8);
                    WriteBYTE(pCreature->GetStats()->m_nDexterityBase, 8);
                    WriteBYTE(pCreature->GetStats()->m_nConstitutionBase, 8);
                    WriteBYTE(pCreature->GetStats()->m_nIntelligenceBase, 8);
                    WriteBYTE(pCreature->GetStats()->m_nWisdomBase, 8);
                    WriteBYTE(pCreature->GetStats()->m_nCharismaBase, 8);
                    WriteBYTE(pCreature->GetStats()->GetSTRStat(), 8);
                    WriteBYTE(pCreature->GetStats()->GetDEXStat(), 8);
                    WriteBYTE(pCreature->GetStats()->GetCONStat(), 8);
                    WriteBYTE(pCreature->GetStats()->GetINTStat(), 8);
                    WriteBYTE(pCreature->GetStats()->GetWISStat(), 8);
                    WriteBYTE(pCreature->GetStats()->GetCHAStat(), 8);
                    WriteCHAR(pCreature->GetStats()->GetWillSavingThrow(0), 8);
                    WriteCHAR(pCreature->GetStats()->GetReflexSavingThrow(0), 8);
                    WriteCHAR(pCreature->GetStats()->GetFortSavingThrow(0), 8);
                }
                else
                    WriteBOOL(0);
            }
            else
                WriteBOOL(0);
        }
        if ((nUpdateMask & PLAYER_UPDATE_SKILLS) && pLastUpdateObject != NULL && pCreature->GetStats() != NULL)
        {
            for (int nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
            {
                WriteCHAR(pCreature->GetStats()->GetSkillRank(nSkill, NULL, 1), 8);
                WriteCHAR(pCreature->GetStats()->GetSkillRank(nSkill, NULL, 0), 8);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_PORTRAIT)
        {
            WriteWORD(pCreature->GetPortraitId(), 16);
            if (pCreature->GetPortraitId() >= PORTRAIT_ID_FIRST_RESERVED)
                WriteCResRef(pCreature->GetPortrait().GetResRefStr(), 16);
        }
        if (nUpdateMask & PLAYER_UPDATE_HIT_POINTS)
        {
            WriteSHORT(pCreature->m_nMaxHitPoints, 16);
            WriteSHORT(pCreature->GetCurrentHitPoints(0), 16);
            WriteSHORT(pCreature->m_nTemporaryHitPoints, 16);
            WriteSHORT(pCreature->GetMaxHitPoints(1), 16);
        }
        if (nUpdateMask & PLAYER_UPDATE_FORCE_POINTS)
        {
            WriteSHORT(pCreature->GetStats()->m_nForcePoints, 16);
            WriteSHORT(pCreature->GetStats()->GetCurrentForcePoints(), 16);
            WriteSHORT(pCreature->GetStats()->m_nTemporaryForcePoints, 16);
            WriteSHORT(pCreature->GetMaxForcePoints(), 16);
        }
        if (nUpdateMask & PLAYER_UPDATE_SPELL_LIKE_ABILITIES)
            ComputeLastUpdate_WriteSpellLikeAbility(pCreature, pPlayerLastUpdate);
        if (pPlayerLastUpdate != NULL)
            pPlayerLastUpdate->ClearSpellAddDeleteLists();
    }
}
// PC 0x0056B830  Xbox 0x0022E9A0
int CSWSMessage::CompareCreatureLists(CExoArrayList<unsigned long>* pFirst,
                                      CExoArrayList<unsigned long>* pSecond)
{
    if (pFirst->num != pSecond->num)
    {
        return 0;
    }
    for (int i = 0; i < pFirst->num; i++)
    {
        if ((*pFirst)[i] != (*pSecond)[i])
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x0056B880  Xbox 0x0022E9F0
int CSWSMessage::ComputeVisibilityLists(
    CSWSCreature* pCreature, CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    CExoArrayList<CSWVisibilityNode*>* pVisibility =
        &pCreature->m_lstVisibilityList;
    CExoArrayList<CSWVisibilityNode*>* pLastVisibility =
        &pLastUpdateObject->m_lstVisibility;
    int nLastCount = pLastVisibility->num;
    int nCount = pVisibility->num;
    if (nCount != nLastCount)
    {
        return 1;
    }
    for (int i = 0; i < nCount; i++)
    {
        CSWVisibilityNode* pNode = (*pVisibility)[i];
        CSWVisibilityNode* pLastNode;
        int bFound = 0;
        for (int j = 0; j < nLastCount; j++)
        {
            if (pNode->m_oidObject == (*pLastVisibility)[i]->m_oidObject)
            {
                pLastNode = (*pLastVisibility)[i];
                bFound = 1;
            }
        }
        if (bFound != 1)
        {
            return 1;
        }
        if (pNode->m_bSeen != pLastNode->m_bSeen ||
            pNode->m_bHeard != pLastNode->m_bHeard ||
            pNode->m_bInvisible != pLastNode->m_bInvisible)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x0056B950  Xbox 0x0022EAC0
unsigned long CSWSMessage::ComputeUpdateRequired(CSWSPlayer* pPlayer,
    CSWSObject* pObject, CLastUpdateObject* pLastUpdateObject, int bPlayerObject)
{
    unsigned long nUpdateMask = 0;
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        pCreature->ComputeAIState();

        unsigned char nReputation;
        if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
            pPlayer->GetGameObject()->AsSWSCreature() != NULL)
            nReputation = pPlayer->GetGameObject()->AsSWSCreature()->GetAIStateReputation(pCreature->GetId());
        else
            nReputation = CSWSObject::AI_STATE_REPUTATION_DEFAULT;

        if (pLastUpdateObject->m_nAIStateFlags != pCreature->m_nAIStateFlags ||
            pLastUpdateObject->m_nAIState != pCreature->m_nAIState ||
            pLastUpdateObject->m_nActivity != (unsigned short)pCreature->m_nActivity ||
            pLastUpdateObject->m_oidAITarget != pCreature->m_oidAITarget ||
            pLastUpdateObject->m_nAIStateReputation != nReputation ||
            pLastUpdateObject->m_bCombatState != pCreature->m_bCombatState ||
            pLastUpdateObject->m_nStealthMode != pCreature->m_nStealthMode)
            nUpdateMask = OBJECT_UPDATE_AI_STATE;

        if (!CompareVisualEffectLists(&pLastUpdateObject->m_lstLoopingVisualEffects,
                                      &pCreature->m_lstLoopingVisualEffects))
            nUpdateMask |= OBJECT_UPDATE_VISUAL_EFFECTS;
        if (pObject->m_fSyncHeight != pLastUpdateObject->m_fUnknown50 ||
            pLastUpdateObject->m_nAnimation != pCreature->m_nAnimation)
            nUpdateMask |= OBJECT_UPDATE_ANIMATION;

        if ((pCreature->m_nAnimation == 10009 || pCreature->m_nAnimation == 10109) &&
            pCreature->m_oidAttackTarget != OBJECT_INVALID &&
            pCreature->m_oidAttackTarget != pLastUpdateObject->m_oidAttackTarget)
            nUpdateMask |= OBJECT_UPDATE_ORIENTATION | OBJECT_UPDATE_ANIMATION;
        if (pCreature->GetLockOrientationToObject() != pLastUpdateObject->m_oidLockOrientationToObject)
            nUpdateMask |= OBJECT_UPDATE_ORIENTATION;

        CPathfindInformation* pPath = pCreature->m_pPathfindInformation;
        int bCharacterDetailsChanged = 0;
        int nClass;
        for (nClass = 0; nClass < pCreature->m_pStats->m_nClassCount; nClass++)
        {
            if (pCreature->m_pStats->GetClass(nClass) != pLastUpdateObject->m_nClass[nClass] ||
                pCreature->m_pStats->GetClassLevel(nClass, 0) != pLastUpdateObject->m_nClassLevel[nClass])
            {
                bCharacterDetailsChanged = 1;
                break;
            }
        }
        if (bCharacterDetailsChanged || pCreature->m_pStats->m_nRace != pLastUpdateObject->m_nRace ||
            pCreature->m_pStats->m_sSubrace != pLastUpdateObject->m_sTag ||
            pCreature->m_pStats->m_nSubraceIndex != pLastUpdateObject->m_nSubraceIndex ||
            pCreature->m_pStats->m_nGender != pLastUpdateObject->m_nGender ||
            pCreature->m_pStats->m_nGoodEvilAlignment != pLastUpdateObject->m_nGoodEvilAlignment ||
            pCreature->m_nF4 != pLastUpdateObject->m_nObjectF4)
        {
            nUpdateMask |= OBJECT_UPDATE_CHARACTER_DETAILS;
        }

        if (!pCreature->AnimationStationary(pCreature->GetAnimation()))
        {
            unsigned short nWayPoints = pPath->m_nWayPointCount / 2;
            float* pWayPoints = pPath->m_pWayPoints;
            if (nWayPoints != 0)
            {
                Vector vDestination(pWayPoints[2 * nWayPoints - 2],
                                    pWayPoints[2 * nWayPoints - 1], 0.0f);
                Vector vDifference;
                vDifference = vDestination - pLastUpdateObject->m_vUnknown54;
                float fDistanceSquared = vDifference * vDifference;
                if (fDistanceSquared >= OBJECT_UPDATE_DISTANCE_SQUARED ||
                    pPath->m_nField_23C != 0)
                {
                    pPath->m_nField_23C = 0;
                    nUpdateMask |= OBJECT_UPDATE_LOCATION | OBJECT_UPDATE_ORIENTATION | OBJECT_UPDATE_ANIMATION;
                }
            }
            if (pCreature->m_bUpdateMovingOrientation == 1 &&
                pLastUpdateObject->m_vOrientation * pCreature->m_vOrientation < OBJECT_UPDATE_ORIENTATION_COSINE)
                nUpdateMask |= OBJECT_UPDATE_LOCATION | OBJECT_UPDATE_ORIENTATION;
        }
        else
        {
            if (nUpdateMask & OBJECT_UPDATE_ANIMATION)
                nUpdateMask |= OBJECT_UPDATE_LOCATION | OBJECT_UPDATE_ORIENTATION;
            else
            {
                Vector vDifference = pLastUpdateObject->m_vUnknown30 - pCreature->m_vPosition;
                if (vDifference * vDifference >= OBJECT_UPDATE_DISTANCE_SQUARED ||
                    (pCreature->GetArea() != NULL && pCreature->GetArea()->GetId() != pLastUpdateObject->m_nUnknown3C) ||
                    pLastUpdateObject->m_vOrientation * pCreature->m_vOrientation < OBJECT_UPDATE_ORIENTATION_COSINE)
                    nUpdateMask |= OBJECT_UPDATE_LOCATION | OBJECT_UPDATE_ORIENTATION;
            }
        }
        if (fabs(pLastUpdateObject->m_fWalkRate - pCreature->GetWalkRate()) > OBJECT_UPDATE_RATE_TOLERANCE ||
            fabs(pLastUpdateObject->m_fRunRate - pCreature->GetRunRate()) > OBJECT_UPDATE_RATE_TOLERANCE)
            nUpdateMask |= OBJECT_UPDATE_MOVEMENT_RATES;

        if (bPlayerObject || pPlayer->GetGameObject()->AsSWSCreature()->GetFaction()->GetCreatureInParty(pCreature->GetId()))
        {
            if (pLastUpdateObject->m_nBaseMaxHitPoints != (short)pCreature->m_nMaxHitPoints ||
                pLastUpdateObject->m_nCurrentHitPoints != pCreature->GetCurrentHitPoints(1) ||
                pLastUpdateObject->m_nTemporaryHitPoints != (short)pCreature->m_nTemporaryHitPoints ||
                pLastUpdateObject->m_nMaxHitPoints != pCreature->GetMaxHitPoints(1))
                nUpdateMask |= OBJECT_UPDATE_HIT_POINTS;
            if (pLastUpdateObject->m_nForcePoints != pCreature->m_pStats->m_nForcePoints ||
                pLastUpdateObject->m_nCurrentForcePoints != pCreature->m_pStats->m_nCurrentForcePoints ||
                pLastUpdateObject->m_nTemporaryForcePoints != pCreature->m_pStats->m_nTemporaryForcePoints ||
                pLastUpdateObject->m_nMaxForcePoints != pCreature->GetMaxForcePoints())
                nUpdateMask |= OBJECT_UPDATE_FORCE_POINTS;
        }
        if (g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pCreature->GetId()) != NULL || bPlayerObject)
        {
            if (pCreature->AsSWSCreature()->m_nAreaWaitState != pLastUpdateObject->m_nAreaWaitState ||
                ((pCreature->AsSWSCreature()->m_nAIStateFlags & CSWSCreature::AI_STATE_FLAG_0040) == CSWSCreature::AI_STATE_FLAG_0040) != pLastUpdateObject->m_bAIStateFlag0040 ||
                pCreature->GetFaction()->GetSingletonParty() != pLastUpdateObject->m_bSingletonParty ||
                pCreature->AsSWSCreature()->m_bInvitedToParty != pLastUpdateObject->m_bInvitedToParty ||
                (pCreature->AsSWSCreature()->GetFaction()->GetLeader() == pCreature->GetId()) != pLastUpdateObject->m_bPartyLeader)
                nUpdateMask |= OBJECT_UPDATE_PARTY_STATE;
        }
        if (pPlayer->GetGameObject() != NULL)
        {
            CSWSCreature* pPlayerCreature = pPlayer->GetGameObject()->AsSWSCreature();
            CSWSCreature* pVisibleCreature = pObject->AsSWSCreature();
            if (pPlayerCreature != NULL && pVisibleCreature != NULL)
            {
                if (pPlayerCreature->GetId() == pVisibleCreature->GetId())
                {
                    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
                    if (pModule != NULL && pModule->m_lstInvisibleCreatures.Contains(pVisibleCreature->GetId()) != pLastUpdateObject->m_bInvisible)
                        nUpdateMask |= OBJECT_UPDATE_PERCEPTION;
                }
                else
                {
                    CSWVisibilityNode* pNode = pVisibleCreature->GetVisibleListElement(pPlayerCreature->GetId());
                    if (pNode != NULL)
                    {
                        if (pNode->m_bHeard != pLastUpdateObject->m_bHeard ||
                            pNode->m_bSeen != pLastUpdateObject->m_bSeen ||
                            pNode->m_bInvisible != pLastUpdateObject->m_bInvisible)
                            nUpdateMask |= OBJECT_UPDATE_PERCEPTION;
                    }
                    else if (pLastUpdateObject->m_bHeard == 1 || pLastUpdateObject->m_bSeen == 1 || pLastUpdateObject->m_bInvisible == 1)
                        nUpdateMask |= OBJECT_UPDATE_PERCEPTION;
                }
            }
        }
        if (pCreature->GetDamageLevel() != pLastUpdateObject->m_nDamageLevel)
            nUpdateMask |= OBJECT_UPDATE_DAMAGE_LEVEL;
        if (pLastUpdateObject->m_nPortraitId != pObject->GetPortraitId() ||
            pLastUpdateObject->m_resRef != pObject->GetPortrait())
            nUpdateMask |= OBJECT_UPDATE_PORTRAIT;
        if (bPlayerObject && pLastUpdateObject->m_nClientSidePathType != pPath->m_nField_28)
            nUpdateMask |= OBJECT_UPDATE_LOCATION | OBJECT_UPDATE_ORIENTATION | OBJECT_UPDATE_ANIMATION;
        if (pLastUpdateObject->m_bDeleteNoFade != pCreature->m_bDeleteNoFade ||
            pLastUpdateObject->m_nFadeOutTime != pCreature->m_nFadeOutTime ||
            pLastUpdateObject->m_bKeepCorpse != pCreature->m_bKeepCorpse)
            nUpdateMask |= OBJECT_UPDATE_CORPSE;
    }

    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM ||
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT ||
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        if (pObject->m_fSyncHeight != pLastUpdateObject->m_fUnknown50)
            nUpdateMask |= OBJECT_UPDATE_ANIMATION;
        if (pLastUpdateObject->m_nAnimation != pObject->m_nAnimation)
            nUpdateMask |= OBJECT_UPDATE_ANIMATION;
        Vector vDifference = pLastUpdateObject->m_vUnknown30 - pObject->m_vPosition;
        if (vDifference * vDifference >= OBJECT_UPDATE_DISTANCE_SQUARED)
            nUpdateMask |= OBJECT_UPDATE_LOCATION;
        if (pLastUpdateObject->m_vOrientation * pObject->m_vOrientation < OBJECT_UPDATE_ORIENTATION_COSINE)
            nUpdateMask |= OBJECT_UPDATE_ORIENTATION;
        if (pLastUpdateObject->m_nPortraitId != pObject->GetPortraitId() || pLastUpdateObject->m_resRef != pObject->GetPortrait())
            nUpdateMask |= OBJECT_UPDATE_PORTRAIT;
        if (pObject->GetArea() != NULL && pObject->GetArea()->GetId() != pLastUpdateObject->m_nUnknown3C)
            nUpdateMask |= OBJECT_UPDATE_LOCATION | OBJECT_UPDATE_ORIENTATION;
        if (!CompareVisualEffectLists(&pLastUpdateObject->m_lstLoopingVisualEffects, &pObject->m_lstLoopingVisualEffects))
            nUpdateMask |= OBJECT_UPDATE_VISUAL_EFFECTS;
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            if (pObject->AsSWSDoor()->m_bLocked != pLastUpdateObject->m_bLocked ||
                pObject->AsSWSDoor()->m_bPlotObject != pLastUpdateObject->m_bPlotObject ||
                pObject->AsSWSDoor()->m_bTrapFlag != pLastUpdateObject->m_bTrapFlag ||
                pObject->AsSWSDoor()->GetFlagged() != pLastUpdateObject->m_bTrapFlagged)
                nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
            else
            {
                CExoArrayList<unsigned long>* pTrapDetectedBy =
                    &pObject->AsSWSDoor()->m_lstTrapDetectedBy;
                if (!CompareCreatureLists(pTrapDetectedBy, &pLastUpdateObject->m_lstTrapDetectedBy))
                    nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            if (pObject->AsSWSPlaceable()->m_bLocked != pLastUpdateObject->m_bLocked ||
                pObject->AsSWSPlaceable()->GetPlotObject() != pLastUpdateObject->m_bPlotObject ||
                pObject->AsSWSPlaceable()->GetTrapFlag() != pLastUpdateObject->m_bTrapFlag ||
                pObject->AsSWSPlaceable()->GetFlagged() != pLastUpdateObject->GetTrapFlagged())
                nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
            else
            {
                CExoArrayList<unsigned long>* pTrapDetectedBy =
                    &pObject->AsSWSPlaceable()->m_lstTrapDetectedBy;
                if (!CompareCreatureLists(pTrapDetectedBy, &pLastUpdateObject->m_lstTrapDetectedBy))
                    nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
            }
        }
    }
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER && pObject->AsSWSTrigger()->GetTrapFlag())
    {
        if (pObject->AsSWSTrigger()->GetFlagged() != pLastUpdateObject->m_bTrapFlagged)
            nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
        unsigned long oidCreator = pObject->AsSWSTrigger()->GetCreator();
        CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreator);
        if (pCreator != NULL)
        {
            if (pObject->AsSWSTrigger()->GetFactionId() != pCreator->GetFactionId())
                pObject->AsSWSTrigger()->m_nFaction = pCreator->GetFactionId();
            if (pLastUpdateObject->m_nFaction != pCreator->GetFactionId())
                nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
        }
        else if (pLastUpdateObject->m_nFaction != pObject->AsSWSTrigger()->GetFactionId())
            nUpdateMask |= OBJECT_UPDATE_TRAP_STATE;
    }
    return nUpdateMask;
}
// PC 0x0056C310  Xbox 0x0022F500
int CSWSMessage::ComputeLastUpdate_GuiFeats(CSWSCreature* pCreature,
                                         CSWSPlayerLastUpdateObject* pLastUpdate)
{
    if ((unsigned short)pCreature->m_pStats->GetNumFeats() != pLastUpdate->m_lstFeats.num)
    {
        return 1;
    }
    if ((unsigned short)pCreature->m_pStats->m_lstBonusFeats.num != pLastUpdate->m_lstBonusFeats.num)
    {
        return 1;
    }
    if (pCreature->m_pStats->m_lstFeatUses.num != pLastUpdate->m_lstFeatUses.num)
    {
        return 1;
    }

    for (unsigned short i = 0;
         i < (unsigned short)pCreature->m_pStats->GetNumFeats(); i++)
    {
        unsigned short nFeat = pCreature->m_pStats->GetFeat(i);
        int bFound = 0;
        for (int j = 0; j < pLastUpdate->m_lstFeats.num; j++)
        {
            if (pLastUpdate->m_lstFeats[j] == nFeat)
            {
                bFound = 1;
            }
        }
        if (!bFound)
        {
            return 1;
        }
    }
    for (unsigned short i = 0;
         i < (unsigned short)pCreature->m_pStats->m_lstBonusFeats.num; i++)
    {
        unsigned short nFeat = pCreature->m_pStats->GetBonusFeat(i);
        int bFound = 0;
        for (int j = 0; j < pLastUpdate->m_lstBonusFeats.num; j++)
        {
            if (pLastUpdate->m_lstBonusFeats[j] == nFeat)
            {
                bFound = 1;
            }
        }
        if (!bFound)
        {
            return 1;
        }
    }
    CExoArrayList<CFeatUseListEntry*>* pFeatUses =
        &pCreature->m_pStats->m_lstFeatUses;
    int nFeatUses = pFeatUses->num;
    for (unsigned short i = 0; i < nFeatUses; i++)
    {
        CFeatUseListEntry* pLastFeatUse = pLastUpdate->m_lstFeatUses[i];
        CFeatUseListEntry* pFeatUse = (*pFeatUses)[i];
        if (pFeatUse->m_nFeat != pLastFeatUse->m_nFeat ||
            pFeatUse->m_nUses != pLastFeatUse->m_nUses)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x0056C430  Xbox 0x0022F620
int CSWSMessage::ComputeLastUpdate_CompareSpellLikeAbility(
    CSWSCreature* pCreature, CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    int nChanges = 0;
    CExoArrayList<CSWSStats_SpellLikeAbility>* pLastAbilities =
        pLastUpdateObject->m_pExtraSpells;
    CExoArrayList<CSWSStats_SpellLikeAbility>* pAbilities =
        pCreature->m_pStats->m_pSpellLikeAbilities;
    int nCount = pAbilities->num;
    int nLastCount = 0;
    if (pLastAbilities != NULL)
        nLastCount = pLastAbilities->num;
    if (nLastCount != nCount)
        nChanges = 1;
    for (int nIndex = 0; nIndex < pAbilities->num; nIndex++)
    {
        if (nIndex >= nLastCount ||
            (*pAbilities)[nIndex].m_nReady !=
                (*pLastAbilities)[nIndex].m_nReady ||
            (*pAbilities)[nIndex].m_nLevel !=
                (*pLastAbilities)[nIndex].m_nLevel ||
            (*pAbilities)[nIndex].m_nSpell !=
                (*pLastAbilities)[nIndex].m_nSpell)
        {
            nChanges++;
        }
    }
    return nChanges;
}
// PC 0x0056C4D0  Xbox 0x0022F6C0
void CSWSMessage::ComputeLastUpdate_WriteSpellLikeAbility(
    CSWSCreature* pCreature, CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    WriteWORD(ComputeLastUpdate_CompareSpellLikeAbility(pCreature,
                                                      pLastUpdateObject), 16);
    CExoArrayList<CSWSStats_SpellLikeAbility>* pLastAbilities =
        pLastUpdateObject->m_pExtraSpells;

    int nCount = pCreature->m_pStats->m_pSpellLikeAbilities->num;
    int nLastCount = 0;
    if (pLastAbilities != NULL)
        nLastCount = pLastAbilities->num;
    CSWSCreatureStats * pStats = pCreature->m_pStats;
    CExoArrayList<CSWSStats_SpellLikeAbility>* pAbilities =
        pStats->m_pSpellLikeAbilities;
    if (nLastCount != nCount)
    {
        WriteBYTE(SPELL_LIKE_ABILITY_SIZE, 8);
        WriteWORD(nCount, 16);
    }
    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        if (nIndex >= nLastCount)
        {
            WriteBYTE(SPELL_LIKE_ABILITY_UPDATE, 8);
            WriteWORD(nIndex, 16);
            WriteBOOL(pAbilities->element[nIndex].m_nReady);
            WriteBYTE(pAbilities->element[nIndex].m_nLevel, 8);
            WriteWORD(pAbilities->element[nIndex].m_nSpell, 16);
        }
        else
        {

            CSWSStats_SpellLikeAbility* pLast = &pLastAbilities->element[nIndex];
            if (pAbilities->element[nIndex].m_nReady != pLast->m_nReady ||
                pAbilities->element[nIndex].m_nLevel != pLast->m_nLevel ||
                pAbilities->element[nIndex].m_nSpell !=
                    pLastAbilities->element[nIndex].m_nSpell)
            {
                WriteBYTE(SPELL_LIKE_ABILITY_UPDATE, 8);
                WriteWORD(nIndex, 16);
                WriteBOOL(pAbilities->element[nIndex].m_nReady);
                WriteBYTE(pAbilities->element[nIndex].m_nLevel, 8);
                WriteWORD(pAbilities->element[nIndex].m_nSpell, 16);
            }
        }
    }
}
// PC 0x0056C610  Xbox 0x0022F800
int CSWSMessage::SendServerToPlayerCamera_ChangeLocation(CSWSPlayer* pPlayer,
                                                         int nUpdateMask,
                                                         float fX, float fY,
                                                         float fZ, int nMode)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(256, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nUpdateMask, 8);
    if ((nUpdateMask & 1) != 0)
    {
        WriteFLOAT(fX, 1.0f, 32);
    }
    if ((nUpdateMask & 2) != 0)
    {
        WriteFLOAT(fY, 1.0f, 32);
    }
    if ((nUpdateMask & 4) != 0)
    {
        WriteFLOAT(fZ, 1.0f, 32);
    }
    if ((nUpdateMask & 8) != 0)
    {
        WriteBYTE(nMode, 8);
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CAMERA, 1, pMessage, nMessageSize);
}
// PC 0x0056C6D0  Xbox 0x0022F8C0
int CSWSMessage::SendServerToPlayerCamera_SetMode(CSWSPlayer* pPlayer,
                                                  unsigned char nMode)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nMode, 8);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CAMERA, 2, pMessage, nMessageSize);
}
// PC 0x0056C730  Xbox 0x0022F920
int CSWSMessage::SendServerToPlayerLogin_CharacterQuery(
    CSWSPlayer* pPlayer, unsigned char& nClassCount, int* pnClasses,
    unsigned char* pnClassLevels, unsigned long& nExperience)
{
    unsigned char i;
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nClassCount, 8);
    for (i = 0; i < nClassCount; i++)
    {
        WriteINT(pnClasses[i], 32);
        WriteBYTE(pnClassLevels[i], 8);
    }
    WriteDWORD(nExperience, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_LOGIN, 10, pMessage, nMessageSize);
}
// PC 0x0056C7F0  Xbox 0x0022F9E0
int CSWSMessage::SendServerToPlayerLogin_NeedCharacter(unsigned long nPlayerId)
{
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_LOGIN, 16, NULL, 0);
}
// PC 0x0056C850  Xbox 0x0022FA60
int CSWSMessage::SendServerToPlayerModule_SaveGameStatus(unsigned char nStatus)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nStatus, 8);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, 3, 13,
                                     pMessage, nMessageSize);
}
// PC 0x0056C8B0  Xbox 0x0022FAC0
int CSWSMessage::SendServerToPlayerLoadBar_StartStallEvent(unsigned long nEvent)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nEvent, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, 44, 1,
                                     pMessage, nMessageSize);
}
// PC 0x0056C910  Xbox 0x0022FB20
int CSWSMessage::SendServerToPlayerLoadBar_UpdateStallEvent(
    unsigned long nEvent, unsigned long nProgress)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nEvent, 32);
    WriteDWORD(nProgress, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, 44, 2,
                                     pMessage, nMessageSize);
}
// PC 0x0056C980  Xbox 0x0022FB90
int CSWSMessage::SendServerToPlayerLoadBar_EndStallEvent(unsigned long nEvent,
                                                         unsigned long nResult)
{
    int nMessageResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nEvent, 32);
    WriteDWORD(nResult, 3);
    nMessageResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nMessageResult == 0)
    {
        return nMessageResult;
    }
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, 44, 3,
                                     pMessage, nMessageSize);
}
// PC 0x0056C9F0  Xbox 0x0022FC00
int CSWSMessage::SendServerToPlayerModule_SetPauseState(unsigned char nPauseType,
                                                       int bPause)
{
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CExoLinkedList<unsigned long>* pClientExclusionList = NULL;
    CExoLinkedList<unsigned long>* pServerExclusionList =
        g_pAppManager->m_pServerExoApp->GetActiveExclusionList();
    CExoLinkedListNode* pPosition;

    if (pServerExclusionList != NULL &&
        pServerExclusionList->GetHeadPos() != NULL)
    {
        pClientExclusionList = new CExoLinkedList<unsigned long>;
        for (pPosition = pServerExclusionList->GetHeadPos();
             pPosition != NULL; pServerExclusionList->GetNext(pPosition))
        {
            pClientExclusionList->AddTail((unsigned long*)
                pClientExoApp->ServerToClientObjectId(
                    (unsigned long)pServerExclusionList->GetAtPos(pPosition)));
        }
    }
    pClientExoApp->SetPauseState(nPauseType, bPause, pClientExclusionList);
    return 1;
}
// PC 0x0056CAC0  Xbox 0x0022FCD0
int CSWSMessage::SendServerToPlayerModule_ExportReply(CSWSPlayer* pPlayer)
{
    CSWSPlayer* pMessagePlayer;
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    pMessagePlayer = pPlayer;
    pPlayer->PackCreatureIntoMessage();
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pMessagePlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_MODULE, 10, pMessage, nMessageSize);
}
// PC 0x0056CB10  Xbox 0x0022FD20
int CSWSMessage::SendServerToPlayerModule_Loading(CSWSPlayer* pPlayer)
{
    return SendServerToPlayerMessage(pPlayer->m_nPlayerId, MESSAGE_MODULE, 6, NULL, 0);
}
// PC 0x0056CB80  Xbox 0x0022FDB0
int CSWSMessage::SendServerToPlayerSaveLoad_Status(CSWSPlayer* pPlayer,
                                                   unsigned char nStatus,
                                                   unsigned long nValue)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    if (pPlayer == 0)
    {
        return 0;
    }
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nStatus, 8);
    WriteDWORD(nValue, 4);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_SAVE_LOAD, 1, pMessage, nMessageSize);
}
// PC 0x0056CBF0
int CSWSMessage::SendServerToPlayerModule_StartStartNewModule()
{
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, MESSAGE_MODULE, 11, NULL, 0);
}
// PC 0x0056CC50  Xbox 0x0022FE20
int CSWSMessage::SendServerToPlayerModule_EndStartNewModule(int bSuccess)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bSuccess);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, 3, 12,
                                     pMessage, nMessageSize);
}
// PC 0x0056CCB0  Xbox 0x0022FE80
int CSWSMessage::SendServerToPlayerModule_DumpPlayer(CSWSPlayer* pPlayer)
{
    return SendServerToPlayerMessage(pPlayer->m_nPlayerId, MESSAGE_MODULE, 8, NULL, 0);
}
// PC 0x0056CD20  Xbox 0x0022FF10
int CSWSMessage::SendServerToPlayerArea_ClientArea(
    CSWSPlayer* pPlayer, CSWSArea* pArea, float fX, float fY, float fZ,
    const Vector& vOrientation, int bPlayerIsNewToModule)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    if (g_pAppManager->GetServerExoApp()->GetServerMode() !=
            CLIENT_SERVER_MODE_SERVER || pModule == NULL)
    {
        return 0;
    }
    CSWSModuleEntryInfo* pEntryInfo = pModule->m_pEntryInfo;
    if (pPlayer->GetGameObject() == NULL || pArea == NULL)
    {
        return 0;
    }
    CreateWriteMessage(0x10000, RECIPIENT_UNSPECIFIED, 1);
    if (pPlayer->GetAreaTransitionBMP() == CSWSPlayer::AREA_TRANSITION_DEFAULT ||
        pPlayer->GetAreaTransitionBMP() == CSWSPlayer::AREA_TRANSITION_USER_DEFINED)
    {
        pPlayer->SetAreaTransitionBMP(pArea->GetLoadScreenID(), "");
    }
    if (pPlayer->GetAreaTransitionBMP() >=
        g_pRules->Get2DArrays()->GetAreaTransition()->m_nNumRows)
    {
        pPlayer->SetAreaTransitionBMP(CSWSPlayer::AREA_TRANSITION_RANDOM, "");
    }
    int nTransition = pPlayer->GetAreaTransitionBMP();
    if (nTransition == CSWSPlayer::AREA_TRANSITION_RANDOM)
    {
        CExoString sAreaTileSet;
        int nRows = g_pRules->Get2DArrays()->GetAreaTransition()->m_nNumRows;
        int nImage = 0;
        CExoString sTileSet;
        int nMatches = 0;
        for (int nRow = CTwoDimArrays::AREA_TRANSITION_FIRST_IMAGE;
             nRow < nRows; ++nRow)
        {
            bool bFound = g_pRules->Get2DArrays()->GetAreaTransition()->GetCExoStringEntry(
                nRow, "TileSet", &sTileSet) == 1;
            if (bFound && sAreaTileSet.CompareNoCase(sTileSet) == 1)
            {
                ++nMatches;
            }
        }
        if (nMatches > 0)
        {
            int nSelected = rand() % nMatches;
            int nMatchedIndex = 0;
            for (int nRow = CTwoDimArrays::AREA_TRANSITION_FIRST_IMAGE;
                 nRow < nRows; ++nRow)
            {
                bool bFound = g_pRules->Get2DArrays()->GetAreaTransition()->GetCExoStringEntry(
                    nRow, "TILESET", &sTileSet) == 1;
                if (bFound && sAreaTileSet.CompareNoCase(sTileSet) == 1)
                {
                    if (nMatchedIndex == nSelected)
                    {
                        nImage = nRow;
                    }
                    ++nMatchedIndex;
                }
            }
        }
        if (nImage == 0)
        {
            nImage = rand() % (nRows - CTwoDimArrays::AREA_TRANSITION_FIRST_IMAGE) +
                     CTwoDimArrays::AREA_TRANSITION_FIRST_IMAGE;
        }
        WriteBYTE(nImage, 8);
    }
    else
    {
        WriteBYTE(nTransition, 8);
    }
    pPlayer->SetAreaTransitionBMP(CSWSPlayer::AREA_TRANSITION_DEFAULT, "");
    float fOrientation = (float)atan2(vOrientation.m_fY, vOrientation.m_fX);
    WriteFLOAT(fX, 1.0f, 32);
    WriteFLOAT(fY, 1.0f, 32);
    WriteFLOAT(fZ, 1.0f, 32);
    WriteFLOAT(fOrientation, 1.0f, 32);
    WriteBOOL(bPlayerIsNewToModule);
    if (pArea->PackAreaIntoMessage((int)pEntryInfo->m_fEntryX,
                                  (int)pEntryInfo->m_fEntryY,
                                  (int)pEntryInfo->m_fEntryZ, pPlayer) == 1)
    {
        if (GetWriteMessage(&pMessage, &nMessageSize) != 0)
        {
            nPlayerId = pPlayer->m_nPlayerId;
            return SendServerToPlayerMessage(nPlayerId, 4, 1,
                                              pMessage, nMessageSize);
        }
    }
    return 0;
}
// PC 0x0056D140  Xbox 0x00230340
int CSWSMessage::SendServerToPlayerArea_VisualEffect(CSWSPlayer* pPlayer,
                                                     unsigned short nEffect,
                                                     Vector vPosition)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(14, RECIPIENT_UNSPECIFIED, 1);
    WriteWORD(nEffect, 16);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AREA, 2, pMessage, nMessageSize);
}
// PC 0x0056D1E0  Xbox 0x002303E0
int CSWSMessage::SendServerToPlayerArea_Weather(CSWSPlayer* pPlayer,
                                                unsigned char nWeather,
                                                int bEnabled)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(5, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nWeather, 8);
    WriteBOOL(bEnabled);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AREA, 4, pMessage, nMessageSize);
}
// PC 0x0056D250  Xbox 0x00230450
int CSWSMessage::SendServerToPlayerSetCustomToken(unsigned long nPlayerId,
                                                  int nToken,
                                                  const CExoString& sValue)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    nMessageSize = sValue.GetLength() + 8;
    CreateWriteMessage(nMessageSize, nPlayerId, 1);
    WriteINT(nToken, 32);
    WriteCExoString(sValue, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CUSTOM_TOKEN, 1, pMessage, nMessageSize);
}
// PC 0x0056D2D0  Xbox 0x002304D0
int CSWSMessage::SendServerToPlayerSetCustomTokenList(unsigned long nPlayerId)
{
    unsigned long nTokenCount = g_pTlkTable->m_nCustomTokenCount;
    unsigned long nCustomTokenCount = nTokenCount;
    unsigned long i;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    for (i = 0; i < nTokenCount; i++)
    {
        if ((int)g_pTlkTable->m_pCustomTokens[i].m_nToken <=
            CTlkTable::CUSTOMTOKEN_RESERVED_END)
        {
            nCustomTokenCount--;
        }
    }
    if (nCustomTokenCount == 0)
    {
        return 1;
    }
    nMessageSize = nCustomTokenCount * 8 + 4;
    CreateWriteMessage(nMessageSize, nPlayerId, 1);
    WriteDWORD(nCustomTokenCount, 32);
    for (i = 0; i < nTokenCount; i++)
    {
        int nToken = g_pTlkTable->m_pCustomTokens[i].m_nToken;
        if (nToken > CTlkTable::CUSTOMTOKEN_RESERVED_END)
        {
            WriteINT(nToken, 32);
            WriteCExoString(g_pTlkTable->m_pCustomTokens[i].m_sValue, 32);
        }
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CUSTOM_TOKEN, CUSTOM_TOKEN_LIST, pMessage, nMessageSize);
}
// PC 0x0056D3C0  Xbox 0x002305C0
int CSWSMessage::SendServerToPlayerChatMultiLang_Helper(unsigned long nPlayerId,
    unsigned char nType, unsigned long oidSpeaker, CExoLocString& lsMessage,
    unsigned long oidTarget, unsigned char nGender, int bListedRecipient,
    const CResRef& resSound, unsigned long oidSound, int nSoundVolume)
{
    unsigned long nMessageSize;
    unsigned char* pMessage;
    if (nType != CHAT_TYPE_MULTILANG)
    {
        return 0;
    }
    nMessageSize = 40;
    CreateWriteMessage(nMessageSize, nPlayerId, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteOBJECTIDServer(oidTarget);
    WriteCExoLocStringServer(lsMessage, nGender);
    WriteCResRef(resSound, 16);
    WriteOBJECTIDServer(oidSound);
    WriteINT(nSoundVolume, 32);
    if (!GetWriteMessage(&pMessage, &nMessageSize))
    {
        return 0;
    }
    if (bListedRecipient)
    {
        nType = CHAT_MULTILANG_LISTED_RECIPIENT;
    }
    else
    {
        nType = CHAT_MULTILANG;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CHAT, nType,
                                   pMessage, nMessageSize);
}
// PC 0x0056D4A0  Xbox 0x002306A0
int CSWSMessage::SendServerToPlayerAIActionPlaySound(unsigned long nPlayerId,
                                                     unsigned long oidSpeaker,
                                                     const CExoString& sSound)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(sSound.GetLength() + 8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteCExoString(sSound, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 9, 7, pMessage, nMessageSize);
}
// PC 0x0056D510  Xbox 0x00230710
int CSWSMessage::SendServerToPlayerSoundObject_Play(CSWSPlayer* pPlayer,
                                                    unsigned long oidSound)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSound);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_SOUND, 2, pMessage, nMessageSize);
}
// PC 0x0056D570  Xbox 0x00230770
int CSWSMessage::SendServerToPlayerSoundObject_Stop(CSWSPlayer* pPlayer,
                                                    unsigned long oidSound)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSound);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_SOUND, 3, pMessage, nMessageSize);
}
// PC 0x0056D5D0  Xbox 0x002307D0
int CSWSMessage::SendServerToPlayerSoundObject_ChangePosition(
    CSWSPlayer* pPlayer, unsigned long oidSound, Vector vPosition)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(16, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSound);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_SOUND, 5, pMessage, nMessageSize);
}
// PC 0x0056D670  Xbox 0x00230870
int CSWSMessage::SendServerToPlayerChat_StrRef(unsigned long nPlayerId,
                                               unsigned long oidSpeaker,
                                               unsigned char nMinor,
                                               unsigned long nStrRef)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteDWORD(nStrRef, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 9, nMinor, pMessage,
                                     nMessageSize);
}
// PC 0x0056D6E0  Xbox 0x002308E0
int CSWSMessage::SendServerToPlayerChat_Party(unsigned long nPlayerId,
                                              unsigned long oidSpeaker,
                                              const CExoString& sMessage)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidSpeaker);
    if (pCreature == NULL)
    {
        return 0;
    }
    CreateWriteMessage(sMessage.GetLength() + 8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteCExoString(sMessage, 32);
    WriteFLOAT(pCreature->m_vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(pCreature->m_vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(pCreature->m_vPosition.m_fZ, 1.0f, 32);
    WriteCExoLocStringServer(*pCreature->GetFirstName(), 0);
    WriteCExoLocStringServer(pCreature->GetLastName(), 0);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 9, 6, pMessage, nMessageSize);
}
// PC 0x0056D7E0  Xbox 0x002309E0
int CSWSMessage::SendServerToPlayerChat_Talk(unsigned long nPlayerId,
                                             unsigned long oidSpeaker,
                                             const CExoString& sMessage)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(sMessage.GetLength() + 8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteCExoString(sMessage, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 9, 1, pMessage, nMessageSize);
}
// PC 0x0056D850  Xbox 0x00230A50
int CSWSMessage::SendServerToPlayerQuickChat(unsigned long nPlayerId,
                                             unsigned long oidSpeaker,
                                             unsigned short nQuickChat)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(10, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteWORD(nQuickChat, 16);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_QUICK_CHAT, 0, pMessage, nMessageSize);
}
// PC 0x0056D8C0  Xbox 0x00230AC0
int CSWSMessage::SendServerToPlayerChat_Tell(unsigned long nPlayerId,
                                            unsigned long oidSpeaker,
                                            const CExoString& sMessage)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidSpeaker);
    if (pCreature == NULL)
    {
        return 0;
    }
    CreateWriteMessage(sMessage.GetLength() + 20, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteCExoString(sMessage, 32);
    WriteFLOAT(pCreature->m_vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(pCreature->m_vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(pCreature->m_vPosition.m_fZ, 1.0f, 32);
    WriteCExoLocStringServer(*pCreature->GetFirstName(), 0);
    WriteCExoLocStringServer(pCreature->GetLastName(), 0);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CHAT, CHAT_TELL,
                                     pMessage, nMessageSize);
}
// PC 0x0056D9C0  Xbox 0x00230BC0
int CSWSMessage::SendServerToPlayerChat_ServerTell(unsigned long nPlayerId,
                                                   const CExoString& sMessage)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(sMessage.GetLength() + 4, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sMessage, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 9, 5, pMessage, nMessageSize);
}
// PC 0x0056DA30  Xbox 0x00230C30
int CSWSMessage::SendServerToPlayerChat_Shout(unsigned long nPlayerId,
                                              unsigned long oidSpeaker,
                                              const CExoString& sMessage)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidSpeaker);
    CreateWriteMessage(sMessage.GetLength() + 20, RECIPIENT_UNSPECIFIED, 1);
    if (pCreature != NULL)
    {
        WriteOBJECTIDServer(oidSpeaker);
        WriteCExoString(sMessage, 32);
        WriteFLOAT(pCreature->m_vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(pCreature->m_vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(pCreature->m_vPosition.m_fZ, 1.0f, 32);
        WriteCExoLocStringServer(*pCreature->GetFirstName(), 0);
        WriteCExoLocStringServer(pCreature->GetLastName(), 0);
    }
    else
    {
        CExoLocString sFirstName;
        CExoLocString sLastName;
        sFirstName.m_dwStringRef = 67458;
        WriteOBJECTIDServer(OBJECT_INVALID);
        WriteCExoString(sMessage, 32);
        WriteFLOAT(0.0f, 1.0f, 32);
        WriteFLOAT(0.0f, 1.0f, 32);
        WriteFLOAT(0.0f, 1.0f, 32);
        WriteCExoLocStringServer(sFirstName, 0);
        WriteCExoLocStringServer(sLastName, 0);
    }
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CHAT, CHAT_SHOUT,
                                     pMessage, nMessageSize);
}
// PC 0x0056DBF0  Xbox 0x00230E00
int CSWSMessage::SendServerToPlayerChat_Whisper(unsigned long nPlayerId,
                                                unsigned long oidSpeaker,
                                                const CExoString& sMessage)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(sMessage.GetLength() + 8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSpeaker);
    WriteCExoString(sMessage, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 9, 3, pMessage, nMessageSize);
}
// PC 0x0056DC60  Xbox 0x00230E70
int CSWSMessage::TestObjectVisible(CSWSObject* pObject,
    CSWSObject* pPlayerObject)
{
    NwssendplayermessageTrace1();
    Vector vObjectPosition = pObject->m_vPosition;
    int bVisible = 0;
    Vector vPlayerPosition = pPlayerObject->m_vPosition;
    int bInArea = 1;
    if (pPlayerObject->GetArea() == NULL &&
        pPlayerObject->AsSWSCreature() != NULL)
    {
        bInArea = 0;
        vPlayerPosition = pPlayerObject->AsSWSCreature()->m_vDisappearPosition;
    }
    float fX = vObjectPosition.m_fX - vPlayerPosition.m_fX;
    float fY = vObjectPosition.m_fY - vPlayerPosition.m_fY;
    float fDistanceSquared = fX * fX + fY * fY;
    float fCheckDistance = 250.0f;
    if (pPlayerObject->AsSWSCreature() != NULL)
    {
        fCheckDistance =
            pPlayerObject->AsSWSCreature()->GetSpotCheckDistance() >=
                pPlayerObject->AsSWSCreature()->GetListenCheckDistance() ?
            pPlayerObject->AsSWSCreature()->GetSpotCheckDistance() :
            pPlayerObject->AsSWSCreature()->GetListenCheckDistance();
    }
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_WAYPOINT ||
        pObject->AsSWSEncounter() != NULL ||
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_STORE ||
        pObject->AsSWSSoundObject() != NULL)
    {
        bVisible = 0;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSArea* pArea = pObject->GetArea();
        if (pPlayerObject->AsSWSCreature() != NULL)
        {
            if (pObject->GetId() == pPlayerObject->GetId() ||
                pObject->AsSWSCreature()->m_bInParty != 0)
            {
                bVisible = 1;
            }
            else
            {
                bVisible = 0;
                if (bInArea == 0)
                {
                    if (fCheckDistance * fCheckDistance >= fDistanceSquared &&
                        pObject->AsSWSCreature()->GetInvisible(
                            pPlayerObject->AsSWSCreature()) == 0)
                    {
                        bVisible = 1;
                    }
                }
                else
                {
                    CSWVisibilityNode* pNode = pPlayerObject->AsSWSCreature()
                        ->GetVisibleListElement(pObject->GetId());
                    if (pNode != NULL &&
                        (pNode->m_bSeen != 0 || pNode->m_bHeard != 0))
                    {
                        bVisible = 1;
                    }
                }
            }
        }
    }
    else if (fCheckDistance * fCheckDistance >= fDistanceSquared)
    {
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                ->GetClientObjectByObjectId(pPlayerObject->GetId());
            bVisible = 1;
            if (pObject->AsSWSTrigger()->m_bMineTrigger != 0)
            {
                if (pObject->AsSWSTrigger()->m_bTrapFlagged != 0)
                {
                    bVisible = 1;
                }
                else
                {
                    int nFaction = pObject->AsSWSTrigger()->m_nFaction;
                    if (nFaction == pPlayerObject->AsSWSCreature()->m_pStats->m_nFactionId ||
                        pObject->GetReputation(pPlayerObject->GetId(), 0) >=
                            CSWSObject::MIN_FRIENDLY_REPUTATION)
                    {
                        bVisible = 1;
                    }
                    else
                    {
                        unsigned long oidPlayer = pPlayerObject->GetId();
                        CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
                        if (pTrigger->m_lstTrapTargets.Contains(oidPlayer))
                        {
                            bVisible = 1;
                        }
                    }
                }
            }
            else if (pObject->AsSWSTrigger()->m_nTriggerType != 0 ||
                     pObject->AsSWSTrigger()->m_nCursor != 0)
            {
                bVisible = 1;
            }
        }
        else
        {
            bVisible = 1;
        }
    }
    return bVisible;
}
// PC 0x0056DF60  Xbox 0x00231180
void CSWSMessage::DeleteLastUpdateObjectsInOtherAreas(CSWSPlayer* pPlayer)
{
    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    if (pObjectArray == NULL)
    {
        return;
    }

    CExoLinkedListNode* pNode = pPlayer->m_pActiveObjectsLastUpdate->GetHeadPos();
    while (pNode != NULL)
    {
        CLastUpdateObject* pLastUpdateObject =
            pPlayer->m_pActiveObjectsLastUpdate->GetAtPos(pNode);

        if (pPlayerObject != NULL &&
            pLastUpdateObject->m_oidObject == pPlayerObject->m_nId)
        {
            pLastUpdateObject->m_nUnknown24 = 0;
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
            continue;
        }

        int bKeep = FALSE;
        CGameObject* pGameObject;
        if (pObjectArray->GetGameObject(pLastUpdateObject->m_oidObject,
                                        &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSObject() != NULL && pPlayerObject != NULL)
        {
            if (pGameObject->AsSWSObject()->GetArea() ==
                pPlayerObject->GetArea())
            {
                bKeep = TRUE;
            }
            else if (pGameObject->AsSWSObject()->GetArea() != NULL &&
                     pPlayerObject->GetArea() == NULL &&
                     pPlayerObject->AsSWSCreature() != NULL &&
                     pGameObject->AsSWSObject()->GetArea()->GetId() ==
                         pPlayerObject->AsSWSCreature()->m_oidDisappearTarget)
            {
                bKeep = TRUE;
            }
        }

        if (bKeep == FALSE)
        {
            WriteCHAR(0x44, 8);
            WriteBYTE(pLastUpdateObject->m_nObjectType, 8);
            WriteOBJECTIDServer(pLastUpdateObject->m_oidObject);
            if (pLastUpdateObject->m_nObjectType ==
                    CGameObject::OBJECT_TYPE_CREATURE ||
                pLastUpdateObject->m_nObjectType ==
                    CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                WriteBOOL(1);
                WriteBOOL(pLastUpdateObject->m_bDeleteNoFade);
                WriteDWORD(pLastUpdateObject->m_nFadeOutTime, 32);
                if (pLastUpdateObject->m_nObjectType ==
                    CGameObject::OBJECT_TYPE_CREATURE)
                {
                    WriteBOOL(pLastUpdateObject->m_bKeepCorpse);
                }
            }
            else if (pLastUpdateObject->m_nObjectType ==
                     CGameObject::OBJECT_TYPE_ITEM)
            {
                WriteBOOL(1);
            }
            else if (pLastUpdateObject->m_nObjectType ==
                     CGameObject::OBJECT_TYPE_TRIGGER)
            {
                WriteBYTE(pLastUpdateObject->m_nFadeOutTime, 8);
            }

            CExoLinkedListNode* pDeleteNode = pNode;
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
            pPlayer->m_pActiveObjectsLastUpdate->Remove(pDeleteNode);
            delete pLastUpdateObject;
        }
        else
        {
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
        }
    }
}
// PC 0x0056E190  Xbox 0x002313B0
void CSWSMessage::DeleteLastUpdateObjectsForObject(CSWSPlayer* pPlayer,
                                                  unsigned long oidObject)
{
    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    if (pObjectArray == NULL)
    {
        return;
    }

    CExoLinkedListNode* pNode = pPlayer->m_pActiveObjectsLastUpdate->GetHeadPos();
    while (pNode != NULL)
    {
        CLastUpdateObject* pLastUpdateObject =
            pPlayer->m_pActiveObjectsLastUpdate->GetAtPos(pNode);
        if (pLastUpdateObject->m_oidObject == oidObject &&
            pLastUpdateObject->m_oidObject != pPlayerObject->m_nId)
        {
            WriteCHAR(GAME_OBJECT_RECORD_DELETE, 8);
            WriteBYTE(pLastUpdateObject->m_nObjectType, 8);
            WriteOBJECTIDServer(pLastUpdateObject->m_oidObject);
            if (pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM ||
                pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE ||
                pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                int bDifferentArea = TRUE;
                CGameObject* pGameObject;
                if (pObjectArray->GetGameObject(pLastUpdateObject->m_oidObject,
                                                &pGameObject) ==
                        CGameObjectArray::SUCCESS &&
                    pGameObject->AsSWSObject() != NULL)
                {
                    if (pGameObject->AsSWSObject()->GetArea() ==
                        pPlayer->GetGameObject()->GetArea())
                    {
                        bDifferentArea = FALSE;
                    }
                }
                WriteBOOL(bDifferentArea);
                if (pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE ||
                    pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                {
                    WriteBOOL(pLastUpdateObject->m_bDeleteNoFade);
                    WriteDWORD(pLastUpdateObject->m_nFadeOutTime, 32);
                    if (pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                    {
                        WriteBOOL(pLastUpdateObject->m_bKeepCorpse);
                    }
                }
            }
            else if (pLastUpdateObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            {
                WriteBYTE(TRIGGER_DELETE_NORMAL, 8);
            }

            CExoLinkedListNode* pDeleteNode = pNode;
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
            pPlayer->m_pActiveObjectsLastUpdate->Remove(pDeleteNode);
            delete pLastUpdateObject;
            return;
        }
        pLastUpdateObject->m_nUnknown24 = 0;
        pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
    }
}
// PC 0x0056E360  Xbox 0x00231580
void CSWSMessage::WriteRepositoryUpdate(CSWSPlayer* pPlayer,
    CSWSObject* pObject, CItemRepository* pRepository,
    CSWSPlayerLUOInventory* pLastUpdateInventory, unsigned char nList, char nGUI)
{
    CExoLinkedListNode* pNode = NULL;
    CSWSPlayerLUOInventoryItem* pUpdateItem = NULL;
    int nItem = pRepository->GetNumItems() - 1;
    unsigned long oidItem;
    pLastUpdateInventory->ItemListSetEnd(pNode, nList);

    while (nItem >= 0 || pNode != NULL)
    {
        CSWSItem* pItem = NULL;
        if (nItem >= 0)
        {
            oidItem = pRepository->ItemListGetItemObjectID(nItem);
            pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
            EXOXBOXASSERT(pItem);
        }
        if (pNode != NULL)
        {
            pUpdateItem = pLastUpdateInventory->ItemListGetUpdateItem(pNode, nList);
        }

        if (nItem >= 0)
        {
            if (pNode != NULL)
            {
                if (oidItem != pUpdateItem->m_oidItem)
                {
                    WriteBYTE(GAME_OBJECT_RECORD_GUI_ELEMENTS, 8);
                    WriteBYTE(nGUI, 8);
                    WriteBYTE(GAME_OBJECT_RECORD_DELETE, 8);
                    WriteOBJECTIDServer(pUpdateItem->m_oidItem);
                    CExoLinkedListNode* pDeleteNode = pNode;
                    pLastUpdateInventory->ItemListGetPrev(pNode, nList);
                    pLastUpdateInventory->ItemListRemove(pDeleteNode, nList);
                }
                else if (pItem->m_nStackSize == pUpdateItem->m_nStackSize &&
                         pItem->GetCost() == pUpdateItem->m_nCost)
                {
                    --nItem;
                    pLastUpdateInventory->ItemListGetPrev(pNode, nList);
                }
                else
                {
                    WriteCHAR(GAME_OBJECT_RECORD_GUI_ELEMENTS, 8);
                    WriteCHAR(nGUI, 8);
                    WriteCHAR(GAME_OBJECT_RECORD_UPDATE, 8);
                    WriteOBJECTIDServer(pItem->m_nId);
                    WriteDWORD(pItem->m_nStackSize, 32);
                    WriteDWORD(pItem->GetCost(), 32);
                    pUpdateItem->m_nStackSize = pItem->m_nStackSize;
                    pUpdateItem->m_nCost = pItem->GetCost();
                    --nItem;
                    pLastUpdateInventory->ItemListGetPrev(pNode, nList);
                }
            }
            else
            {
                WriteCHAR(GAME_OBJECT_RECORD_GUI_ELEMENTS, 8);
                WriteCHAR(nGUI, 8);
                WriteCHAR(GAME_OBJECT_RECORD_ADD, 8);
                WriteOBJECTIDServer(oidItem);
                if (pItem->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
                {
                    AddItemAppearanceToMessage(pItem);
                    AddActiveItemPropertiesToMessage(pItem, pObject->AsSWSCreature());
                }
                unsigned long nStackSize = pItem->m_nStackSize;
                pLastUpdateInventory->ItemListAddHead(oidItem, nList,
                    nStackSize, pItem->GetCost());
                --nItem;
            }
        }
        else
        {
            WriteCHAR(GAME_OBJECT_RECORD_GUI_ELEMENTS, 8);
            WriteCHAR(nGUI, 8);
            WriteCHAR(GAME_OBJECT_RECORD_DELETE, 8);
            WriteOBJECTIDServer(pUpdateItem->m_oidItem);
            CExoLinkedListNode* pDeleteNode = pNode;
            pLastUpdateInventory->ItemListGetPrev(pNode, nList);
            pLastUpdateInventory->ItemListRemove(pDeleteNode, nList);
        }
    }
}
// PC 0x00567090  Xbox 0x0022A0C0
void CSWSMessage::AddItemAppearanceToMessage(CSWSItem* pItem)
{
    int nBaseItem;
    CSWBaseItem* pBaseItem;

    nBaseItem = pItem->m_nBaseItem;
    WriteDWORD(nBaseItem, 32);
    pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
#ifdef _XBOX
    if (pBaseItem == NULL)
    {
        EXOXBOXASSERT(0);
    }
    else
#else
    if (pBaseItem != NULL)
#endif
    {
        WriteBYTE(pItem->m_nModelVariation, 8);
        if (pBaseItem->m_bUnknown18 == 1)
        {
            WriteBYTE(pItem->m_nBodyVariation, 8);
            WriteBYTE(pItem->m_nTextureVar, 8);
        }
    }
}
// PC 0x00567100  Xbox 0x0022A140
unsigned long CSWSMessage::ComputeAppearanceUpdateRequired(
    CSWSObject* pObject, CLastUpdateObject* pLastUpdateObject)
{
    unsigned long nAppearanceMask = 0;
    if (pObject->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
    {
        return 0;
    }
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    pCreature->UpdateAppearanceForEquippedItems();
    if (pLastUpdateObject->m_nAppearance != pCreature->m_cAppearanceInfo.m_nAppearance)
    {
        nAppearanceMask = APPEARANCE_UPDATE_TYPE;
    }
    if (pLastUpdateObject->m_nAppearancePart0 != pCreature->m_cAppearanceInfo.m_nAppearancePart0)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART0;
    }
    if (pLastUpdateObject->m_nAppearancePart1 != pCreature->m_cAppearanceInfo.m_nAppearancePart1)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART1;
    }
    if (pLastUpdateObject->m_nAppearancePart2 != pCreature->m_cAppearanceInfo.m_nAppearancePart2)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART2;
    }
    if (pLastUpdateObject->m_nAppearancePart3 != pCreature->m_cAppearanceInfo.m_nAppearancePart3)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART3;
    }
    if (pLastUpdateObject->m_nAppearancePart4 != pCreature->m_cAppearanceInfo.m_nAppearancePart4)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART4;
    }
    if (pLastUpdateObject->m_nAppearancePart5 != pCreature->m_cAppearanceInfo.m_nAppearancePart5)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART5;
    }
    if (pLastUpdateObject->m_nAppearancePart6 != pCreature->m_cAppearanceInfo.m_nAppearancePart6)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART6;
    }
    if (pLastUpdateObject->m_nAppearanceState != pCreature->m_cAppearanceInfo.m_bUseBackupHead)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_STATE;
    }
    if (pLastUpdateObject->m_nAppearanceVariant != pCreature->m_cAppearanceInfo.m_nAppearanceHead)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_VARIANT;
    }
    if (pLastUpdateObject->m_nAppearancePart7 != pCreature->m_cAppearanceInfo.m_nAppearancePart7)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART7;
    }
    if (pLastUpdateObject->m_nAppearancePart8 != pCreature->m_cAppearanceInfo.m_nAppearancePart8)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_PART8;
    }
    if (pLastUpdateObject->m_oidAppearanceLeftHand != pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand ||
        pLastUpdateObject->m_oidAppearanceRightHand != pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand ||
        pLastUpdateObject->m_oidAppearanceArmour != pCreature->m_cAppearanceInfo.m_oidAppearanceArmour ||
        pLastUpdateObject->m_oidAppearanceHead != pCreature->m_cAppearanceInfo.m_oidAppearanceHead)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_EQUIPMENT;
    }
    if (pLastUpdateObject->m_nUnknown2A != pCreature->m_nSoundSetFile)
    {
        nAppearanceMask |= APPEARANCE_UPDATE_FLAGS;
    }
    return nAppearanceMask;
}
// PC 0x00567240  Xbox 0x0022A280
int CSWSMessage::ComputeInventoryUpdateRequired(CSWSPlayer* pPlayer,
                                                unsigned long nSlot)
{
    CSWSObject* pGameObject;
    CSWInventory* pInventory;
    CSWSItem* pItem;

    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
    {
        pGameObject = pPlayer->GetGameObject();
        if (pGameObject->AsSWSCreature() != NULL &&
            pGameObject->AsSWSCreature()->m_pInventory != NULL)
        {
            pInventory = pGameObject->AsSWSCreature()->m_pInventory;
            pItem = pInventory->GetItemInSlot(1 << nSlot);
            if (pItem != NULL)
            {
                if (pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                        ->m_oidSlot[nSlot] != pItem->m_nId)
                {
                    return 1;
                }
                return pItem->m_nStackSize !=
                       pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                           ->m_nSlotIndex[nSlot];
            }
            if (pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                    ->m_oidSlot[nSlot] != CGameObjectArray::INVALIDOBJID)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x005672F0  Xbox 0x0022A330
int CSWSMessage::CompareVisualEffectLists(CExoArrayList<CLoopingVisualEffect*>* pFirst,
                                         CExoArrayList<CLoopingVisualEffect*>* pSecond)
{
    int bIsBeam;

    for (int i = 0; i < pFirst->num; i++)
    {
        CLoopingVisualEffect* pEffect = pFirst->element[i];
        unsigned short nVisualEffectId = pEffect->m_nVisualEffectId;
        int bFound = 0;
        bIsBeam = pEffect->GetIsBeam();
        for (int j = 0; j < pSecond->num && !bFound; j++)
        {
            if (pSecond->element[j]->m_nVisualEffectId == nVisualEffectId &&
                (!bIsBeam || pSecond->element[j]->m_oidCreator == pEffect->m_oidCreator))
            {
                bFound = 1;
            }
        }
        if (!bFound)
        {
            return 0;
        }
    }
    for (int i = 0; i < pSecond->num; i++)
    {
        CLoopingVisualEffect* pEffect = pSecond->element[i];
        unsigned short nVisualEffectId = pEffect->m_nVisualEffectId;
        int bFound = 0;
        bIsBeam = pEffect->GetIsBeam();
        for (int j = 0; j < pFirst->num && !bFound; j++)
        {
            if (pFirst->element[j]->m_nVisualEffectId == nVisualEffectId &&
                (!bIsBeam || pFirst->element[j]->m_oidCreator == pEffect->m_oidCreator))
            {
                bFound = 1;
            }
        }
        if (!bFound)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x00567430  Xbox 0x0022A470
unsigned short CSWSMessage::ComputeLastUpdate_GuiSkills(CSWSCreature* pCreature)
{
    CSWSCreatureStats* pStats;
    short nSkills;
    int nResult;

    pStats = pCreature->m_pStats;
    nSkills = 0;
    if (pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS) != 0)
    {
        nSkills = 2;
    }
    if (pStats->GetCanUseSkill(CSWRules::SKILL_STEALTH) != 0)
    {
        nSkills |= 0x10;
    }
    if (pStats->GetCanUseSkill(CSWRules::SKILL_STEALTH) != 0)
    {
        nSkills |= 0x10;
    }
    nResult = pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY);
    if (nResult != 0)
    {
        nSkills |= 0x20;
    }
    return nSkills;
}
// PC 0x00567490  Xbox 0x0022A4D0
int CSWSMessage::ComputeLastUpdate_GuiKnownSpellUses(
    CSWSCreature* pCreature,
    CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    for (int nClass = 0; nClass < pCreature->m_pStats->m_nClassCount; nClass++)
    {
        if (CSWClass::IsJedi(int(pCreature->m_pStats->GetClass(nClass))))
        {
            for (int nSpellLevel = 0; nSpellLevel < 1; nSpellLevel++)
            {
                unsigned char nUsesLeft =
                    pLastUpdateObject->GetKnownSpellUsesLeft(nClass, nSpellLevel);
                if (nUsesLeft != pCreature->m_pStats->GetSpellsPerDayLeft(
                                     nClass, nSpellLevel))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
// PC 0x00567520  Xbox 0x0022A560
void CSWSMessage::UpdateLastUpdateActionQueue(
    CSWSCreature* pCreature,
    CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    int i;
    CSWActionNode* pNode = pCreature->m_aActionQueue;

    for (i = 0; i < 10; i++)
    {
        pLastUpdateObject->m_aActions[i].m_nGroupActionId =
            pNode[i].m_nGroupActionId;
        pLastUpdateObject->m_aActions[i].m_nActionId =
            pNode[i].m_nActionId;
        pLastUpdateObject->m_aActions[i].m_nActionType =
            pNode[i].m_nActionType;
        pLastUpdateObject->m_aActions[i].m_oidTarget =
            pNode[i].m_oidTarget;
        pLastUpdateObject->m_aActions[i].m_vLocation =
            pNode[i].m_vLocation;
        pLastUpdateObject->m_aActions[i].m_nParameter =
            pNode[i].m_nParameter;
    }
}
// PC 0x00567590  Xbox 0x0022A5D0
int CSWSMessage::ComputeLastUpdate_ActionQueue(
    CSWSCreature* pCreature,
    CSWSPlayerLastUpdateObject* pLastUpdateObject)
{
    int i;
    CSWActionNode* pNode = pCreature->m_aActionQueue;

    for (i = 0; i < 10; i++)
    {
        if (pNode[i].m_nGroupActionId !=
                pLastUpdateObject->m_aActions[i].m_nGroupActionId ||
            pNode[i].m_nActionId !=
                pLastUpdateObject->m_aActions[i].m_nActionId ||
            pNode[i].m_nActionType !=
                pLastUpdateObject->m_aActions[i].m_nActionType ||
            pNode[i].m_oidTarget !=
                pLastUpdateObject->m_aActions[i].m_oidTarget ||
            equal(pNode[i].m_vLocation,
                pLastUpdateObject->m_aActions[i].m_vLocation) == 0)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00567600  Xbox 0x0022A640
unsigned short CSWSMessage::ComputeLastUpdate_PlayerState(
    CSWSCreature* pCreature)
{
    short nState;

    nState = 0;
    if (pCreature->GetDetectMode() == 1)
    {
        nState = PLAYER_STATE_DETECT;
    }
    if (pCreature->m_nStealthMode == 1)
    {
        nState |= PLAYER_STATE_STEALTH;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_PARRY)
    {
        nState |= PLAYER_STATE_ASSOCIATE_1;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK)
    {
        nState |= PLAYER_STATE_ASSOCIATE_2;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK)
    {
        nState |= PLAYER_STATE_ASSOCIATE_3;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
    {
        nState |= PLAYER_STATE_ASSOCIATE_5;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
    {
        nState |= PLAYER_STATE_ASSOCIATE_6;
    }
    if (pCreature->m_nUnknown8E8 == 1)
    {
        nState |= PLAYER_STATE_CREATURE_8E8;
    }
    if (pCreature->m_nSpecialWalkAnimation != 0)
    {
        nState |= PLAYER_STATE_SPECIAL_WALK;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL)
    {
        nState |= PLAYER_STATE_ASSOCIATE_4;
    }
    if (pCreature->m_bPMIsDisguised != 0)
    {
        nState |= PLAYER_STATE_DISGUISED;
    }
    if (pCreature->m_bCommandable != 0)
    {
        nState |= PLAYER_STATE_CREATURE_E8;
    }
    if (pCreature->m_nExcitedStateTimer > 0)
    {
        nState |= PLAYER_STATE_CREATURE_384;
    }
    return nState;
}
// PC 0x005676B0  Xbox 0x0022A6F0
unsigned char CSWSMessage::ComputeLastUpdate_AssociateState(
    CSWSCreature* pCreature)
{
    unsigned char nState;

    nState = 0;
    if (pCreature->GetDetectMode() == 1)
    {
        nState = 1;
    }
    if (pCreature->m_nStealthMode == 1)
    {
        nState |= 2;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_PARRY)
    {
        nState |= 4;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK)
    {
        nState |= 8;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK)
    {
        nState |= 0x10;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
    {
        nState |= 0x20;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
    {
        nState |= 0x40;
    }
    if (pCreature->m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL)
    {
        nState |= 0x80;
    }
    return nState;
}
// PC 0x00567710  Xbox 0x0022A750
void CSWSMessage::WriteGameObjUpdate_WriteInventorySlotAdd(
    CSWSCreature* pCreature, CSWSItem* pItem, unsigned long nSlot)
{
    WriteCHAR(0x41, 8);
    WriteOBJECTIDServer(pItem->GetId());
    WriteDWORD(nSlot, 32);
    if (nSlot == 16 || nSlot == 32)
    {
        WriteBOOL(pCreature->GetLastInstantEquip(nSlot));
    }
    AddItemAppearanceToMessage(pItem);
    AddActiveItemPropertiesToMessage(pItem, pCreature);
}
// PC 0x00567780  Xbox 0x0022A7C0
int CSWSMessage::SelectCategoryForGameObject(CGameObject* pGameObject,
    CSWSObject* pPlayerObject)
{
    int nCategory = GAME_OBJECT_CATEGORY_NONE;

    if (pGameObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pPlayerObject->m_nId == pGameObject->m_nId)
        {
            nCategory = GAME_OBJECT_CATEGORY_PLAYER;
        }
        else if (pCreature->m_bCombatState == 1)
        {
            nCategory = GAME_OBJECT_CATEGORY_CREATURE_COMBAT;
        }
        else
        {
            nCategory = GAME_OBJECT_CATEGORY_CREATURE;
        }
    }
    else if (pGameObject->AsSWSDoor() != NULL)
    {
        nCategory = GAME_OBJECT_CATEGORY_DOOR;
    }
    else if (pGameObject->AsSWSPlaceable() != NULL)
    {
        CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
        if (pPlaceable->m_bStatic == 0)
        {
            nCategory = GAME_OBJECT_CATEGORY_PLACEABLE;
        }
    }
    else if (pGameObject->AsSWSItem() != NULL)
    {
        nCategory = GAME_OBJECT_CATEGORY_ITEM;
    }
    else if (pGameObject->AsSWSAreaOfEffectObject() != NULL)
    {
        nCategory = GAME_OBJECT_CATEGORY_AREA_OF_EFFECT;
    }
    else if (pGameObject->AsSWSTrigger() != NULL)
    {
        nCategory = GAME_OBJECT_CATEGORY_TRIGGER;
    }

    return nCategory;
}

static CSWSPlayerLUOSortedObjectList* AllocateSortedObjectLists(int nCount)
{
    return new CSWSPlayerLUOSortedObjectList[nCount];
}
// PC 0x00567850  Xbox 0x0022A890
CSWSPlayerLUOSortedObjectList* CSWSMessage::SortObjectsForGameObjectUpdate(
    CSWSPlayer* pPlayer, CSWSObject* pPlayerObject,
    CGameObjectArray* pObjectArray, int* pnObjectCount)
{
    int nObjectCount = 0;
    int nAllocated = 0;
    CSWSPlayerLUOSortedObjectList* pObjects = NULL;
    CGameObject* pObject = NULL;
    CSWSArea* pArea = NULL;
    Vector vPlayerPosition;
    if (pPlayerObject != NULL)
    {
        pArea = pPlayerObject->GetArea();
        vPlayerPosition = pPlayerObject->m_vPosition;
    }
    if (pArea == NULL)
    {
        CSWSCreature* pCreature = pPlayerObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
            if (pModule != NULL)
            {
                pArea = pModule->GetArea();
                vPlayerPosition = pCreature->m_vDisappearPosition;
            }
        }
    }
    if (pArea != NULL)
    {
        unsigned long oidObject;
        if (pArea->GetFirstObjectInArea(oidObject) == 1)
        {
            do
            {
                if (pObjectArray->GetGameObject(oidObject, &pObject) ==
                        CGameObjectArray::SUCCESS && pObject != NULL &&
                    pObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
                {
                    CSWSObject* pServerObject = (CSWSObject*)pObject;
                    float fY = pServerObject->m_vPosition.m_fY - vPlayerPosition.m_fY;
                    float fX = pServerObject->m_vPosition.m_fX - vPlayerPosition.m_fX;
                    float fDistanceSquared = fX * fX + fY * fY;
                    int bInclude = 0;
                    float fVisibleDistance = DEFAULT_OBJECT_VISIBLE_DISTANCE;
                    if (pPlayerObject->AsSWSCreature() != NULL)
                    {
                        fVisibleDistance =
                            pPlayerObject->AsSWSCreature()->GetSpotCheckDistance() >=
                            pPlayerObject->AsSWSCreature()->GetListenCheckDistance() ?
                            pPlayerObject->AsSWSCreature()->GetSpotCheckDistance() :
                            pPlayerObject->AsSWSCreature()->GetListenCheckDistance();
                    }
                    if (fDistanceSquared < fVisibleDistance * fVisibleDistance ||
                        pPlayer->GetLastUpdateObject(pPlayerObject->GetId()) != NULL)
                        bInclude = 1;
                    unsigned long nCategory = SelectCategoryForGameObject(pObject, pPlayerObject);
                    if (bInclude == 1 && nCategory != GAME_OBJECT_CATEGORY_NONE)
                    {
                        if (nAllocated == nObjectCount)
                        {
                            CSWSPlayerLUOSortedObjectList* pNewObjects = new CSWSPlayerLUOSortedObjectList[nAllocated + CSWSPlayerLUOSortedObjectList::ALLOCATION_INCREMENT];
                            if (nObjectCount > 0)
                            {
                                for (int nCopy = 0; nCopy < nObjectCount; ++nCopy)
                                {
                                    pNewObjects[nCopy].m_fDistance = pObjects[nCopy].m_fDistance;
                                    pNewObjects[nCopy].m_nCategory = pObjects[nCopy].m_nCategory;
                                    pNewObjects[nCopy].m_oidObject = pObjects[nCopy].m_oidObject;
                                }
                                delete[] pObjects;
                            }
                            pObjects = pNewObjects;
                            nAllocated += CSWSPlayerLUOSortedObjectList::ALLOCATION_INCREMENT;
                        }
                        int nInsert;
                        for (nInsert = nObjectCount - 1; nInsert >= 0; --nInsert)
                        {
                            if (nCategory < pObjects[nInsert].m_nCategory ||
                                (nCategory == pObjects[nInsert].m_nCategory &&
                                 fDistanceSquared < pObjects[nInsert].m_fDistance *
                                                    pObjects[nInsert].m_fDistance))
                            {
                                pObjects[nInsert + 1].m_fDistance = pObjects[nInsert].m_fDistance;
                                pObjects[nInsert + 1].m_nCategory = pObjects[nInsert].m_nCategory;
                                pObjects[nInsert + 1].m_oidObject = pObjects[nInsert].m_oidObject;
                            }
                            else
                                break;
                        }
                        ++nInsert;
                        pObjects[nInsert].m_fDistance = sqrt(fDistanceSquared);
                        pObjects[nInsert].m_nCategory = nCategory;
                        pObjects[nInsert].m_oidObject = oidObject;
                        ++nObjectCount;
                    }
                }
            } while (pArea->GetNextObjectInArea(oidObject) == 1);
        }
    }
    *pnObjectCount = nObjectCount;
    return pObjects;
}
// PC 0x0056E5F0  Xbox 0x00231810
int CSWSMessage::SendServerToPlayerSafeProjectileProjectile(
    CSWSPlayer* pPlayer, unsigned long oidSource, unsigned long oidTarget,
    Vector vStart, Vector vEnd, unsigned long nProjectileType,
    unsigned char nProjectileFlags, int nDelay, unsigned char nSpellType,
    unsigned char nSpellLevel, int nSpellId)
{
    int nResult = 0;
    unsigned long nPlayerId;
    unsigned char* pMessage = 0;
    unsigned long nMessageSize = 0;

    CreateWriteMessage(47, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSource);
    WriteOBJECTIDServer(oidTarget);
    WriteVector(vStart);
    WriteVector(vEnd);
    WriteDWORD(nProjectileType, 32);
    WriteBYTE(nProjectileFlags, 8);
    WriteINT(nDelay, 32);
    WriteBYTE(nSpellType, 8);
    WriteBYTE(nSpellLevel, 8);
    WriteINT(nSpellId, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) != 0)
    {
        nPlayerId = pPlayer->m_nPlayerId;
        nResult =
            SendServerToPlayerMessage(nPlayerId, MESSAGE_SAFE_PROJECTILE, 2, pMessage, nMessageSize);
    }
    return nResult;
}
// PC 0x0056E6E0  Xbox 0x00231900
int CSWSMessage::SendServerToPlayerSafeProjectileSpell(
    CSWSPlayer* pPlayer, unsigned long oidSource, unsigned long oidTarget,
    Vector vStart, Vector vEnd, unsigned long nProjectileType,
    unsigned char nSpellType, unsigned long nSpellId, unsigned char nPathType,
    unsigned char nSpellLevel, unsigned char nCasterLevel, unsigned long nDelay,
    int nVisualEffect)
{
    int nResult;
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(49, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidSource);
    WriteOBJECTIDServer(oidTarget);
    WriteFLOAT(vStart.m_fX, 1.0f, 32);
    WriteFLOAT(vStart.m_fY, 1.0f, 32);
    WriteFLOAT(vStart.m_fZ, 1.0f, 32);
    WriteFLOAT(vEnd.m_fX, 1.0f, 32);
    WriteFLOAT(vEnd.m_fY, 1.0f, 32);
    WriteFLOAT(vEnd.m_fZ, 1.0f, 32);
    WriteDWORD(nProjectileType, 32);
    WriteBYTE(nSpellType, 8);
    WriteDWORD(nDelay, 32);
    WriteINT(nVisualEffect, 32);
    if (nSpellType == 6)
    {
        WriteDWORD(nSpellId, 32);
    }
    else if (nSpellType == 7)
    {
        WriteDWORD(nSpellId, 32);
        WriteBYTE(nCasterLevel, 8);
    }
    else
    {
        WriteBYTE(nCasterLevel, 8);
        WriteBYTE(nPathType, 8);
        WriteBYTE(nSpellLevel, 8);
    }
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_SAFE_PROJECTILE, 1, pMessage, nMessageSize);
}
// PC 0x0056E850  Xbox 0x00231A70
void CSWSMessage::TestPartyObjectUpdateDifferences(CSWSPlayer* pPlayer,
    CSWSCreature* pCreature, CLastUpdatePartyObject** ppLastUpdateObject,
    unsigned long* pnUpdateMask)
{
    CSWSCreature* pPlayerCreature = pPlayer->GetGameObject()->AsSWSCreature();
    CExoLinkedListNode* pNode = pPlayer->m_pPartyObjectsLastUpdate->GetHeadPos();
    *pnUpdateMask = 0;
    while (pNode != NULL)
    {
        *ppLastUpdateObject =
            pPlayer->m_pPartyObjectsLastUpdate->GetAtPos(pNode);
        if ((*ppLastUpdateObject)->m_oidObject == pCreature->GetId())
        {
            (*ppLastUpdateObject)->m_bUpdated = TRUE;
            *pnUpdateMask = 0;
            pCreature->ComputeAIState();
            unsigned char nAIStateReputation;
            if (pPlayerCreature != NULL)
            {
                nAIStateReputation =
                    pPlayerCreature->GetAIStateReputation(pCreature->GetId());
            }
            else
            {
                nAIStateReputation = 2;
            }

            if ((*ppLastUpdateObject)->m_nAIStateFlags != pCreature->m_nAIStateFlags ||
                (*ppLastUpdateObject)->m_nAIState != pCreature->m_nAIState ||
                (*ppLastUpdateObject)->m_nActivity != (unsigned short)pCreature->m_nActivity ||
                (*ppLastUpdateObject)->m_oidAITarget != pCreature->m_oidAITarget ||
                (*ppLastUpdateObject)->m_nAIStateReputation != nAIStateReputation ||
                (*ppLastUpdateObject)->m_bCombatState != pCreature->m_bCombatState ||
                (*ppLastUpdateObject)->m_nStealthMode != pCreature->m_nStealthMode)
            {
                *pnUpdateMask |= OBJECT_UPDATE_AI_STATE;
            }

            if (pCreature->m_oidArea != (*ppLastUpdateObject)->m_oidArea)
            {
                *pnUpdateMask |= OBJECT_UPDATE_AREA | OBJECT_UPDATE_POSITION;
            }

            if (pCreature->m_oidArea == pPlayerCreature->m_oidArea)
            {

                int nTileDeltaX = 1;
                int nTileDeltaY = 0;
                if (abs(nTileDeltaX) + abs(nTileDeltaY) <= 3)
                {
                    Vector vDelta = (*ppLastUpdateObject)->m_vPosition -
                                    pCreature->m_vPosition;
                    if (vDelta * vDelta >= 4.0f)
                    {
                        *pnUpdateMask |= OBJECT_UPDATE_POSITION;
                    }
                }
            }

            if ((*ppLastUpdateObject)->m_nCurrentHitPoints != pCreature->GetCurrentHitPoints(TRUE) ||
                (*ppLastUpdateObject)->m_nBaseMaxHitPoints != (short)pCreature->m_nMaxHitPoints ||
                (*ppLastUpdateObject)->m_nTemporaryHitPoints != (short)pCreature->m_nTemporaryHitPoints ||
                (*ppLastUpdateObject)->m_nMaxHitPoints != pCreature->GetMaxHitPoints(TRUE))
            {
                *pnUpdateMask |= OBJECT_UPDATE_HIT_POINTS;
            }

            if ((*ppLastUpdateObject)->m_nCurrentForcePoints != pCreature->m_pStats->m_nCurrentForcePoints ||
                (*ppLastUpdateObject)->m_nForcePoints != pCreature->m_pStats->m_nForcePoints ||
                (*ppLastUpdateObject)->m_nTemporaryForcePoints != pCreature->m_pStats->m_nTemporaryForcePoints ||
                (*ppLastUpdateObject)->m_nMaxForcePoints != pCreature->GetMaxForcePoints())
            {
                *pnUpdateMask |= OBJECT_UPDATE_FORCE_POINTS;
            }

            if (pCreature->AsSWSCreature()->m_nAreaWaitState != (*ppLastUpdateObject)->m_nAreaWaitState ||
                ((pCreature->AsSWSCreature()->m_nAIStateFlags & CSWSCreature::AI_STATE_FLAG_0040) ==
                    CSWSCreature::AI_STATE_FLAG_0040) != (*ppLastUpdateObject)->m_bAIStateFlag0040 ||
                (pCreature->AsSWSCreature()->GetFaction()->GetLeader() == pCreature->GetId() ? TRUE : FALSE) !=
                    (*ppLastUpdateObject)->m_bPartyLeader ||
                pCreature->GetFaction()->GetSingletonParty() != (*ppLastUpdateObject)->m_bSingletonParty ||
                pCreature->AsSWSCreature()->GetInvitedToParty() != (*ppLastUpdateObject)->m_bInvitedToParty ||
                pCreature->m_nPoisonState != (*ppLastUpdateObject)->m_nPoisonState ||
                pCreature->m_bDiseased != (*ppLastUpdateObject)->m_bDiseased)
            {
                *pnUpdateMask |= OBJECT_UPDATE_PARTY_STATE;
            }

            if ((*ppLastUpdateObject)->m_nPortraitId != pCreature->GetPortraitId() ||
                (*ppLastUpdateObject)->m_cPortrait != pCreature->GetPortrait())
            {
                *pnUpdateMask |= OBJECT_UPDATE_PORTRAIT;
            }
            return;
        }

        pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
    }

    *ppLastUpdateObject = new CLastUpdatePartyObject;
    (*ppLastUpdateObject)->m_bUpdated = TRUE;
    (*ppLastUpdateObject)->m_oidObject = pCreature->m_nId;
    *pnUpdateMask = OBJECT_UPDATE_NEW_PARTY_OBJECT;
    pPlayer->m_pPartyObjectsLastUpdate->AddHead(*ppLastUpdateObject);
}
// PC 0x0056EBE0  Xbox 0x00231E20
int CSWSMessage::SendServerToPlayerExamineGui_CreatureData(
    CSWSPlayer* pPlayer, unsigned long oidObject)
{
    unsigned long nPlayerId = pPlayer->m_nPlayerId;
    g_pAppManager->m_pServerExoApp->GetNetLayer()->GetPlayerInfo(nPlayerId);
    CGameObject* pGameObject = NULL;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CExoLocString lsDescription;
    if (pObjectArray->GetGameObject(oidObject, &pGameObject) !=
        CGameObjectArray::SUCCESS)
        return 0;
    CSWSCreature* pCreature = pGameObject->AsSWSCreature();
    if (pCreature == NULL)
        return 0;

    lsDescription = pCreature->GetStats()->m_lsDescription;
    unsigned long nMessageSize = 8;
    pPlayer->GetLastUpdateObject(oidObject);
    CreateWriteMessage(nMessageSize, nPlayerId, 1);
    WriteOBJECTIDServer(pCreature->GetId());
    WriteCExoLocStringServer(lsDescription, 0);
    WriteBOOL(1);
    WriteCExoLocStringServer(*pCreature->GetFirstName(), 0);
    WriteCExoLocStringServer(pCreature->GetLastName(), 0);
    unsigned short nPortraitId = pCreature->GetPortraitId();
    WriteWORD(nPortraitId, 16);
    if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
        WriteCResRef(pCreature->GetStats()->m_cPortrait, 16);
    WriteBYTE(pCreature->GetDamageLevel(), 8);
    WriteBYTE(pPlayer->GetGameObject()->AsSWSCreature()->
        GetAIStateReputation(pCreature->GetId()), 8);
    CSWSCreatureStats* pStats = pCreature->GetStats();
    float fChallengeRating;
    if (pStats->GetIsPC())
        fChallengeRating = (float)pStats->GetLevel(0);
    else
        fChallengeRating = pStats->m_fChallengeRating;
    WriteFLOAT(fChallengeRating, 1.0f, 32);
    unsigned char* pMessage;
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
        return 0;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_EXAMINE_GUI,
        EXAMINE_GUI_CREATURE_DATA, pMessage, nMessageSize);
}
// PC 0x0056EE40  Xbox 0x002320A0
int CSWSMessage::SendServerToPlayerExamineGui_ItemData(
    CSWSPlayer* pPlayer, unsigned long oidItem)
{
    unsigned long nPlayerId;
    CSWSItem* pItem;
    int nResult;
    unsigned char* pMessage;
    unsigned long nMessageSize;

    nPlayerId = pPlayer->m_nPlayerId;
    CreateWriteMessage(0x80, nPlayerId, 1);
    pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        WriteOBJECTIDServer(pItem->GetId());

        int bIdentified = TRUE;
        WriteBOOL(bIdentified);
        if (pCreature != NULL)
        {
            WriteBOOL(pCreature->CanUseItem(pItem, 0));
        }
        else
        {
            WriteBOOL(0);
        }

        CExoLocString* pDescription =
            bIdentified ? &pItem->m_sDescIdentified : &pItem->m_sDescription;
        CExoLocString lsDescription = *pDescription;
        if (HasValidString(lsDescription, 0))
        {
            WriteCExoLocStringServer(lsDescription, 0);
        }
        else if (bIdentified)
        {
            pDescription = &pItem->m_sDescription;
            lsDescription = *pDescription;
            WriteCExoLocStringServer(lsDescription, 0);
        }
        else
        {
            WriteCExoLocStringServer(CExoLocString(), 0);
        }

        int bMeetsLevelRequirement = 0;
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            if (pStats->GetLevel(0) >= pItem->GetMinEquipLevel())
            {
                bMeetsLevelRequirement = 1;
            }
        }
        WriteBOOL(bMeetsLevelRequirement);
        if (!bMeetsLevelRequirement)
        {
            WriteBYTE(pItem->GetMinEquipLevel(), 8);
        }
        int nCharges = pItem->m_nCharges;
        WriteINT(nCharges, 32);
        WriteINT(pItem->GetWeight(), 32);

        int nActiveProperties = pItem->m_nActiveProperties;
        int nPassiveProperties = pItem->m_nPassiveProperties;
        int i;
        CSWItemProperty* pProperty;
        for (i = 0; i < nPassiveProperties; i++)
        {
            pProperty = pItem->GetPassiveProperty(i);
        }
        WriteWORD(nActiveProperties + nPassiveProperties, 16);
        for (i = 0; i < nActiveProperties; i++)
        {
            pProperty = pItem->GetActiveProperty(i);
            unsigned short nPropertyType = pProperty->m_nPropertyType;
            WriteWORD(nPropertyType, 16);
            unsigned short nSubtype = pProperty->m_nSubtype;
            WriteWORD(nSubtype, 16);
            unsigned short nCostValue = pProperty->m_nCostValue;
            WriteWORD(nCostValue, 16);
            unsigned char nParam1Value = pProperty->m_nParam1Value;
            WriteBYTE(nParam1Value, 8);
        }
        for (i = 0; i < nPassiveProperties; i++)
        {
            pProperty = pItem->GetPassiveProperty(i);
        }
    }
    else
    {
        WriteOBJECTIDServer(CGameObjectArray::INVALIDOBJID);
    }
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_EXAMINE_GUI, EXAMINE_GUI_ITEM_DATA, pMessage,
                                     nMessageSize);
}
// PC 0x0056F0A0  Xbox 0x00232310
int CSWSMessage::SendServerToPlayerExamineGui_PlaceableData(
    CSWSPlayer* pPlayer, unsigned long oidObject)
{
    unsigned long nPlayerId;
    CGameObjectArray* pObjectArray;
    CGameObject* pGameObject;
    CSWSPlaceable* pPlaceable;
    int nResult;
    unsigned char* pMessage;
    unsigned long nMessageSize;

    nPlayerId = pPlayer->m_nPlayerId;
    CreateWriteMessage(0x80, nPlayerId, 1);
    pGameObject = NULL;
    pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
    if (pObjectArray->GetGameObject(oidObject, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        pPlaceable = pGameObject->AsSWSPlaceable();
        if (pPlaceable != NULL)
        {
            WriteOBJECTIDServer(pPlaceable->m_nId);
            WriteCExoLocStringServer(pPlaceable->m_lsDescription, 0);
        }
    }
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 0x1B, 4, pMessage,
                                     nMessageSize);
}
// PC 0x0056F160  Xbox 0x002323D0
int CSWSMessage::SendServerToPlayerExamineGui_MineData(
    CSWSPlayer* pPlayer, unsigned long oidObject, CSWSCreature* pCreature,
    int bRecoverable)
{
    unsigned long nPlayerId;
    CGameObject* pGameObject;
    int nSkillRank;
    int nDisarmDC;
    unsigned char* pMessage;
    unsigned long nMessageSize;

    nPlayerId = pPlayer->m_nPlayerId;
    pGameObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidObject);
    if (pGameObject == NULL)
        return 0;
    nSkillRank = pCreature->m_pStats->GetSkillRank(
        CSWRules::SKILL_DEMOLITIONS, NULL, 0) + 20;
    g_pAppManager->m_pServerExoApp->GetNetLayer()->GetPlayerInfo(nPlayerId);
    CreateWriteMessage(10, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteBOOL(bRecoverable);
    if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        nDisarmDC = pGameObject->AsSWSTrigger()->m_nTrapDisarmDC;
        CSWSTrigger* pTrigger = pGameObject->AsSWSTrigger();
        WriteBYTE(pTrigger->m_nTrapType, 8);
        if (pGameObject->AsSWSTrigger()->m_bTrapDisarmable &&
            nDisarmDC <= CSWRules::MAX_TRAP_DISARM_DC)
        {
            if (nDisarmDC <= nSkillRank - 15)
                WriteBYTE(EXAMINE_MINE_EASY, 8);
            else if (nDisarmDC <= nSkillRank - 10)
                WriteBYTE(EXAMINE_MINE_MODERATE, 8);
            else if (nDisarmDC <= nSkillRank - 5)
                WriteBYTE(EXAMINE_MINE_DIFFICULT, 8);
            else if (nDisarmDC <= nSkillRank)
                WriteBYTE(EXAMINE_MINE_VERY_DIFFICULT, 8);
            else
                WriteBYTE(EXAMINE_MINE_IMPOSSIBLE, 8);
        }
        else
            WriteBYTE(EXAMINE_MINE_IMPOSSIBLE, 8);
    }
    else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        nDisarmDC = pGameObject->AsSWSDoor()->m_nDisarmDC;
        CSWSDoor* pDoor = pGameObject->AsSWSDoor();
        WriteBYTE(pDoor->m_nTrapType, 8);
        if (pGameObject->AsSWSDoor()->m_bTrapDisarmable &&
            nDisarmDC <= CSWRules::MAX_TRAP_DISARM_DC)
        {
            if (nDisarmDC <= nSkillRank - 15)
                WriteBYTE(EXAMINE_MINE_EASY, 8);
            else if (nDisarmDC <= nSkillRank - 10)
                WriteBYTE(EXAMINE_MINE_MODERATE, 8);
            else if (nDisarmDC <= nSkillRank - 5)
                WriteBYTE(EXAMINE_MINE_DIFFICULT, 8);
            else if (nDisarmDC <= nSkillRank)
                WriteBYTE(EXAMINE_MINE_VERY_DIFFICULT, 8);
            else
                WriteBYTE(EXAMINE_MINE_IMPOSSIBLE, 8);
        }
        else
            WriteBYTE(EXAMINE_MINE_IMPOSSIBLE, 8);
    }
    else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        nDisarmDC = pGameObject->AsSWSPlaceable()->m_nDisarmDC;
        CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
        WriteBYTE(pPlaceable->m_nTrapType, 8);
        if (pGameObject->AsSWSPlaceable()->m_bTrapDisarmable &&
            nDisarmDC <= CSWRules::MAX_TRAP_DISARM_DC)
        {
            if (nDisarmDC <= nSkillRank - 15)
                WriteBYTE(EXAMINE_MINE_EASY, 8);
            else if (nDisarmDC <= nSkillRank - 10)
                WriteBYTE(EXAMINE_MINE_MODERATE, 8);
            else if (nDisarmDC <= nSkillRank - 5)
                WriteBYTE(EXAMINE_MINE_DIFFICULT, 8);
            else if (nDisarmDC <= nSkillRank)
                WriteBYTE(EXAMINE_MINE_VERY_DIFFICULT, 8);
            else
                WriteBYTE(EXAMINE_MINE_IMPOSSIBLE, 8);
        }
        else
            WriteBYTE(EXAMINE_MINE_IMPOSSIBLE, 8);
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
        return 0;
    return SendServerToPlayerMessage(nPlayerId, 0x1B, 5, pMessage,
                                     nMessageSize);
}
// PC 0x0056F370  Xbox 0x002325E0
int CSWSMessage::SendServerToPlayerExamineGui_DoorData(
    CSWSPlayer* pPlayer, unsigned long oidObject)
{
    unsigned long nPlayerId;
    CGameObjectArray* pObjectArray;
    CGameObject* pGameObject;
    CSWSDoor* pDoor;
    int nResult;
    unsigned char* pMessage;
    unsigned long nMessageSize;

    nPlayerId = pPlayer->m_nPlayerId;
    CreateWriteMessage(0x80, nPlayerId, 1);
    pGameObject = NULL;
    pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
    if (pObjectArray->GetGameObject(oidObject, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        pDoor = pGameObject->AsSWSDoor();
        if (pDoor != NULL)
        {
            WriteOBJECTIDServer(pDoor->m_nId);
            WriteCExoLocStringServer(pDoor->m_locDescription, 0);
        }
    }
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_EXAMINE_GUI,
                                     EXAMINE_GUI_DOOR_DATA, pMessage,
                                     nMessageSize);
}
// PC 0x0056F430  Xbox 0x002326A0
int CSWSMessage::SendServerToPlayerGameObjUpdate_ObjControl(
    unsigned long nPlayerId, unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 5, 2, pMessage, nMessageSize);
}
// PC 0x0056F490  Xbox 0x00232700
void CSWSMessage::WriteGameObjUpdate_CharacterSheet(CSWSPlayer* pPlayer,
                                                  unsigned long nUpdateMask)
{
    CSWSPlayerCharSheetGUI* pGUI = pPlayer->m_pCharSheetGUI;
    CSWCreatureStatsUpdate* pLastStats = pGUI->m_pStatsUpdate;
    unsigned long oidCreature = pGUI->m_oidCreature;
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->
        GetCreatureByGameObjectID(oidCreature);
    CSWSCreatureStats* pStats = pCreature->m_pStats;

    WriteCHAR('G', 8);
    WriteCHAR('S', 8);
    WriteOBJECTIDServer(pCreature->GetId());
    WriteDWORD(nUpdateMask, 32);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_ABILITIES)
    {
        WriteBYTE(pStats->GetSTRStat(), 8);
        WriteBYTE(pStats->GetDEXStat(), 8);
        WriteBYTE(pStats->GetCONStat(), 8);
        WriteBYTE(pStats->GetINTStat(), 8);
        WriteBYTE(pStats->GetWISStat(), 8);
        WriteBYTE(pStats->GetCHAStat(), 8);
        WriteCHAR(pStats->GetTotalSTRBonus(), 8);
        WriteCHAR(pStats->GetTotalDEXBonus(), 8);
        WriteCHAR(pStats->GetTotalCONBonus(), 8);
        WriteCHAR(pStats->GetTotalINTBonus(), 8);
        WriteCHAR(pStats->GetTotalWISBonus(), 8);
        WriteCHAR(pStats->GetTotalCHABonus(), 8);
        WriteCHAR(pStats->m_nStrengthBase, 8);
        WriteCHAR(pStats->m_nDexterityBase, 8);
        WriteCHAR(pStats->m_nConstitutionBase, 8);
        WriteCHAR(pStats->m_nIntelligenceBase, 8);
        WriteCHAR(pStats->m_nWisdomBase, 8);
        WriteCHAR(pStats->m_nCharismaBase, 8);
        WriteCHAR(pStats->GetDEXMod(1) - pStats->GetDEXMod(0), 8);
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_FORTITUDE)
        WriteCHAR(pStats->GetFortSavingThrow(0), 8);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_WILL)
        WriteCHAR(pStats->GetWillSavingThrow(0), 8);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_REFLEX)
        WriteCHAR(pStats->GetReflexSavingThrow(0), 8);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_EXPERIENCE)
        WriteDWORD(pStats->GetExperience(), 32);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_ATTACK_BONUS)
    {
        WriteBYTE(pStats->GetBaseAttackBonus(), 8);
        WriteBOOL(pStats->GetUseMonkAttackTables(0));
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_COMBAT_INFORMATION)
    {
        CCombatInformation* pCombat = pStats->m_pCombatInformation;
        CExoArrayList<CCombatInformationNode*>* pAttacks = &pCombat->m_lstAttacks;
        WriteBYTE(pCombat->GetNumAttacks(), 3);
        WriteCHAR(pCombat->GetOnHandAttackMod(), 8);
        WriteCHAR(pCombat->GetOnHandDamageMod(), 8);
        WriteCHAR(pCombat->GetForceResistance(), 8);
        WriteBYTE(pCombat->GetArcaneSpellFailure(), 7);
        WriteBYTE(pCombat->GetArmorCheckPenalty(), 5);
        WriteBYTE(pCombat->GetUnarmedDamageDice(), 5);
        WriteBYTE(pCombat->GetUnarmedDamageDie(), 5);
        WriteBYTE(pCombat->GetOnHandCriticalRange(), 4);
        WriteBYTE(pCombat->GetOnHandCriticalMultiplier(), 3);
        WriteBOOL(pCombat->GetOffHandWeaponEquipped());
        if (pCombat->GetOffHandWeaponEquipped() == TRUE)
        {
            WriteCHAR(pCombat->GetOffHandAttackMod(), 8);
            WriteCHAR(pCombat->GetOffHandDamageMod(), 8);
            WriteBYTE(pCombat->GetOffHandCriticalRange(), 4);
            WriteBYTE(pCombat->GetOffHandCriticalMultiplier(), 3);
        }
        if (pAttacks != NULL)
        {
            WriteBYTE(pAttacks->num, 8);
            for (int nAttack = 0; nAttack < pAttacks->num; nAttack++)
            {
                WriteCHAR((*pAttacks)[nAttack]->m_nModifier, 8);
                WriteBYTE((*pAttacks)[nAttack]->m_nWeaponWield, 3);
                if ((*pAttacks)[nAttack]->m_nVersusRace != g_pRules->m_nNumRaces)
                {
                    WriteBOOL(TRUE);
                    WriteBYTE((*pAttacks)[nAttack]->m_nVersusRace, 8);
                }
                else
                    WriteBOOL(FALSE);
                if ((*pAttacks)[nAttack]->m_nVersusGoodEvil != 0)
                {
                    WriteBOOL(TRUE);
                    WriteBYTE((*pAttacks)[nAttack]->m_nVersusGoodEvil, 3);
                }
                else
                    WriteBOOL(FALSE);
            }
        }
        else
            WriteBYTE(0, 8);
        CExoArrayList<CCombatInformationNode*>* pDamage = &pCombat->m_lstDamage;
        if (pDamage != NULL)
        {
            WriteBYTE(pCombat->m_lstDamage.num, 8);
            for (int nDamage = 0; nDamage < pDamage->num; nDamage++)
            {
                WriteCHAR((*pDamage)[nDamage]->m_nModifier, 8);
                WriteBYTE((*pDamage)[nDamage]->m_nModifierType, 4);
                WriteBYTE((*pDamage)[nDamage]->m_nWeaponWield, 3);
                if ((*pDamage)[nDamage]->m_nVersusRace != g_pRules->m_nNumRaces)
                {
                    WriteBOOL(TRUE);
                    WriteBYTE((*pDamage)[nDamage]->m_nVersusRace, 8);
                }
                else
                    WriteBOOL(FALSE);
                if ((*pDamage)[nDamage]->m_nVersusGoodEvil != 0)
                {
                    WriteBOOL(TRUE);
                    WriteBYTE((*pDamage)[nDamage]->m_nVersusGoodEvil, 3);
                }
                else
                    WriteBOOL(FALSE);
            }
        }
        else
            WriteBYTE(0, 8);
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_ARMOR_CLASS)
        WriteSHORT(pCreature->GetArmorClass(), 16);
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_SKILL_RANKS)
    {
        WriteDWORD(pLastStats->m_nSkillUpdates, g_pRules->m_nNumSkills);
        for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
        {
            if (pLastStats->m_nSkillUpdates &
                (CSWCreatureStatsUpdate::SKILL_UPDATE_BIT << nSkill))
            {
                WriteCHAR(pStats->GetSkillRank(nSkill, NULL, 0), 8);
                WriteBOOL(pStats->GetCanUseSkill(nSkill));
            }
        }
    }
    if (nUpdateMask & CSWSPlayerCharSheetGUI::UPDATE_FEAT_LISTS)
    {
        WriteWORD((unsigned short)pCreature->m_pStats->m_lstFeats.num, 16);
        for (unsigned short nFeat = 0;
             nFeat < (unsigned short)pCreature->m_pStats->m_lstFeats.num; nFeat++)
        {
            unsigned short nFeatId = pCreature->m_pStats->GetFeat(nFeat);
            WriteWORD(nFeatId, 16);
        }
        WriteWORD((unsigned short)pCreature->m_pStats->m_lstBonusFeats.num, 16);
        for (unsigned short nBonusFeat = 0;
             nBonusFeat < (unsigned short)pCreature->m_pStats->m_lstBonusFeats.num;
             nBonusFeat++)
        {
            unsigned short nFeatId = pCreature->m_pStats->GetBonusFeat(nBonusFeat);
            WriteWORD(nFeatId, 16);
        }
    }
    pCreature->m_pStats->UpdateLastStatsObject(nUpdateMask, pLastStats);
}
// PC 0x0056FB10  Xbox 0x00232D80
int CSWSMessage::SendServerToPlayerInventory_Drop(unsigned long nPlayerId,
                                                  unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 3, pMessage, nMessageSize);
}
// PC 0x0056FB70  Xbox 0x00232DE0
int CSWSMessage::SendServerToPlayerInventory_DropCancel(unsigned long nPlayerId,
                                                        unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 4, pMessage, nMessageSize);
}
// PC 0x0056FBD0  Xbox 0x00232E40
int CSWSMessage::SendServerToPlayerInventory_EquipCancel(
    unsigned long nPlayerId, unsigned long oidObject,
    unsigned long nInventorySlot)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteDWORD(nInventorySlot, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 2, pMessage, nMessageSize);
    g_pAppManager->m_pServerExoApp->GetClientObjectByPlayerId(nPlayerId, 0);
    return 1;
}
// PC 0x0056FC50  Xbox 0x00232EC0
int CSWSMessage::SendServerToPlayerInventory_Pickup(unsigned long nPlayerId,
                                                    unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 5, pMessage, nMessageSize);
}
// PC 0x0056FCB0  Xbox 0x00232F20
int CSWSMessage::SendServerToPlayerInventory_PickupCancel(
    unsigned long nPlayerId, unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 6, pMessage, nMessageSize);
}
// PC 0x0056FD10  Xbox 0x00232F80
int CSWSMessage::SendServerToPlayerInventory_Unequip(unsigned long nPlayerId,
                                                     unsigned long oidObject,
                                                     int bFlag)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteBOOL(bFlag);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 7, pMessage, nMessageSize);
}
// PC 0x0056FD80  Xbox 0x00232FF0
int CSWSMessage::SendServerToPlayerInventory_UnequipCancel(
    unsigned long nPlayerId, unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, 8, pMessage, nMessageSize);
}
// PC 0x0056FDE0  Xbox 0x00233050
int CSWSMessage::SendServerToPlayerLogin_Confirm(unsigned long nPlayerId)
{
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_LOGIN, 5, NULL, 0);
}
// PC 0x00567BA0  Xbox 0x0022AD70
void CSWSMessage::WriteGameObjUpdate_WorkRemaining(CSWSObject* pObject,
                                                   CSWSArea* pArea, int nValue,
                                                   int nValue2)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL && (pArea == NULL || pCreature->m_bWorkRemainingCompleteSent == 0) &&
        (nValue != nValue2 || pCreature->m_bWorkRemainingCompleteSent == 0))
    {
        WriteBYTE(0x57, 8);
        WriteBYTE(nValue, 8);
        WriteBYTE(nValue2, 8);
        if (nValue == nValue2)
        {
            pCreature->m_bWorkRemainingCompleteSent = 1;
        }
    }
}
// PC 0x00567C20  Xbox 0x0022ADF0
void CSWSMessage::AddTriggerGeometryToMessage(CSWSTrigger* pTrigger)
{
    WriteBYTE(pTrigger->GetGeometryPointCount(), 8);
    for (int i = 0; i < pTrigger->GetGeometryPointCount(); i++)
    {
        WriteFLOAT(pTrigger->GetGeometry()[i].m_fX, 1.0f, 32);
        WriteFLOAT(pTrigger->GetGeometry()[i].m_fY, 1.0f, 32);
        WriteFLOAT(pTrigger->GetGeometry()[i].m_fZ, 1.0f, 32);
    }
}
// PC 0x00567CB0  Xbox 0x0022AE80
void CSWSMessage::StoreValuesInLastUpdatePartyObject(
    CSWSCreature* pPlayerCreature, CLastUpdatePartyObject* pLastUpdateObject,
    CSWSCreature* pCreature, unsigned long nUpdateMask)
{
    if (nUpdateMask & OBJECT_UPDATE_AI_STATE)
    {
        pLastUpdateObject->m_nAIStateFlags = pCreature->m_nAIStateFlags;
        pLastUpdateObject->m_nAIState = pCreature->m_nAIState;
        pLastUpdateObject->m_nActivity = pCreature->m_nActivity;
        pLastUpdateObject->m_oidAITarget = pCreature->m_oidAITarget;
        pLastUpdateObject->m_nAIStateReputation =
            pPlayerCreature->GetAIStateReputation(pCreature->GetId());
        pLastUpdateObject->m_bCombatState = pCreature->m_bCombatState;
        pLastUpdateObject->m_nStealthMode = pCreature->m_nStealthMode;
    }
    if (nUpdateMask & OBJECT_UPDATE_AREA)
    {
        pLastUpdateObject->m_oidArea = pCreature->m_oidArea;
    }
    if (nUpdateMask & OBJECT_UPDATE_POSITION)
    {
        pLastUpdateObject->m_vPosition.m_fX = pCreature->m_vPosition.m_fX;
        pLastUpdateObject->m_vPosition.m_fY = pCreature->m_vPosition.m_fY;
    }
    if (nUpdateMask & OBJECT_UPDATE_HIT_POINTS)
    {
        pLastUpdateObject->m_nBaseMaxHitPoints = pCreature->m_nMaxHitPoints;
        pLastUpdateObject->m_nCurrentHitPoints = pCreature->GetCurrentHitPoints(1);
        pLastUpdateObject->m_nTemporaryHitPoints = pCreature->m_nTemporaryHitPoints;
        pLastUpdateObject->m_nMaxHitPoints = pCreature->GetMaxHitPoints(1);
    }
    if (nUpdateMask & OBJECT_UPDATE_FORCE_POINTS)
    {
        pLastUpdateObject->m_nForcePoints = pCreature->m_pStats->m_nForcePoints;
        pLastUpdateObject->m_nCurrentForcePoints = pCreature->m_pStats->m_nCurrentForcePoints;
        pLastUpdateObject->m_nTemporaryForcePoints = pCreature->m_pStats->m_nTemporaryForcePoints;
        pLastUpdateObject->m_nMaxForcePoints = pCreature->GetMaxForcePoints();
    }
    if (nUpdateMask & OBJECT_UPDATE_PARTY_STATE)
    {
        pLastUpdateObject->m_nAreaWaitState = pCreature->m_nAreaWaitState;
        pLastUpdateObject->m_bAIStateFlag0040 =
            (pCreature->m_nAIStateFlags & CSWSCreature::AI_STATE_FLAG_0040) ==
            CSWSCreature::AI_STATE_FLAG_0040;
        pLastUpdateObject->m_bSingletonParty = pCreature->GetFaction()->GetSingletonParty();
        pLastUpdateObject->m_bPartyLeader =
            pCreature->GetFaction()->GetLeader() == pCreature->GetId();
        pLastUpdateObject->m_bInvitedToParty = pCreature->m_bInvitedToParty;
        pLastUpdateObject->m_bDiseased = pCreature->m_bDiseased;
        pLastUpdateObject->m_nPoisonState = pCreature->m_nPoisonState;
    }
    if (nUpdateMask & OBJECT_UPDATE_PORTRAIT)
    {
        pLastUpdateObject->m_nPortraitId = pCreature->GetPortraitId();
        pLastUpdateObject->m_cPortrait = pCreature->GetPortrait();
    }
}
// PC 0x0056FE40  Xbox 0x002330D0
int CSWSMessage::SendServerToPlayerPlayerList_All(CSWSPlayer* pRecipient)
{
    CreateWriteMessage(512, RECIPIENT_UNSPECIFIED, 1);
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->GetServerExoApp()->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 0;
    }
    unsigned char nPlayers = 0;
    CExoLinkedListNode* pPosition;
    for (pPosition = pPlayerList->GetHeadPos(); pPosition != NULL;
         pPlayerList->GetNext(pPosition))
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer->GetGameObject() != NULL)
        {
            nPlayers++;
        }
    }
    WriteBYTE(nPlayers, 8);
    for (pPosition = pPlayerList->GetHeadPos(); pPosition != NULL;
         pPlayerList->GetNext(pPosition))
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        unsigned long nListedPlayerId = pPlayer->m_nPlayerId;
        CGameObject* pObject = pPlayer->GetGameObject();
        if (pObject != NULL)
        {
            unsigned long oidObject = pObject->GetId();
            CExoString sPlayerName(
                g_pAppManager->GetServerExoApp()->GetNetLayer()->GetPlayerInfo(
                    nListedPlayerId)->m_sPlayerName);
            unsigned long oidCharacter = OBJECT_INVALID;
            CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
            if (pCreature != NULL && pCreature->GetStats()->GetIsPC() == 1)
            {
                oidCharacter = pCreature->GetId();
            }
            int bCharacter = 0;
            CExoLocString lsFirstName;
            CExoLocString lsLastName;
            CResRef cPortrait;
            pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidCharacter);
            if (pCreature != NULL)
            {
                bCharacter = 1;
                lsFirstName = pCreature->GetStats()->m_lsFirstName;
                lsLastName = pCreature->GetStats()->m_lsLastName;
                cPortrait = pCreature->GetStats()->GetPortrait();
            }
            WriteDWORD(nListedPlayerId, 32);
            WriteOBJECTIDServer(oidObject);
            WriteCExoString(sPlayerName, 32);
            WriteBOOL(bCharacter);
            if (bCharacter == 1)
            {
                WriteOBJECTIDServer(oidCharacter);
                WriteCExoLocStringServer(lsFirstName, 0);
                WriteCExoLocStringServer(lsLastName, 0);
                unsigned short nPortraitId = pCreature->GetPortraitId();
                WriteWORD(nPortraitId, 16);
                if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
                {
                    WriteCResRef(cPortrait, 16);
                }
            }
        }
    }
    unsigned char* pMessage;
    unsigned long nMessageSize;
    if (nPlayers != 0 && GetWriteMessage(&pMessage, &nMessageSize) != 0)
    {
        return SendServerToPlayerMessage(pRecipient->GetPlayerId(),
            MESSAGE_PLAYER_LIST, PLAYER_LIST_ALL, pMessage, nMessageSize);
    }
    return 0;
}
// PC 0x00570180  Xbox 0x00233410
int CSWSMessage::SendServerToAllPlayersCreatureUpdate_StripEquippedItems(
    unsigned long oidCreature)
{
    CServerExoApp* pServerExoApp;
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pPosition;
    CSWSPlayer* pPlayer;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPlayerList = pServerExoApp->GetPlayerList();
    for (pPosition = pPlayerList->GetHeadPos(); pPosition != NULL;
         pPlayerList->GetNext(pPosition))
    {
        pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer->GetLastUpdateObject(oidCreature) != NULL)
        {
            CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
            WriteOBJECTIDServer(oidCreature);
            if (GetWriteMessage(&pMessage, &nMessageSize) != 0)
            {
                SendServerToPlayerMessage(pPlayer->m_nPlayerId, 5, 6,
                                          pMessage, nMessageSize);
            }
        }
    }
    return 1;
}
// PC 0x00570230  Xbox 0x002334C0
int CSWSMessage::SendServerToPlayerDebugInfo_Creature(CSWSPlayer* pPlayer,
                                                     unsigned long oidObject)
{
    unsigned char* pMessage;
    unsigned long nMessageSize;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidObject);
    if (pCreature == NULL || pCreature->GetFaction() == NULL ||
        pCreature->m_pInventory == NULL ||
        pCreature->GetItemRepository(TRUE) == NULL ||
        pCreature->m_pStats == NULL)
    {
        return 0;
    }

    CreateWriteMessage(256, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteCExoString(pCreature->m_sTag, 32);
    CSWSFaction* pFaction = pCreature->GetFaction();
    WriteCExoString(pFaction->m_sName, 32);
    WriteCResRef(pCreature->GetDialogResref(), 16);
    WriteSHORT(pCreature->GetCurrentHitPoints(TRUE), 16);
    WriteSHORT(pCreature->GetMaxHitPoints(TRUE), 16);
    WriteSHORT(pCreature->GetArmorClass(), 16);
    int nAILevel = pCreature->m_nAILevel;
    WriteINT(nAILevel, 32);
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    float fChallengeRating = pStats->m_fChallengeRating;
    WriteFLOAT(fChallengeRating, 1.0f, 32);
    if (pCreature->GetArea() != NULL)
    {
        CSWSArea* pArea = pCreature->GetArea();
        WriteCExoString(pArea->m_sTag, 32);
    }
    else
    {
        CExoString sAreaTag("");
        WriteCExoString(sAreaTag, 32);
    }

    unsigned long nEquippedSlots = 0;
    for (unsigned char nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; nSlot++)
    {
        CSWInventory* pInventory = pCreature->m_pInventory;
        if (pInventory->GetItemInSlot(1 << nSlot) != NULL)
        {
            nEquippedSlots |= 1 << nSlot;
        }
    }
    WriteDWORD(nEquippedSlots, 32);
    for (unsigned char nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; nSlot++)
    {
        CSWInventory* pInventory = pCreature->m_pInventory;
        CSWSItem* pItem = pInventory->GetItemInSlot(1 << nSlot);
        if (pItem != NULL)
        {
            WriteCExoString(pItem->m_sTag, 32);
        }
    }
    WriteDWORD(pCreature->GetItemRepository(TRUE)->GetNumItems(), 32);
    for (unsigned long nItem = 0;
         nItem < pCreature->GetItemRepository(TRUE)->GetNumItems(); nItem++)
    {
        CSWSItem* pItem = pCreature->GetItemRepository(TRUE)->ItemListGetItem(nItem);
        if (pItem != NULL)
        {
            WriteCExoString(pItem->m_sTag, 32);
        }
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    unsigned long nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_DEBUG_INFO,
                                     DEBUG_INFO_CREATURE, pMessage, nMessageSize);
}
// PC 0x00567E60  Xbox 0x0022B030
int CSWSMessage::SendServerToPlayerMessage(unsigned long nPlayerId,
                                           unsigned char nMajor,
                                           unsigned char nMinor,
                                           unsigned char* pMessage,
                                           unsigned long nMessageSize)
{
    unsigned long nSize;
    int nResult;

    if (nMessageSize != 0)
    {
        nSize = nMessageSize;
    }
    else
    {
        nSize = 3;
        pMessage = new unsigned char[3];
        m_nRecipient = 0xFFFFFFFF;
    }
    if (pMessage == NULL)
    {
        return 0;
    }
    pMessage[0] = 80;
    pMessage[1] = nMajor;
    pMessage[2] = nMinor;
    EXOXBOXASSERT(m_nRecipient == 0xFFFFFFFF || (nPlayerId != CNetLayer::PLAYERID_ALL_PLAYERS && nPlayerId != CNetLayer::PLAYERID_ALL_SERVERADMINS && m_nRecipient == nPlayerId));
    nResult =
        g_pAppManager->m_pServerExoApp->GetNetLayer()->SendMessageToPlayer(
            nPlayerId, pMessage, nSize, 0);
    if (nMessageSize == 0)
    {
        delete[] pMessage;
    }
    return nResult;
}

#ifndef _XBOX
int CSWSMessage::SendServerToPlayerModule_Info(unsigned long nPlayerId)  // PC 0x00567EF0  Xbox 0x0022B0E0
{
    CSWSModule* pModule;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if ((g_pAppManager->m_pServerExoApp->GetServerMode() == 1 ||
         g_pAppManager->m_pServerExoApp->GetServerMode() == 2) &&
        pModule != NULL)
    {
        CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
        pModule->PackModuleIntoMessage(nPlayerId);
        if (GetWriteMessage(&pMessage, &nMessageSize) != 0)
        {
            return SendServerToPlayerMessage(nPlayerId, 3, 1, pMessage,
                                             nMessageSize);
        }
    }
    return 0;
}
#endif
int CSWSMessage::SendServerToPlayerParty_List(unsigned long nPlayerId,  // PC 0x00567F90  Xbox 0x0022B190
                                              int nCount,
                                              unsigned long* pMembers,
                                              unsigned char nMinor,
                                              unsigned long oidExclude)
{
    unsigned long nMemberCount;
    int i;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMemberCount = 0;
    for (i = 0; i < nCount; i++)
    {
        if (pMembers[i] != oidExclude)
        {
            nMemberCount++;
        }
    }
    CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nMemberCount, 32);
    for (i = 0; i < nCount; i++)
    {
        if (pMembers[i] != oidExclude)
        {
            WriteOBJECTIDServer(pMembers[i]);
        }
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    return SendServerToPlayerMessage(nPlayerId, 14, nMinor, pMessage,
                                     nMessageSize);
}
// PC 0x00568030  Xbox 0x0022B230
int CSWSMessage::SendServerToPlayerPlayerList_Add(unsigned long nPlayerId,
                                                CSWSPlayer* pPlayer)
{
    unsigned long nListedPlayerId = pPlayer->m_nPlayerId;
    unsigned long oidObject = pPlayer->GetGameObject()->GetId();
    CExoString sPlayerName(
        g_pAppManager->GetServerExoApp()->GetNetLayer()->GetPlayerInfo(
            nListedPlayerId)->m_sPlayerName);
    unsigned long oidCharacter = OBJECT_INVALID;
    CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
    if (pCreature != NULL && pCreature->GetStats()->GetIsPC() == 1)
    {
        oidCharacter = pCreature->GetId();
    }
    int bCharacter = 0;
    CExoLocString lsFirstName;
    CExoLocString lsLastName;
    CResRef cPortrait;
    pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
        oidCharacter);
    if (pCreature != NULL)
    {
        bCharacter = 1;
        lsFirstName = pCreature->GetStats()->m_lsFirstName;
        lsLastName = pCreature->GetStats()->m_lsLastName;
        cPortrait = pCreature->GetStats()->GetPortrait();
    }
    CreateWriteMessage(256, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nListedPlayerId, 32);
    WriteOBJECTIDServer(oidObject);
    WriteCExoString(sPlayerName, 32);
    WriteBOOL(bCharacter);
    if (bCharacter == 1)
    {
        WriteOBJECTIDServer(oidCharacter);
        WriteCExoLocStringServer(lsFirstName, 0);
        WriteCExoLocStringServer(lsLastName, 0);
        unsigned short nPortraitId = pCreature->GetPortraitId();
        WriteWORD(nPortraitId, 16);
        if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
        {
            WriteCResRef(cPortrait, 16);
        }
    }
    unsigned char* pMessage;
    unsigned long nMessageSize;
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_PLAYER_LIST,
                                     PLAYER_LIST_ADD, pMessage, nMessageSize);
}
// PC 0x005682B0  Xbox 0x0022B4D0
int CSWSMessage::SendServerToPlayerPlayerList_Delete(unsigned long nPlayerId,
                                                     CSWSPlayer* pPlayer)
{
    unsigned long oidPlayer;
    int nResult;
    unsigned char* pMessage;
    unsigned long nMessageSize;

    oidPlayer = pPlayer->m_nPlayerId;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(oidPlayer, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, 10, 3, pMessage, nMessageSize);
}
// PC 0x00568310  Xbox 0x0022B530
int CSWSMessage::SendServerToPlayerServerStatus_Status(unsigned long nPlayerId)
{
    switch (g_pAppManager->m_pServerExoApp->GetServerMode())
    {
    case CServerExoAppInternal::SERVER_MODE_IDLE:
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_SERVER_STATUS,
                                        SERVER_STATUS_RESET, NULL, 0);
    case CServerExoAppInternal::SERVER_MODE_LOADING:
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_SERVER_STATUS,
                                        SERVER_STATUS_RUN_REQUEST, NULL, 0);
    case CServerExoAppInternal::SERVER_MODE_RUNNING:
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_SERVER_STATUS,
                                        SERVER_STATUS_CHARACTER_LOGIN, NULL, 0);
    default:
        return 0;
    }
}
// PC 0x005683D0  Xbox 0x0022B5A0
void CSWSMessage::UpdateLastUpdateInventory(CSWSPlayer* pPlayer,
                                            unsigned long nSlot)
{
    CSWSObject* pGameObject;
    CSWInventory* pInventory;
    CSWSItem* pItem;

    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
    {
        pGameObject = pPlayer->GetGameObject();
        if (pGameObject->AsSWSCreature() != NULL &&
            pGameObject->AsSWSCreature()->m_pInventory != NULL)
        {
            pInventory = pGameObject->AsSWSCreature()->m_pInventory;
            pItem = pInventory->GetItemInSlot(1 << nSlot);
            if (pItem != NULL)
            {
                pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                    ->m_oidSlot[nSlot] = pItem->m_nId;
                pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                    ->m_nSlotIndex[nSlot] = pItem->m_nStackSize;
            }
            else
            {
                pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                    ->m_oidSlot[nSlot] = CGameObjectArray::INVALIDOBJID;
                pPlayer->m_pInventoryGUI->m_pLastUpdateObject
                    ->m_nSlotIndex[nSlot] = -1;
            }
        }
    }
}
// PC 0x00568480  Xbox 0x0022B650
void CSWSMessage::UpdateLastUpdateObjectAppearance(
    CSWSObject* pObject, CLastUpdateObject* pLastUpdateObject,
    unsigned long nAppearanceMask)
{
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (nAppearanceMask & APPEARANCE_UPDATE_NAME)
        {
            pLastUpdateObject->m_lsName = pCreature->m_pStats->m_lsFirstName;
            pLastUpdateObject->m_lsDescription = pCreature->m_pStats->m_lsLastName;
        }
        pLastUpdateObject->m_oidAppearanceLeftHand = pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand;
        pLastUpdateObject->m_oidAppearanceRightHand = pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand;
        pLastUpdateObject->m_oidAppearanceArmour = pCreature->m_cAppearanceInfo.m_oidAppearanceArmour;
        pLastUpdateObject->m_oidAppearanceHead = pCreature->m_cAppearanceInfo.m_oidAppearanceHead;
        pLastUpdateObject->m_nAppearance = pCreature->m_cAppearanceInfo.m_nAppearance;
        pLastUpdateObject->m_nAppearancePart0 = pCreature->m_cAppearanceInfo.m_nAppearancePart0;
        pLastUpdateObject->m_nAppearancePart1 = pCreature->m_cAppearanceInfo.m_nAppearancePart1;
        pLastUpdateObject->m_nAppearancePart2 = pCreature->m_cAppearanceInfo.m_nAppearancePart2;
        pLastUpdateObject->m_nAppearancePart3 = pCreature->m_cAppearanceInfo.m_nAppearancePart3;
        pLastUpdateObject->m_nAppearancePart4 = pCreature->m_cAppearanceInfo.m_nAppearancePart4;
        pLastUpdateObject->m_nAppearancePart5 = pCreature->m_cAppearanceInfo.m_nAppearancePart5;
        pLastUpdateObject->m_nAppearancePart6 = pCreature->m_cAppearanceInfo.m_nAppearancePart6;
        pLastUpdateObject->m_nAppearancePart7 = pCreature->m_cAppearanceInfo.m_nAppearancePart7;
        pLastUpdateObject->m_nAppearancePart8 = pCreature->m_cAppearanceInfo.m_nAppearancePart8;
        pLastUpdateObject->m_nAppearanceState = pCreature->m_cAppearanceInfo.m_bUseBackupHead;
        pLastUpdateObject->m_nAppearanceVariant = pCreature->m_cAppearanceInfo.m_nAppearanceHead;
    }
}
// PC 0x00568570  Xbox 0x0022B740
int CSWSMessage::SendServerToPlayerCCMessage(unsigned long nPlayerId,
    unsigned char nType, CSWCCMessageData* pMessageData,
    CSWSCombatAttackData* pAttackData)
{
    unsigned char* pMessage = NULL;
    unsigned long nMessageSize = 0;
    if (pMessageData == NULL)
    {
        return 0;
    }
    switch (nType)
    {
    case CC_MESSAGE_OBJECT_TEXT:
        CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteDWORD(pMessageData->GetInteger(0), 32);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_DAMAGE_WITHOUT_ATTACKER:
        CreateWriteMessage(6, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        for (int i = 0; i < 15; ++i)
        {
            if (pMessageData->GetInteger(i) > -1)
            {
                WriteBOOL(1);
                WriteWORD((unsigned short)pMessageData->GetInteger(i), 14);
            }
            else
            {
                WriteBOOL(0);
            }
        }
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_DAMAGE:
        CreateWriteMessage(10, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        for (int i = 0; i < 15; ++i)
        {
            if (pMessageData->GetInteger(i) > -1)
            {
                WriteBOOL(1);
                WriteWORD((unsigned short)pMessageData->GetInteger(i), 14);
            }
            else
            {
                WriteBOOL(0);
            }
        }
        WriteBOOL(g_bEnableCombatDebugging);
        if (g_bEnableCombatDebugging)
        {
            WriteCExoString(pMessageData->GetString(0), 32);
        }
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_MELEE_ATTACK:
    case CC_MESSAGE_RANGED_ATTACK:
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType, NULL, 0);

    case CC_MESSAGE_FORCE_RESISTANCE:
        CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteBYTE((unsigned char)pMessageData->GetInteger(0), 8);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_SPELL_RESISTANCE_CHECK:
        if (pMessageData->GetInteger(2) == 0)
        {
            return 1;
        }
        CreateWriteMessage(20, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteBYTE((unsigned char)pMessageData->GetInteger(0), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(1), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(2), 8);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_SAVING_THROW:
        CreateWriteMessage(10, RECIPIENT_UNSPECIFIED, 1);
        WriteBYTE((unsigned char)pMessageData->GetInteger(0), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(1), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(2), 5);
        WriteCHAR((char)pMessageData->GetInteger(3), 8);
        WriteCHAR((char)pMessageData->GetInteger(4), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(5), 7);
        WriteOBJECTIDServer(pMessageData->GetInteger(6));
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_DEATH:
        CreateWriteMessage(12, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteINT(pMessageData->GetInteger(0), 32);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_SPELL_DATA:
        CreateWriteMessage(28, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(2));
        WriteINT(pMessageData->GetInteger(0), 32);
        if (pMessageData->GetObjectID(CSWCCMessageData::SPELL_DATA_ITEM) != OBJECT_INVALID)
        {
            WriteBOOL(1);
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
                pMessageData->GetObjectID(CSWCCMessageData::SPELL_DATA_ITEM));
            if (pItem != NULL)
            {
                WriteCExoLocStringServer(pItem->m_sLocalizedName, 0);
            }
        }
        else
        {
            WriteBOOL(0);
        }
        WriteINT(pMessageData->GetInteger(1), 32);
        WriteINT(pMessageData->GetInteger(2), 32);
        WriteINT(pMessageData->GetInteger(3), 32);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_SKILL:
        CreateWriteMessage(32, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteDWORD(pMessageData->GetInteger(0), 32);
        WriteBYTE((unsigned char)pMessageData->GetInteger(1), 5);
        WriteCHAR((char)pMessageData->GetInteger(2), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(3), 7);
        WriteBOOL(pMessageData->GetInteger(4));
        WriteBYTE((unsigned char)pMessageData->GetInteger(5), 8);
        WriteDWORD(pMessageData->GetInteger(6), 32);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_TOUCH_ATTACK:
        CreateWriteMessage(12, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteCHAR((char)pMessageData->GetInteger(0), 8);
        WriteBYTE((unsigned char)pMessageData->GetInteger(1), 5);
        WriteBYTE((unsigned char)pMessageData->GetInteger(2), 4);
        WriteBOOL(pMessageData->GetInteger(3));
        if (pMessageData->GetInteger(4) > 0)
        {
            WriteBOOL(1);
            WriteBYTE((unsigned char)pMessageData->GetInteger(4), 7);
        }
        else
        {
            WriteBOOL(0);
        }
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_INITIATIVE:
        CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteBYTE((unsigned char)pMessageData->GetInteger(0), 5);
        WriteCHAR((char)pMessageData->GetInteger(1), 8);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_FEEDBACK:
        CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
        WriteWORD((unsigned short)pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE), 16);
        if (pMessageData->GetObjectID(0) != OBJECT_INVALID &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_ITEM_RECEIVED &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_ITEM_LOST &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_SCRIPT_REPORT &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_ACTION_LIST_OVERFLOW &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_EFFECT_LIST_OVERFLOW &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_ACTION_LIST_ERASED &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_EFFECT_LIST_ERASED &&
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) != CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW)
        {
            WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_SKILL_COOLDOWN ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_OBJECT_COUNT ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_COUNTER_PAIR ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ALIGNMENT_SHIFT ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_GOLD_ADDED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_GOLD_REMOVED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_HEALED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_FORCE_POINTS_REGAINED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_GAINED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_LOST ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_FORCE_POWER_NAME)
        {
            WriteINT(pMessageData->GetInteger(0), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_SPELL_IMMUNITY ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_RESISTED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_REDUCTION ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_OBJECT_COUNTER ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_OBJECT_COUNTER_REMAINING ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_COUNTER_PAIR ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ALIGNMENT_SHIFT ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ATTRIBUTE_REDUCED)
        {
            WriteINT(pMessageData->GetInteger(1), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_OBJECT_COUNTER_REMAINING ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ATTRIBUTE_REDUCED)
        {
            WriteINT(pMessageData->GetInteger(2), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ATTRIBUTE_REDUCED)
        {
            WriteFLOAT(pMessageData->GetFloat(0), 1.0f, 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ITEM_RECEIVED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ITEM_LOST)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
                pMessageData->GetObjectID(0));
            if (pItem != NULL)
            {
                WriteBOOL(1);
                WriteCExoLocStringServer(pItem->m_sLocalizedName, 0);
            }
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_OBJECT_ITEM_TEXT ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_OBJECT_ITEM_NOTICE)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
                pMessageData->GetObjectID(1));
            if (pItem != NULL)
            {
                WriteBOOL(1);
                WriteCExoLocStringServer(pItem->m_sLocalizedName, 0);
            }
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_FLOATY_DATA &&
            pMessageData->GetObjectID(0) != OBJECT_INVALID)
        {
            WriteINT(pMessageData->GetInteger(0), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_FLOATY_STRING &&
            pMessageData->GetObjectID(0) != OBJECT_INVALID)
        {
            WriteCExoString(pMessageData->GetString(0), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_SCRIPT_REPORT)
        {
            WriteOBJECTIDServer(pMessageData->GetObjectID(1));
            WriteCExoString(pMessageData->GetString(0), 32);
            WriteCExoString(pMessageData->GetString(1), 32);
            WriteINT(pMessageData->GetInteger(0), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ACTION_LIST_OVERFLOW ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_EFFECT_LIST_OVERFLOW ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_ACTION_LIST_ERASED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_EFFECT_LIST_ERASED ||
            pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW)
        {
            WriteOBJECTIDServer(pMessageData->GetObjectID(1));
            WriteCExoString(pMessageData->GetString(1), 32);
            WriteINT(pMessageData->GetInteger(0), 32);
            WriteINT(pMessageData->GetInteger(1), 32);
            if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW)
            {
                WriteCExoString(pMessageData->GetString(2), 32);
                WriteINT(pMessageData->GetInteger(2), 32);
                WriteINT(pMessageData->GetInteger(3), 32);
            }
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == CSWSCreature::FEEDBACK_CUSTOM_STRING)
        {
            WriteCExoString(pMessageData->GetString(0), 32);
        }
        if (pMessageData->GetInteger(FEEDBACK_INTEGER_MESSAGE) == FEEDBACK_MESSAGE_EXPERIENCE_GAINED)
        {
            WriteINT(pMessageData->GetInteger(FEEDBACK_INTEGER_EXPERIENCE), 32);
        }
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_DISPEL_MAGIC:
        CreateWriteMessage(pMessageData->GetInteger(0) * 2 + 5,
            RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteBYTE((unsigned char)pMessageData->GetInteger(0), 8);
        for (unsigned char i = 0;
            i < (unsigned char)pMessageData->GetInteger(0); ++i)
        {
            WriteWORD((unsigned short)pMessageData->GetInteger(i + 1), 9);
        }
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_STEALTH_DETECTION:
        CreateWriteMessage(32, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteBOOL(pMessageData->GetInteger(0));
        WriteSHORT((unsigned short)pMessageData->GetInteger(1), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(2), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(3), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(4), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(5), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(6), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(7), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(8), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(9), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(10), 16);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_MINE_DETECTION:
        CreateWriteMessage(12, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteSHORT((unsigned short)pMessageData->GetInteger(0), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(1), 16);
        WriteSHORT((unsigned short)pMessageData->GetInteger(2), 16);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_FORCE_POINTS_SPENT:
        CreateWriteMessage(14, RECIPIENT_UNSPECIFIED, 1);
        WriteCExoString(pMessageData->GetString(0), 32);
        WriteINT(pMessageData->GetInteger(0), 32);
        WriteINT(pMessageData->GetInteger(1), 32);
        WriteINT(pMessageData->GetInteger(2), 32);
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_ATTACK_SUMMARY:
        CreateWriteMessage(42, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteOBJECTIDServer(pMessageData->GetObjectID(1));
        WriteINT(pMessageData->GetInteger(0), 32);
        WriteWORD((unsigned short)pMessageData->GetInteger(1), 16);
        WriteINT(pMessageData->GetInteger(2), 32);
        WriteINT(pMessageData->GetInteger(3), 32);
        WriteINT(pMessageData->GetInteger(4), 32);
        WriteINT(pMessageData->GetInteger(5), 32);
        WriteINT(pMessageData->GetInteger(6), 32);
        WriteINT(pMessageData->GetInteger(7), 32);
        WriteINT(pMessageData->GetInteger(8), 32);
        WriteBOOL(pMessageData->GetInteger(9));
        WriteBOOL(pMessageData->GetInteger(10));
        WriteBOOL(pMessageData->GetInteger(11));
        if (!GetWriteMessage(&pMessage, &nMessageSize))
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);

    case CC_MESSAGE_ATTACK_BREAKDOWN:
        CreateWriteMessage(84, RECIPIENT_UNSPECIFIED, 1);
        WriteINT(pMessageData->GetInteger(0), 32);
        WriteINT(pMessageData->GetInteger(1), 32);
        WriteINT(pMessageData->GetInteger(2), 32);
        WriteINT(pMessageData->GetInteger(3), 32);
        WriteINT(pMessageData->GetInteger(4), 32);
        WriteINT(pMessageData->GetInteger(5), 32);
        WriteINT(pMessageData->GetInteger(6), 32);
        WriteINT(pMessageData->GetInteger(7), 32);
        WriteINT(pMessageData->GetInteger(8), 32);
        WriteINT(pMessageData->GetInteger(9), 32);
        WriteINT(pMessageData->GetInteger(10), 32);
        WriteINT(pMessageData->GetInteger(11), 32);
        WriteINT(pMessageData->GetInteger(12), 32);
        WriteINT(pMessageData->GetInteger(13), 32);
        WriteINT(pMessageData->GetInteger(14), 32);
        WriteINT(pMessageData->GetInteger(15), 32);
        WriteINT(pMessageData->GetInteger(16), 32);
        WriteINT(pMessageData->GetInteger(17), 32);
        WriteINT(pMessageData->GetInteger(18), 32);
        WriteINT(pMessageData->GetInteger(19), 32);
        WriteDWORD(pMessageData->GetInteger(20), 32);
        break;

    case CC_MESSAGE_THREAT_BREAKDOWN:
        CreateWriteMessage(32, RECIPIENT_UNSPECIFIED, 1);
        WriteINT(pMessageData->GetInteger(0), 32);
        WriteINT(pMessageData->GetInteger(1), 32);
        WriteINT(pMessageData->GetInteger(2), 32);
        WriteINT(pMessageData->GetInteger(3), 32);
        WriteINT(pMessageData->GetInteger(4), 32);
        WriteINT(pMessageData->GetInteger(5), 32);
        WriteDWORD(pMessageData->GetInteger(6), 32);
        break;

    case CC_MESSAGE_DEFENSE_BREAKDOWN:
        CreateWriteMessage(36, RECIPIENT_UNSPECIFIED, 1);
        WriteINT(pMessageData->GetInteger(0), 32);
        WriteINT(pMessageData->GetInteger(1), 32);
        WriteINT(pMessageData->GetInteger(2), 32);
        WriteINT(pMessageData->GetInteger(3), 32);
        WriteINT(pMessageData->GetInteger(4), 32);
        WriteINT(pMessageData->GetInteger(5), 32);
        WriteINT(pMessageData->GetInteger(6), 32);
        WriteINT(pMessageData->GetInteger(7), 32);
        WriteDWORD(pMessageData->GetInteger(8), 32);
        break;

    case CC_MESSAGE_DAMAGE_BREAKDOWN:
        CreateWriteMessage(112, RECIPIENT_UNSPECIFIED, 1);
        {
            int i;
            for (i = 0; i < 15; ++i)
            {
                WriteINT(pMessageData->GetInteger(i), 32);
            }
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteINT(pMessageData->GetInteger(i++), 32);
            WriteDWORD(pMessageData->GetInteger(i++), 32);
            WriteDWORD(pMessageData->GetInteger(i++), 32);
            WriteDWORD(pMessageData->GetInteger(i++), 32);
        }
        break;

    case CC_MESSAGE_EFFECT_APPLICATION_BREAKDOWN:
    case CC_MESSAGE_DEFLECT_BREAKDOWN:
        CreateWriteMessage(40, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteDWORD(pMessageData->GetInteger(0), 32);
        WriteDWORD(pMessageData->GetInteger(1), 32);
        WriteDWORD(pMessageData->GetInteger(2), 32);
        WriteDWORD(pMessageData->GetInteger(3), 32);
        WriteDWORD(pMessageData->GetInteger(4), 32);
        WriteDWORD(pMessageData->GetInteger(5), 32);
        WriteDWORD(pMessageData->GetInteger(6), 32);
        WriteDWORD(pMessageData->GetInteger(7), 32);
        WriteDWORD(pMessageData->GetInteger(8), 32);
        break;

    case CC_MESSAGE_POISONED:
        CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
        WriteOBJECTIDServer(pMessageData->GetObjectID(0));
        WriteDWORD(pMessageData->GetInteger(0), 32);
        break;
    }
    int nResult = 0;
    if (GetWriteMessage(&pMessage, &nMessageSize))
    {
        nResult = SendServerToPlayerMessage(nPlayerId, MESSAGE_CC, nType,
            pMessage, nMessageSize);
    }
    return nResult;
}
// PC 0x00569B80  Xbox 0x0022CDC0
int CSWSMessage::SendServerToPlayerCheatNasty(unsigned long nPlayerId,
                                              unsigned long oidObject)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_CHEAT, 11, pMessage, nMessageSize);
}
// PC 0x00569BE0  Xbox 0x0022CE20
int CSWSMessage::SendServerToPlayerCheatDebugMode(int bDebugMode)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bDebugMode);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(CNetLayer::PLAYERID_ALL_PLAYERS, 15, 21,
                                     pMessage, nMessageSize);
}
// PC 0x00569C40  Xbox 0x0022CE80
int CSWSMessage::SendServerToPlayerGameObjUpdateVisEffect(CSWSPlayer* pPlayer,
    unsigned short nVisualEffectId, unsigned long oidTarget,
    unsigned long oidCreator, unsigned long oidBeamTarget,
    unsigned char nBeamType, unsigned char nBeamMode,
    Vector vTargetPosition, float fDuration)
{
    CSWSObject* pCreator = NULL;
    CSWSObject* pTarget = NULL;
    CGameObject* pCreatorObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidCreator);
    CGameObject* pTargetObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    unsigned long nMessageSize;
    unsigned char* pMessage;
    int nResult;

    if (pCreatorObject != NULL)
    {
        pCreator = pCreatorObject->AsSWSObject();
    }
    if (pTargetObject != NULL)
    {
        pTarget = pTargetObject->AsSWSObject();
    }

    CreateWriteMessage(6, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidTarget);
    WriteWORD(nVisualEffectId, 16);
    if (pTarget == NULL)
    {
        WriteFLOAT(vTargetPosition.m_fX, 1.0f, 32);
        WriteFLOAT(vTargetPosition.m_fY, 1.0f, 32);
        WriteFLOAT(vTargetPosition.m_fZ, 1.0f, 32);
    }
    else
    {
        WriteFLOAT(pTarget->m_vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(pTarget->m_vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(pTarget->m_vPosition.m_fZ, 1.0f, 32);
    }

    if (CLoopingVisualEffect::GetIsBeam(nVisualEffectId))
    {
        EXOXBOXASSERT(pCreator);
        WriteOBJECTIDServer(oidCreator);
        WriteOBJECTIDServer(oidBeamTarget);
        WriteBYTE(nBeamType, 8);
        WriteBYTE(nBeamMode, 8);
        WriteFLOAT(pCreator->m_vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(pCreator->m_vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(pCreator->m_vPosition.m_fZ, 1.0f, 32);
        if (oidTarget == OBJECT_INVALID)
        {
            WriteFLOAT(fDuration, 1.0f, 32);
        }
    }

    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    unsigned long nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId,
        MESSAGE_GAME_OBJECT_UPDATE, GAME_OBJECT_UPDATE_VISUAL_EFFECT,
        pMessage, nMessageSize);
}
// PC 0x00569E10  Xbox 0x0022D060
int CSWSMessage::SendServerToPlayerUpdateActiveItemProperties(
    CSWSPlayer* pPlayer, unsigned long oidItem, unsigned char nProperty,
    unsigned char nMask, unsigned char* pValues)
{
    unsigned long nPlayerId;
    unsigned long nPropertyMask;
    int nValue;
    int i;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(5, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidItem);
    WriteBYTE(nProperty, 8);
    WriteBYTE(nMask, 8);
    nPropertyMask = nMask;
    nValue = 0;
    for (i = 8; i != 0; i--)
    {
        if (((1 << nValue) & nPropertyMask) != 0)
        {
            WriteBYTE(pValues[nValue], 8);
        }
        nValue++;
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_ITEM_PROPERTY, 1, pMessage, nMessageSize);
}
// PC 0x00569EC0  Xbox 0x0022D110
int CSWSMessage::SendServerToPlayerVoiceChat_Play(CSWSPlayer* pPlayer,
                                                  unsigned long oidObject,
                                                  unsigned char nVolume)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = 5;
    CreateWriteMessage(5, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteBYTE(nVolume, 8);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_VOICE_CHAT, 1, pMessage, nMessageSize);
}
// PC 0x00569F40  Xbox 0x0022D190
int CSWSMessage::SendServerToPlayerGuiContainerObject_Open(
    CSWSPlayer* pPlayer, unsigned long oidObject, int bOpen)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = 8;
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteINT(bOpen, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_GUI_CONTAINER, 1, pMessage, nMessageSize);
}
// PC 0x00569FC0  Xbox 0x0022D210
int CSWSMessage::SendServerPlayerItemUpdate_DestroyItem(CSWSPlayer* pPlayer,
                                                        unsigned long oidItem)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidItem);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, 5, 7, pMessage, nMessageSize);
}
// PC 0x0056A020  Xbox 0x0022D270
int CSWSMessage::SendServerToPlayerJournalAddQuest(
    CSWSPlayer* pPlayer, const CExoString& sQuestTag, int nState,
    unsigned long nDate, unsigned short nTime, int bPicture,
    unsigned long nNameStrRef, unsigned long nTextStrRef, CExoLocString& lsName,
    CExoLocString& lsText, int nPriority, int nPercentComplete, int bComplete)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = sQuestTag.GetLength() + 22;
    CreateWriteMessage(nMessageSize, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoLocStringServer(lsName, 0);
    WriteCExoLocStringServer(lsText, 0);
    WriteDWORD(nNameStrRef, 32);
    WriteDWORD(nTextStrRef, 32);
    WriteCExoString(sQuestTag, 32);
    WriteINT(nState, 32);
    WriteDWORD(nDate, 32);
    WriteWORD(nTime, 16);
    WriteBOOL(bPicture);
    WriteINT(nPriority, 32);
    WriteINT(nPercentComplete, 32);
    WriteINT(bComplete, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 6, pMessage, nMessageSize);
}
// PC 0x0056A130  Xbox 0x0022D380
int CSWSMessage::SendServerToPlayerJournalRemoveQuest(
    CSWSPlayer* pPlayer, const CExoString& sQuestTag)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = sQuestTag.GetLength();
    CreateWriteMessage(nMessageSize, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sQuestTag, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 7, pMessage, nMessageSize);
}
// PC 0x0056A1A0  Xbox 0x0022D3F0
int CSWSMessage::SendServerToPlayerJournalAddWorld(
    CSWSPlayer* pPlayer, int nIndex, const CExoString& sTitle,
    const CExoString& sText, unsigned long nTag, unsigned long nPriority)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = sTitle.GetLength() + sText.GetLength() + 12;
    CreateWriteMessage(nMessageSize, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nIndex, 32);
    WriteCExoString(sTitle, 32);
    WriteCExoString(sText, 32);
    WriteDWORD(nTag, 32);
    WriteDWORD(nPriority, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 1, pMessage, nMessageSize);
}
// PC 0x0056A260  Xbox 0x0022D4B0
int CSWSMessage::SendServerToPlayerJournalAddWorldStrref(
    CSWSPlayer* pPlayer, unsigned long nIndex, unsigned long nTitleStrRef,
    unsigned long nTextStrRef, unsigned long nTagStrRef)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize = 16;
    unsigned char* pMessage;

    CreateWriteMessage(16, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nIndex, 32);
    WriteDWORD(nTitleStrRef, 32);
    WriteDWORD(nTextStrRef, 32);
    WriteDWORD(nTagStrRef, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 2, pMessage, nMessageSize);
}
// PC 0x0056A300  Xbox 0x0022D550
int CSWSMessage::SendServerToPlayerJournalDeleteWorld(CSWSPlayer* pPlayer,
                                                      int nIndex)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize = 4;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nIndex, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 3, pMessage, nMessageSize);
}
// PC 0x0056A370  Xbox 0x0022D5C0
int CSWSMessage::SendServerToPlayerJournalDeleteWorldStrref(
    CSWSPlayer* pPlayer, unsigned long nStrRef)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize = 4;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nStrRef, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 4, pMessage, nMessageSize);
}
// PC 0x0056A3E0  Xbox 0x0022D630
int CSWSMessage::SendServerToPlayerJournalDeleteWorldAll(CSWSPlayer* pPlayer)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize = 1;
    unsigned char* pMessage;

    CreateWriteMessage(1, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(1, 8);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 5, pMessage, nMessageSize);
}
// PC 0x0056A450  Xbox 0x0022D6A0
int CSWSMessage::SendServerToPlayerJournalFullUpdateNotNeeded(
    CSWSPlayer* pPlayer, int bFullUpdate)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    if (bFullUpdate != 0)
    {
        CreateWriteMessage(12, RECIPIENT_UNSPECIFIED, 1);
        WriteBOOL(1);
        WriteBOOL(1);
        WriteINT(0, 32);
    }
    else
    {
        CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
        WriteBOOL(0);
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 9, pMessage, nMessageSize);
}
// PC 0x0056A4E0  Xbox 0x0022D730
int CSWSMessage::SendServerToPlayerJournalUpdated(CSWSPlayer* pPlayer,
                                                  int bAdded, int bCompleted,
                                                  CExoLocString& lsJournalEntry)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = 8;
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteCExoLocStringServer(lsJournalEntry, 0);
    WriteBOOL(bAdded);
    WriteBOOL(bCompleted);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 12, pMessage, nMessageSize);
}
// PC 0x0056A560  Xbox 0x0022D7B0
int CSWSMessage::SendServerToPlayerLevelUp_Confirmation(unsigned long nPlayerId,
                                                        int bSuccess)
{
    if (bSuccess != 0)
    {
        return SendServerToPlayerMessage(nPlayerId, MESSAGE_LEVEL_UP, 1, NULL, 0);
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_LEVEL_UP, 2, NULL, 0);
}
// PC 0x0056A5F0  Xbox 0x0022D860
int CSWSMessage::SendServerToPlayerModuleUpdate_Time(
    CSWSPlayer* pPlayer, unsigned char nUpdateMask, unsigned long nTimeOfDay,
    unsigned long nDayOfMonth, unsigned long nMonth, unsigned long nYear,
    unsigned char nCalendar, unsigned long nCalendarDay)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(13, RECIPIENT_UNSPECIFIED, 1);
    WriteBYTE(nUpdateMask, 8);
    if ((nUpdateMask & 1) != 0)
    {
        WriteBYTE(nCalendar, 8);
        if (nCalendar == 3 || nCalendar == 4)
        {
            WriteDWORD(nCalendarDay, 32);
        }
    }
    if ((nUpdateMask & 2) != 0)
    {
        WriteBYTE(nYear, 8);
    }
    if ((nUpdateMask & 4) != 0)
    {
        WriteBYTE(nMonth, 8);
    }
    if ((nUpdateMask & 8) != 0)
    {
        WriteBYTE(nDayOfMonth, 8);
    }
    if ((nUpdateMask & 16) != 0)
    {
        WriteDWORD(nTimeOfDay, 32);
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_MODULE, 3, pMessage, nMessageSize);
}
// PC 0x0056A6D0  Xbox 0x0022D940
int CSWSMessage::SendServerToPlayerMapPinEnabled(CSWSPlayer* pPlayer,
                                                 unsigned long oidMapPin,
                                                 int bEnabled)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidMapPin);
    WriteBOOL(bEnabled);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_MAP_PIN, 1, pMessage, nMessageSize);
}
// PC 0x0056A740  Xbox 0x0022D9B0
int CSWSMessage::SendServerToPlayerMapPinAdded(CSWSPlayer* pPlayer,
                                               Vector vPosition,
                                               const CExoString& sNote,
                                               unsigned long nMapPin)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(sNote.GetLength() + 16, RECIPIENT_UNSPECIFIED, 1);
    WriteFLOAT(vPosition.m_fX, 1.0f, 32);
    WriteFLOAT(vPosition.m_fY, 1.0f, 32);
    WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    WriteCExoString(sNote, 32);
    WriteDWORD(nMapPin, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_MAP_PIN, 4, pMessage, nMessageSize);
}
// PC 0x0056A7F0  Xbox 0x0022DA60
int CSWSMessage::SendServerToPlayerDebugInfo_Item(CSWSPlayer* pPlayer,
                                                  unsigned long oidObject)
{
    CSWSItem* pItem;
    CSWItemProperty* pPassiveProperty;
    int i;
    unsigned char* pMessage;
    unsigned long nMessageSize;
    unsigned long nPlayerId;

    pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidObject);
    if (pItem == NULL)
    {
        return 0;
    }
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteCExoString(pItem->m_sTag, 32);
    WriteINT(pItem->m_nPassiveProperties, 32);
    for (i = 0; i < pItem->m_nPassiveProperties; i++)
    {
        pPassiveProperty = pItem->GetPassiveProperty(i);
        if (pPassiveProperty != NULL)
        {
            WriteWORD(pPassiveProperty->m_nPropertyType, 16);
        }
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, 0x21, 2, pMessage,
                                     nMessageSize);
}

#include "nwstrigger.h"
// PC 0x0056A8C0  Xbox 0x0022DB30
int CSWSMessage::SendServerToPlayerDebugInfo_Placeable(CSWSPlayer* pPlayer,
                                                     unsigned long oidObject)
{
    CSWSPlaceable* pPlaceable;
    CServerExoAppInternal* pServer;
    CFactionManager* pFactionManager;
    CSWSFaction* pFaction;
    unsigned char* pMessage;
    unsigned long nMessageSize;
    unsigned long nPlayerId;

    pPlaceable = g_pAppManager->GetServerExoApp()->GetPlaceableByGameObjectID(oidObject);
    if (pPlaceable == NULL)
    {
        return 0;
    }
    pServer = g_pAppManager->GetServerExoApp()->m_pcExoAppInternal;
    if (pServer == NULL)
    {
        return 0;
    }
    pFactionManager = pServer->m_pFactionManager;
    if (pFactionManager == NULL)
    {
        return 0;
    }
    int nFaction = pPlaceable->m_nFaction;
    pFaction = pFactionManager->GetFaction(nFaction);
    if (pFaction == NULL)
    {
        return 0;
    }
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteCExoLocStringServer(*pPlaceable->GetFirstName(), 0);
    WriteCExoString(pPlaceable->m_sTag, 32);
    WriteBYTE(pPlaceable->m_nHardness, 8);
    WriteSHORT(pPlaceable->GetCurrentHitPoints(1), 16);
    WriteBYTE(pPlaceable->m_nFortSave, 8);
    WriteBYTE(pPlaceable->m_nReflexSave, 8);
    WriteBYTE(pPlaceable->m_nWillSave, 8);
    WriteCResRef(pPlaceable->m_cTemplateResRef, 16);
    WriteCExoString(pFaction->m_sName, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_DEBUG_INFO,
                                     DEBUG_INFO_PLACEABLE, pMessage,
                                     nMessageSize);
}
// PC 0x0056AA30  Xbox 0x0022DCA0
int CSWSMessage::SendServerToPlayerDebugInfo_Area(CSWSPlayer* pPlayer,
                                                  unsigned long oidObject)
{
    CSWSArea* pArea;
    unsigned char* pMessage;
    unsigned long nMessageSize;
    unsigned long nPlayerId;

    pArea = g_pAppManager->m_pServerExoApp->GetAreaByGameObjectID(oidObject);
    if (pArea == NULL)
    {
        return 0;
    }
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidObject);
    WriteCExoString(pArea->m_sTag, 32);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, 0x21, 4, pMessage,
                                     nMessageSize);
}
// PC 0x0056AAC0  Xbox 0x0022DD30
int CSWSMessage::SendServerToPlayerPopUpGUIPanel(unsigned long oidPC,
    int nGUIPanel, int bRespawnButtonEnabled, int bWaitForHelpButtonEnabled,
    int nHelpStringReference, const CExoString& sHelpString)
{
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPC);
    if (pPlayer != NULL)
    {
        if (nGUIPanel == GUI_PANEL_PLAYER_DEATH)
        {
            CSWSCreature* pCreature =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPC);
            if (pCreature != NULL && pCreature->m_pStats->m_bIsPC)
            {
                CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
                WriteBOOL(bRespawnButtonEnabled);
                WriteBOOL(bWaitForHelpButtonEnabled);
                WriteINT(nHelpStringReference, 32);
                WriteCExoString(sHelpString, 32);
                if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
                {
                    return 0;
                }
                unsigned long nPlayerId = pPlayer->m_nPlayerId;
                return SendServerToPlayerMessage(nPlayerId,
                    MESSAGE_POP_UP_GUI, POP_UP_GUI_PLAYER_DEATH,
                    pMessage, nMessageSize);
            }
        }
        else if (nGUIPanel == GUI_PANEL_PARTY_INVITATION_RECEIVED)
        {
            CSWSCreature* pCreature =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPC);
            if (pCreature != NULL && pCreature->m_pStats->m_bIsPC)
            {
                CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
                WriteCExoString(sHelpString, 32);
                if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
                {
                    return 0;
                }
                unsigned long nPlayerId = pPlayer->m_nPlayerId;
                return SendServerToPlayerMessage(nPlayerId,
                    MESSAGE_POP_UP_GUI, POP_UP_GUI_CUSTOM,
                    pMessage, nMessageSize);
            }
        }
    }
    return 1;
}
// PC 0x0056AC20  Xbox 0x0022DE90
int CSWSMessage::HasValidString(CExoLocString& lsString, unsigned char nGender)
{
    CExoString sString;
    int nPlayerLanguage;
    int nModuleLanguage;

    if (g_pAppManager->GetServerExoApp() == NULL)
    {
        return 0;
    }
    nPlayerLanguage =
        g_pAppManager->GetServerExoApp()->GetPlayerLanguage(m_nRecipient);
    nModuleLanguage = g_pAppManager->GetServerExoApp()->GetModuleLanguage();

    if (!lsString.GetStringLoc(nPlayerLanguage, &sString, nGender) &&
        (nPlayerLanguage == 0 ||
         !lsString.GetStringLoc(0, &sString, nGender)) &&
        (nModuleLanguage == nPlayerLanguage || nModuleLanguage == 0 ||
         !lsString.GetStringLoc(nModuleLanguage, &sString, nGender)))
    {
        unsigned char nStringGender;

        if (!lsString.GetString(0, &nPlayerLanguage, &sString,
                                &nStringGender) &&
            (int)lsString.m_dwStringRef != -1)
        {
            return 1;
        }
    }
    if (sString.GetLength() > 0)
    {
        return 1;
    }
    return 0;
}
// PC 0x0056AD60  Xbox 0x0022DFE0
int CSWSMessage::SendServerToPlayerAmbientMusicPlay(unsigned long nPlayerId,
                                                   int bPlay)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bPlay);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 1, pMessage, nMessageSize);
}
// PC 0x0056ADC0  Xbox 0x0022E040
int CSWSMessage::SendServerToPlayerAmbientMusicSetDelay(unsigned long nPlayerId,
                                                        int nDelay)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nDelay, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 2, pMessage, nMessageSize);
}
// PC 0x0056AE20  Xbox 0x0022E0A0
int CSWSMessage::SendServerToPlayerAmbientMusicChangeTrack(
    unsigned long nPlayerId, int bDay, int nTrack)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bDay);
    WriteINT(nTrack, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 3, pMessage, nMessageSize);
}
// PC 0x0056AE90  Xbox 0x0022E110
int CSWSMessage::SendServerToPlayerAmbientBattleMusicPlay(
    unsigned long nPlayerId, int bPlay)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bPlay);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 4, pMessage, nMessageSize);
}
// PC 0x0056AEF0  Xbox 0x0022E170
int CSWSMessage::SendServerToPlayerAmbientBattleMusicChange(
    unsigned long nPlayerId, int nTrack)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteINT(nTrack, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 5, pMessage, nMessageSize);
}
// PC 0x0056AF50  Xbox 0x0022E1D0
int CSWSMessage::SendServerToPlayerAmbientSoundLoopPlay(unsigned long nPlayerId,
                                                        int bPlay)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bPlay);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 6, pMessage, nMessageSize);
}
// PC 0x0056AFB0  Xbox 0x0022E230
int CSWSMessage::SendServerToPlayerAmbientSoundLoopChange(
    unsigned long nPlayerId, int bDay, int nTrack)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bDay);
    WriteINT(nTrack, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 7, pMessage, nMessageSize);
}
// PC 0x0056B020  Xbox 0x0022E2A0
int CSWSMessage::SendServerToPlayerAmbientSoundVolumeChange(
    unsigned long nPlayerId, int bDay, int nVolume)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bDay);
    WriteINT(nVolume, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_AMBIENT, 8, pMessage, nMessageSize);
}
// PC 0x0056B090  Xbox 0x0022E310
int CSWSMessage::SendServerToPlayerCharacterDownloadReply(CSWSPlayer* pPlayer)
{
    int bRestorePlot = 0;
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    if (pPlayer->GetGameObject() != NULL)
    {
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature != NULL)
        {
            if (pCreature->m_bRestorePlotOnDownload)
            {
                pCreature->m_bPlotObject = 0;
                bRestorePlot = 1;
            }
            if (pPlayer->PackCreatureIntoMessage() == 0)
            {
                if (bRestorePlot)
                    pCreature->m_bPlotObject = 1;
                return 0;
            }
            if (bRestorePlot)
                pCreature->m_bPlotObject = 1;
            if (GetWriteMessage(&pMessage, &nMessageSize) != 0)
            {
                nPlayerId = pPlayer->m_nPlayerId;
                return SendServerToPlayerMessage(nPlayerId,
                    MESSAGE_CHARACTER_DOWNLOAD, CHARACTER_DOWNLOAD_REPLY,
                    pMessage, nMessageSize);
            }
        }
    }
    return 0;
}
// PC 0x0056B140  Xbox 0x0022E3C0
int CSWSMessage::SendServerToPlayerCharacterDownloadFail(CSWSPlayer* pPlayer)
{
    CExoString sCharacter;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    if (pPlayer->SaveServerCharacter() == 0)
        return 0;
    pPlayer->m_cServerCharacter.CopyToString(sCharacter);
    CreateWriteMessage(sCharacter.GetLength(), RECIPIENT_UNSPECIFIED, 1);
    WriteCExoString(sCharacter, 32);
    char nCharacterType = pPlayer->m_nCharacterType;
    WriteBOOL(nCharacterType == CHARACTER_TYPE_SERVER_PLAYER);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
        return 0;
    return SendServerToPlayerMessage(pPlayer->GetPlayerId(),
        MESSAGE_CHARACTER_DOWNLOAD, CHARACTER_DOWNLOAD_FILE,
        pMessage, nMessageSize);
}
// PC 0x0056B240  Xbox 0x0022E4C0
int CSWSMessage::SendServerToPlayerInventory_LearnScroll(
    unsigned long nPlayerId, unsigned long oidItem, unsigned char nMinor)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidItem);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    SendServerToPlayerMessage(nPlayerId, MESSAGE_INVENTORY, nMinor, pMessage, nMessageSize);
    return 1;
}
// PC 0x0056B2B0  Xbox 0x0022E530
int CSWSMessage::SendServerToPlayerShutDownServer(unsigned long nPlayerId,
                                                  unsigned long nReason)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteDWORD(nReason, 32);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    SendServerToPlayerMessage(nPlayerId, MESSAGE_SHUTDOWN_SERVER, 0, pMessage, nMessageSize);
    return 1;
}
// PC 0x0056B320  Xbox 0x0022E5A0
int CSWSMessage::SendServerToPlayerDestroyDeathGUI(unsigned long nPlayerId)
{
    SendServerToPlayerMessage(nPlayerId, MESSAGE_POP_UP_GUI, 2, NULL, 0);
    return 1;
}
// PC 0x0056B380  Xbox 0x0022E620
int CSWSMessage::SendServerToPlayerGuiTimingEvent(CSWSPlayer* pPlayer,
                                                  int bStarted,
                                                  unsigned char nEventType,
                                                  unsigned long nEventId)
{
    unsigned long nPlayerId;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(8, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bStarted);
    if (bStarted != 0)
    {
        WriteDWORD(nEventId, 32);
        WriteBYTE(nEventType, 8);
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_GUI_TIMING_EVENT, 1, pMessage, nMessageSize);
}
// PC 0x0056B400  Xbox 0x0022E6A0
int CSWSMessage::SendServerToPlayerPlayModuleCharacterListResponse(
    unsigned long nPlayerId, unsigned long oidPlayer, int bStart)
{
    unsigned long nMessageSize;
    unsigned char* pMessage;

    nMessageSize = 4;
    CreateWriteMessage(4, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(bStart);
    WriteDWORD(oidPlayer, 32);
    if (bStart)
    {
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                oidPlayer);
        if (pCreature == NULL)
        {
            return 0;
        }

        CExoLocString lsFirstName;
        CExoLocString lsLastName;
        unsigned char nGender = pCreature->GetGender();
        lsFirstName = *pCreature->GetFirstName();
        lsLastName = pCreature->GetLastName();
        WriteCExoLocStringServer(lsFirstName, nGender);
        WriteCExoLocStringServer(lsLastName, nGender);

        unsigned short nPortraitId = pCreature->GetPortraitId();
        WriteWORD(nPortraitId, 16);
        if (nPortraitId == PORTRAIT_ID_RESERVED_FFFF ||
            nPortraitId == PORTRAIT_ID_RESERVED_FFFE)
        {
            WriteCResRef(pCreature->GetPortrait(), 16);
        }

        CSWSCreatureStats* pStats = pCreature->m_pStats;
        unsigned char nClassCount = pStats->m_nClassCount;
        WriteBYTE(nClassCount, 8);
        for (int nMultiClass = 0; nMultiClass < nClassCount; nMultiClass++)
        {
            unsigned char nClass = pCreature->m_pStats->GetClass(nMultiClass);
            unsigned char nLevel =
                pCreature->m_pStats->GetClassLevel(nMultiClass, 0);
            WriteBYTE(nClass, 8);
            WriteBYTE(nLevel, 8);
        }
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    return SendServerToPlayerMessage(nPlayerId,
                                     MESSAGE_PLAY_MODULE_CHARACTER_LIST,
                                     PLAY_MODULE_CHARACTER_LIST_RESPONSE,
                                     pMessage, nMessageSize);
}
// PC 0x0056B610  Xbox 0x0022E8C0
int CSWSMessage::SendServerToPlayerGUICharacterSheet_NotPermitted(
    unsigned long nPlayerId, unsigned long oidCreature)
{
    int nResult;
    unsigned long nMessageSize;
    unsigned char* pMessage;
    CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(oidCreature);
    nResult = GetWriteMessage(&pMessage, &nMessageSize);
    if (nResult == 0)
    {
        return nResult;
    }
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_GUI_CHARACTER_SHEET, 2, pMessage, nMessageSize);
}
// PC 0x0056B670  Xbox 0x0022E920
int CSWSMessage::SendServerToPlayerItemScriptAdd(unsigned long nPlayerId,
                                                 CSWSArea* pArea,
                                                 CSWSItem* pItem,
                                                 Vector vPosition)
{
    unsigned long nMessageSize;
    unsigned char* pMessage;

    CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
    WriteOBJECTIDServer(pItem->m_nId);
    AddItemAppearanceToMessage(pItem);
    AddActiveItemPropertiesToMessage(pItem, NULL);
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
    {
        return 0;
    }
    return SendServerToPlayerMessage(nPlayerId, 5, 8, pMessage, nMessageSize);
}
// PC 0x00576FC0  Xbox 0x0023A310
void CSWSMessage::StoreValuesInLastUpdateObject(
    CSWSPlayer* pPlayer, CLastUpdateObject* pLastUpdateObject,
    CSWSObject* pObject, unsigned long nUpdateMask,
    unsigned long nAppearanceMask)
{
    UpdateLastUpdateObject(pPlayer, pObject, pLastUpdateObject, nUpdateMask);
    UpdateLastUpdateObjectAppearance(pObject, pLastUpdateObject,
                                            nAppearanceMask);
}
// PC 0x00577000  Xbox 0x0023A350
CLastUpdateObject* CSWSMessage::CreateNewLastUpdateObject(CSWSPlayer* pPlayer,
    CSWSObject* pObject, unsigned long* pnUpdateMask,
    unsigned long* pnAppearanceMask)
{
    CLastUpdateObject* pLastUpdateObject = new CLastUpdateObject;

    pLastUpdateObject->m_nUnknown24 = 1;
    pLastUpdateObject->m_oidObject = pObject->m_nId;
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = (CSWSCreature*)pObject;
        pCreature->UpdateAppearanceForEquippedItems();
        pLastUpdateObject->m_nAppearance = pCreature->m_cAppearanceInfo.m_nAppearance;
        pLastUpdateObject->m_nAppearancePart1 = pCreature->m_cAppearanceInfo.m_nAppearancePart1;
        pLastUpdateObject->m_nAppearancePart3 = pCreature->m_cAppearanceInfo.m_nAppearancePart3;
        pLastUpdateObject->m_nAppearancePart6 = pCreature->m_cAppearanceInfo.m_nAppearancePart6;
        pLastUpdateObject->m_nAppearancePart0 = pCreature->m_cAppearanceInfo.m_nAppearancePart0;
        pLastUpdateObject->m_nAppearancePart2 = pCreature->m_cAppearanceInfo.m_nAppearancePart2;
        pLastUpdateObject->m_nAppearancePart7 = pCreature->m_cAppearanceInfo.m_nAppearancePart7;
        pLastUpdateObject->m_nAppearancePart4 = pCreature->m_cAppearanceInfo.m_nAppearancePart4;
        pLastUpdateObject->m_nAppearancePart5 = pCreature->m_cAppearanceInfo.m_nAppearancePart5;
        pLastUpdateObject->m_nAppearancePart8 = pCreature->m_cAppearanceInfo.m_nAppearancePart8;
        pLastUpdateObject->m_oidAppearanceArmour = pCreature->m_cAppearanceInfo.m_oidAppearanceArmour;
        pLastUpdateObject->m_oidAppearanceHead = pCreature->m_cAppearanceInfo.m_oidAppearanceHead;
        pLastUpdateObject->m_oidAppearanceLeftHand = pCreature->m_cAppearanceInfo.m_oidAppearanceLeftHand;
        pLastUpdateObject->m_oidAppearanceRightHand = pCreature->m_cAppearanceInfo.m_oidAppearanceRightHand;
        pLastUpdateObject->m_nAppearanceState = pCreature->m_cAppearanceInfo.m_bUseBackupHead;
        pLastUpdateObject->m_nAppearanceVariant = pCreature->m_cAppearanceInfo.m_nAppearanceHead;
        pLastUpdateObject->m_nUnknown2A = pCreature->m_nSoundSetFile;
        for (int i = 0; i < CLastUpdateObjectRecord::NUM_RECORDS; i++)
        {
            pLastUpdateObject->m_aRecords[i].m_nUnknown4 = 0;
            pLastUpdateObject->m_aRecords[i].m_oidObject =
                CGameObjectArray::INVALIDOBJID;
            pLastUpdateObject->m_aRecords[i].m_nUnknown8 = CLastUpdateObjectRecord::UNSET_VALUE;
            pLastUpdateObject->m_aRecords[i].m_nUnknownC = 0;
        }
        *pnAppearanceMask = APPEARANCE_UPDATE_ALL;
    }
    else
    {
        memset(pLastUpdateObject, 0, 36);
    }

    unsigned char nObjectType = pObject->m_nObjectType;
    if (nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        *pnUpdateMask = NEW_OBJECT_UPDATE_DOOR;
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        *pnUpdateMask = NEW_OBJECT_UPDATE_ITEM;
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        *pnUpdateMask = NEW_OBJECT_UPDATE_AREA_EFFECT;
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        *pnUpdateMask = NEW_OBJECT_UPDATE_TRIGGER;
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        *pnUpdateMask = NEW_OBJECT_UPDATE_PLACEABLE;
    }
    else
    {
        *pnUpdateMask = NEW_OBJECT_UPDATE_CREATURE;
        if (pPlayer != NULL)
        {
            pPlayer->GetGameObject();
        }
    }

    UpdateLastUpdateObject(pPlayer, pObject, pLastUpdateObject, *pnUpdateMask);
    WriteCHAR(GAME_OBJECT_RECORD_ADD, 8);
    WriteBYTE(nObjectType, 8);
    WriteOBJECTIDServer(pObject->GetId());
    if (nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        WriteBYTE(pObject->AsSWSCreature()->GetInParty() != 0, 8);
        WriteFLOAT(pObject->m_vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(pObject->m_vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(pObject->m_vPosition.m_fZ, 1.0f, 32);
        WriteFLOAT(pObject->m_vOrientation.m_fX, 1.0f, 32);
        WriteFLOAT(pObject->m_vOrientation.m_fY, 1.0f, 32);
        WriteFLOAT(pObject->m_vOrientation.m_fZ, 1.0f, 32);
        WriteWORD(pObject->m_nAnimation, 16);
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        AddItemAppearanceToMessage(pObject->AsSWSItem());
        AddActiveItemPropertiesToMessage(pObject->AsSWSItem(),
            pPlayer->GetGameObject()->AsSWSCreature());
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        AddDoorAppearanceToMessage(pObject->AsSWSDoor());
        int bHostile;
        if (pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) <
                CSWSObject::MIN_FRIENDLY_REPUTATION &&
            pObject->AsSWSDoor()->GetFactionId() !=
                pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId())
        {
            bHostile = 1;
        }
        else
        {
            bHostile = 0;
        }
        WriteCExoLocStringServer(*pObject->AsSWSDoor()->GetFirstName(), 0);
        WriteBOOL(bHostile);
        WriteWORD(pObject->AsSWSDoor()->GetAnimation(), 16);
        WriteBOOL(pObject->AsSWSDoor()->m_bLocked);
        WriteBOOL(pObject->AsSWSDoor()->m_bLockable);
        WriteBOOL(pObject->m_bPlotObject);
        WriteBOOL(pObject->AsSWSDoor()->GetIsLinked());
        WriteCExoLocStringServer(
            pObject->AsSWSDoor()->GetTransitionDestination(), 0);
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        AddAreaOfEffectObjectToMessage(pObject->AsSWSAreaOfEffectObject());
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        WriteCExoLocStringServer(*pObject->AsSWSTrigger()->GetFirstName(), 0);
        WriteBOOL(pObject->AsSWSTrigger()->m_bMineTrigger);
        WriteBOOL(pObject->AsSWSTrigger()->m_nTriggerType);
        if (pObject->AsSWSTrigger()->m_bMineTrigger)
        {
            WriteBYTE(CSWSTrigger::CURSOR_MINE, 8);
        }
        else
        {
            WriteBYTE(pObject->AsSWSTrigger()->GetCursor(), 8);
        }
        if (pObject->AsSWSTrigger()->m_bMineTrigger)
        {
            WriteBOOL(pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) <
                    CSWSObject::MIN_FRIENDLY_REPUTATION &&
                pObject->AsSWSTrigger()->GetFactionId() !=
                    pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId());
            WriteBYTE(pObject->AsSWSTrigger()->GetTrapType(), 8);
        }
        WriteFLOAT(pObject->AsSWSTrigger()->GetHighlightHeight(), 1.0f, 32);
        WriteCExoLocStringServer(
            pObject->AsSWSTrigger()->GetTransitionDestination(), 0);
        AddTriggerGeometryToMessage(pObject->AsSWSTrigger());
    }
    else if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        WriteCExoLocStringServer(*pObject->AsSWSPlaceable()->GetFirstName(), 0);
        WriteBOOL(pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) <
                CSWSObject::MIN_FRIENDLY_REPUTATION &&
            pObject->AsSWSPlaceable()->GetFactionId() !=
                pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId());
        AddPlaceableAppearanceToMessage(pObject->AsSWSPlaceable());
    }
    pPlayer->m_pActiveObjectsLastUpdate->AddHead(pLastUpdateObject);
    return pLastUpdateObject;
}
// PC 0x00577670  Xbox 0x0023A9D0
void CSWSMessage::WriteGameObjUpdate_MajorGUIPanels(CSWSPlayer* pPlayer)
{
    CExoArrayList<unsigned long> lstEquipmentSlots;
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        CSWSPlayerLastUpdateObject* pPlayerUpdate = pPlayer->m_pLastUpdateObject;
        CLastUpdateObject* pObjectUpdate = pPlayer->GetLastUpdateObject(pObject->GetId());
        if (pObjectUpdate != NULL)
        {
            unsigned long nUpdateMask = 0;
            if (pPlayerUpdate == NULL)
            {
                pPlayerUpdate = pPlayer->CreateNewPlayerLastUpdateObject();
                ComputeLastUpdate_GuiKnownSpells(pObject->AsSWSCreature(), pPlayerUpdate);

                pObject->AsSWSCreature();
                for (unsigned char nClass = 0; nClass < 2; nClass++)
                {
                }
                nUpdateMask = PLAYER_UPDATE_ALL;
            }
            else
            {
                TestPlayerUpdateDifferences(pPlayer, pPlayerUpdate, pObjectUpdate, nUpdateMask);
            }
            if (nUpdateMask != 0)
            {
                WriteGameObjUpdate_PlayerUpdate(pPlayer, pPlayerUpdate, pObjectUpdate, nUpdateMask);
                StoreValuesInLastPlayerUpdateObject(pPlayer, pPlayerUpdate, pObjectUpdate, nUpdateMask);
            }
        }
        int bInventoryOpen = pPlayer->GetInventoryGUIInfo()->m_bOpen;
        if (bInventoryOpen == 1)
        {
            unsigned long oidContainer = pPlayer->GetContainerGUIInfo()->m_oidContainer;
            if (oidContainer != CGameObjectArray::INVALIDOBJID)
            {
                CGameObject* pContainer;
                CGameObjectArray* pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
                if (pObjectArray->GetGameObject(
                        oidContainer, &pContainer) == CGameObjectArray::SUCCESS)
                {
                    CItemRepository* pRepository;
                    if (pContainer->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
                    {
                        CSWSItem* pItem = pContainer->AsSWSItem();
                        pRepository = pItem->m_pRepository;
                    }
                    unsigned char nObjectType = pContainer->m_nObjectType;
                    if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                    {
                        CSWSPlaceable* pPlaceable = pContainer->AsSWSPlaceable();
                        pRepository = pPlaceable->m_pcItemRepository;
                    }
                    CSWSPlayerLUOInventory* pLastUpdate = pPlayer->GetInventoryGUIInfo()->m_pLastUpdateObject;
                    WriteRepositoryUpdate(pPlayer, pObject, pRepository,
                        pLastUpdate, 0, GUI_UPDATE_CONTAINER);
                }
            }
            else
            {
#ifndef _XBOX
                int bNPCInventory = 0;
                unsigned long oidPlayerObject = pPlayer->m_oidNWSObject;
                CSWSObject* pPlayerObject = pObject;
                if (g_pAppManager->m_pServerExoApp->GetServerInfo()->m_bPartyPicking && g_pAppManager->m_pClientExoApp->GetInGameGui()->GetPartyAccessPanelUp(1))
                {
                    int nNPC = g_pAppManager->m_pClientExoApp->GetInGameGui()->GetCurrentEquipScreenNPC(); unsigned long oidNPC = g_pAppManager->m_pServerExoApp->GetPartyTable()->GetNPCObject(nNPC, 0, 1);
                    pObject = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidNPC);
                    if (pObject != NULL)
                    {
                        bNPCInventory = 1;
                        pPlayer->m_oidNWSObject = oidNPC;
                    }
                    else
                    {
                        pObject = pPlayerObject;
                    }
                }
#endif
                for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; nSlot++)
                {
                    if (ComputeInventoryUpdateRequired(pPlayer, nSlot) == 1)
                    {
                        UpdateLastUpdateInventory(pPlayer, nSlot);
                        unsigned long nSlotMask = 1 << nSlot;
                        CSWInventory* pInventory = pObject->AsSWSCreature()->m_pInventory;
                        if (pInventory->GetItemInSlot(nSlotMask) == NULL)
                        {
                            WriteCHAR(GAME_OBJECT_RECORD_GUI_ELEMENTS, 8);
                            WriteCHAR(GUI_UPDATE_INVENTORY, 8);
                            WriteCHAR(GAME_OBJECT_RECORD_DELETE, 8);
                            WriteDWORD(nSlotMask, 32);
                        }
                        else
                        {
                            lstEquipmentSlots.Add(nSlotMask);
                        }
                    }
                }
                for (int nItem = 0; nItem < lstEquipmentSlots.num; nItem++)
                {
                    WriteCHAR(GAME_OBJECT_RECORD_GUI_ELEMENTS, 8);
                    WriteCHAR(GUI_UPDATE_INVENTORY, 8);
                    WriteCHAR(GAME_OBJECT_RECORD_ADD, 8);
                    WriteDWORD(lstEquipmentSlots[nItem], 32);
                    CSWInventory* pInventory = pObject->AsSWSCreature()->m_pInventory;
                    CSWSItem* pItem = pInventory->GetItemInSlot(lstEquipmentSlots[nItem]);
                    WriteOBJECTIDServer(pItem->GetId());
                    AddItemAppearanceToMessage(pItem);
                    AddActiveItemPropertiesToMessage(pItem, pObject->AsSWSCreature());
                }
#ifndef _XBOX
                if (bNPCInventory)
                {   pPlayer->m_oidNWSObject = oidPlayerObject;
                    pObject = pPlayerObject; }
#endif
            }
            CItemRepository* pRepository = pObject->AsSWSCreature()->GetItemRepository(1);
            CSWSPlayerLUOInventory* pLastUpdate = pPlayer->GetInventoryGUIInfo()->m_pLastUpdateObject;
            WriteRepositoryUpdate(pPlayer, pObject, pRepository,
                pLastUpdate, 1, GUI_UPDATE_REPOSITORY);
        }
    }
}
// PC 0x00574400  Xbox 0x00237790
void CSWSMessage::StoreValuesInLastPlayerUpdateObject(CSWSPlayer* pPlayer,
    CSWSPlayerLastUpdateObject* pPlayerLastUpdate,
    CLastUpdateObject* pLastUpdateObject, unsigned long nUpdateMask)
{
    CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
    if (pCreature != NULL)
    {
        if (nUpdateMask & PLAYER_UPDATE_CHARACTER)
        {
            pPlayerLastUpdate->m_nArmorClass = pCreature->GetArmorClass();
            pPlayerLastUpdate->m_nGold = pCreature->GetGold();
            pPlayerLastUpdate->m_nEncumbranceState = pCreature->m_nEncumbranceState;
            pPlayerLastUpdate->m_bCanLevelUp = pCreature->GetStats()->CanLevelUp();
            pPlayerLastUpdate->m_nLevel = pCreature->GetStats()->GetLevel(0);
            pPlayerLastUpdate->m_nExperience = pCreature->GetStats()->m_nExperience;
            pCreature->m_bComputeWeightCarried = 0;
        }
        if (nUpdateMask & PLAYER_UPDATE_GUI_SKILLS)
            pPlayerLastUpdate->m_nGuiSkills = ComputeLastUpdate_GuiSkills(pCreature);
        if (nUpdateMask & PLAYER_UPDATE_STATE)
            pPlayerLastUpdate->m_nPlayerState = ComputeLastUpdate_PlayerState(pCreature);
        if (nUpdateMask & PLAYER_UPDATE_GUI_FEATS)
        {
            pPlayerLastUpdate->m_lstFeats.num = 0;
            for (int i = 0; i < (unsigned short)pCreature->GetStats()->GetNumFeats(); i++)
                pPlayerLastUpdate->m_lstFeats.Add(pCreature->GetStats()->GetFeat(i));
            pPlayerLastUpdate->m_lstBonusFeats.num = 0;
            for (int i = 0; i < (unsigned short)pCreature->GetStats()->m_lstBonusFeats.num; i++)
                pPlayerLastUpdate->m_lstBonusFeats.Add(pCreature->GetStats()->GetBonusFeat(i));
            for (int i = pPlayerLastUpdate->m_lstFeatUses.num - 1; i >= 0; i--)
            {
                delete pPlayerLastUpdate->m_lstFeatUses[i];
                pPlayerLastUpdate->m_lstFeatUses[i] = NULL;
                pPlayerLastUpdate->m_lstFeatUses.DelIndex(i);
            }
            CExoArrayList<CFeatUseListEntry*>* pFeatUses = &pCreature->GetStats()->m_lstFeatUses;
            for (int i = 0; i < pFeatUses->num; i++)
            {
                CFeatUseListEntry* pEntry = new CFeatUseListEntry;
                pEntry->m_nFeat = (*pFeatUses)[i]->m_nFeat;
                pEntry->m_nUses = (*pFeatUses)[i]->m_nUses;
                pPlayerLastUpdate->m_lstFeatUses.Add(pEntry);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_KNOWN_SPELLS)
        {
            pPlayerLastUpdate->ClearKnownSpells();
            for (unsigned char nClass = 0; nClass < CSWSCreatureStats::MAX_CLASSES; nClass++)
            {
                for (unsigned char nSpell = 0;
                     nSpell < pCreature->GetStats()->GetNumberKnownSpells(nClass, 0); nSpell++)
                {
                    pPlayerLastUpdate->AddKnownSpell(nClass, 0,
                        pCreature->GetStats()->GetKnownSpell(nClass, 0, nSpell));
                }
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_ACTION_QUEUE)
            UpdateLastUpdateActionQueue(pCreature, pPlayerLastUpdate);
        if (nUpdateMask & PLAYER_UPDATE_VISIBILITY)
            UpdateLastUpdateVisibilityList(pCreature, pPlayerLastUpdate);
        if ((nUpdateMask & PLAYER_UPDATE_ABILITIES) && pLastUpdateObject != NULL && pCreature->GetStats() != NULL)
        {
            pLastUpdateObject->m_nStrengthBase = pCreature->GetStats()->m_nStrengthBase;
            pLastUpdateObject->m_nDexterityBase = pCreature->GetStats()->m_nDexterityBase;
            pLastUpdateObject->m_nConstitutionBase = pCreature->GetStats()->m_nConstitutionBase;
            pLastUpdateObject->m_nIntelligenceBase = pCreature->GetStats()->m_nIntelligenceBase;
            pLastUpdateObject->m_nWisdomBase = pCreature->GetStats()->m_nWisdomBase;
            pLastUpdateObject->m_nCharismaBase = pCreature->GetStats()->m_nCharismaBase;
            pLastUpdateObject->m_nStrength = pCreature->GetStats()->GetSTRStat();
            pLastUpdateObject->m_nDexterity = pCreature->GetStats()->GetDEXStat();
            pLastUpdateObject->m_nConstitution = pCreature->GetStats()->GetCONStat();
            pLastUpdateObject->m_nIntelligence = pCreature->GetStats()->GetINTStat();
            pLastUpdateObject->m_nWisdom = pCreature->GetStats()->GetWISStat();
            pLastUpdateObject->m_nCharisma = pCreature->GetStats()->GetCHAStat();
        }
        if ((nUpdateMask & PLAYER_UPDATE_SKILLS) && pLastUpdateObject != NULL && pCreature->GetStats() != NULL)
        {
            for (int nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
            {
                pLastUpdateObject->m_nSkillBase[nSkill] = pCreature->GetStats()->GetSkillRank(nSkill, NULL, 1);
                pLastUpdateObject->m_nSkill[nSkill] = pCreature->GetStats()->GetSkillRank(nSkill, NULL, 0);
            }
        }
        if (nUpdateMask & PLAYER_UPDATE_PORTRAIT)
        {
            pLastUpdateObject->m_nPortraitId = pCreature->GetPortraitId();
            pLastUpdateObject->m_resRef = pCreature->GetPortrait();
        }
        if (nUpdateMask & PLAYER_UPDATE_HIT_POINTS)
        {
            pLastUpdateObject->m_nBaseMaxHitPoints = pCreature->m_nMaxHitPoints;
            pLastUpdateObject->m_nCurrentHitPoints = pCreature->GetCurrentHitPoints(0);
            pLastUpdateObject->m_nTemporaryHitPoints = pCreature->m_nTemporaryHitPoints;
            pLastUpdateObject->m_nMaxHitPoints = pCreature->GetMaxHitPoints(1);
        }
        if (nUpdateMask & PLAYER_UPDATE_FORCE_POINTS)
        {
            pLastUpdateObject->m_nForcePoints = pCreature->GetStats()->m_nForcePoints;
            pLastUpdateObject->m_nCurrentForcePoints = pCreature->GetStats()->GetCurrentForcePoints();
            pLastUpdateObject->m_nTemporaryForcePoints = pCreature->GetStats()->m_nTemporaryForcePoints;
            pLastUpdateObject->m_nMaxForcePoints = pCreature->GetMaxForcePoints();
        }
        if (nUpdateMask & PLAYER_UPDATE_SPELL_LIKE_ABILITIES)
            ComputeLastUpdate_StoreUpdateSpellLikeAbility(pCreature, pPlayerLastUpdate);
    }
}
// PC 0x005779F0  Xbox 0x0023ACA0
int CSWSMessage::WriteGameObjUpdate_PartyAIState(CSWSPlayer* pPlayer)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
    CSWSCreature* pPlayerCreature = pPlayerObject->AsSWSCreature();
    CSWSFaction* pFaction = pPlayerCreature->GetFaction();
    if (pFaction != NULL)
    {
        unsigned long* pMembers;
        unsigned long nMembers = pFaction->GetMemberList(&pMembers);
        for (unsigned long nMember = 0; nMember < nMembers; nMember++)
        {
            if (pMembers[nMember] == pPlayerObject->m_nId)
            {
                continue;
            }

            CGameObject* pGameObject;
            if (pObjectArray->GetGameObject(pMembers[nMember], &pGameObject) !=
                CGameObjectArray::SUCCESS)
            {
                continue;
            }

            CSWSCreature* pCreature = pGameObject->AsSWSCreature();
            if (pCreature->m_nUnknown39C == 1)
            {
                continue;
            }

            CLastUpdatePartyObject* pLastUpdateObject = NULL;
            unsigned long nTestMask;
            TestPartyObjectUpdateDifferences(pPlayer, pCreature,
                                             &pLastUpdateObject, &nTestMask);
            unsigned long nUpdateMask = nTestMask;
            if (nUpdateMask != 0)
            {
                WriteGameObjUpdate_UpdateObject(pPlayer, pCreature, NULL,
                                                nUpdateMask, 0);
            }
            StoreValuesInLastUpdatePartyObject(pPlayerCreature,
                                               pLastUpdateObject, pCreature,
                                               nUpdateMask);
        }
    }

    CExoLinkedListNode* pNode = pPlayer->m_pPartyObjectsLastUpdate->GetHeadPos();
    while (pNode != NULL)
    {
        CLastUpdatePartyObject* pLastUpdateObject =
            pPlayer->m_pPartyObjectsLastUpdate->GetAtPos(pNode);
        if (pLastUpdateObject->m_bUpdated == FALSE)
        {
            CExoLinkedListNode* pDeleteNode = pNode;
            pPlayer->m_pPartyObjectsLastUpdate->GetNext(pNode);
            pPlayer->m_pPartyObjectsLastUpdate->Remove(pDeleteNode);
            delete pLastUpdateObject;
        }
        else
        {
            pLastUpdateObject->m_bUpdated = FALSE;
            pPlayer->m_pPartyObjectsLastUpdate->GetNext(pNode);
        }
    }
    return TRUE;
}
// PC 0x00574A10  Xbox 0x00237DA0
void CSWSMessage::WriteGameObjUpdate_UpdateObject(CSWSPlayer* pPlayer,
    CSWSObject* pObject, CLastUpdateObject* pLastUpdateObject,
    unsigned long nUpdateMask, unsigned long nAppearanceMask)
{
    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
    if (pPlayerObject == NULL)
        return;
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE &&
        pLastUpdateObject != NULL && nAppearanceMask != 0)
        WriteGameObjUpdate_UpdateAppearance(pObject, pLastUpdateObject,
                                            nAppearanceMask);
    WriteCHAR(GAME_OBJECT_RECORD_UPDATE, 8);
    WriteBYTE(pObject->m_nObjectType, 8);
    WriteOBJECTIDServer(pObject->GetId());
    WriteDWORD(nUpdateMask, 32);
    if (nUpdateMask & OBJECT_UPDATE_LOCATION)
    {
        Vector vPosition = pObject->m_vPosition;
        WriteFLOAT(vPosition.m_fX, 1.0f, 32);
        WriteFLOAT(vPosition.m_fY, 1.0f, 32);
        WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
    }
    if (nUpdateMask & OBJECT_UPDATE_ORIENTATION)
    {
        Vector vOrientation = pObject->m_vOrientation;
        WriteFLOAT(vOrientation.m_fX, 1.0f, 32);
        WriteFLOAT(vOrientation.m_fY, 1.0f, 32);
        WriteFLOAT(vOrientation.m_fZ, 1.0f, 32);
        if (pObject->AsSWSCreature() != NULL)
        {
            if (pObject->AsSWSCreature()->GetLockOrientationToObject() != CGameObjectArray::INVALIDOBJID)
            {
                WriteBYTE(1, 8);
                WriteOBJECTIDServer(pObject->AsSWSCreature()->GetLockOrientationToObject());
            }
            else
                WriteBYTE(0, 8);
        }
    }
    if (nUpdateMask & OBJECT_UPDATE_PORTRAIT)
    {
        unsigned short nPortrait = pObject->GetPortraitId();
        WriteWORD(nPortrait, 16);
        if (nPortrait >= PORTRAIT_ID_RESERVED_FFFE)
            WriteCResRef(pObject->GetPortrait(), 16);
    }
    unsigned short nAnimation;
    int bAnimation = nUpdateMask & OBJECT_UPDATE_ANIMATION;
    if (nUpdateMask & OBJECT_UPDATE_ANIMATION)
    {
        float fSyncHeight = pObject->m_fSyncHeight;
        WriteFLOAT(fSyncHeight, 1.0f, 32);
    }
    if (bAnimation)
    {
        nAnimation = pObject->GetAnimation();
        WriteWORD(nAnimation, 16);
        if (pObject->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();

            if (nAnimation == 10009 || nAnimation == 10109)
            {
#ifdef _XBOX
                int nCurrentAttack = pCreature->GetCombatRound()->m_nCurrentAttack;
                EXOXBOXASSERT(nCurrentAttack >= 0);
                int nAttacks = nCurrentAttack + 1;
#else
                int nAttacks = pCreature->GetCombatRound()->m_nCurrentAttack + 1;
#endif
                WriteBYTE(nAttacks, 2);
                for (int i = 0; i < nAttacks; i++)
                {
                    unsigned long oidReactObject = pCreature->m_pcCombatRound->GetAttack(i)->m_oidReactObject;
                    WriteOBJECTIDServer(oidReactObject);
                    unsigned short nReactionAnimation = pCreature->m_pcCombatRound->GetAttack(i)->m_nReactionAnimation;
                    WriteWORD(nReactionAnimation, 16);
                    unsigned short nReactionAnimationLength = pCreature->m_pcCombatRound->GetAttack(i)->m_nReactionAnimationLength;
                    WriteWORD(nReactionAnimationLength, 16);
                    unsigned short nAnimationLength = pCreature->m_pcCombatRound->GetAttack(i)->m_nAnimationLength;
                    WriteWORD(nAnimationLength, 16);
                    unsigned char nResult = pCreature->m_pcCombatRound->GetAttack(i)->m_nAttackResult;
                    WriteBYTE(nResult, 4);
                    unsigned short nAttackType = pCreature->m_pcCombatRound->GetAttack(i)->m_nAttackType;
                    WriteWORD(nAttackType, 16);
                    unsigned short nDamage = pCreature->m_pcCombatRound->GetAttack(i)->GetTotalDamage(1);
                    WriteWORD(nDamage, 9);
                    int bRangedAttack = pCreature->m_pcCombatRound->GetAttack(i)->m_bRangedAttack;
                    WriteBOOL(bRangedAttack);
                    int bKillingBlow = pCreature->m_pcCombatRound->GetAttack(i)->m_bKillingBlow;
                    WriteBOOL(bKillingBlow);
                    unsigned char nWeaponType = pCreature->m_pcCombatRound->GetAttack(i)->m_nWeaponAttackType;
                    WriteBYTE(nWeaponType, 4);
                }
            }
            if (nAnimation == 10015 || nAnimation == 10016 || nAnimation == 11000 ||
                nAnimation == 10017 || nAnimation == 10018 || nAnimation == 10019 ||
                nAnimation == 10020 || nAnimation == 10061 || nAnimation == 10401)
            {
                int nSpell = pCreature->m_nLastSpellId;
                WriteDWORD(nSpell, 32);
                if (nSpell == CSWSpell::FORCE_POWER_LIGHT_SABER_THROW)
                {
                    CSWSItem* pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                    if (pItem != NULL)
                        WriteOBJECTIDServer(pItem->GetId());
                    else
                        WriteOBJECTIDServer(OBJECT_INVALID);
                }
                else
                {
                    unsigned long oidItem = pCreature->m_oidSpellCasterItem;
                    WriteOBJECTIDServer(oidItem);
                }
            }
            if (nAnimation == 10017 || nAnimation == 10018 || nAnimation == 10019 ||
                nAnimation == 10020 || nAnimation == 10061 || nAnimation == 10401)
            {
                if (pCreature->m_oidSpellTargetObject != OBJECT_INVALID)
                {
                    WriteBYTE(1, 8);
                    unsigned long oidTarget = pCreature->m_oidSpellTargetObject;
                    WriteOBJECTIDServer(oidTarget);
                }
                else
                {
                    WriteBYTE(2, 8);
                    WriteFLOAT(pCreature->m_vSpellTargetPosition.m_fX, 1.0f, 32);
                    WriteFLOAT(pCreature->m_vSpellTargetPosition.m_fY, 1.0f, 32);
                    WriteFLOAT(pCreature->m_vSpellTargetPosition.m_fZ, 1.0f, 32);
                }
                unsigned long nProjectileTime = pCreature->m_nLastSpellProjectileTime;
                WriteDWORD(nProjectileTime, 32);
            }
        }
    }
    if (nUpdateMask & OBJECT_UPDATE_VISUAL_EFFECTS)
    {
        CExoArrayList<CLoopingVisualEffect*>* pEffects = &pObject->m_lstLoopingVisualEffects;
        int nEffects = pEffects->num;
        int nLastEffects = pLastUpdateObject->m_lstLoopingVisualEffects.num;
        CExoArrayList<unsigned __int64> lstChanges;
        CExoArrayList<unsigned short> lstAdded;
        CLoopingVisualEffect* pEffect;
        unsigned short nEffect;
        int bFound;
        int i;
        int j;
        for (i = 0; i < nEffects; i++)
        {
            pEffect = (*pEffects)[i];
            nEffect = pEffect->m_nVisualEffectId;
            bFound = 0;
            if (pEffect->GetIsBeam())
            {
                for (j = 0; j < lstChanges.num; j++)
                {
                    unsigned __int64 nChange = lstChanges[j];
                    CLoopingVisualEffect* pAdded = (CLoopingVisualEffect*)(unsigned long)nChange;
                    if (pAdded->m_nVisualEffectId == nEffect &&
                        pAdded->m_nBeamFlag == pEffect->m_nBeamFlag &&
                        pAdded->m_oidCreator == pEffect->m_oidCreator)
                    {
                        bFound = 1;
                        break;
                    }
                }
                if (bFound)
                    continue;
            }
            else if (lstAdded.Contains(nEffect) > 0)
                continue;
            for (j = 0; j < nLastEffects && !bFound; j++)
            {
                if (pEffect->GetIsBeam())
                {
                    CLoopingVisualEffect* pLast = pLastUpdateObject->m_lstLoopingVisualEffects[j];
                    if (pLast->m_nVisualEffectId == nEffect &&
                        pLast->m_nBeamFlag == pEffect->m_nBeamFlag &&
                        pLast->m_oidCreator == pEffect->m_oidCreator)
                        bFound = 1;
                }
                else if (pLastUpdateObject->m_lstLoopingVisualEffects[j]->m_nVisualEffectId == nEffect)
                    bFound = 1;
            }
            if (!bFound)
            {
                unsigned __int64 nChange = (unsigned __int64)(unsigned long)pEffect;
                nChange |= (unsigned __int64)GAME_OBJECT_RECORD_ADD << 32;
                lstChanges.AddUnique(nChange);
                lstAdded.Add(nEffect);
            }
        }
        for (i = 0; i < nLastEffects; i++)
        {
            pEffect = pLastUpdateObject->m_lstLoopingVisualEffects[i];
            nEffect = pEffect->m_nVisualEffectId;
            bFound = 0;
            for (j = 0; j < nEffects && !bFound; j++)
            {
                if (pEffect->GetIsBeam())
                {
                    CLoopingVisualEffect* pCurrent = (*pEffects)[j];
                    if (pCurrent->m_nVisualEffectId == nEffect &&
                        pCurrent->m_nBeamFlag == pEffect->m_nBeamFlag &&
                        pCurrent->m_oidCreator == pEffect->m_oidCreator)
                        bFound = 1;
                }
                else if ((*pEffects)[j]->m_nVisualEffectId == nEffect)
                    bFound = 1;
            }
            if (!bFound)
            {
                unsigned __int64 nChange = (unsigned __int64)(unsigned long)pEffect;
                nChange |= (unsigned __int64)GAME_OBJECT_RECORD_DELETE << 32;
                lstChanges.AddUnique(nChange);
            }
        }
        int nChanges = lstChanges.num;
        WriteWORD(nChanges, 16);
        for (int i = 0; i < lstChanges.num; i++)
        {
            CLoopingVisualEffect* pEffect = (CLoopingVisualEffect*)(unsigned long)lstChanges[i];
            unsigned short nEffect = pEffect->m_nVisualEffectId;
            WriteBYTE((unsigned char)((unsigned long*)&lstChanges[i])[1], 8);
            WriteWORD(nEffect, 16);
            if (pEffect->GetIsBeam())
            {
                WriteOBJECTIDServer(pEffect->m_oidCreator);
                WriteBYTE(pEffect->m_nBeamFlag, 8);
            }
        }
    }
    if (nUpdateMask & OBJECT_UPDATE_TRAP_STATE)
    {
        int bTrap = 0;
        int bHostile = 0;
        int bLocked = 0;
        int bPlot = 0;
        if (pObject->AsSWSTrigger() != NULL)
        {
            if (pObject->AsSWSTrigger()->m_bMineTrigger)
            {
                if (pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) < CSWSObject::MIN_FRIENDLY_REPUTATION)
                    if (pObject->AsSWSTrigger()->GetFactionId() !=
                        pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId())
                        bHostile = 1;
                WriteBOOL(bHostile);
            }
        }
        else if (pObject->AsSWSDoor() != NULL)
        {
            if (pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) < CSWSObject::MIN_FRIENDLY_REPUTATION)
                if (pObject->AsSWSDoor()->GetFactionId() !=
                    pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId())
                    bHostile = 1;
            if ((unsigned char)pObject->AsSWSDoor()->m_bTrapFlagged ||
                pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) >= CSWSObject::MIN_FRIENDLY_REPUTATION ||
                pObject->AsSWSDoor()->GetFactionId() == pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId() ||
                pObject->AsSWSDoor()->m_lstTrapDetectedBy.Contains(pPlayer->GetGameObject()->AsSWSCreature()->m_nId))
            {
                if (pObject->AsSWSDoor()->m_bTrapFlag)
                    bTrap = 1;
            }
            if (pObject->AsSWSDoor()->m_bLocked == 1)
                bLocked = 1;
            if (pObject->AsSWSDoor()->m_bPlotObject == 1)
                bPlot = 1;
            WriteBOOL(bHostile);
            WriteBOOL(bTrap);
            WriteBOOL(bLocked);
            WriteBOOL(bPlot);
        }
        else if (pObject->AsSWSPlaceable() != NULL)
        {
            if (pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) < CSWSObject::MIN_FRIENDLY_REPUTATION)
                if (pObject->AsSWSPlaceable()->GetFactionId() !=
                    pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId())
                    bHostile = 1;
            if ((unsigned char)pObject->AsSWSPlaceable()->m_bTrapFlagged ||
                pObject->GetReputation(pPlayer->GetGameObject()->GetId(), 0) >= CSWSObject::MIN_FRIENDLY_REPUTATION ||
                pObject->AsSWSPlaceable()->GetFactionId() == pPlayer->GetGameObject()->AsSWSCreature()->GetFactionId() ||
                pObject->AsSWSPlaceable()->m_lstTrapDetectedBy.Contains(pPlayer->GetGameObject()->AsSWSCreature()->m_nId))
            {
                if (pObject->AsSWSPlaceable()->m_nTrapFlag)
                    bTrap = 1;
            }
            if (pObject->AsSWSPlaceable()->m_bLocked == 1)
                bLocked = 1;
            if (pObject->AsSWSPlaceable()->m_bPlotObject == 1)
                bPlot = 1;
            WriteBOOL(bHostile);
            WriteBOOL(bTrap);
            WriteBOOL(bLocked);
            WriteBOOL(bPlot);
        }
    }
    if (pObject->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
        return;
    CSWSCreature* pCreature = (CSWSCreature*)pObject;
    if (nUpdateMask & OBJECT_UPDATE_ANIMATION)
    {
        unsigned char nPathType = pCreature->m_pPathfindInformation->m_nField_28;
        if (pObject->m_nId == pPlayerObject->m_nId)
        {
            if (nPathType != 0 && pCreature->m_bJumped == 1)
            {
                pCreature->m_bJumped = 0;
                nPathType = 0;
            }
        }
        else
            nPathType = 0;
        WriteBYTE(nPathType, 8);
    }
    if (bAnimation)
    {
        CPathfindInformation* pPath = pCreature->m_pPathfindInformation;
        if (pObject->AnimationStationary(nAnimation) == 1)
            WriteWORD(0, 16);
        else
        {
            unsigned short nPoints = pPath->m_nWayPointCount / 2;
            int nFirst = pPath->m_nField_9C / 2;
            if (nPoints >= nFirst)
            {
                nPoints -= nFirst;
                WriteWORD(nPoints, 16);
                if (nPoints > 0)
                {
                    WriteBOOL(pCreature->m_bUpdateMovingOrientation);
                    if (pCreature->m_bUpdateMovingOrientation)
                    {
                    float fPathTime = (float)(pCreature->m_fClientPathTime * 1.0f);
                    WriteFLOAT(fPathTime, 1.0f, 32);
                }
                }
                nPoints += nFirst;
            }
            else
                WriteWORD(0, 16);
            if (nPoints != 0)
            {
                float* pPoints = pPath->m_pWayPoints;
                for (int i = nFirst; i < nPoints; i++)
                {
                    WriteFLOAT(pPoints[2 * i], 1.0f, 32);
                    WriteFLOAT(pPoints[2 * i + 1], 1.0f, 32);
                    WriteFLOAT(0.0f, 1.0f, 32);
                }
            }
        }
    }
    if (nUpdateMask & OBJECT_UPDATE_AI_STATE)
    {
        unsigned short nFlags = pCreature->m_nAIStateFlags;
        unsigned short nActivity = pCreature->m_nActivity;
        unsigned char nState = pCreature->m_nAIState;
        int bCombat = pCreature->m_bCombatState;
        unsigned char nStealth = pCreature->m_nStealthMode;
        int nStealthValue = pCreature->m_nStealthModeValue;
        unsigned char nReputation;
        if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
            pPlayer->GetGameObject()->AsSWSCreature() != NULL)
            nReputation = pPlayer->GetGameObject()->AsSWSCreature()->GetAIStateReputation(pObject->m_nId);
        else
            nReputation = 2;
        WriteWORD(nFlags, 16);
        WriteBYTE(nState, 8);
        WriteWORD(nActivity, 16);
        WriteBYTE(nReputation, 8);
        WriteBYTE(nStealth, 8);
        WriteBOOL(nStealthValue);
        WriteBOOL(bCombat);
        if (nState == 2)
            WriteOBJECTIDServer(pCreature->m_oidAITarget);
    }
    if (nUpdateMask & OBJECT_UPDATE_MOVEMENT_RATES)
    {
        WriteFLOAT(pCreature->GetWalkRate(), 1.0f, 32);
        WriteFLOAT(pCreature->GetRunRate(), 1.0f, 32);
    }
    if (nUpdateMask & OBJECT_UPDATE_AREA)
        WriteOBJECTIDServer(pObject->m_oidArea);
    if (nUpdateMask & OBJECT_UPDATE_POSITION)
    {
        WriteINT((unsigned long)pObject->m_vPosition.m_fX, 10);
        WriteINT((unsigned long)pObject->m_vPosition.m_fY, 10);
    }
    if (nUpdateMask & OBJECT_UPDATE_HIT_POINTS)
    {
        WriteSHORT(pObject->GetCurrentHitPoints(1), 16);
        WriteSHORT(pObject->m_nMaxHitPoints, 16);
        WriteSHORT(pObject->m_nTemporaryHitPoints, 16);
        WriteSHORT(pObject->GetMaxHitPoints(1), 16);
    }
    if (nUpdateMask & OBJECT_UPDATE_FORCE_POINTS)
    {
        WriteSHORT(pCreature->m_pStats->m_nCurrentForcePoints, 16);
        WriteSHORT(pCreature->m_pStats->m_nForcePoints, 16);
        WriteSHORT(pCreature->m_pStats->m_nTemporaryForcePoints, 16);
        WriteSHORT(pCreature->GetMaxForcePoints(), 16);
    }
    if (nUpdateMask & OBJECT_UPDATE_NAME)
    {
        WriteCExoLocStringServer(pCreature->m_pStats->m_lsFirstName, 0);
        WriteCExoLocStringServer(pCreature->m_pStats->m_lsLastName, 0);
    }
    if (nUpdateMask & OBJECT_UPDATE_PARTY_STATE)
    {
        WriteBOOL(pCreature->m_nAreaWaitState);
        WriteBOOL((pCreature->m_nAIStateFlags & CSWSCreature::AI_STATE_FLAG_0040) == CSWSCreature::AI_STATE_FLAG_0040);
        WriteBOOL(pCreature->GetFaction()->GetSingletonParty());
        WriteBOOL(pCreature->GetFaction()->GetLeader() == pObject->GetId());
        WriteBOOL(pCreature->m_bInvitedToParty);
        if (pCreature->m_bInvitedToParty)
        {
            WriteOBJECTIDServer(pCreature->m_oidInvitedBy);
            CSWSCreature* pInviter = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(pCreature->m_oidInvitedBy);
            WriteCExoLocStringServer(pInviter->m_pStats->m_lsFirstName, 0);
            WriteCExoLocStringServer(pInviter->m_pStats->m_lsLastName, 0);
        }
        WriteBOOL(pCreature->m_bDiseased);
        WriteBOOL(pCreature->m_nPoisonState);
    }
    if (nUpdateMask & OBJECT_UPDATE_PERCEPTION)
    {
        CSWSCreature* pPlayerCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pPlayerCreature != NULL)
        {
            if (pObject->m_nId == pPlayerCreature->m_nId)
            {
                WriteBOOL(1);
                WriteBOOL(1);
                CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
                if (pModule != NULL)
                    WriteBOOL(pModule->m_lstInvisibleCreatures.Contains(pObject->GetId()));
                else
                    WriteBOOL(0);
            }
            else
            {
                CSWVisibilityNode* pNode = pCreature->GetVisibleListElement(pPlayerCreature->m_nId);
                if (pNode != NULL)
                {
                    WriteBOOL(pNode->m_bHeard);
                    WriteBOOL(pNode->m_bSeen);
                    WriteBOOL(pNode->m_bInvisible);
                }
                else
                {
                    WriteBOOL(0);
                    WriteBOOL(0);
                    WriteBOOL(0);
                }
            }
        }
    }
    if (nUpdateMask & OBJECT_UPDATE_ASSOCIATE_STATE)
    {
        unsigned char nModes = 0;
        if (pCreature->GetDetectMode() == 1) nModes |= CREATURE_MODE_DETECT;
        if (pCreature->m_nStealthMode == 1) nModes |= CREATURE_MODE_STEALTH;
        unsigned char nCombatMode = pCreature->m_nActiveCombatMode;
        if (nCombatMode == SWSOBJECT_COMBAT_MODE_PARRY) nModes |= CREATURE_MODE_PARRY;
        if (nCombatMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK) nModes |= CREATURE_MODE_POWER_ATTACK;
        if (nCombatMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK) nModes |= CREATURE_MODE_IMPROVED_POWER_ATTACK;
        if (nCombatMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS) nModes |= CREATURE_MODE_FLURRY;
        if (nCombatMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT) nModes |= CREATURE_MODE_RAPID_SHOT;
        if (nCombatMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL) nModes |= CREATURE_MODE_COUNTERSPELL;
        WriteBYTE(nModes, 8);
    }
    if (nUpdateMask & OBJECT_UPDATE_DAMAGE_LEVEL)
        WriteBYTE(pObject->GetDamageLevel(), 8);
    if (nUpdateMask & OBJECT_UPDATE_CHARACTER_DETAILS)
    {
        WriteWORD(pCreature->m_pStats->m_nRace, 16);
        WriteCExoString(pCreature->m_pStats->m_sSubrace, 32);
        WriteBYTE(pCreature->m_pStats->m_nSubraceIndex, 8);
        WriteBYTE(pCreature->m_pStats->m_nGender, 8);
        WriteSHORT(pCreature->m_pStats->m_nGoodEvilAlignment, 16);
        WriteBOOL(pObject->m_nF4);
        WriteBYTE(pCreature->m_pStats->m_nClassCount, 8);
        for (int i = 0; i < pCreature->m_pStats->m_nClassCount; i++)
        {
            WriteBYTE(pCreature->m_pStats->GetClass(i), 8);
            WriteBYTE(pCreature->m_pStats->GetClassLevel(i, 0), 8);
        }
    }
}
// PC 0x00575C80  Xbox 0x00239010
void CSWSMessage::UpdateLastUpdateObject(
    CSWSPlayer* pPlayer, CSWSObject* pObject,
    CLastUpdateObject* pLastUpdateObject, unsigned long nUpdateMask)
{
    pLastUpdateObject->m_nObjectType = pObject->m_nObjectType;
    if (nUpdateMask & OBJECT_UPDATE_ANIMATION)
        pLastUpdateObject->m_fUnknown50 = pObject->GetAnimationSpeed();
    if (nUpdateMask & OBJECT_UPDATE_ANIMATION)
        pLastUpdateObject->m_nAnimation = pObject->m_nAnimation;
    if (nUpdateMask & OBJECT_UPDATE_LOCATION)
    {
        pLastUpdateObject->m_vUnknown30 = pObject->m_vPosition;
        if (pObject->GetArea() != NULL)
            pLastUpdateObject->m_nUnknown3C = pObject->GetArea()->GetId();
        else
            pLastUpdateObject->m_nUnknown3C = OBJECT_INVALID;
    }
    if (nUpdateMask & OBJECT_UPDATE_ORIENTATION)
    {
        pLastUpdateObject->m_vOrientation = pObject->m_vOrientation;
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
            pLastUpdateObject->m_oidLockOrientationToObject =
                pObject->AsSWSCreature()->GetLockOrientationToObject();
        if (pObject->m_nAnimation == 10009 ||
            pObject->m_nAnimation == CREATURE_ANIMATION_MELEE_SEQUENCE)
            pLastUpdateObject->m_oidAttackTarget =
                pObject->AsSWSCreature()->m_oidAttackTarget;
    }
    if (nUpdateMask & OBJECT_UPDATE_PORTRAIT)
    {
        pLastUpdateObject->m_nPortraitId = pObject->GetPortraitId();
        pLastUpdateObject->m_resRef = pObject->GetPortrait();
    }
    if (nUpdateMask & OBJECT_UPDATE_VISUAL_EFFECTS)
        AssignVisualEffectLists(&pLastUpdateObject->m_lstLoopingVisualEffects,
                                &pObject->m_lstLoopingVisualEffects);
    if (nUpdateMask & OBJECT_UPDATE_TRAP_STATE)
    {
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            CExoArrayList<unsigned long>* pTrapDetectedBy =
                &pObject->AsSWSDoor()->m_lstTrapDetectedBy;
            AssignCreatureLists(pTrapDetectedBy, &pLastUpdateObject->m_lstTrapDetectedBy);
            pLastUpdateObject->m_bTrapFlag = pObject->AsSWSDoor()->m_bTrapFlag;
            pLastUpdateObject->m_bTrapFlagged =
                (unsigned char)pObject->AsSWSDoor()->m_bTrapFlagged;
            pLastUpdateObject->m_bLocked = pObject->AsSWSDoor()->m_bLocked;
            pLastUpdateObject->m_bPlotObject = pObject->AsSWSDoor()->GetPlotObject();
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CExoArrayList<unsigned long>* pTrapDetectedBy =
                &pObject->AsSWSPlaceable()->m_lstTrapDetectedBy;
            AssignCreatureLists(pTrapDetectedBy, &pLastUpdateObject->m_lstTrapDetectedBy);
            pLastUpdateObject->m_bTrapFlag = pObject->AsSWSPlaceable()->m_nTrapFlag;
            pLastUpdateObject->m_bTrapFlagged =
                (unsigned char)pObject->AsSWSPlaceable()->m_bTrapFlagged;
            pLastUpdateObject->m_bLocked = pObject->AsSWSPlaceable()->m_bLocked;
            pLastUpdateObject->m_bPlotObject = pObject->AsSWSPlaceable()->GetPlotObject();
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            if (pObject->AsSWSTrigger()->GetTrapFlag())
            {
                pLastUpdateObject->m_nFaction = pObject->AsSWSTrigger()->GetFactionId();
                pLastUpdateObject->m_bTrapFlagged = pObject->AsSWSTrigger()->m_bTrapFlagged;
            }
        }
    }
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE &&
        (nUpdateMask & OBJECT_UPDATE_LOCATION))
    {
        CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
        pLastUpdateObject->m_bDeleteNoFade = pPlaceable->m_bDeleteNoFade;
        pLastUpdateObject->m_nFadeOutTime = pPlaceable->m_nFadeOutTime;
        pLastUpdateObject->m_bKeepCorpse = pPlaceable->m_bKeepCorpse;
    }
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (nUpdateMask & OBJECT_UPDATE_ANIMATION)
            pLastUpdateObject->m_nClientSidePathType =
                pCreature->m_pPathfindInformation->m_nField_28;
        if (nUpdateMask & OBJECT_UPDATE_AI_STATE)
        {
            pLastUpdateObject->m_nAIStateFlags = pCreature->m_nAIStateFlags;
            pLastUpdateObject->m_nAIState = pCreature->m_nAIState;
            pLastUpdateObject->m_nActivity = pCreature->m_nActivity;
            pLastUpdateObject->m_oidAITarget = pCreature->m_oidAITarget;
            pLastUpdateObject->m_bCombatState = pCreature->m_bCombatState;
            pLastUpdateObject->m_nStealthMode = pCreature->m_nStealthMode;
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                pPlayer->GetGameObject()->AsSWSCreature() != NULL)
                pLastUpdateObject->m_nAIStateReputation =
                    pPlayer->GetGameObject()->AsSWSCreature()->GetAIStateReputation(pCreature->GetId());
            else
                pLastUpdateObject->m_nAIStateReputation = CSWSObject::AI_STATE_REPUTATION_DEFAULT;
        }
        if ((nUpdateMask & OBJECT_UPDATE_ANIMATION) &&
            !pCreature->AnimationStationary(pLastUpdateObject->m_nAnimation))
        {
            CPathfindInformation* pPath = pCreature->m_pPathfindInformation;
            unsigned short nPoints = pPath->m_nWayPointCount / 2;
            if (pPath->m_pWayPoints != NULL && nPoints != 0)
                pLastUpdateObject->m_vUnknown54 = Vector(
                    pPath->m_pWayPoints[(nPoints - 1) * 2],
                    pPath->m_pWayPoints[(nPoints - 1) * 2 + 1], 0.0f);
        }
        if (nUpdateMask & OBJECT_UPDATE_MOVEMENT_RATES)
        {
            pLastUpdateObject->m_fWalkRate = pCreature->GetWalkRate();
            pLastUpdateObject->m_fRunRate = pCreature->GetRunRate();
        }
        if (nUpdateMask & OBJECT_UPDATE_DAMAGE_LEVEL)
            pLastUpdateObject->m_nDamageLevel = pCreature->GetDamageLevel();
        if (nUpdateMask & OBJECT_UPDATE_HIT_POINTS)
        {
            pLastUpdateObject->m_nCurrentHitPoints = pCreature->GetCurrentHitPoints(1);
            pLastUpdateObject->m_nBaseMaxHitPoints = pCreature->m_nMaxHitPoints;
            pLastUpdateObject->m_nTemporaryHitPoints = pCreature->m_nTemporaryHitPoints;
            pLastUpdateObject->m_nMaxHitPoints = pCreature->GetMaxHitPoints(1);
        }
        if (nUpdateMask & OBJECT_UPDATE_FORCE_POINTS)
        {
            pLastUpdateObject->m_nCurrentForcePoints = pCreature->m_pStats->m_nCurrentForcePoints;
            pLastUpdateObject->m_nForcePoints = pCreature->m_pStats->m_nForcePoints;
            pLastUpdateObject->m_nTemporaryForcePoints = pCreature->m_pStats->m_nTemporaryForcePoints;
            pLastUpdateObject->m_nMaxForcePoints = pCreature->GetMaxForcePoints();
        }
        if (nUpdateMask & OBJECT_UPDATE_PARTY_STATE)
        {
            pLastUpdateObject->m_nAreaWaitState = pCreature->m_nAreaWaitState;
            pLastUpdateObject->m_bAIStateFlag0040 =
                (pCreature->m_nAIStateFlags & CSWSCreature::AI_STATE_FLAG_0040) == CSWSCreature::AI_STATE_FLAG_0040;
            pLastUpdateObject->m_bSingletonParty = pCreature->GetFaction()->GetSingletonParty();
            pLastUpdateObject->m_bPartyLeader = pCreature->GetFaction()->GetLeader() == pCreature->GetId();
            pLastUpdateObject->m_bInvitedToParty = pCreature->m_bInvitedToParty;
        }
        if (nUpdateMask & OBJECT_UPDATE_PERCEPTION)
        {
            CSWSCreature* pPlayerCreature = pPlayer->GetGameObject()->AsSWSCreature();
            if (pPlayerCreature != NULL)
            {
                if (pPlayerCreature->GetId() == pCreature->GetId())
                {
                    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
                    if (pModule != NULL)
                        pLastUpdateObject->m_bInvisible = pModule->m_lstInvisibleCreatures.Contains(pCreature->GetId());
                }
                else
                {
                    CSWVisibilityNode* pNode = pCreature->GetVisibleListElement(pPlayerCreature->GetId());
                    if (pNode != NULL)
                    {
                        pLastUpdateObject->m_bHeard = pNode->m_bHeard;
                        pLastUpdateObject->m_bSeen = pNode->m_bSeen;
                        pLastUpdateObject->m_bInvisible = pNode->m_bInvisible;
                    }
                    else
                    {
                        pLastUpdateObject->m_bHeard = 0;
                        pLastUpdateObject->m_bSeen = 0;
                        pLastUpdateObject->m_bInvisible = 0;
                    }
                }
            }
        }
        if (nUpdateMask & OBJECT_UPDATE_ASSOCIATE_STATE)
            pLastUpdateObject->m_nAssociateState = ComputeLastUpdate_AssociateState(pCreature);
        if (nUpdateMask & OBJECT_UPDATE_CHARACTER_DETAILS)
        {
            pLastUpdateObject->m_nRace = pCreature->m_pStats->m_nRace;
            pLastUpdateObject->m_sTag = pCreature->m_pStats->m_sSubrace;
            pLastUpdateObject->m_nSubraceIndex = pCreature->m_pStats->m_nSubraceIndex;
            pLastUpdateObject->m_nGender = pCreature->m_pStats->m_nGender;
            pLastUpdateObject->m_nGoodEvilAlignment = pCreature->m_pStats->m_nGoodEvilAlignment;
            pLastUpdateObject->m_nObjectF4 = pCreature->m_nF4;
            for (int i = 0; i < pCreature->m_pStats->GetNumClasses(); i++)
            {
                pLastUpdateObject->m_nClass[i] = pCreature->m_pStats->GetClass(i);
                pLastUpdateObject->m_nClassLevel[i] = pCreature->m_pStats->GetClassLevel(i, 0);
            }
        }
        if (nUpdateMask & OBJECT_UPDATE_CORPSE)
        {
            pLastUpdateObject->m_bDeleteNoFade = pCreature->m_bDeleteNoFade;
            pLastUpdateObject->m_nFadeOutTime = pCreature->m_nFadeOutTime;
            pLastUpdateObject->m_bKeepCorpse = pCreature->m_bKeepCorpse;
        }
    }
}
// PC 0x004C5DF0  Xbox 0x00198DC0
inline NWPlayerCharacterList_st::~NWPlayerCharacterList_st()
{
}
// PC 0x00576360  Xbox 0x002396B0
int CSWSMessage::SendServerToPlayerCharList(CSWSPlayer* pPlayer)
{
    CExoString sCharacter;
    CExoArrayList<CExoString> lstFiles;
    unsigned long nMessageSize = 2;
    CExoString sPlayerName;
    CExoString sDirectory;
    CExoArrayList<NWPlayerCharacterList_st*> lstCharacters;
    CResStruct cCharacterStruct;
    CResStruct cClassStruct;
    CResList cClassList;
    NWPlayerCharacterListClass_st cClass;
    int bSuccess;
    unsigned char* pMessage;
    int i;
    unsigned long nClassCount;
    unsigned long j;

    CreateWriteMessage(nMessageSize, RECIPIENT_UNSPECIFIED, 1);
    g_pAppManager->GetServerExoApp()->GetNetLayer()->GetPlayerInfo(
        pPlayer->GetPlayerId());
    sPlayerName = pPlayer->GetPlayerName();
    g_pExoBase->GetDirectoryList(&lstFiles, "SERVERVAULT:", RESTYPE_INVALID, 1, 0);
    if (lstFiles.Contains(sPlayerName))
    {
        lstFiles.SetSize(0);
        sDirectory.Format("SERVERVAULT:%s", sPlayerName.CStr());
        g_pExoBase->GetDirectoryList(&lstFiles, sDirectory, RESTYPE_BIC, 0, 0);
        if (lstFiles.num != 0)
        {
            g_pExoResMan->AddResourceDirectory(sDirectory);
            for (i = 0; i < lstFiles.num; i++)
            {
                sCharacter = lstFiles[i];
                sCharacter = sCharacter.SubString(0, sCharacter.GetLength() - 4);
                CResGFF* pGFF = new CResGFF(RESTYPE_BIC, "BIC ", sCharacter);
                if (pGFF->m_bLoaded)
                {
                    pGFF->GetTopLevelStruct(&cCharacterStruct);
                    NWPlayerCharacterList_st* pCharacter = new NWPlayerCharacterList_st;
                    pCharacter->m_lsFirstName = pGFF->ReadFieldCExoLocString(
                        &cCharacterStruct, "FirstName", bSuccess, CExoLocString());
                    pCharacter->m_lsLastName = pGFF->ReadFieldCExoLocString(
                        &cCharacterStruct, "LastName", bSuccess, CExoLocString());
                    pCharacter->m_cCharacter = sCharacter;
                    pCharacter->m_bCharacterExists = NWPlayerCharacterList_st::CHARACTER_LIST_PLAYER_VAULT;
                    pCharacter->m_nPortraitId = pGFF->ReadFieldWORD(
                        &cCharacterStruct, "PortraitId", bSuccess, PORTRAIT_ID_RESERVED_FFFF);
                    pCharacter->m_cPortrait = pGFF->ReadFieldCResRef(
                        &cCharacterStruct, "Portrait", bSuccess, "");
                    if (pGFF->GetList(&cClassList, &cCharacterStruct, "ClassList"))
                    {
                        nClassCount = pGFF->GetListCount(&cClassList);
                        for (j = 0; j < nClassCount; j++)
                        {
                            if (pGFF->GetListElement(&cClassStruct, &cClassList, j))
                            {
                                cClass.m_nClass = pGFF->ReadFieldINT(
                                    &cClassStruct, "Class", bSuccess, -1);
                                cClass.m_nClassLevel = pGFF->ReadFieldSHORT(
                                    &cClassStruct, "ClassLevel", bSuccess, 0);
                                pCharacter->m_ClassList.Add(cClass);
                            }
                        }
                    }
                    if (!lstCharacters.DerefContains(pCharacter))
                        lstCharacters.AddUnique(pCharacter);
                    else
                        delete pCharacter;
                }
                delete pGFF;
            }
            g_pExoResMan->RemoveResourceDirectory(sDirectory);
        }
    }
    g_pExoBase->GetDirectoryList(&lstFiles, "SERVERVAULT:", RESTYPE_BIC, 0, 0);
    if (lstFiles.num != 0)
    {
        for (i = 0; i < lstFiles.num; i++)
        {
            sCharacter = lstFiles[i];
            sCharacter = sCharacter.SubString(0, sCharacter.GetLength() - 4);
            CResGFF* pGFF = new CResGFF(RESTYPE_BIC, "BIC ", sCharacter);
            if (pGFF->m_bLoaded)
            {
                pGFF->GetTopLevelStruct(&cCharacterStruct);
                NWPlayerCharacterList_st* pCharacter = new NWPlayerCharacterList_st;
                pCharacter->m_lsFirstName = pGFF->ReadFieldCExoLocString(
                    &cCharacterStruct, "FirstName", bSuccess, CExoLocString());
                pCharacter->m_lsLastName = pGFF->ReadFieldCExoLocString(
                    &cCharacterStruct, "LastName", bSuccess, CExoLocString());
                pCharacter->m_cCharacter = sCharacter;
                pCharacter->m_bCharacterExists = NWPlayerCharacterList_st::CHARACTER_LIST_SERVER_VAULT;
                pCharacter->m_nPortraitId = pGFF->ReadFieldWORD(
                    &cCharacterStruct, "PortraitId", bSuccess, PORTRAIT_ID_RESERVED_FFFF);
                pCharacter->m_cPortrait = pGFF->ReadFieldCResRef(
                    &cCharacterStruct, "Portrait", bSuccess, "");
                if (pGFF->GetList(&cClassList, &cCharacterStruct, "ClassList"))
                {
                    nClassCount = pGFF->GetListCount(&cClassList);
                    for (j = 0; j < nClassCount; j++)
                    {
                        if (pGFF->GetListElement(&cClassStruct, &cClassList, j))
                        {
                            cClass.m_nClass = pGFF->ReadFieldINT(
                                &cClassStruct, "Class", bSuccess, -1);
                            cClass.m_nClassLevel = pGFF->ReadFieldSHORT(
                                &cClassStruct, "ClassLevel", bSuccess, 0);
                            pCharacter->m_ClassList.Add(cClass);
                        }
                    }
                }
                if (!lstCharacters.DerefContains(pCharacter))
                    lstCharacters.AddUnique(pCharacter);
                else
                    delete pCharacter;
            }
            delete pGFF;
        }
    }
    if (g_pAppManager->GetServerExoApp()->GetModule()->m_bIsSaveGame)
        g_pAppManager->GetServerExoApp()->GetModule()->PackPlayerCharacterListIntoMessage(
            pPlayer, lstCharacters);
    WriteWORD(lstCharacters.num, 16);
    for (i = 0; i < lstCharacters.num; i++)
    {
        NWPlayerCharacterList_st* pCharacter = lstCharacters[i];
        WriteCExoLocStringServer(pCharacter->m_lsFirstName, 0);
        WriteCExoLocStringServer(pCharacter->m_lsLastName, 0);
        WriteCResRef(pCharacter->m_cCharacter, 16);
        WriteBYTE(pCharacter->m_bCharacterExists, 8);
        WriteWORD(pCharacter->m_nPortraitId, 16);
        WriteCResRef(pCharacter->m_cPortrait, 16);
        nClassCount = pCharacter->m_ClassList.num;
        WriteBYTE(nClassCount, 8);
        for (j = 0; j < nClassCount; j++)
        {
            WriteINT(pCharacter->m_ClassList[j].m_nClass, 32);
            WriteBYTE(pCharacter->m_ClassList[j].m_nClassLevel, 8);
        }
        delete pCharacter;
    }
    if (!GetWriteMessage(&pMessage, &nMessageSize))
        return 0;
    return SendServerToPlayerMessage(pPlayer->GetPlayerId(), MESSAGE_CHARACTER_LIST, CHAR_LIST_REPLY, pMessage, nMessageSize);
}
// PC 0x00577B60  Xbox 0x0023AE10
int CSWSMessage::SendServerToPlayerJournalFullUpdate(CSWSPlayer* pPlayer)
{
    int bFullUpdate = 0;
    CExoArrayList<CSWSPlayerJournalQuestUpdates>* pUpdates = NULL;
    if (pPlayer->GetGameObject() == NULL)
        return SendServerToPlayerJournalFullUpdateNotNeeded(pPlayer, 1);
    CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
    if (pCreature == NULL)
        return SendServerToPlayerJournalFullUpdateNotNeeded(pPlayer, 1);
    CSWSJournal* pJournal = pCreature->GetJournal();
    if (pJournal == NULL)
        return SendServerToPlayerJournalFullUpdateNotNeeded(pPlayer, 1);

    int nUpdates;
    int nEntries;
    if (pPlayer->m_pJournalGUI->m_bUpdateRequired != 0)
    {
        nUpdates = pJournal->m_lstEntries.num;
        bFullUpdate = 1;
    }
    else
    {
        if (pPlayer->m_pJournalGUI->m_lstQuestUpdates.num <= 0)
            return SendServerToPlayerJournalFullUpdateNotNeeded(pPlayer, 0);
        pUpdates = &pPlayer->m_pJournalGUI->m_lstQuestUpdates;
        if (pUpdates == NULL)
            return SendServerToPlayerJournalFullUpdateNotNeeded(pPlayer, 0);
        nUpdates = pUpdates->num;
        nEntries = pJournal->m_lstEntries.num;
    }

    unsigned long nMessageSize = 30 * nUpdates + 12;
    unsigned char* pMessage;
    CreateWriteMessage(nMessageSize, RECIPIENT_UNSPECIFIED, 1);
    WriteBOOL(1);
    WriteBOOL(bFullUpdate);
    WriteINT(nUpdates, 32);
    if (bFullUpdate)
    {
        for (int nEntry = 0; nEntry < nUpdates; nEntry++)
        {
            unsigned short nFlags = CSWSPlayerJournalQuestUpdates::QUEST_NEW;
            if (!pJournal->m_lstEntries[nEntry].m_bNew)
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_QUIET;
            WriteWORD(nFlags, 16);
            WriteCExoString(pJournal->m_lstEntries[nEntry].m_sPlotID, 32);
            WriteCExoLocStringServer(pJournal->m_lstEntries[nEntry].m_lsName, 0);
            WriteCExoLocStringServer(pJournal->m_lstEntries[nEntry].m_lsText, 0);
            WriteDWORD(pJournal->m_lstEntries[nEntry].m_nDate, 32);
            WriteDWORD(pJournal->m_lstEntries[nEntry].m_nTime, 32);
            WriteINT(pJournal->m_lstEntries[nEntry].m_nState, 32);
            WriteDWORD(pJournal->m_lstEntries[nEntry].m_nPriority, 32);
            WriteWORD(pJournal->m_lstEntries[nEntry].m_nPicture, 16);
            WriteBOOL(pJournal->m_lstEntries[nEntry].m_bCompleted);
            WriteINT(pJournal->m_lstEntries[nEntry].m_nPlotIndex, 32);
            WriteINT(pJournal->m_lstEntries[nEntry].m_nPlanetID, 32);
            WriteINT(pJournal->m_lstEntries[nEntry].m_nXP, 32);
            pJournal->m_lstEntries[nEntry].m_bNew = 0;
        }
    }
    else
    {
        for (int nUpdate = 0; nUpdate < nUpdates; nUpdate++)
        {
            unsigned short nFlags = (*pUpdates)[nUpdate].m_nQuest;
            if (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_REMOVED)
            {
                WriteWORD(nFlags, 16);
                WriteCExoString((*pUpdates)[nUpdate].m_sTag, 32);
            }
            else
            {
                for (int nEntry = 0; nEntry < nEntries; nEntry++)
                {
                    if (pJournal->m_lstEntries[nEntry].m_sPlotID == (*pUpdates)[nUpdate].m_sTag)
                    {
                        WriteWORD(nFlags, 16);
                        WriteCExoString(pJournal->m_lstEntries[nEntry].m_sPlotID, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_NAME | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteCExoLocStringServer(pJournal->m_lstEntries[nEntry].m_lsName, 0);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_TEXT | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteCExoLocStringServer(pJournal->m_lstEntries[nEntry].m_lsText, 0);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_DATE | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteDWORD(pJournal->m_lstEntries[nEntry].m_nDate, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_TIME | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteDWORD(pJournal->m_lstEntries[nEntry].m_nTime, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_STATE | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteINT(pJournal->m_lstEntries[nEntry].m_nState, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_PRIORITY | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteDWORD(pJournal->m_lstEntries[nEntry].m_nPriority, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_PICTURE | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteWORD(pJournal->m_lstEntries[nEntry].m_nPicture, 16);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_COMPLETED | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteBOOL(pJournal->m_lstEntries[nEntry].m_bCompleted);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_PLOTINDEX | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteINT(pJournal->m_lstEntries[nEntry].m_nPlotIndex, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_PLANETID | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteINT(pJournal->m_lstEntries[nEntry].m_nPlanetID, 32);
                        if (nFlags & (CSWSPlayerJournalQuestUpdates::QUEST_XP | CSWSPlayerJournalQuestUpdates::QUEST_NEW))
                            WriteINT(pJournal->m_lstEntries[nEntry].m_nXP, 32);
                        break;
                    }
                }
            }
        }
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
        return 0;
    if (pUpdates != NULL)
        pUpdates->SetSize(0);
    pPlayer->m_pJournalGUI->m_bUpdateRequired = 0;
    unsigned long nPlayerId = pPlayer->m_nPlayerId;
    return SendServerToPlayerMessage(nPlayerId, MESSAGE_JOURNAL, 9, pMessage, nMessageSize);
}
// PC 0x00577FA0  Xbox 0x0023B2A0
void CSWSMessage::TestObjectUpdateDifferences(CSWSPlayer* pPlayer,
    CSWSObject* pObject, CLastUpdateObject** ppLastUpdateObject,
    unsigned long* pnUpdateMask, unsigned long* pnAppearanceMask)
{
    CSWSObject* pPlayerObject = pPlayer->GetGameObject();
    CExoLinkedListNode* pPosition =
        pPlayer->m_pActiveObjectsLastUpdate->GetHeadPos();
    while (pPosition != NULL)
    {
        *ppLastUpdateObject =
            pPlayer->m_pActiveObjectsLastUpdate->GetAtPos(pPosition);
        if ((*ppLastUpdateObject)->m_oidObject == pObject->m_nId)
        {
            (*ppLastUpdateObject)->m_nUnknown24 = 1;
            *pnUpdateMask = ComputeUpdateRequired(pPlayer, pObject,
                *ppLastUpdateObject, pObject->GetId() == pPlayerObject->GetId());
            *pnAppearanceMask = ComputeAppearanceUpdateRequired(pObject,
                *ppLastUpdateObject);
            return;
        }
        pPlayer->m_pActiveObjectsLastUpdate->GetNext(pPosition);
    }
    *ppLastUpdateObject = CreateNewLastUpdateObject(pPlayer, pObject,
        pnUpdateMask, pnAppearanceMask);
}
// PC 0x00578060  Xbox 0x0023B360
void CSWSMessage::ComputeGameObjectUpdateForObject(CSWSPlayer* pPlayer,
    CSWSObject* pPlayerObject, CGameObjectArray* pObjectArray,
    unsigned long oidObject)
{
    CGameObject* pGameObject;

    if (pObjectArray->GetGameObject(oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS && pGameObject != NULL)
    {
        CSWSObject* pObject = pGameObject->AsSWSObject();
        int bVisible = TestObjectVisible(pObject, pPlayerObject);
        pObject->m_nClientState = bVisible;
        if (bVisible == 1)
        {
            CLastUpdateObject* pLastUpdateObject = NULL;
            unsigned long nUpdateMask;
            unsigned long nAppearanceMask;

            TestObjectUpdateDifferences(pPlayer, pObject, &pLastUpdateObject,
                &nUpdateMask, &nAppearanceMask);
            if (nAppearanceMask != 0 || nUpdateMask != 0)
            {
                WriteGameObjUpdate_UpdateObject(pPlayer, pObject,
                    pLastUpdateObject, nUpdateMask, nAppearanceMask);
            }
            StoreValuesInLastUpdateObject(pPlayer, pLastUpdateObject, pObject,
                nUpdateMask, nAppearanceMask);
        }
        else
        {
            DeleteLastUpdateObjectsForObject(pPlayer, pObject->GetId());
        }
    }
}
// PC 0x00578130  Xbox 0x0023B430
int CSWSMessage::ComputeGameObjectUpdateForCategory(unsigned long nCategory,
    unsigned long nMessageSizeLimit, CSWSPlayer* pPlayer,
    CSWSObject* pPlayerObject, CGameObjectArray* pObjectArray,
    CSWSPlayerLUOSortedObjectList* pObjects, int nObjectCount)
{
    CGameObject* pGameObject = NULL;

    CSWSArea* pArea = NULL;
    if (pPlayerObject != NULL)
        pArea = pPlayerObject->GetArea();

    if (pArea == NULL)
    {
        CSWSCreature* pCreature = pPlayerObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
            if (pModule != NULL)
                pArea = pModule->GetArea();
        }
    }
    if (pArea != NULL)
    {
        for (int nObject = 0; nObject < nObjectCount; ++nObject)
        {
            if (pObjects[nObject].m_nCategory == nCategory)
            {
                unsigned long oidObject = pObjects[nObject].m_oidObject;
                if (pObjectArray->GetGameObject(oidObject, &pGameObject) ==
                    CGameObjectArray::SUCCESS)
                {
                    ComputeGameObjectUpdateForObject(pPlayer, pPlayerObject,
                        pObjectArray, oidObject);
                    if (PeekAtWriteMessageSize() >= nMessageSizeLimit)
                        return 1;
                }
            }
        }
    }
    return 0;
}
// PC 0x00578200  Xbox 0x0023B500
int CSWSMessage::SendServerToPlayerGameObjUpdate(CSWSPlayer* pPlayer,
    unsigned long oidObject)
{
    CSWSObject* pPlayerObject;
    int nStage;
    CGameObjectArray* pObjectArray;
    int bMessageFull;
    CSWSArea* pArea;
    int nObjectCount;
    CSWSPlayerLUOSortedObjectList* pObjects;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    pPlayerObject = pPlayer->GetGameObject();
    pArea = NULL;
    if (pPlayerObject != NULL)
    {
        pArea = pPlayerObject->GetArea();
    }
    pObjectArray = g_pAppManager->GetServerExoApp()->GetObjectArray();
    if (pObjectArray == NULL)
    {
        return 0;
    }
    nStage = GAME_OBJECT_STAGE_DELETE_OTHER_AREAS;
    CreateWriteMessage(GAME_OBJECT_UPDATE_BUFFER_SIZE, pPlayer->GetPlayerId(), 1);
    bMessageFull = 0;
    DeleteLastUpdateObjectsInOtherAreas(pPlayer);
    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    if ((unsigned long)PeekAtWriteMessageSize() < GAME_OBJECT_UPDATE_SIZE_LIMIT)
    {
        nStage = GAME_OBJECT_STAGE_PLAYER;
        if (pArea != NULL && pPlayerObject != NULL)
        {
            ComputeGameObjectUpdateForObject(pPlayer, pPlayerObject, pObjectArray,
                pPlayerObject->GetId());
        }
        if ((unsigned long)PeekAtWriteMessageSize() < GAME_OBJECT_UPDATE_SIZE_LIMIT)
        {
            nStage = GAME_OBJECT_STAGE_MAJOR_GUI;
            if (pArea != NULL && pPlayerObject != NULL)
            {
                WriteGameObjUpdate_MajorGUIPanels(pPlayer);
            }
            if ((unsigned long)PeekAtWriteMessageSize() >= GAME_OBJECT_UPDATE_SIZE_LIMIT)
            {
                bMessageFull = 1;
            }
        }
        else
        {
            bMessageFull = 1;
        }
    }
    else
    {
        bMessageFull = 1;
    }
    pObjects = SortObjectsForGameObjectUpdate(pPlayer, pPlayerObject,
        pObjectArray, &nObjectCount);
    if (bMessageFull == 0)
    {
        nStage = GAME_OBJECT_STAGE_COMBAT;
        if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_CREATURE_COMBAT,
                GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                pObjectArray, pObjects, nObjectCount) == 0)
        {
            nStage = GAME_OBJECT_STAGE_MINOR_GUI;
            WriteGameObjUpdate_MinorGUIPanels(pPlayer);
            if ((unsigned long)PeekAtWriteMessageSize() < GAME_OBJECT_UPDATE_SIZE_LIMIT)
            {
                nStage = GAME_OBJECT_STAGE_RESERVED_CATEGORY;
                if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_RESERVED_3,
                        GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                        pObjectArray, pObjects, nObjectCount) == 0)
                {
                    nStage = GAME_OBJECT_STAGE_CREATURES;
                    if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_CREATURE,
                            GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                            pObjectArray, pObjects, nObjectCount) == 0)
                    {
                        nStage = GAME_OBJECT_STAGE_PARTY_AI;
                        WriteGameObjUpdate_PartyAIState(pPlayer);
                        if ((unsigned long)PeekAtWriteMessageSize() < GAME_OBJECT_UPDATE_SIZE_LIMIT)
                        {
                            nStage = GAME_OBJECT_STAGE_TRIGGERS;
                            if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_TRIGGER,
                                    GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                                    pObjectArray, pObjects, nObjectCount) == 0)
                            {
                                nStage = GAME_OBJECT_STAGE_DOORS;
                                if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_DOOR,
                                        GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                                        pObjectArray, pObjects, nObjectCount) == 0)
                                {
                                    nStage = GAME_OBJECT_STAGE_ITEMS;
                                    if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_ITEM,
                                            GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                                            pObjectArray, pObjects, nObjectCount) == 0)
                                    {
                                        nStage = GAME_OBJECT_STAGE_PLACEABLES;
                                        if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_PLACEABLE,
                                                GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                                                pObjectArray, pObjects, nObjectCount) == 0)
                                        {
                                            nStage = GAME_OBJECT_STAGE_AREA_EFFECTS;
                                            if (ComputeGameObjectUpdateForCategory(GAME_OBJECT_CATEGORY_AREA_OF_EFFECT,
                                                    GAME_OBJECT_UPDATE_SIZE_LIMIT, pPlayer, pPlayerObject,
                                                    pObjectArray, pObjects, nObjectCount) == 0)
                                            {
                                                nStage = GAME_OBJECT_STAGE_COMPLETE;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    WriteGameObjUpdate_WorkRemaining(pPlayerObject, pArea, nStage,
        GAME_OBJECT_STAGE_COMPLETE);
    if (pObjects != NULL)
    {
        delete[] pObjects;
    }
    if (GetWriteMessage(&pMessage, &nMessageSize) != 0)
    {
        if (nMessageSize != 0)
        {
            return SendServerToPlayerMessage(pPlayer->GetPlayerId(),
                MESSAGE_GAME_OBJECT_UPDATE, GAME_OBJECT_UPDATE_RECORDS,
                pMessage, nMessageSize);
        }
        return 1;
    }
    return 0;
}
// PC 0x00578520  Xbox 0x0023B820
int CSWSMessage::SendServerToPlayerGameObjUpdate(CSWSPlayer* pPlayer)
{
    return SendServerToPlayerGameObjUpdate(pPlayer, OBJECT_INVALID);
}

static inline void NwssendplayermessageTrace1()
{
}

#ifdef _XBOX
// PC 0x00567EF0  Xbox 0x0022B0E0
int CSWSMessage::SendServerToPlayerModule_Info(unsigned long nPlayerId)
{
    CSWSModule* pModule;
    unsigned long nMessageSize;
    unsigned char* pMessage;

    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if ((g_pAppManager->m_pServerExoApp->GetServerMode() == 1 ||
         g_pAppManager->m_pServerExoApp->GetServerMode() == 2) &&
        pModule != NULL)
    {
        CreateWriteMessage(128, RECIPIENT_UNSPECIFIED, 1);
        pModule->PackModuleIntoMessage(nPlayerId);
        if (GetWriteMessage(&pMessage, &nMessageSize) == 0)
        {
            return 0;
        }
        return SendServerToPlayerMessage(nPlayerId, 3, 1, pMessage,
                                         nMessageSize);
    }
    return 0;
}
#endif
