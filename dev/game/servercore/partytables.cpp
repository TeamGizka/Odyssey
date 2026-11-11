#include "exobase.h"
#include "exoresman.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "clientexoapp.h"
#include "nwsitem.h"
#include "nwjournal.h"
#include "swparty.h"
#include "nwsobject.h"
#include "nwsplayer.h"
#include "swguiabilitieschargen.h"
#include "nwccreature.h"
#include "clientoptions.h"
#include "gameobjectarray.h"
#include "nwsparty.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwclass.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "nwmessage.h"
#include "nwitem.h"
#include "swguiingame.h"
#include <math.h>
#include "partytables.h"
#include "swmain.h"

extern __int64 g_nCurrentSessionStartFILETIME;

inline void PartyTableReleaseStorage(CItemRepository* pItems, CSWSJournal* pJournal)
{
    delete pItems;
    delete pJournal;
}
// PC 0x0057C840
int CSWPartyTable::SpawnNPC(int nNPC, int bUsePosition, const Vector& vPosition,
                            const Vector& vOrientation, int bResurrect)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    unsigned long oidNPC = GetNPCObject(nNPC, 1, bResurrect);
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidNPC);
    if (pCreature != NULL)
    {
        int nOldValue = pCreature->m_bCommandable;
        pCreature->m_bCommandable = 1;
        pCreature->ClearAllActions(1);
        pCreature->m_bCommandable = nOldValue;

        CSWSArea* pArea = pServerExoApp->GetModule()->GetArea();
        Vector vSpawn;
        Vector vSafe;
        Vector vFacing;
        if (bUsePosition != 0)
        {
            vSpawn = vPosition;
            vFacing = vOrientation;
        }
        else
        {
            vSpawn = pCreature->m_vPosition;
            vFacing = pCreature->m_vOrientation;
        }
        if (pArea->GetRoom(vSpawn, NULL) != NULL)
        {
            pArea->ComputeSafeLocation(
                vSpawn, 20.0f, pCreature->GetPathfindInformation(), 1, &vSafe);
        }
        pCreature->AddToArea(pArea, vSafe.m_fX, vSafe.m_fY, vSafe.m_fZ, 1, 0);
        pCreature->SetOrientation(vFacing);

        float fPercent = 1.0f;
        int nPercentXP;
        int bFound;
        {
            CExoString sColumn("PercentXP");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pNPC);
            C2DA* pNPCTable = p2DArrays->m_pNPC;
            bFound = pNPCTable->GetINTEntry(nNPC, sColumn, &nPercentXP);
        }
        if (bFound != 0 && nPercentXP != 0)
        {
            fPercent = nPercentXP * 0.01f;
        }

        int nXP = pCreature->m_pStats->m_nExperience - pCreature->m_nUnknown22C;
        int nPoolXP = m_nXPPool;
        if (nXP < (int)(nPoolXP * fPercent))
        {
            pCreature->ReceiveExperience(
                nPoolXP - (int)((float)nXP / nPercentXP * 100.0f));
            CClientOptions* pClientOptions =
                g_pAppManager->GetClientExoApp()->GetClientOptions();
            if (pClientOptions->m_bAutoLevelUpNPCs != 0 &&
                pCreature->m_pStats->CanLevelUp() != 0)
            {
                pCreature->m_pStats->AutoLevelup(1);
            }
        }
    }
    return oidNPC;
}
// PC 0x0057CC50
void CSWPartyTable::DistributeExperience(int nExperience, int bFeedback)
{
    if (nExperience > 0)
    {
        int nXPPool = m_nXPPool;
        m_nXPPool = nXPPool + nExperience;
        for (int i = 0; i < m_nNumMembers; i++)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(
                m_aoidNPC[m_anMembers[i]]);
            if (pCreature != NULL)
            {
                pCreature->m_sTag.CStr();
                pCreature->ReceiveExperience(nExperience);
                if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                            ->m_bAutoLevelUpNPCs != 0 &&
                    pCreature->m_pStats->CanLevelUp() != 0)
                {
                    pCreature->m_pStats->AutoLevelup(1);
                }
            }
        }

        CSWSClient* pClient =
            g_pAppManager->m_pServerExoApp->GetPlayerList()
                ->GetHead();
        CSWSPlayer* pPlayer = pClient->AsSWSPlayer();
        unsigned long oidPlayer = pPlayer->m_oidPCObject;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pPlayerCreature =
            pServerExoApp->GetCreatureByGameObjectID(oidPlayer);
        if (pPlayerCreature != NULL)
        {
            pPlayerCreature->ReceiveExperience(nExperience);
            if (bFeedback != 0)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetInteger(CSWSMessage::FEEDBACK_INTEGER_EXPERIENCE,
                                  nExperience);
                pPlayerCreature->SendFeedbackMessage(
                    CSWSMessage::FEEDBACK_MESSAGE_EXPERIENCE_GAINED, pData);
            }
        }
    }
}
// PC 0x0057CE60
void CSWPartyTable::SetSoloMode(int bSoloMode, int bUnstealth)
{
    if (bSoloMode == 0 && bUnstealth != 0)
    {
        UnstealthParty(0);
    }
    m_bSoloMode = bSoloMode;
}
// PC 0x0057CF10
void CSWPartyTable::UpdateMembers(int bClearActions)
{
    for (int i = 0; i < 9; i++)
    {
        SaveMember(i, bClearActions);
    }
}
#ifndef _XBOX
int CSWPartyTable::RemoveMember(int nNPC)  // PC 0x0057EB80
{
    if (m_nControlledNPC != -1 || m_nNumMembers <= 0 || nNPC < 0 || nNPC >= 9 ||
        m_abNPCAvailable[nNPC] == 0)
    {
        return 0;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    unsigned long oidNPC = GetNPCObject(nNPC, 0, 1);
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidNPC);
    if (pCreature != NULL)
    {
        pCreature->ClearAllEffects(0);
    }

    for (int i = 0; i < m_nNumMembers; i++)
    {
        if (nNPC == m_anMembers[i])
        {
            for (int j = i + 1; j < m_nNumMembers; j++)
            {
                m_anMembers[j - 1] = m_anMembers[j];
            }
            if (--m_nNumMembers == 0 && m_bSoloMode != 0)
            {
                m_bSoloMode = 0;
            }
            return 1;
        }
    }
    return 0;
}
#endif
int CSWPartyTable::AddMember(int nNPC, unsigned long oidMember)  // PC 0x0057D020
{
    if (m_nControlledNPC == -1 && m_nNumMembers != 2 && nNPC >= 0 && nNPC < 9 &&
        m_abNPCAvailable[nNPC] != 0 &&
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidMember) !=
            NULL)
    {
        for (int i = 0; i < m_nNumMembers; i++)
        {
            if (nNPC == m_anMembers[i])
            {
                return 0;
            }
        }
        for (int j = 0; j < 9; j++)
        {
            if (j != nNPC && m_aoidNPC[j] == oidMember)
            {
                return 0;
            }
        }

        m_anMembers[m_nNumMembers] = nNPC;
        m_aoidNPC[nNPC] = oidMember;

        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pClientCreature =
            pClientExoApp->GetCreatureByGameObjectID(
                pClientExoApp->ServerToClientObjectId(oidMember));
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                oidMember);
        if (pClientCreature != NULL)
        {
            pClientCreature->SetInParty(1);
        }
        if (pCreature != NULL)
        {
            pCreature->SetInParty(1, 1);
            TransferInventory(pCreature);
            pCreature->ForceEquipClothing();
        }
        if (m_nNumMembers == 0)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pPlayer = pServerExoApp->GetCreatureByGameObjectID(
                pServerExoApp->GetPlayerCreatureId());
            if (pPlayer != NULL && pPlayer->m_nStealthMode == 1)
            {
                m_bSoloMode = 1;
            }
        }
        m_nNumMembers++;
        return 1;
    }
    return 0;
}
// PC 0x0057D180
void CSWPartyTable::CreateParty()
{
    if (m_bInventoryDirty == 0)
    {
        AddGameInProgress();
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        pClientExoApp->GetSWParty();

        CResStruct stTop;
        CResStruct stItem;
        CResList lstItemList;
        if (m_nControlledNPC == -1)
        {
            for (unsigned int nMember = 0; nMember < m_nNumMembers; nMember++)
            {
                Vector vOrientation;
                Vector vPosition;
                unsigned long oidNPC = SpawnNPC(
                    m_anMembers[nMember], 0, vPosition, vOrientation,
                    !g_pAppManager->m_pServerExoApp->GetLoadFromSaveGame());
                CSWCCreature* pClientCreature =
                    pClientExoApp->GetCreatureByGameObjectID(
                        pClientExoApp->ServerToClientObjectId(oidNPC));
                CSWSCreature* pCreature =
                    pServerExoApp->GetCreatureByGameObjectID(oidNPC);
                if (pClientCreature != NULL)
                {
                    pClientCreature->SetInParty(1);
                }
                if (pCreature != NULL)
                {
                    pCreature->SetInParty(1, 1);
                }
            }
        }
        else
        {
            m_aoidNPC[m_nControlledNPC] = pServerExoApp->GetPlayerCreatureId();
        }

        CResGFF* pGFF = new CResGFF(0, "INV ", "INVENTORY");
        if (pGFF->GetData() != 0)
        {
            if (m_pPartyItems == NULL)
            {
                m_pPartyItems = new CItemRepository(
                    pServerExoApp->GetPlayerCreatureId(), 1);
            }
            else
            {
                m_pPartyItems->ClearItems();
                m_bInventoryDirty = 0;
            }

            pGFF->GetTopLevelStruct(&stTop);
            pGFF->GetList(&lstItemList, &stTop, "ItemList");
            for (unsigned long i = 0; i < pGFF->GetListCount(&lstItemList); i++)
            {
                CSWSItem* pItem = NULL;
                pGFF->GetListElement(&stItem, &lstItemList, i);
                pItem = new CSWSItem(OBJECT_INVALID);
                if (pItem != NULL)
                {
                    if (pItem->LoadItem(pGFF, &stItem) == 0)
                    {
                        delete pItem;
                    }
                    else if (m_pPartyItems->AddItem(&pItem, 1, 0, 0) != 0 &&
                             pItem != NULL)
                    {
                        pItem->SetPossessor(
                            pServerExoApp->GetPlayerCreatureId(), 0, 0,
                            OBJECT_INVALID);
                    }
                }
            }
        }
        if (pGFF != NULL)
        {
            delete pGFF;
        }
        RemoveGameInProgress();

        CSWSCreature* pPlayer = pServerExoApp->GetPlayerCreature();
        if (pPlayer != NULL)
        {
            TransferInventory(pPlayer);
        }
        for (int j = 0; j < 9; j++)
        {
            CSWSCreature* pNPCCreature =
                pServerExoApp->GetCreatureByGameObjectID(m_aoidNPC[j]);
            if (pNPCCreature != NULL)
            {
                TransferInventory(pNPCCreature);
            }
        }

        pPlayer = pServerExoApp->GetPlayerCreature();
        if (pPlayer != NULL)
        {
            pPlayer->ForceEquipClothing();
        }
        for (int k = 0; k < 9; k++)
        {
            CSWSCreature* pNPCCreature =
                pServerExoApp->GetCreatureByGameObjectID(m_aoidNPC[k]);
            if (pNPCCreature != NULL)
            {
                pNPCCreature->ForceEquipClothing();
            }
        }
        m_bInventoryDirty = 1;
    }
}
// PC 0x0057D800
void CSWPartyTable::PositionParty()
{
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pPlayer = pServerExoApp->GetCreatureByGameObjectID(
        pServerExoApp->GetPlayerCreatureId());
    Vector vFacing = pPlayer->m_vOrientation;
    pParty->ResetAllFollowData(pPlayer->m_vPosition, Yaw(vFacing), 0);
    if (m_bSkipPartyPositioning == 0)
    {
        Vector vPosition;
        int nMember = 0;
        if (m_nNumMembers > 0)
        {
            CSWPartyFollowDataArray* pPaths = &pParty->m_cFollowPaths;
            Vector vAxis(0.0f, 0.0f, 1.0f);
            CSWPartyMemberData* pMember = &pParty->m_aPartyMemberData[1];
            int* pNPC = m_anMembers;
            do
            {
                EXOXBOXASSERT(nMember + 1 >= 0 && nMember + 1 < 3);
                Vector vOffset = pMember->m_vFormationOffset;
                EXOXBOXASSERT(nMember + 1 >= 0 && nMember + 1 < 3);
                Vector vFollow = pMember->m_vFormationPosition;
                EXOXBOXASSERT(nMember + 1 >= 0 && nMember + 1 < 3);
                int nPathIndex = pMember->m_nFormationPathIndex;
                float fFacing = pPaths->GetPathData(nPathIndex)->m_fFacing;
                vPosition =
                    vFollow +
                    Quaternion(vAxis, DegToRad(fFacing)) *
                        vOffset;
                unsigned long oidNPC = GetNPCObject(*pNPC, 0, 1);
                g_pAppManager->m_pServerExoApp->ComputeSafeLocation(
                    vPosition, 10.0f, oidNPC, 0, &vPosition);
                CSWSCreature* pNPCCreature =
                    pServerExoApp->GetCreatureByGameObjectID(oidNPC);
                pNPCCreature->SetPosition(vPosition, 1, 1, 0);
                EXOXBOXASSERT(nMember + 1 >= 0 && nMember + 1 < 3);
                pMember->m_vLastPosition = vPosition;
                pNPCCreature->SetOrientation(vFacing);
                nMember++;
                pMember++;
                pNPC++;
            } while (nMember < m_nNumMembers);
        }
        UnstealthParty(0);
        m_bSoloMode = 0;
    }
    m_bSkipPartyPositioning = 0;
}
// PC 0x0057DC50
int CSWPartyTable::LoadTableInfo()
{
    CResGFF* pGFF = new CResGFF(0, "PT  ", "PARTYTABLE");
    if (pGFF != NULL && pGFF->m_bLoaded != 0)
    {
        int bSuccess;
        CResStruct stTop;
        CResStruct stEntry;
        CResList lstItems;
        unsigned char aTutorialDefault[6];

        ClearTable();
        pGFF->GetTopLevelStruct(&stTop);
        pGFF->GetList(&lstItems, &stTop, "PT_MEMBERS");
        m_nGold = pGFF->ReadFieldDWORD(&stTop, "PT_GOLD", bSuccess, 0);
        m_nXPPool = pGFF->ReadFieldINT(&stTop, "PT_XP_POOL", bSuccess, 0);
        bSuccess = 1;
        m_nPlayedSeconds =
            pGFF->ReadFieldDWORD(&stTop, "PT_PLAYEDSECONDS", bSuccess, 0);
        if (bSuccess == 0)
        {
            m_nPlayedSeconds =
                60 *
                pGFF->ReadFieldDWORD(&stTop, "PT_PLAYEDMINUTES", bSuccess, 0);
        }
        m_nControlledNPC =
            pGFF->ReadFieldINT(&stTop, "PT_CONTROLLED_NPC", bSuccess, -1);
        int bSoloMode = m_bSoloMode;
        bSoloMode = pGFF->ReadFieldBYTE(&stTop, "PT_SOLOMODE", bSuccess, bSoloMode);
        if (bSoloMode == 0)
        {
            UnstealthParty(0);
        }
        m_bSoloMode = bSoloMode;
#ifndef _XBOX
        int bCheatUsed = pGFF->ReadFieldBYTE(&stTop, "PT_CHEAT_USED", bSuccess, 0);
        m_bCheatUsed = bCheatUsed;
#endif
        int nNumMembers = pGFF->ReadFieldBYTE(&stTop, "PT_NUM_MEMBERS", bSuccess, 0);
        m_nNumMembers = nNumMembers;
        if (m_nNumMembers < 0)
        {
            m_nNumMembers = 0;
        }
        else if (pGFF->GetListCount(&lstItems) < (unsigned long)m_nNumMembers)
        {
            m_nNumMembers = pGFF->GetListCount(&lstItems);
        }
        for (int nMember = 0; nMember < m_nNumMembers; nMember++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nMember);
            m_anMembers[nMember] =
                pGFF->ReadFieldINT(&stEntry, "PT_MEMBER_ID", bSuccess, -1);
            int bLeader = pGFF->ReadFieldBYTE(&stEntry, "PT_IS_LEADER", bSuccess, 0);
            if (bLeader != 0)
            {
                m_nLeader = m_anMembers[nMember];
            }
        }

        pGFF->GetList(&lstItems, &stTop, "PT_AVAIL_NPCS");
        int nNumNPCs;
        if (pGFF->GetListCount(&lstItems) < MAX_NPCS)
        {
            nNumNPCs = pGFF->GetListCount(&lstItems);
        }
        else
        {
            nNumNPCs = MAX_NPCS;
        }
        for (int nNPC = 0; nNPC < nNumNPCs; nNPC++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nNPC);
            m_abNPCAvailable[nNPC] =
                pGFF->ReadFieldBYTE(&stEntry, "PT_NPC_AVAIL", bSuccess, 0);
            m_anNPCSelectability[nNPC] =
                pGFF->ReadFieldBYTE(&stEntry, "PT_NPC_SELECT", bSuccess, 0);
        }
        m_nAIState = pGFF->ReadFieldINT(&stTop, "PT_AISTATE", bSuccess, 0);
        m_nFollowState =
            pGFF->ReadFieldINT(&stTop, "PT_FOLLOWSTATE", bSuccess, 0);

        if (pGFF->GetStructFromStruct(&stEntry, &stTop, "GlxyMap") != 0)
        {
            unsigned long nNumPlanets =
                pGFF->ReadFieldDWORD(&stEntry, "GlxyMapNumPnts", bSuccess, 0);
            unsigned long nPlanetMask =
                pGFF->ReadFieldDWORD(&stEntry, "GlxyMapPlntMsk", bSuccess, 0);
            m_nSelectedPlanet =
                pGFF->ReadFieldINT(&stEntry, "GlxyMapSelPnt", bSuccess, -1);
            if (nNumPlanets == 16)
            {
                for (int nPlanet = 0; nPlanet < 16; nPlanet++)
                {
                    if (((1 << nPlanet) & nPlanetMask) != 0)
                    {
                        m_abPlanetAvailable[nPlanet] = 1;
                    }
                    if (((1 << (nPlanet + 16)) & nPlanetMask) != 0)
                    {
                        m_abPlanetSelectable[nPlanet] = 1;
                    }
                }
            }
        }

        pGFF->GetList(&lstItems, &stTop, "PT_PAZAAKCARDS");
        for (int nCard = 0; nCard < PAZAAK_CARD_COUNT; nCard++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nCard);
            m_anPazaakCards[nCard] =
                pGFF->ReadFieldINT(&stEntry, "PT_PAZAAKCOUNT", bSuccess, 0);
        }
        pGFF->GetList(&lstItems, &stTop, "PT_PAZSIDELIST");
        for (int nSide = 0; nSide < 10; nSide++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nSide);
            m_anPazaakSideDeck[nSide] =
                pGFF->ReadFieldINT(&stEntry, "PT_PAZSIDECARD", bSuccess, 0);
        }

        {
            memset(aTutorialDefault, 0, sizeof(aTutorialDefault));
            CGuiInGame* pInGameGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
            pGFF->ReadFieldVOID(&stTop, pInGameGui->m_aTutorialWindowsShown,
                sizeof(aTutorialDefault), "PT_TUT_WND_SHOWN", bSuccess, aTutorialDefault);
        }
        int nLastPanelDefault = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nLastGuiPanel;
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nLastGuiPanel =
            pGFF->ReadFieldINT(&stTop, "PT_LAST_GUI_PNL", bSuccess, nLastPanelDefault);

        CExoString sMessage;
        pGFF->GetList(&lstItems, &stTop, "PT_FB_MSG_LIST");
        for (int nFeedback = 0; nFeedback < (int)pGFF->GetListCount(&lstItems);
             nFeedback++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nFeedback);
            sMessage = pGFF->ReadFieldCExoString(&stEntry, "PT_FB_MSG_MSG", bSuccess, "");
            unsigned long nType = pGFF->ReadFieldDWORD(
                &stEntry, "PT_FB_MSG_TYPE", bSuccess, 0x10000000);
            unsigned char nColor =
                pGFF->ReadFieldBYTE(&stEntry, "PT_FB_MSG_COLOR", bSuccess, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(
                sMessage, nType, nColor);
        }

        CExoString sSpeaker;
        pGFF->GetList(&lstItems, &stTop, "PT_DLG_MSG_LIST");
        for (int nDialog = 0; nDialog < (int)pGFF->GetListCount(&lstItems);
             nDialog++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nDialog);
            sSpeaker = pGFF->ReadFieldCExoString(&stEntry, "PT_DLG_MSG_SPKR", bSuccess, "");
            sMessage = pGFF->ReadFieldCExoString(&stEntry, "PT_DLG_MSG_MSG", bSuccess, "");
            g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->AppendToDialogBuffer(sMessage, sSpeaker);
        }

        pGFF->GetList(&lstItems, &stTop, "PT_COST_MULT_LIST");
        for (int nBaseItem = 0;
             nBaseItem < g_pRules->m_pBaseItems->m_nNumBaseItems; nBaseItem++)
        {
            pGFF->GetListElement(&stEntry, &lstItems, nBaseItem);
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
            pBaseItem->m_fCostMultiplier =
                pGFF->ReadFieldFLOAT(&stEntry, "PT_COST_MULT_VALUE", bSuccess,
                                     1.0f);
        }

        LoadJournal(pGFF, &stTop);
        delete pGFF;
        return 1;
    }
