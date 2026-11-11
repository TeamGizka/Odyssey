#include "exobase.h"

#include "appmanager.h"
#include "exoresman.h"
#include "netlayer.h"
#include "resgff.h"
#include "serverexoapp.h"
#include "nwupdatestats.h"
#include "nwsplayerluo.h"
#include "nwsplayer.h"
#include "nwsarea.h"
#include "nwsitem.h"
#include "partytables.h"
#include "nwmessage.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwscreaturestats.h"

#define LOCAL_CHARACTER_RESREF "TEMP_CHAR"
// PC 0x00563040  Xbox 0x001C1D40
CSWSPlayer::~CSWSPlayer()
{
    delete m_pPartyObjectsLastUpdate;
    m_pPartyObjectsLastUpdate = NULL;

    if (m_pActiveObjectsLastUpdate != NULL)
    {
        while (!m_pActiveObjectsLastUpdate->IsEmpty())
        {
            CLastUpdateObject* pLastUpdateObject =
                m_pActiveObjectsLastUpdate->RemoveHead();
            delete pLastUpdateObject;
        }
        delete m_pActiveObjectsLastUpdate;
        m_pActiveObjectsLastUpdate = NULL;
    }

    delete m_pCharSheetGUI;
    m_pCharSheetGUI = NULL;
    delete m_pJournalGUI;
    m_pJournalGUI = NULL;
    delete m_pInventoryGUI;
    m_pInventoryGUI = NULL;
    delete m_pContainerGUI;
    m_pContainerGUI = NULL;
    delete m_pLastUpdateObject;
    m_pLastUpdateObject = NULL;
}
// PC 0x00560BA0  Xbox 0x001BF710
void CSWSPlayerCharSheetGUI::SetCreatureDisplayed(unsigned long oidCreature)
{
    if (m_oidCreature != oidCreature)
    {
        if (m_pStatsUpdate != NULL)
        {
            delete m_pStatsUpdate;
            m_pStatsUpdate = new CSWCreatureStatsUpdate;
        }
    }
    m_oidCreature = oidCreature;
}
// PC 0x00560C20  Xbox 0x001BF790
CSWSPlayerInventoryGUI::CSWSPlayerInventoryGUI()
{
    m_bOpen = 0;
    m_pLastUpdateObject = new CSWSPlayerLUOInventory;
}
// PC 0x00560C90  Xbox 0x001BF800
void CSWSPlayerInventoryGUI::SetOpen(int bOpen)
{
    if (bOpen == 0)
    {
        if (m_bOpen != 0)
        {
            m_pLastUpdateObject->ClearSlots();
            m_pLastUpdateObject->ClearRepository();
        }
    }
    m_bOpen = bOpen;
}
// PC 0x00560CC0  Xbox 0x001BF830
int CSWSPlayer::GetIsAllowedToSave()
{
    return m_bIsAllowedToSave != 0;
}
// PC 0x00560CD0  Xbox 0x001BF840
void CSWSPlayer::SetAreaTransitionBMP(int nState, const CExoString& sBMP)
{
    if (nState == 1)
    {
        m_sAreaTransitionBMP = sBMP;
    }
    m_nAreaTransitionState = nState;
}
// PC 0x00560D00  Xbox 0x001BF870
int CSWSPlayer::GetCharacterInfoFromIFO(unsigned char& nClassCount,
                                        int* pnClasses,
                                        unsigned char* pnClassLevels,
                                        unsigned long& nExperience)
{
    CResRef cModule("Module");
    CResGFF* pRes = (CResGFF*)g_pExoResMan->GetResObject(cModule, RESTYPE_IFO);
    CResStruct stTopLevel;
    CResStruct stPlayer;
    CResList lstPlayers;
    CResList lstClasses;
    CResStruct stClass;
    int bSuccess;

    if (pRes == NULL)
    {
        return 0;
    }
    ((CRes*)pRes)->Demand();
    pRes->GetTopLevelStruct(&stTopLevel);
    if (pRes->GetList(&lstPlayers, &stTopLevel, "Mod_PlayerList") == 0)
    {
        g_pExoResMan->ReleaseResObject(pRes);
        return 0;
    }
    unsigned long nPlayerIndex = m_nModulePlayerIndex;
    if (pRes->GetListElement(&stPlayer, &lstPlayers, nPlayerIndex) == 0)
    {
        g_pExoResMan->ReleaseResObject(pRes);
        return 0;
    }
    nExperience = pRes->ReadFieldDWORD(&stPlayer, "Experience", bSuccess, 0);
    if (pRes->GetList(&lstClasses, &stPlayer, "ClassList") == 1)
    {
        nClassCount = (unsigned char)pRes->GetListCount(&lstClasses);
        for (unsigned long i = 0; i < nClassCount; ++i)
        {
            pRes->GetListElement(&stClass, &lstClasses, i);
            pnClasses[i] = pRes->ReadFieldINT(&stClass, "Class", bSuccess, -1);
            pnClassLevels[i] = (unsigned char)pRes->ReadFieldSHORT(
                &stClass, "ClassLevel", bSuccess, 0);
        }
    }
    ((CRes*)pRes)->Release();
    g_pExoResMan->ReleaseResObject(pRes);
    return 1;
}
// PC 0x00560E60  Xbox 0x001BF9D0
int CSWSPlayer::LoadCreatureData(CResRef cResRef, CSWSCreature* pCreature)
{
    CResGFF* pRes = new CResGFF(RESTYPE_BIC, "BIC ", cResRef);
    if (!pRes->m_bLoaded)
    {
        pRes->Dump(0);
        delete pRes;
        return 0;
    }

    m_cServerCharacter = cResRef;
    CResStruct cStruct;
    pRes->GetTopLevelStruct(&cStruct);
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    int nResult = pStats->ReadStatsFromGff(
        pRes, &cStruct,
        &pCreature->m_cAppearanceInfo);
    if (nResult != 0)
    {
        pRes->Dump(0);
        delete pRes;
        return nResult;
    }

    pCreature->SetCurrentHitPoints(pCreature->GetMaxHitPoints(1));
    int bSuccess;
    pRes->ReadFieldBYTE(&cStruct, "DetectMode", bSuccess, 0);
    pCreature->SetActivity(CSWSCreature::ACTIVITY_DETECT, 1, 0);
    int nStealthMode = pRes->ReadFieldBYTE(
        &cStruct, "StealthMode", bSuccess, 0);
    pCreature->SetActivity(CSWSCreature::ACTIVITY_STEALTH, nStealthMode, 0);
    pCreature->m_nCreatureSize = pRes->ReadFieldINT(
        &cStruct, "CreatureSize", bSuccess, CREATURE_SIZE_MEDIUM);
    pCreature->m_bPMIsDisguised = pRes->ReadFieldBYTE(
        &cStruct, "PM_IsDisguised", bSuccess, 0);
    if (pCreature->m_bPMIsDisguised)
    {
        pCreature->m_nPMAppearance = pRes->ReadFieldWORD(
            &cStruct, "PM_Appearance", bSuccess, 0);
    }
    pCreature->ReadScriptsFromGff(pRes, &cStruct);
    pCreature->ReadItemsFromGff(pRes, &cStruct, 0, 1, 0);
    pCreature->m_pStats->ReadSpellsFromGff(pRes, &cStruct);
    pCreature->PostProcess();
    pRes->Dump(0);
    delete pRes;
    return 0;
}
// PC 0x00561080  Xbox 0x001BFBF0
CSWSObject* CSWSPlayer::GetGameObject()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSObject* pGameObject =
        (CSWSObject*)pServerExoApp->GetGameObject(m_oidNWSObject);
    if (pGameObject == NULL ||
        pGameObject->m_nObjectType <= CGameObject::OBJECT_TYPE_AREA)
    {
        return NULL;
    }
    return (CSWSObject*)pGameObject;
}
// PC 0x005610B0  Xbox 0x001BFC20
CExoString CSWSPlayer::GetPlayerName()
{
    CNetLayerPlayerInfo* pPlayerInfo =
        g_pAppManager->m_pServerExoApp->GetNetLayer()->GetPlayerInfo(
            m_nPlayerId);

    if (pPlayerInfo != 0)
    {
        return pPlayerInfo->m_sPlayerName;
    }
    return CExoString("");
}
// PC 0x00561100  Xbox 0x001BFC70
CSWSPlayerLastUpdateObject* CSWSPlayer::CreateNewPlayerLastUpdateObject()
{
    CSWSPlayerLastUpdateObject* pLastUpdateObject = m_pLastUpdateObject;

    if (pLastUpdateObject != NULL)
    {
        delete pLastUpdateObject;
        m_pLastUpdateObject = NULL;
    }
    m_pLastUpdateObject = new CSWSPlayerLastUpdateObject;
    m_pLastUpdateObject->ClearKnownSpellUsesLeft();
    return m_pLastUpdateObject;
}
// PC 0x00561190  Xbox 0x001BFD00
int CSWSPlayer::PermittedToDisplayCharacterSheet(unsigned long oidCreature)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp == NULL)
    {
        return 0;
    }
    CSWSCreature* pCreature =
        pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature == NULL)
    {
        return 0;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pPlayerCreature =
        pServerExoApp->GetCreatureByGameObjectID(m_oidNWSObject);
    if (pPlayerCreature == NULL)
    {
        return 0;
    }
    return pPlayerCreature->m_nId == pCreature->m_nId;
}
// PC 0x00561260  Xbox 0x001BFDD0
int CSWSPlayerCharSheetGUI::ComputeCharacterSheetUpdateRequired(CSWSPlayer* pPlayer)
{
    int nUpdate = 0;
    int bOtherCreature = 0;
    unsigned long oidCreature = m_oidCreature;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature == NULL)
    {
        return 0;
    }
    if (pPlayer->GetGameObject() != NULL &&
        pPlayer->GetGameObject()->m_nId != m_oidCreature)
    {
        bOtherCreature = 1;
    }
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    CSWCreatureStatsUpdate* pUpdate = m_pStatsUpdate;
    switch (m_nUpdateType)
    {
    case UPDATE_NONE:
        return 0;
    case UPDATE_STATS:
    {
        unsigned char nDexterity = pUpdate->m_nDexterity;
        unsigned char nConstitution = pUpdate->m_nConstitution;
        unsigned char nIntelligence = pUpdate->m_nIntelligence;
        unsigned char nWisdom = pUpdate->m_nWisdom;
        unsigned char nCharisma = pUpdate->m_nCharisma;
        unsigned char nStrength = pUpdate->m_nStrength;
        nUpdate = (pStats->GetSTRStat() != nStrength) |
                  (pStats->GetDEXStat() != nDexterity) |
                  (pStats->GetCONStat() != nConstitution) |
                  (pStats->GetINTStat() != nIntelligence) |
                  (pStats->GetWISStat() != nWisdom) |
                  (pStats->GetCHAStat() != nCharisma);
        nUpdate |= (pStats->m_nStrengthBase != pUpdate->m_nStrengthBase) |
                   (pStats->m_nDexterityBase != pUpdate->m_nDexterityBase) |
                   (pStats->m_nConstitutionBase != pUpdate->m_nConstitutionBase) |
                   (pStats->m_nIntelligenceBase != pUpdate->m_nIntelligenceBase) |
                   (pStats->m_nWisdomBase != pUpdate->m_nWisdomBase) |
                   (pStats->m_nCharismaBase != pUpdate->m_nCharismaBase);
        char nDexterityModifierBonus = pUpdate->m_nDexterityModifierBonus;
        if (pStats->GetDEXMod(1) - pStats->GetDEXMod(0) !=
            nDexterityModifierBonus)
        {
            nUpdate |= UPDATE_ABILITIES;
        }
        char nStrengthBonus = pUpdate->m_nStrengthBonus;
        char nDexterityBonus = pUpdate->m_nDexterityBonus;
        char nConstitutionBonus = pUpdate->m_nConstitutionBonus;
        char nIntelligenceBonus = pUpdate->m_nIntelligenceBonus;
        char nWisdomBonus = pUpdate->m_nWisdomBonus;
        char nCharismaBonus = pUpdate->m_nCharismaBonus;
        nUpdate |= ((char)pStats->GetTotalSTRBonus() != nStrengthBonus) |
                   ((char)pStats->GetTotalDEXBonus() != nDexterityBonus) |
                   ((char)pStats->GetTotalCONBonus() != nConstitutionBonus) |
                   ((char)pStats->GetTotalINTBonus() != nIntelligenceBonus) |
                   ((char)pStats->GetTotalWISBonus() != nWisdomBonus) |
                   ((char)pStats->GetTotalCHABonus() != nCharismaBonus);
        char nFortitudeSave = pUpdate->m_nFortitudeSave;
        char nWillSave = pUpdate->m_nWillSave;
        char nReflexSave = pUpdate->m_nReflexSave;
        unsigned long nExperience = pStats->m_nExperience;
        long nCachedExperience = pUpdate->m_nExperience;
        nUpdate |= (pStats->GetFortSavingThrow(0) != nFortitudeSave
                        ? UPDATE_FORTITUDE : 0) |
                   (pStats->GetWillSavingThrow(0) != nWillSave
                        ? UPDATE_WILL : 0) |
                   (pStats->GetReflexSavingThrow(0) != nReflexSave
                        ? UPDATE_REFLEX : 0) |
                   (nExperience != nCachedExperience
                        ? UPDATE_EXPERIENCE : 0);
        unsigned char nBaseAttackBonus = pUpdate->m_nBaseAttackBonus;
        if (pStats->GetBaseAttackBonus() != nBaseAttackBonus)
        {
            nUpdate |= UPDATE_ATTACK_BONUS;
        }
        else
        {
            int bUseMonkAttackTables = pUpdate->m_bUseMonkAttackTables;
            if (pStats->GetUseMonkAttackTables(0) != bUseMonkAttackTables)
            {
                nUpdate |= UPDATE_ATTACK_BONUS;
            }
        }
        CCombatInformation* pCachedCombatInformation = pUpdate->m_pCombatInformation;
        CCombatInformation* pCombatInformation = pStats->m_pCombatInformation;
        if (*pCombatInformation != *pCachedCombatInformation)
        {
            nUpdate |= UPDATE_COMBAT_INFORMATION;
        }
        if (bOtherCreature == 1)
        {
            short nArmorClass = pUpdate->m_nArmorClass;
            if (pCreature->GetArmorClass() != nArmorClass)
            {
                nUpdate |= UPDATE_ARMOR_CLASS;
            }
        }
        break;
    }
    case UPDATE_SKILLS:
        pUpdate->m_nSkillUpdates = 0;
        for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
        {
            unsigned char nCachedRank = pUpdate->m_pSkillRanks[nSkill];
            if ((unsigned char)pStats->GetSkillRank(nSkill, NULL, 0) != nCachedRank)
            {
                pUpdate->m_nSkillUpdates |= CSWCreatureStatsUpdate::SKILL_UPDATE_BIT << nSkill;
            }
        }
        if (pUpdate->m_nSkillUpdates != 0)
        {
            nUpdate = UPDATE_SKILL_RANKS;
        }
        break;
    case UPDATE_FEATS:
        bOtherCreature = 0;
        if (pCreature->m_nId != pUpdate->m_oidCreature)
        {
            bOtherCreature = 1;
        }
        if ((unsigned short)pCreature->m_pStats->GetNumFeats() != pUpdate->m_lstFeats.num)
        {
            bOtherCreature = 1;
        }
        if ((unsigned short)pStats->m_lstBonusFeats.num != pUpdate->m_lstBonusFeats.num)
        {
            bOtherCreature = 1;
        }
        if (!bOtherCreature)
        {
            for (unsigned short nFeat = 0;
                 nFeat < (unsigned short)pCreature->m_pStats->GetNumFeats(); ++nFeat)
            {
                unsigned short nCurrentFeat = pCreature->m_pStats->GetFeat(nFeat);
                int bFound = 0;
                for (int nCachedFeat = 0; nCachedFeat < pUpdate->m_lstFeats.num; ++nCachedFeat)
                {
                    if (pUpdate->m_lstFeats[nCachedFeat] == nCurrentFeat)
                    {
                        bFound = 1;
                    }
                }
                if (!bFound)
                {
                    bOtherCreature = 1;
                }
            }
            if (!bOtherCreature)
            {
                for (unsigned short nBonusFeat = 0;
                     nBonusFeat < (unsigned short)pCreature->m_pStats->m_lstBonusFeats.num;
                     ++nBonusFeat)
                {
                    unsigned short nCurrentBonusFeat = pCreature->m_pStats->GetBonusFeat(nBonusFeat);
                    int bFound = 0;
                    for (int nCachedFeat = 0; nCachedFeat < pUpdate->m_lstBonusFeats.num;
                         ++nCachedFeat)
                    {
                        if (pUpdate->m_lstBonusFeats[nCachedFeat] == nCurrentBonusFeat)
                        {
                            bFound = 1;
                        }
                    }
                    if (!bFound)
                    {
                        bOtherCreature = 1;
                    }
                }
            }
        }
        if (bOtherCreature == 1)
        {
            unsigned long oidCreature = pCreature->m_nId;
            if (pPlayer->GetGameObject() != NULL &&
                pPlayer->GetGameObject()->m_nId != oidCreature)
            {
                nUpdate = UPDATE_FEAT_LISTS;
            }
        }
        break;
    }
    return nUpdate;
}
// PC 0x00561820  Xbox 0x001C0390
CLastUpdateObject* CSWSPlayer::GetLastUpdateObject(unsigned long oidObject)
{
    CExoLinkedListNode* pNode = m_pActiveObjectsLastUpdate->GetHeadPos();

    while (pNode != NULL)
    {
        CLastUpdateObject* pLastUpdateObject =
            m_pActiveObjectsLastUpdate->GetAtPos(pNode);
        if (pLastUpdateObject->m_oidObject == oidObject)
        {
            return pLastUpdateObject;
        }
        m_pActiveObjectsLastUpdate->GetNext(pNode);
    }
    return NULL;
}
// PC 0x00561870  Xbox 0x001C03E0
int CSWSPlayer::PackCreatureIntoMessage()
{
    int bSuccess = 1;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CResGFF* pGFF = new CResGFF;
    CResStruct* pStruct = new CResStruct;
    int bCreated = pGFF->CreateGFFFile(pStruct, "BIC ", "V2.0");
    if (bCreated)
    {
        CExoString sFileName;
        CExoString sPlayerName;
        CSWSCreature* pCreature = GetGameObject()->AsSWSCreature();
        if (pCreature != NULL)
        {
            if (pCreature->m_bPMIsDisguised == 1)
            {
                for (int i = 0; i < pCreature->m_lstEffects.num; ++i)
                {
                    if (pCreature->m_lstEffects[i]->GetEffectType() ==
                        CSWSEffectListHandler::EFFECT_DISGUISE)
                    {
                        pCreature->m_lstEffects[i]->SetInteger(2, 1);
                        unsigned __int64 nEffectId =
                            pCreature->m_lstEffects[i]->m_nID;
                        pCreature->RemoveEffectById(nEffectId);
                        break;
                    }
                }
                pCreature->m_pStats->UpdateCombatInformation();
            }
            pMessage->CreateWriteMessage(CHARACTER_MESSAGE_INITIAL_SIZE,
                                         CSWMessage::RECIPIENT_UNSPECIFIED, 1);
            pCreature->SaveCreature(pGFF, pStruct);
            sFileName = "TEMP:temp_charfile";
            pGFF->WriteGFFFile(sFileName, RESTYPE_BIC);
            g_pExoResMan->AddResourceDirectory("TEMP:");
            CExoFile* pFile = new CExoFile(sFileName, RESTYPE_BIC, "rb");
            if (pFile->FileOpened())
            {
                unsigned long nSize = pFile->GetSize();
                char* pData = new char[nSize];
                pFile->Read(pData, 1, nSize);
                delete pFile;
                pMessage->WriteCExoLocStringServer(*pCreature->GetFirstName(), 0);
                pMessage->WriteCExoLocStringServer(pCreature->GetLastName(), 0);
                pMessage->WriteDWORD(nSize, 32);
                pMessage->WriteVOIDPtr(pData, nSize);
                g_pExoResMan->RemoveResourceDirectory("TEMP:");
                g_pExoResMan->RemoveFile(sFileName, RESTYPE_BIC);
                delete [] pData;
            }
            else
            {
                bSuccess = 0;
                delete pFile;
            }
        }
        else
        {
            bSuccess = 0;
        }
    }
    else
    {
        bSuccess = 0;
    }
    delete pStruct;
    delete pGFF;
    return bSuccess;
}
// PC 0x00561C40  Xbox 0x001C07C0
void CSWSPlayer::SetGameObject(CSWSObject* pObject)
{
    unsigned long oidOldObject = m_oidNWSObject;
    CSWSArea* pOldArea = NULL;
    CSWSArea* pNewArea = NULL;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer != NULL)
    {
        CGameObject* pOldObject = pServer->GetGameObject(oidOldObject);
        if (pOldObject != NULL &&
            pOldObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            pOldArea = ((CSWSObject*)pOldObject)->GetArea();
        }
    }
    if (pObject != NULL)
    {
        unsigned long oidNewObject = pObject->m_nId;
        m_oidNWSObject = oidNewObject;
        pServer = g_pAppManager->m_pServerExoApp;
        if (pServer != NULL)
        {
            CGameObject* pNewObject = pServer->GetGameObject(oidNewObject);
            if (pNewObject != NULL &&
                pNewObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            {
                pNewArea = ((CSWSObject*)pNewObject)->GetArea();
            }
        }
        if (oidOldObject != oidNewObject)
        {
            m_pJournalGUI->m_bUpdateRequired = 1;
            ClearPlayerLastUpdateObject();
        }
        if (g_pAppManager->m_pServerExoApp != NULL &&
            g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL &&
            g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyItems() != NULL)
        {
            pServer = g_pAppManager->m_pServerExoApp;
            pServer->GetPartyTable()->GetPartyItems()->SetParentId(m_oidNWSObject);
        }
    }
    else
    {
        m_oidNWSObject = OBJECT_INVALID;
    }
    if (pOldArea != pNewArea)
    {
        if (pNewArea != NULL)
            pNewArea->IncrementPlayersInArea();
        if (pOldArea != NULL)
            pOldArea->DecrementPlayersInArea();
    }
}
// PC 0x00561D70  Xbox 0x001C09C0
int CSWSPlayer::LoadLocalCharacter()
{
    CSWSObject* pCreature =
        (CSWSObject*)new CSWSCreature(OBJECT_INVALID, 1);
    SetGameObject(pCreature);
    m_oidPCObject = pCreature->m_nId;
    m_nCharacterType = CHARACTER_TYPE_LOCAL;

    int nResult = LoadCreatureData(LOCAL_CHARACTER_RESREF,
                                   (CSWSCreature*)pCreature);
    ((CSWSCreature*)pCreature)->SetInParty(1, 1);
    if (nResult != 0)
    {
        SetGameObject((CSWSObject*)NULL);
        m_oidPCObject = OBJECT_INVALID;
        delete pCreature;
    }
    return nResult;
}
// PC 0x00561E30  Xbox 0x001C0AC0
int CSWSPlayer::LoadCharacterFromIFO(unsigned long nPlayerIndex,
                                      int bLoadModuleCharacter,
                                      int bPostProcess)
{
    Vector vPosition;
    Vector vOrientation;
    int bTemporaryCharacter = 0;
    int bCreatedResource = 0;
    CResGFF* pRes;
    CResStruct stTopLevel;
    CResStruct stPlayer;
    CResList lstPlayers;
    int bSuccess;

    if (nPlayerIndex == (unsigned long)-1)
    {
        g_pExoResMan->AddResourceDirectory("TEMP:");
        nPlayerIndex = m_nPlayerIndex;
        bTemporaryCharacter = 1;
        CResRef cResRef("pifo");
        pRes = (CResGFF*)g_pExoResMan->GetResObject(cResRef, RESTYPE_IFO);
        if (pRes == NULL)
        {
            pRes = new CResGFF;
            g_pExoResMan->SetResObject("pifo", RESTYPE_IFO, pRes);
            strncpy(pRes->m_szFileType, "IFO ", 4);
            bCreatedResource = 1;
        }
    }
    else
    {
        CResRef cResRef("Module");
        pRes = (CResGFF*)g_pExoResMan->GetResObject(cResRef, RESTYPE_IFO);
    }

    if (pRes == NULL)
    {
        return 0;
    }
    pRes->Demand();
    pRes->GetTopLevelStruct(&stTopLevel);
    if (pRes->GetList(&lstPlayers, &stTopLevel, "Mod_PlayerList") == 0)
    {
        pRes->Release();
        g_pExoResMan->ReleaseResObject(pRes);
        return 0;
    }
    if (pRes->GetListElement(&stPlayer, &lstPlayers, nPlayerIndex) == 0)
    {
        pRes->Release();
        g_pExoResMan->ReleaseResObject(pRes);
        return 0;
    }

    unsigned long oidCreature = pRes->ReadFieldDWORD(
        &stPlayer, "ObjectId", bSuccess, OBJECT_INVALID);
    CSWSCreature* pCreature = new CSWSCreature(oidCreature, 1);
    SetGameObject((CSWSObject*)pCreature);
    m_oidPCObject = pCreature->GetId();
    unsigned long oidArea = pRes->ReadFieldDWORD(
        &stPlayer, "AreaId", bSuccess, 0);
    pCreature->SetInParty(1, 1);
    if (pCreature->m_pStats->ReadStatsFromGff(
            pRes, &stPlayer,
            &pCreature->m_cAppearanceInfo) != 0)
    {
        g_pExoResMan->ReleaseResObject(pRes);
        delete pCreature;
        return 0;
    }

    pCreature->LoadJournal(pRes, &stPlayer);
    pRes->ReadFieldBYTE(&stPlayer, "DetectMode", bSuccess, 0);
    pCreature->SetActivity(CSWSCreature::ACTIVITY_DETECT, 1, 0);
    int nStealthMode = pRes->ReadFieldBYTE(
        &stPlayer, "StealthMode", bSuccess, 0);
    pCreature->SetActivity(CSWSCreature::ACTIVITY_STEALTH, nStealthMode, 0);
    pCreature->m_nCreatureSize = pRes->ReadFieldINT(
        &stPlayer, "CreatureSize", bSuccess, CREATURE_SIZE_MEDIUM);
    if (bTemporaryCharacter == 1)
    {
        pCreature->ReadItemsFromGff(pRes, &stPlayer, 0, 1, 0);
    }
    else
    {
        pCreature->ReadItemsFromGff(pRes, &stPlayer, 1, 1, 0);
    }
    pCreature->LoadObjectState(pRes, &stPlayer);
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    pStats->ReadSpellsFromGff(pRes, &stPlayer);
    pCreature->m_bPMIsDisguised = pRes->ReadFieldBYTE(
        &stPlayer, "PM_IsDisguised", bSuccess, 0);
    if (pCreature->m_bPMIsDisguised)
    {
        pCreature->m_nPMAppearance = pRes->ReadFieldWORD(
            &stPlayer, "PM_Appearance", bSuccess, 0);
    }
    pCreature->ReadScriptsFromGff(pRes, &stPlayer);
    vPosition.m_fX = pRes->ReadFieldFLOAT(&stPlayer, "XPosition", bSuccess, 0.0f);
    vPosition.m_fY = pRes->ReadFieldFLOAT(&stPlayer, "YPosition", bSuccess, 0.0f);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(&stPlayer, "ZPosition", bSuccess, 0.0f);
    vOrientation.m_fX = pRes->ReadFieldFLOAT(&stPlayer, "XOrientation", bSuccess, 0.0f);
    vOrientation.m_fY = pRes->ReadFieldFLOAT(&stPlayer, "YOrientation", bSuccess, 0.0f);
    vOrientation.m_fZ = pRes->ReadFieldFLOAT(&stPlayer, "ZOrientation", bSuccess, 0.0f);
    pCreature->CSWSObject::SetOrientation(vOrientation);
    if (bTemporaryCharacter == 0 && bLoadModuleCharacter != 0)
    {
        m_bModuleCharacterLoaded = 1;
    }
    else
    {
        m_bModuleCharacterLoaded = 0;
    }
    if (bPostProcess != 0)
    {
        pCreature->PostProcess();
    }
    pRes->Release();
    g_pExoResMan->ReleaseResObject(pRes);
    if (bCreatedResource == 1)
    {
        delete pRes;
    }
    if (bTemporaryCharacter == 1)
    {
        g_pExoResMan->RemoveResourceDirectory("TEMP:");
    }
    else
    {
        pCreature->m_oidDisappearTarget = oidArea;
        pCreature->m_vDisappearPosition = vPosition;
        pCreature->m_bWorkRemainingCompleteSent = 0;
    }
    return 1;
}
// PC 0x00562380  Xbox 0x001C1030
int CSWSPlayer::LoadServerCharacter(CResRef cResRef)
{
    CSWSObject* pCreature =
        (CSWSObject*)new CSWSCreature(OBJECT_INVALID, 1);
    SetGameObject(pCreature);
    unsigned long oidCreature = pCreature->m_nId;
    m_oidPCObject = oidCreature;
    m_nCharacterType = CHARACTER_TYPE_SERVER;

    g_pExoResMan->RemoveResourceDirectory("SERVERVAULT:");
    g_pExoResMan->AddResourceDirectory("SERVERVAULT:");

    int nResult = LoadCreatureData(cResRef, (CSWSCreature*)pCreature);
    if (nResult != 0)
    {
        SetGameObject((CSWSObject*)NULL);
        m_oidPCObject = OBJECT_INVALID;
        delete pCreature;
        return nResult;
    }
    return 0;
}
// PC 0x005624C0  Xbox 0x001C11B0
int CSWSPlayer::SaveServerCharacter()
{
    CExoString sResRef;
    CExoString sFileName;
    CExoString sDirectory;
    CExoString sBaseName;
    int nSuffix = 1;
    CExoString sSuffix;
    CExoArrayList<CExoString> lstFiles;

    if (m_nCharacterType != CHARACTER_TYPE_SERVER &&
        m_nCharacterType != CHARACTER_TYPE_SERVER_PLAYER)
        return 0;

    CSWSCreature* pCreature = GetGameObject()->AsSWSCreature();
    if (pCreature == NULL)
        return 0;

    if (pCreature->m_bPMIsDisguised == 1)
    {
        int i = 0;
        if (pCreature->m_lstEffects.num > 0)
        {
            do
            {
                if (pCreature->m_lstEffects[i]->m_nType ==
                    CSWSEffectListHandler::EFFECT_DISGUISE)
                {
                    pCreature->m_lstEffects[i]->SetInteger(2, 1);
                    unsigned __int64 nEffectId =
                        pCreature->m_lstEffects[i]->m_nID;
                    pCreature->RemoveEffectById(nEffectId);
                    break;
                }
                ++i;
            } while (i < pCreature->m_lstEffects.num);
        }
    }

    CResGFF* pGFF = new CResGFF;
    CResStruct* pStruct = new CResStruct;
    if (pGFF->CreateGFFFile(pStruct, "BIC ", "V2.0") == 0)
    {
        delete pGFF;
        delete pStruct;
        return 0;
    }

    if (pCreature->m_bRestorePlotOnDownload)
    {
        pCreature->m_bPlotObject = 0;
        if (pCreature->SaveCreature(pGFF, pStruct) == 0)
        {
            delete pGFF;
            delete pStruct;
            pCreature->m_bPlotObject = 1;
            return 0;
        }
        pCreature->m_bPlotObject = 1;
    }
    else
    {
        if (pCreature->SaveCreature(pGFF, pStruct) == 0)
        {
            delete pGFF;
            delete pStruct;
            return 0;
        }
    }
    delete pStruct;

    int bSaved = 0;
    if (m_cServerCharacter != "")
    {
        m_cServerCharacter.CopyToString(sResRef);
        if (m_nCharacterType == CHARACTER_TYPE_SERVER_PLAYER)
            sFileName.Format("SERVERVAULT:%s%s%s", GetPlayerName().CStr(),
                             "\\", sResRef.CStr());
        else
            sFileName.Format("SERVERVAULT:%s", sResRef.CStr());
        if (pGFF->WriteGFFFile(sFileName, RESTYPE_BIC))
        {
            CResRef cResRef(sResRef);
            m_cServerCharacter = cResRef;
            bSaved = 1;
        }
    }
    if (!bSaved)
    {
        m_nCharacterType = CHARACTER_TYPE_SERVER_PLAYER;
        sDirectory.Format("SERVERVAULT:%s", GetPlayerName().CStr());
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        sBaseName = pStats->GetFullName();
        sBaseName.StripNonAlphaNumeric(1, 0, 0);
        if (sBaseName[0] == 0)
            sBaseName = "0";
        else
            sBaseName = sBaseName.SubString(0, 16);
        sResRef = sBaseName;
        g_pExoBase->GetDirectoryList(&lstFiles, sDirectory, RESTYPE_BIC, 0, 0);
        while (lstFiles.Contains(sResRef + ".bic"))
        {
            sSuffix = CExoString(nSuffix);
            sBaseName = sBaseName.SubString(0, 16 - sSuffix.GetLength());
            sResRef = sBaseName + sSuffix;
            ++nSuffix;
        }
        sFileName.Format("%s%s%s", sDirectory.CStr(), "\\", sResRef.CStr());
        if (pGFF->WriteGFFFile(sFileName, RESTYPE_BIC) == 0)
        {
            delete pGFF;
            return 0;
        }
        {
            CResRef cResRef(sResRef);
            m_cServerCharacter = cResRef;
        }
    }
    g_pExoResMan->UpdateResourceDirectory("SERVERVAULT:");
    delete pGFF;
    return 1;
}
// PC 0x00562C70  Xbox 0x001C1970
CSWSPlayer::CSWSPlayer(unsigned long nPlayerId)
    : CSWSClient(nPlayerId)
{
    m_bFloatyEffects = 0;
    m_nAreaLoadState = 0;
    m_oidNWSObject = CGameObjectArray::INVALIDOBJID;
    m_anLastGameObjectUpdateTime[0] = 0;
    m_anLastGameObjectUpdateTime[1] = 0;
    m_oidLastControlledObject = CGameObjectArray::INVALIDOBJID;
    m_pActiveObjectsLastUpdate = new CExoLinkedList<CLastUpdateObject>;
    m_pPartyObjectsLastUpdate = new CExoLinkedList<CLastUpdatePartyObject>;
    m_pCharSheetGUI = new CSWSPlayerCharSheetGUI;
    m_nAreaTransitionState = -1;
    m_sAreaTransitionBMP = "";
    m_pJournalGUI = new CSWSPlayerJournalGUI;
    m_pInventoryGUI = new CSWSPlayerInventoryGUI;
    m_pContainerGUI = new CSWSPlayerContainerGUI;
    m_bIsAllowedToSave = 1;
    m_bModuleCharacterLoaded = 0;
    m_nModulePlayerIndex = -1;
    m_n60 = 1;
    m_nCharacterType = 0;
    m_cServerCharacter = "";
    m_n7C = 0;
    m_oidPCObject = CGameObjectArray::INVALIDOBJID;
    m_pLastUpdateObject = NULL;
    m_nPlayModuleCharacterListState = 0;
    m_nPlayerIndex = 0;
}

void CSWSPlayer::ClearPlayerLastUpdateObject()
{
    if (m_pLastUpdateObject != NULL)
    {
        m_pLastUpdateObject->ClearActionQueue();
        m_pLastUpdateObject->ClearKnownSpells();
        m_pLastUpdateObject->ClearKnownSpellUsesLeft();
        m_pLastUpdateObject->ClearSpellAddDeleteLists();
        m_pLastUpdateObject->ClearFeats();
    }
}
// PC 0x00562E40  Xbox 0x001C1B40
void CSWSPlayer::ClearPlayerOnDestroyGame()
{
    CSWSPlayerInventoryGUI* pInventoryGUI = m_pInventoryGUI;
    if (pInventoryGUI->m_pLastUpdateObject != NULL)
    {
        pInventoryGUI->m_pLastUpdateObject->ClearContainer();
    }

    if (m_pPartyObjectsLastUpdate != NULL)
    {
        delete m_pPartyObjectsLastUpdate;
        m_pPartyObjectsLastUpdate = new CExoLinkedList<CLastUpdatePartyObject>;
    }

    if (m_pActiveObjectsLastUpdate != NULL)
    {
        while (!m_pActiveObjectsLastUpdate->IsEmpty())
        {
            delete m_pActiveObjectsLastUpdate->RemoveHead();
        }
    }

    if (m_pCharSheetGUI != NULL)
    {
        delete m_pCharSheetGUI;
        m_pCharSheetGUI = new CSWSPlayerCharSheetGUI;
    }
    if (m_pContainerGUI != NULL)
    {
        delete m_pContainerGUI;
        m_pContainerGUI = new CSWSPlayerContainerGUI;
    }
    if (m_pJournalGUI != NULL)
    {
        delete m_pJournalGUI;
        m_pJournalGUI = new CSWSPlayerJournalGUI;
    }
    if (m_pInventoryGUI != NULL)
    {
        delete m_pInventoryGUI;
        m_pInventoryGUI = new CSWSPlayerInventoryGUI;
    }
}