#ifdef _XBOX
    HandleLoadError(0xA3A8);
#else
    HandleLoadError(0xBC4E);
#endif
    if (pGFF != NULL)
    {
        delete pGFF;
    }
    return 0;
}
// PC 0x0057E990
void CSWPartyTable::Save(const CExoString& sAlias, int bSaveMembers)
{
    if (bSaveMembers != 0)
    {
        for (int i = 0; i < 9; i++)
        {
            SaveMember(i, 0);
        }
        UpdateInventory(0);
    }
    SaveTableInfo(sAlias);
}
// PC 0x005795E0
void CSWPartyTable::ClearTable()
{
#ifndef _XBOX
    m_nGold = 0;
    m_nGameInProgress = 0;
    m_nControlledNPC = -1;
    m_nXPPool = 0;
    m_nReturnFlag = 1;
    m_nReturnStrref = 32179;
    m_nReturnTitleStrref = 42120;
    m_nPlayedSeconds = 0;
    m_bSkipPartyPositioning = 0;

    for (int i = 0; i < 16; i++)
    {
        m_abPlanetAvailable[i] = 0;
        m_abPlanetSelectable[i] = 0;
    }

    m_nSelectedPlanet = -1;
    m_bSoloMode = 0;
#ifndef _XBOX
    m_bCheatUsed = 0;
#endif

    if (m_pPartyItems != NULL)
    {
        delete m_pPartyItems;
        m_pPartyItems = NULL;
    }
#endif

    m_nNumMembers = 0;
    m_nAIState = 0;
    m_nFollowState = 0;
    m_nLeader = -1;
    m_bInventoryDirty = 0;

    for (int i = 0; i < 9; i++)
    {
        m_abNPCAvailable[i] = 0;
        m_anNPCSelectability[i] = 1;
        m_aoidNPC[i] = OBJECT_INVALID;
    }

#ifdef _XBOX
    for (int i = 0; i < 5; i++)
    {
        m_anPazaakCards[i] = 2;
    }
    for (int i = 5; i < 18; i++)
    {
        m_anPazaakCards[i] = 0;
    }
#else
    m_anPazaakCards[0] = 2;
    m_anPazaakCards[1] = 2;
    m_anPazaakCards[2] = 2;
    m_anPazaakCards[3] = 2;
    m_anPazaakCards[4] = 2;
    for (int i = 5; i < 18; i++)
    {
        m_anPazaakCards[i] = 0;
    }
#endif
    for (int i = 0; i < 10; i++)
    {
        m_anPazaakSideDeck[i] = -1;
    }

    if (m_pPartyJournal != NULL)
    {
        delete m_pPartyJournal;
        m_pPartyJournal = NULL;
    }
}
// PC 0x00579780
CItemRepository* CSWPartyTable::GetPartyItems()
{
    if (m_pPartyItems == NULL)
    {
        m_pPartyItems = new CItemRepository(
            g_pAppManager->m_pServerExoApp->GetPlayerCreatureId(), 1);
    }
    return m_pPartyItems;
}
// PC 0x00579800
CSWSJournal* CSWPartyTable::GetPartyJournal()
{
    if (m_pPartyJournal == NULL)
    {
        m_pPartyJournal = new CSWSJournal;
    }
    return m_pPartyJournal;
}
// PC 0x00579870
void CSWPartyTable::LoadJournal(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    CResList lstEntries;

    if (GetPartyJournal() != NULL)
    {
        if (pRes->GetList(&lstEntries, pStruct, "JNL_Entries") != 0)
        {
            g_nJournalSortType = pRes->ReadFieldINT(
                pStruct, "JNL_SortOrder", bSuccess, g_nJournalSortType);

            CResStruct stEntry;
            int nCount;
            CExoString sPlotID;
            nCount = pRes->GetListCount(&lstEntries);
            for (int i = 0; i < nCount; i++)
            {
                pRes->GetListElement(&stEntry, &lstEntries, i);
                {
                    CExoString sDefault("");
                    sPlotID = pRes->ReadFieldCExoString(&stEntry, "JNL_PlotID",
                                                        bSuccess, sDefault);
                }
                unsigned long nState =
                    pRes->ReadFieldINT(&stEntry, "JNL_State", bSuccess, 0);
                GetPartyJournal()->SetState(sPlotID, nState, 1);
                unsigned long nDate =
                    pRes->ReadFieldDWORD(&stEntry, "JNL_Date", bSuccess, 0);
                GetPartyJournal()->SetDate(sPlotID, nDate, 1);
                unsigned long nTime =
                    pRes->ReadFieldDWORD(&stEntry, "JNL_Time", bSuccess, 0);
                GetPartyJournal()->SetTime(sPlotID, nTime, 1);
            }
        }
    }
}
// PC 0x00579A40
void CSWPartyTable::ClearInventory()
{
    if (m_pPartyItems != NULL)
    {
        m_pPartyItems->ClearItems();
    }
    m_bInventoryDirty = 0;
}
// PC 0x00579AF0
CExoString CSWPartyTable::GetFilename(int nNPC)
{
    CExoString sFilename;
    sFilename.Format("AVAILNPC%d", nNPC);
    return sFilename;
}
// PC 0x00579B80
int CSWPartyTable::GetIsNPCAvailable(int nNPC)
{
    if (nNPC >= 0 && nNPC < 9)
    {
        return m_abNPCAvailable[nNPC];
    }
    return 0;
}

unsigned char CSWPartyTable::GetNumNPCAvailable()
{
    unsigned char nNum = 0;
    int* pAvailable = m_abNPCAvailable;
    for (int i = 9; i != 0; i--)
    {
        if (*pAvailable != 0)
        {
            nNum++;
        }
        pAvailable++;
    }
    return nNum;
}
// PC 0x00579C00
int CSWPartyTable::SetNPCObject(int nNPC, unsigned long oidNPC)
{
    if (nNPC >= 0 && nNPC < 9 && m_abNPCAvailable[nNPC] != 0)
    {
        m_aoidNPC[nNPC] = oidNPC;
    }
    return nNPC;
}
// PC 0x00579C50
int CSWPartyTable::GetIsMember(int nNPC)
{
    for (int i = 0; i < m_nNumMembers; i++)
    {
        if (nNPC == m_anMembers[i])
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00579CB0
int CSWPartyTable::GetIsMember(unsigned long oidObject)
{
    for (int nNPC = 0; nNPC < 9; nNPC++)
    {
        if (oidObject == m_aoidNPC[nNPC])
        {
            for (int i = 0; i < m_nNumMembers; i++)
            {
                if (m_anMembers[i] == nNPC)
                {
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}
// PC 0x00579D00
int CSWPartyTable::GetIsAvailable(unsigned long oidObject)
{
    for (int i = 0; i < 9; i++)
    {
        if (oidObject == m_aoidNPC[i])
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00579D30
unsigned char CSWPartyTable::GetNPCSelectability(int nNPC)
{
    if (nNPC >= 0 && nNPC < 9 && m_abNPCAvailable[nNPC] != 0)
    {
        return m_anNPCSelectability[nNPC];
    }
    return 0xFF;
}
// PC 0x00579D60
int CSWPartyTable::SetNPCSelectability(int nNPC, unsigned char nSelectability)
{
    if (nNPC >= 0 && nNPC < 9 && m_abNPCAvailable[nNPC] != 0)
    {
        m_anNPCSelectability[nNPC] = nSelectability;
    }
    return nNPC;
}
#ifndef _XBOX
int CSWPartyTable::KillNPCObject(int nNPC, int bFade)  // PC 0x00579D80
{
    if (nNPC < 0 || nNPC >= 9)
    {
        return 0;
    }
    if (GetIsMember(nNPC) != 0)
    {
        return 0;
    }

    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            m_aoidNPC[nNPC]);
    m_aoidNPC[nNPC] = OBJECT_INVALID;
    if (pCreature == NULL)
    {
        return 0;
    }

    if (bFade != 0)
    {
        CSWCCreature* pClientCreature = pCreature->GetClientCreature();
        if (pClientCreature != NULL)
        {
            pClientCreature->SetFadeState(CSWCObject::FADE_STATE_HIDDEN);
            pClientCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 1, 0, 0);
            pClientCreature->m_bFadeLocked = 1;
        }
    }

    delete pCreature;
    return 1;
}
#endif
int CSWPartyTable::GetNPCID(unsigned long oidObject)  // PC 0x00579EB0
{
    if (oidObject == OBJECT_INVALID)
    {
        return -1;
    }

    for (int i = 0; i < 9; i++)
    {
        if (m_aoidNPC[i] == oidObject)
        {
            return i;
        }
    }
    return -1;
}
// PC 0x00579F20
int CSWPartyTable::AddGameInProgress()
{
    if (m_nGameInProgress == 0)
    {
        CExoString sAlias("GAMEINPROGRESS:");
        g_pExoResMan->AddResourceDirectory(sAlias);
    }
    return ++m_nGameInProgress;
}
// PC 0x00579FA0
void CSWPartyTable::RemoveGameInProgress()
{
    if (--m_nGameInProgress == 0)
    {
        CExoString sAlias("GAMEINPROGRESS:");
        g_pExoResMan->RemoveResourceDirectory(sAlias);
    }
}
// PC 0x0057A010
int CSWPartyTable::ClearMembers()
{
    for (int i = 0; i < 9; i++)
    {
        m_aoidNPC[i] = OBJECT_INVALID;
    }
    return OBJECT_INVALID;
}
// PC 0x0057A070
int CSWPartyTable::ClearMember(int nNPC)
{
    if (nNPC >= 0 && nNPC < 9)
    {
        m_aoidNPC[nNPC] = OBJECT_INVALID;
    }
    return nNPC;
}
// PC 0x0057A090
int CSWPartyTable::GetIsLeader(unsigned long oidObject)
{
    if (oidObject == OBJECT_INVALID)
    {
        return 0;
    }

    for (int nNPC = 0; nNPC < 9; nNPC++)
    {
        if (oidObject == m_aoidNPC[nNPC])
        {
            return m_nLeader == nNPC;
        }
    }
    return 0;
}
// PC 0x0057A0D0
void CSWPartyTable::SetLeader(unsigned long oidObject)
{
    m_nLeader = -1;
    if (oidObject != OBJECT_INVALID)
    {
        for (int i = 0; i < 9; i++)
        {
            if (oidObject == m_aoidNPC[i])
            {
                m_nLeader = i;
                break;
            }
        }
    }
}
// PC 0x0057A110
void CSWPartyTable::SetPartyFollowState(int nFollowState)
{
    m_nFollowState = 0;
    g_pAppManager->m_pClientExoApp->GetSWParty()->FollowStateChanged();
}
// PC 0x0057A140
int CSWPartyTable::SetReturnStrref(int nReturnFlag, unsigned long nStrref,
                                   unsigned long nTitleStrref)
{
    m_nReturnFlag = nReturnFlag;
    m_nReturnStrref = nStrref;
    m_nReturnTitleStrref = nTitleStrref;
    return nTitleStrref;
}
// PC 0x0057A170
int CSWPartyTable::SetPlanetSelectable(unsigned long nPlanet, int bSelectable)
{
    if (nPlanet < 16)
    {
        m_abPlanetSelectable[nPlanet] = bSelectable;
    }
    return nPlanet;
}
// PC 0x0057A190
int CSWPartyTable::GetPlanetSelectable(unsigned long nPlanet)
{
    if (nPlanet < 16)
    {
        return m_abPlanetSelectable[nPlanet];
    }
    return 0;
}
// PC 0x0057A1B0
int CSWPartyTable::SetPlanetAvailable(unsigned long nPlanet, int bAvailable)
{
    if (nPlanet < 16)
    {
        m_abPlanetAvailable[nPlanet] = bAvailable;
    }
    return nPlanet;
}
// PC 0x0057A1D0
int CSWPartyTable::GetPlanetAvailable(unsigned long nPlanet)
{
    if (nPlanet < 16)
    {
        return m_abPlanetAvailable[nPlanet];
    }
    return 0;
}
// PC 0x0057A1F0
int CSWPartyTable::SetSelectedPlanet(int nPlanet)
{
    if (nPlanet >= 0 && nPlanet < 16 && m_abPlanetAvailable[nPlanet] != 0 &&
        m_abPlanetSelectable[nPlanet] != 0)
    {
        m_nSelectedPlanet = nPlanet;
    }
    return nPlanet;
}
// PC 0x0057A220
void CSWPartyTable::AutoLevelUpNPCs()
{
    int i = 0;
    if (m_nNumMembers > 0)
    {
        do
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(
                m_aoidNPC[m_anMembers[i]]);
            if (pCreature != NULL && pCreature->m_pStats->CanLevelUp() != 0)
            {
                pCreature->m_pStats->AutoLevelup(1);
            }
            i++;
        } while (i < m_nNumMembers);
    }
}
// PC 0x0057A280
int CSWPartyTable::GetTotalPlayedSeconds()
{
    FILETIME ftNow;
    GetSystemTimeAsFileTime(&ftNow);

    __int64 nNow = ftNow.dwLowDateTime | ((__int64)ftNow.dwHighDateTime << 32);
    __int64 nElapsed = nNow - g_nCurrentSessionStartFILETIME;
    if (nElapsed < 0)
    {
        nElapsed = 0;
    }
    return m_nPlayedSeconds + (nElapsed + 5000000) / 10000000ui64;
}
// PC 0x0057A2F0
void CSWPartyTable::UnstealthParty(int nFlag)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer = pServerExoApp->GetClientObjectByObjectId(
        pServerExoApp->GetPlayerCreatureId());
    if (pPlayer != NULL)
    {
        unsigned long oidCreature = pPlayer->m_oidPCObject;
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                oidCreature);
        if (pCreature != NULL)
        {
            pCreature->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, nFlag);
        }
    }

    unsigned char nMember = 0;
    if (m_nNumMembers > 0)
    {
        do
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pMember = pServer->GetCreatureByGameObjectID(
                m_aoidNPC[m_anMembers[nMember]]);
            if (pMember != NULL)
            {
                pMember->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, nFlag);
            }
            nMember++;
        } while (nMember < m_nNumMembers);
    }
}
// PC 0x0057A380
int CSWPartyTable::ResetCurrentSessionStartTime()
{
    FILETIME ftNow;
    GetSystemTimeAsFileTime(&ftNow);
    g_nCurrentSessionStartFILETIME =
        ftNow.dwLowDateTime | ((__int64)ftNow.dwHighDateTime << 32);
    return ftNow.dwLowDateTime;
}
// PC 0x0057A5B0
CSWPartyTable::CSWPartyTable()
{
    m_pPartyItems = NULL;
    m_pPartyJournal = NULL;
#ifdef _XBOX
    m_nGold = 0;
    m_nGameInProgress = 0;
    m_nControlledNPC = -1;
    m_nXPPool = 0;
    m_nReturnFlag = 1;
    m_nReturnStrref = 32179;
    m_nReturnTitleStrref = 42120;
    m_nPlayedSeconds = 0;
    m_bSkipPartyPositioning = 0;

    for (int i = 0; i < 16; i++)
    {
        m_abPlanetAvailable[i] = 0;
        m_abPlanetSelectable[i] = 0;
    }

    m_nSelectedPlanet = -1;
    m_bSoloMode = 0;
#endif
    ClearTable();
}
// PC 0x0057A610
CSWPartyTable::~CSWPartyTable()
{
    if (m_pPartyItems != NULL)
    {
        delete m_pPartyItems;
        m_pPartyItems = NULL;
    }

    if (m_pPartyJournal != NULL)
    {
        delete m_pPartyJournal;
        m_pPartyJournal = NULL;
    }
}
// PC 0x0057A6B0
void CSWPartyTable::SaveJournal(CResGFF* pRes, CResStruct* pStruct)
{
    CSWSJournal* pJournal = GetPartyJournal();
    if (pJournal != NULL && pJournal->m_lstEntries.num != 0)
    {
        CResList lstEntries;
        CResStruct stEntry;

        pRes->WriteFieldINT(pStruct, g_nJournalSortType, "JNL_SortOrder");
        pRes->AddList(&lstEntries, pStruct, "JNL_Entries");
        for (int i = 0; i < pJournal->m_lstEntries.num; i++)
        {
            pRes->AddListElement(&stEntry, &lstEntries, 0);
            pRes->WriteFieldCExoString(
                &stEntry, pJournal->m_lstEntries.element[i].m_sPlotID,
                "JNL_PlotID");
            pRes->WriteFieldINT(&stEntry,
                                pJournal->m_lstEntries.element[i].m_nState,
                                "JNL_State");
            pRes->WriteFieldDWORD(&stEntry,
                                  pJournal->m_lstEntries.element[i].m_nDate,
                                  "JNL_Date");
            pRes->WriteFieldDWORD(&stEntry,
                                  pJournal->m_lstEntries.element[i].m_nTime,
                                  "JNL_Time");
        }
    }
}
// PC 0x0057A7A0
void CSWPartyTable::SaveMember(int nNPC, int bClearActions)
{
    if (nNPC >= 0 && nNPC < 9)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature =
            pServerExoApp->GetCreatureByGameObjectID(m_aoidNPC[nNPC]);
        if (pCreature != NULL)
        {
            if (bClearActions != 0)
            {
                pCreature->ClearAllActions(1);
            }

            CExoString sFilename = GetFilename(nNPC);
            CResGFF* pGFF = NULL;
            pGFF = new CResGFF;
            CResStruct stTop;
            pGFF->CreateGFFFile(&stTop, "UTC ", "V2.0");
            pCreature->SaveCreature(pGFF, &stTop);

            CExoString sPath = CExoString("GAMEINPROGRESS:") + sFilename;
            {
                CExoString sFile(sPath.CStr());
                pGFF->WriteGFFFile(sFile, RESTYPE_UTC);
            }
            delete pGFF;
        }
    }
}
// PC 0x0057AE60
void CSWPartyTable::UpdateInventory(int bClearItems)
{
    if (m_pPartyItems != NULL)
    {
        CResGFF* pGFF = new CResGFF;
        CResStruct stTop;
        CResStruct stItem;
        CResList lstItems;
        pGFF->CreateGFFFile(&stTop, "INV ", "V2.0");
        pGFF->AddList(&lstItems, &stTop, "ItemList");

        for (unsigned long nItem = 0; nItem < m_pPartyItems->m_lstItems.num;
             nItem++)
        {
            CSWSItem* pItem = m_pPartyItems->ItemListGetItem(nItem);
            if (pItem != NULL &&
                pGFF->AddListElement(&stItem, &lstItems, 0) != 0)
            {
                pItem->SaveItem(pGFF, &stItem);
            }
        }

        CExoString sPath("GAMEINPROGRESS:INVENTORY");
        pGFF->WriteGFFFile(sPath.CStr(), 0);
        if (bClearItems != 0)
        {
            m_pPartyItems->ClearItems();
        }
        delete pGFF;
    }
}
// PC 0x0057B010
void CSWPartyTable::TransferInventory(CSWSCreature* pCreature)
{
    CSWSItem* pItem;
    if (pCreature != NULL)
    {
        int nOldValue = pCreature->m_bInParty;
        pCreature->SetInParty(0, 0);
        m_nGold += pCreature->GetGold();
        CItemRepository* pItems = pCreature->GetItemRepository(1);
        pCreature->SetInParty(nOldValue, 0);

        if (m_pPartyItems == NULL)
        {
            m_pPartyItems = new CItemRepository(
                g_pAppManager->m_pServerExoApp->GetPlayerCreatureId(), 1);
        }
        for (unsigned long i = 0; i < pItems->m_lstItems.num; i++)
        {
            pItem = pItems->ItemListGetItem(i);
            if (pItem != NULL)
            {
                pItem->SetPossessor(CGameObjectArray::MAXCHAROBJID, 0, 0,
                                    OBJECT_INVALID);
                m_pPartyItems->AddItem(&pItem, 1, 0, 1);
            }
        }
        pItems->ClearItems();
    }
}
#ifndef _XBOX
int CSWPartyTable::AddNPC(int nNPC, CSWSCreature* pCreature)  // PC 0x0057B260
{
    if (nNPC < 0 || nNPC >= MAX_NPCS)
    {
        return 0;
    }
    if (pCreature == NULL || m_abNPCAvailable[nNPC] != 0)
    {
        return 0;
    }

    m_abNPCAvailable[nNPC] = 1;
    TransferInventory(pCreature);

    int nBaseXP = 0;
    float fPercent = 1.0f;
    int nPercentXP;
    int bFound;
    {
        CExoString sColumn("PercentXP");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        EXOXBOXASSERT(p2DArrays->m_pNPC);
        C2DA* pNPCTable = p2DArrays->m_pNPC;
        bFound = pNPCTable->GetINTEntry(nNPC, sColumn, &nPercentXP);
    }
    if (bFound != 0 && nPercentXP != 0)
    {
        fPercent = nPercentXP * 0.01f;
    }
    int nXPPool = m_nXPPool;
    int nPoolXP = (int)(nXPPool * fPercent);

    int nLevel = 1;
    if (pCreature->GetStats()->GetLevel(0) >= 1)
    {
        int nNextLevelXP = 1000;
        do
        {
            nBaseXP = nBaseXP + nNextLevelXP - 1000;
            nLevel++;
            nNextLevelXP += 1000;
        } while (nLevel <= pCreature->GetStats()->GetLevel(0));
    }
    if (nBaseXP <= nPoolXP)
    {
        pCreature->m_nUnknown22C = 0;
    }
    else
    {
        pCreature->m_nUnknown22C = nBaseXP - nPoolXP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
    CSWSCreature* pPlayer = pServerExoApp->GetCreatureByGameObjectID(
        pServerExoApp->GetPlayerCreatureId());
    if (pPlayer != NULL && pPlayer->GetFaction() != NULL)
    {
        pPlayer->GetFaction()->AddMember(pCreature->GetId(), 0);
    }
    pCreature->GetStats()->SetMovementRate(0);

    CExoString sPath;
    sPath.Format("GAMEINPROGRESS:%s", GetFilename(nNPC));
    CResGFF* pGFF = new CResGFF;
    CResStruct stTop;
    pGFF->CreateGFFFile(&stTop, "UTC ", "V2.0");
    pCreature->SaveCreature(pGFF, &stTop);
    return pGFF->WriteGFFFile(sPath.CStr(), RESTYPE_UTC);
}
#endif
int CSWPartyTable::AddNPC(int nNPC, const CExoString& sTemplate)  // PC 0x0057B760
{
    if (nNPC < 0 || nNPC >= 9 || m_abNPCAvailable[nNPC] != 0)
    {
        return 0;
    }

    CSWSCreature* pCreature = new CSWSCreature(OBJECT_INVALID, 0);
    CResRef cResRef(sTemplate);
    if (pCreature->LoadFromTemplate(cResRef, 0) == 0)
    {
        delete (CSWSObject*)pCreature;
        return 0;
    }

    int nResult = AddNPC(nNPC, pCreature);
    delete (CSWSObject*)pCreature;
    return nResult;
}
// PC 0x0057B920
int CSWPartyTable::RemoveNPC(int nNPC)
{
    if (nNPC < 0)
    {
        return 0;
    }

    if (nNPC >= 9)
    {
        return 0;
    }

    if (m_abNPCAvailable[nNPC] == 0)
    {
        return 0;
    }

    m_abNPCAvailable[nNPC] = 0;
    return 1;
}
// PC 0x0057BB30
unsigned long CSWPartyTable::GetNPCObject(int nNPC, int bLoad, int bResurrect)
{
    if (nNPC >= 0 && nNPC < 9 && m_abNPCAvailable[nNPC] != 0)
    {
        if (m_aoidNPC[nNPC] != OBJECT_INVALID)
        {
            return m_aoidNPC[nNPC];
        }
        if (bLoad != 0)
        {
            CSWSCreature* pCreature = new CSWSCreature(OBJECT_INVALID, 0);
            AddGameInProgress();
            bool bFailed = pCreature->LoadFromTemplate(GetFilename(nNPC), 0) == 0;
            if (bFailed)
            {
                delete pCreature;
                return OBJECT_INVALID;
            }
            RemoveGameInProgress();

            if (bResurrect != 0 && pCreature->GetCurrentHitPoints(0) <= 0)
            {
                pCreature->m_nF0 = 1;
                CGameEffect* pEffect = new CGameEffect(1);
                pEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
                pEffect->m_nSubType =
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
                pCreature->ApplyEffect(pEffect, 0, 0);
            }
            m_aoidNPC[nNPC] = pCreature->m_nId;
            return pCreature->m_nId;
        }
    }
    return OBJECT_INVALID;
}
// PC 0x0057BCF0
void CSWPartyTable::SaveTableInfo(const CExoString& sAlias)
{
    CExoString sFile;
    CResGFF* pGFF = NULL;
    unsigned int nPlanetMask = 0;
    pGFF = new CResGFF;
    EXOXBOXASSERT(pGFF);
    CResStruct stEntry;
    CResStruct stTop;
    CResList lstItems;

    int bCreated =
        pGFF->CreateGFFFile(&stTop, "PT  ", "V2.0");
    if (bCreated == 1)
    {
        sFile = sAlias + "\\" + "PARTYTABLE";
        m_nPlayedSeconds = GetTotalPlayedSeconds();
        ResetCurrentSessionStartTime();

        pGFF->WriteFieldDWORD(&stTop, m_nGold, "PT_GOLD");
        pGFF->WriteFieldINT(&stTop, m_nXPPool, "PT_XP_POOL");
        pGFF->WriteFieldDWORD(&stTop, m_nPlayedSeconds, "PT_PLAYEDSECONDS");
        pGFF->WriteFieldINT(&stTop, m_nControlledNPC, "PT_CONTROLLED_NPC");
        int bSoloMode = m_bSoloMode;
        pGFF->WriteFieldBYTE(&stTop, bSoloMode, "PT_SOLOMODE");
#ifndef _XBOX
        int bCheatUsed = m_bCheatUsed;
        pGFF->WriteFieldBYTE(&stTop, bCheatUsed, "PT_CHEAT_USED");
#endif
        pGFF->WriteFieldBYTE(&stTop, m_nNumMembers, "PT_NUM_MEMBERS");

        pGFF->AddList(&lstItems, &stTop, "PT_MEMBERS");
        for (int nMember = 0; nMember < m_nNumMembers; nMember++)
        {
            int bLeader = m_anMembers[nMember] == m_nLeader;
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            pGFF->WriteFieldINT(&stEntry, m_anMembers[nMember], "PT_MEMBER_ID");
            pGFF->WriteFieldBYTE(&stEntry, bLeader, "PT_IS_LEADER");
        }

        pGFF->AddList(&lstItems, &stTop, "PT_AVAIL_NPCS");
        for (int nNPC = 0; nNPC < MAX_NPCS; nNPC++)
        {
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            pGFF->WriteFieldBYTE(&stEntry, m_abNPCAvailable[nNPC],
                                 "PT_NPC_AVAIL");
            pGFF->WriteFieldBYTE(&stEntry, m_anNPCSelectability[nNPC],
                                 "PT_NPC_SELECT");
        }
        pGFF->WriteFieldINT(&stTop, m_nAIState, "PT_AISTATE");
        pGFF->WriteFieldINT(&stTop, m_nFollowState, "PT_FOLLOWSTATE");

        for (int nPlanet = 0; nPlanet < GALAXY_MAP_NUM_POINTS; nPlanet++)
        {
            if (m_abPlanetAvailable[nPlanet] != 0)
            {
                nPlanetMask |= 1 << nPlanet;
            }
            if (m_abPlanetSelectable[nPlanet] != 0)
            {
                nPlanetMask |= 1 << (nPlanet + GALAXY_MAP_SELECTABLE_SHIFT);
            }
        }
        pGFF->AddStructToStruct(&stEntry, &stTop, "GlxyMap", 0);
        pGFF->WriteFieldDWORD(&stEntry, GALAXY_MAP_NUM_POINTS, "GlxyMapNumPnts");
        pGFF->WriteFieldDWORD(&stEntry, nPlanetMask, "GlxyMapPlntMsk");
        pGFF->WriteFieldINT(&stEntry, m_nSelectedPlanet, "GlxyMapSelPnt");

        pGFF->AddList(&lstItems, &stTop, "PT_PAZAAKCARDS");
        for (int nCard = 0; nCard < PAZAAK_CARD_COUNT; nCard++)
        {
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            pGFF->WriteFieldINT(&stEntry, m_anPazaakCards[nCard],
                                "PT_PAZAAKCOUNT");
        }
#ifndef _XBOX
        pGFF->AddListElement(&stEntry, &lstItems, 0);
        int bPazaakCheatUsed = m_bCheatUsed;
        pGFF->WriteFieldBYTE(&stEntry, bPazaakCheatUsed, "PT_PAZAAKCOUNT");
#endif

        pGFF->AddList(&lstItems, &stTop, "PT_PAZSIDELIST");
        for (int nSide = 0; nSide < PAZAAK_SIDE_DECK_SIZE; nSide++)
        {
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            pGFF->WriteFieldINT(&stEntry, m_anPazaakSideDeck[nSide],
                                "PT_PAZSIDECARD");
        }

        CGuiInGame* pInGameGui =
            g_pAppManager->GetClientExoApp()->GetInGameGui();
        pGFF->WriteFieldVOID(&stTop, pInGameGui->m_aTutorialWindowsShown,
                             sizeof(pInGameGui->m_aTutorialWindowsShown),
                             "PT_TUT_WND_SHOWN");
        int nLastGuiPanel =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nLastGuiPanel;
        pGFF->WriteFieldINT(&stTop, nLastGuiPanel, "PT_LAST_GUI_PNL");

        pGFF->AddList(&lstItems, &stTop, "PT_FB_MSG_LIST");
        SFeedbackMessage* pMessages =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMessageBuffer();
        for (int nMessage = 0;
             nMessage < g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->GetMessageBufferSize();
             nMessage++)
        {
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            pGFF->WriteFieldCExoString(&stEntry, pMessages[nMessage].m_sMessage,
                                       "PT_FB_MSG_MSG");
            pGFF->WriteFieldDWORD(&stEntry, pMessages[nMessage].m_nType,
                                  "PT_FB_MSG_TYPE");
            pGFF->WriteFieldBYTE(&stEntry, pMessages[nMessage].m_nColor,
                                 "PT_FB_MSG_COLOR");
        }

        pGFF->AddList(&lstItems, &stTop, "PT_DLG_MSG_LIST");
        CGuiInGameDialogMessage* pDialog =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pDialogBuffer;
        for (int nDialog = 0;
             nDialog < (int)g_pAppManager->GetClientExoApp()->GetInGameGui()
                           ->m_nDialogBufferSize;
             nDialog++, pDialog++)
        {
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            pGFF->WriteFieldCExoString(&stEntry, pDialog->m_sSpeaker,
                                       "PT_DLG_MSG_SPKR");
            pGFF->WriteFieldCExoString(&stEntry, pDialog->m_sMessage,
                                       "PT_DLG_MSG_MSG");
        }

        pGFF->AddList(&lstItems, &stTop, "PT_COST_MULT_LIST");
        for (int nBaseItem = 0;
             nBaseItem < g_pRules->m_pBaseItems->m_nNumBaseItems; nBaseItem++)
        {
            pGFF->AddListElement(&stEntry, &lstItems, 0);
            float fCostMultiplier =
                g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_fCostMultiplier;
            pGFF->WriteFieldFLOAT(&stEntry, fCostMultiplier, "PT_COST_MULT_VALUE");
        }

        SaveJournal(pGFF, &stTop);
        pGFF->WriteGFFFile(sFile, 0);
    }

    if (sAlias == "REBOOTDATA:")
    {
        g_pExoResMan->CreateSignatureFile(sFile, 0, 0, 0);
    }
    if (pGFF != NULL)
    {
        delete pGFF;
    }
}
// PC 0x0057E9C0
void CSWPartyTable::GivePlotXP(const CExoString& sPlotID, int nPercent)
{
    int nXP = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pPlotXP);
    C2DA* pPlot2DA = p2DArrays->m_pPlotXP;
    if (pPlot2DA != NULL)
    {
        {
            CExoString sColumn("XP");
            pPlot2DA->GetINTEntry(sPlotID, sColumn, &nXP);
        }
        int nGiven = (int)ceil(nPercent * nXP * 0.01f);
        if (nGiven > 0)
        {
            DistributeExperience(nGiven, 1);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(
                2, nGiven);
        }
    }
}
// PC 0x0057EAA0
void CSWPartyTable::GivePlotXP(unsigned long nPlotIndex, int nPercent)
{
    int nXP = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pPlotXP);
    C2DA* pPlot2DA = p2DArrays->m_pPlotXP;
    if (pPlot2DA != NULL)
    {
        {
            CExoString sColumn("XP");
            pPlot2DA->GetINTEntry(nPlotIndex, sColumn, &nXP);
        }
        int nGiven = (int)ceil(nPercent * nXP * 0.01f);
        if (nGiven > 0)
        {
            DistributeExperience(nGiven, 1);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(
                2, nGiven);
        }
    }
}

inline unsigned long CSWSPlayer::GetPCObject() { return m_oidPCObject; }
// PC 0x0057EC70
int CSWPartyTable::SwitchPlayerCharacter(int nNPC)
{
    CExoString sPath;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer =
        (CSWSPlayer*)pServerExoApp->GetPlayerList()->GetHead();
    CSWSCreature* pCurrent =
        pServerExoApp->GetCreatureByGameObjectID(pPlayer->GetPCObject());
    CSWSArea* pArea = pCurrent->GetArea();
    int bMoved = 1;
    Vector vPosition;
    Vector vOrientation;
    CSWSCreature* pTarget = NULL;
    int nNewControlled;

    if (nNPC == -1)
    {
        if (m_nControlledNPC == -1)
        {
            return 1;
        }

        CSWSCreature* pOldPlayer = pServerExoApp->GetPlayerCreature();
        pOldPlayer->GetStats()->m_bIsPC = 0;
        pOldPlayer->m_nAreaWaitState = 0;
        SaveMember(m_nControlledNPC, 0);

        pTarget = new CSWSCreature(OBJECT_INVALID, 0);
        AddGameInProgress();
        if (pTarget->LoadFromTemplate("PC", 0) == 0)
        {
            delete pTarget;
            return 0;
        }
        RemoveGameInProgress();
        vPosition = pCurrent->m_vPosition;
        vOrientation = pCurrent->m_vOrientation;
        KillNPCObject(m_nControlledNPC, 1);
        nNewControlled = -1;
    }
    else if (GetIsNPCAvailable(nNPC) != 0)
    {
        if (m_nControlledNPC == nNPC)
        {
            return 1;
        }

        if (m_nControlledNPC == -1)
        {
            sPath = "GAMEINPROGRESS:PC";
            CResGFF* pGFF;
            {
                CResStruct stTop;
                pGFF = new CResGFF;
                pGFF->CreateGFFFile(&stTop, "UTC ", "V2.0");
                pCurrent->SaveCreature(pGFF, &stTop);
            }
            if (pGFF->WriteGFFFile(sPath.CStr(), RESTYPE_UTC) == 0)
            {
                return 0;
            }
        }
        else
        {
            unsigned long oidOldPlayer = pServerExoApp->GetPlayerCreatureId();
            CSWSCreature* pOldPlayer =
                pServerExoApp->GetCreatureByGameObjectID(oidOldPlayer);
            pOldPlayer->GetStats()->m_bIsPC = 0;
            pOldPlayer->m_nAreaWaitState = 0;
            SaveMember(m_nControlledNPC, 0);
        }

        pTarget = pServerExoApp->GetCreatureByGameObjectID(GetNPCObject(nNPC, 0, 1));
        if (pTarget == NULL)
        {
            pTarget = pServerExoApp->GetCreatureByGameObjectID(
                GetNPCObject(nNPC, 1, 1));
            vPosition = pCurrent->m_vPosition;
            vOrientation = pCurrent->m_vOrientation;
        }
        else
        {
            bMoved = 0;
        }

        if (m_nControlledNPC == -1)
        {
            CSWCCreature* pClientCreature = pCurrent->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pClientCreature->SetFadeState(CSWCObject::FADE_STATE_HIDDEN);
                pClientCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 1, 0, 0);
                pClientCreature->m_bFadeLocked = 1;
            }
            if (pCurrent != NULL)
            {
                delete pCurrent;
            }
        }
        else
        {
            KillNPCObject(m_nControlledNPC, 1);
        }
        nNewControlled = nNPC;
    }

    if (pTarget != NULL)
    {
        pClientExoApp->GetSWParty()->ClearParty();
        pTarget->SetInParty(1, 1);
        pTarget->GetStats()->m_bIsPC = 1;
        pTarget->m_nAreaWaitState = 1;
        CItemRepository* pItems = pTarget->GetItemRepository(1);
        if (pItems != NULL)
        {
            for (unsigned long i = 0; i < pItems->GetNumItems(); i++)
            {
                CSWSItem* pItem = m_pPartyItems->ItemListGetItem(i);
                if (pItem != NULL)
                {
                    pItem->SetPossessor(pTarget->GetId(), 0, 0, OBJECT_INVALID);
                }
            }
        }
        if (bMoved != 0)
        {
            pTarget->AddToArea(pArea, vPosition.m_fX, vPosition.m_fY,
                               vPosition.m_fZ, 0, 0);
            pTarget->SetOrientation(vOrientation);
        }
        pPlayer->m_oidPCObject = pTarget->m_nId;
        pPlayer->SetGameObject(pTarget);

        int nMember = m_nNumMembers - 1;
        while (nMember >= 0)
        {
            int nMemberNPC = m_anMembers[nMember];
            RemoveMember(nMemberNPC);
            KillNPCObject(nMemberNPC, 1);
            nMember--;
        }
        m_nControlledNPC = nNewControlled;
        return 1;
    }
    return 0;
}

#ifdef _XBOX

int CSWPartyTable::RemoveMember(int nNPC)
{
    if (m_nControlledNPC == -1 && m_nNumMembers > 0 && nNPC >= 0 && nNPC < 9 &&
        m_abNPCAvailable[nNPC] != 0)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        unsigned long oidNPC = GetNPCObject(nNPC, 0, 1);
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidNPC);
        if (pCreature != NULL)
        {
            pCreature->ClearAllEffects(0);
        }

        for (int i = 0; i < m_nNumMembers; i++)
        {
            if (nNPC == m_anMembers[i])
            {
                for (int j = i + 1; j < m_nNumMembers; j++)
                {
                    m_anMembers[j - 1] = m_anMembers[j];
                }
                if (--m_nNumMembers == 0 && m_bSoloMode != 0)
                {
                    m_bSoloMode = 0;
                }
                return 1;
            }
        }
    }
    return 0;
}

int CSWPartyTable::KillNPCObject(int nNPC, int bFade)
{
    if (nNPC < 0 || nNPC >= 9)
    {
        return 0;
    }
    if (GetIsMember(nNPC) != 0)
    {
        return 0;
    }

    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            m_aoidNPC[nNPC]);
    m_aoidNPC[nNPC] = OBJECT_INVALID;
    if (pCreature != NULL)
    {
        if (bFade != 0)
        {
            CSWCCreature* pClientCreature = pCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pClientCreature->SetFadeState(CSWCObject::FADE_STATE_HIDDEN);
                pClientCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 1, 0, 0);
                pClientCreature->m_bFadeLocked = 1;
            }
        }

        delete pCreature;
        return 1;
    }
    return 0;
}

int CSWPartyTable::AddNPC(int nNPC, CSWSCreature* pCreature)
{
    if (nNPC < 0 || nNPC >= MAX_NPCS || pCreature == NULL || m_abNPCAvailable[nNPC] != 0)
    {
        return 0;
    }

    m_abNPCAvailable[nNPC] = 1;
    TransferInventory(pCreature);

    int nBaseXP = 0;
    float fPercent = 1.0f;
    int nPercentXP;
    int bFound;
    {
        CExoString sColumn("PercentXP");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        EXOXBOXASSERT(p2DArrays->m_pNPC);
        C2DA* pNPCTable = p2DArrays->m_pNPC;
        bFound = pNPCTable->GetINTEntry(nNPC, sColumn, &nPercentXP);
    }
    if (bFound != 0 && nPercentXP != 0)
    {
        fPercent = nPercentXP * 0.01f;
    }
    int nXPPool = m_nXPPool;
    int nPoolXP = (int)(nXPPool * fPercent);

    int nLevel = 1;
    if (pCreature->GetStats()->GetLevel(0) >= 1)
    {
        int nNextLevelXP = 1000;
        do
        {
            nBaseXP = nBaseXP + nNextLevelXP - 1000;
            nLevel++;
            nNextLevelXP += 1000;
        } while (nLevel <= pCreature->GetStats()->GetLevel(0));
    }
    if (nBaseXP <= nPoolXP)
    {
        pCreature->m_nUnknown22C = 0;
    }
    else
    {
        pCreature->m_nUnknown22C = nBaseXP - nPoolXP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
    CSWSCreature* pPlayer = pServerExoApp->GetCreatureByGameObjectID(
        pServerExoApp->GetPlayerCreatureId());
    if (pPlayer != NULL && pPlayer->GetFaction() != NULL)
    {
        pPlayer->GetFaction()->AddMember(pCreature->GetId(), 0);
    }
    pCreature->GetStats()->SetMovementRate(0);

    CExoString sPath;
    sPath.Format("GAMEINPROGRESS:%s", GetFilename(nNPC));
    CResGFF* pGFF = new CResGFF;
    CResStruct stTop;
    pGFF->CreateGFFFile(&stTop, "UTC ", "V2.0");
    pCreature->SaveCreature(pGFF, &stTop);
    return pGFF->WriteGFFFile(sPath.CStr(), RESTYPE_UTC);
}
#endif
