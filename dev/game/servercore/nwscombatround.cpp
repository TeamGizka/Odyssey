#include "exobase.h"
#include "resgff.h"
#include <math.h>
#include "appmanager.h"
#include "serverexoapp.h"
#include "swguiabilitieschargen.h"
#include "nwclass.h"
#include "nwitem.h"
#include "nwsitem.h"
#include "nwsdoor.h"
#include "nwsplaceable.h"
#include "nwscript.h"
#include "clientexoapp.h"
#include "swparty.h"
#include "nwscombatround.h"
#include "nwseffect.h"
#include "gameobjectarray.h"
#include "nwrules.h"
#include "nwcanimbase.h"
#include "nwccreature.h"

static inline void NwscombatroundTrace1();
#define NWSCOMBATROUND_TRACE() NwscombatroundTrace1()

int g_nCombatStretch = 0;
// PC 0x004015A0
CSWSCombatRoundAction::~CSWSCombatRoundAction()
{
}
// PC 0x004015A0
CSWSCombatSubAttack::~CSWSCombatSubAttack()
{
}
// PC 0x0052AD20
CSWSCombatAttackData::CSWSCombatAttackData()
{
    ClearAttackData();
}
// PC 0x0052AF50
void CSWSCombatAttackData::ResolveAttackToMeleeSubAttack(
    int nAttackNumber, unsigned long nAnimationTime)
{
    CSWSCombatSubAttack subAttack;

    subAttack.m_nAttackNumber = nAttackNumber;
    subAttack.m_nAnimationTime = nAnimationTime;
    subAttack.m_nAttackResult = m_nAttackResult;
    subAttack.m_nWeaponAttackType = m_nWeaponAttackType;
    subAttack.m_nAttackType = m_nAttackType;
    subAttack.m_bRangedAttack = 0;

    for (int i = 0; i < 15; i++)
    {
        subAttack.m_nDamage[i] = m_nDamage[i];
        m_nDamage[i] = -1;
    }

    subAttack.m_Extra1 = m_Extra1;
    subAttack.m_Extra2 = m_Extra2;
    subAttack.m_Flags = m_Flags;
    subAttack.m_DamageBreakdown = m_DamageBreakdown;
    subAttack.m_Extra4 = m_Extra4;

    m_lstSubAttacks.Add(subAttack);
}
// PC 0x0052B060
void CSWSCombatAttackData::ResolveAttackToRangedSubAttack(
    int nAttackNumber, unsigned long nAnimationTime, int nArg3, int nArg4,
    int nArg5)
{
    CSWSCombatSubAttack subAttack;

    subAttack.m_nAttackNumber = nAttackNumber;
    subAttack.m_nAnimationTime = nAnimationTime;
    subAttack.m_nUnknown10 = nArg3;
    subAttack.m_nUnknown14 = nArg4;
    subAttack.m_nUnknown2C = nArg5;
    subAttack.m_nAttackResult = m_nAttackResult;
    subAttack.m_nWeaponAttackType = m_nWeaponAttackType;
    subAttack.m_nAttackType = m_nAttackType;
    subAttack.m_vRangedTarget = m_vRangedTarget;
    subAttack.m_nReactionDelay = m_nReactionDelay;
    subAttack.m_bRangedAttack = 1;

    for (int i = 0; i < 15; i++)
    {
        subAttack.m_nDamage[i] = m_nDamage[i];
        m_nDamage[i] = -1;
    }

    subAttack.m_Extra1 = m_Extra1;
    subAttack.m_Extra2 = m_Extra2;
    subAttack.m_Flags = m_Flags;
    subAttack.m_DamageBreakdown = m_DamageBreakdown;
    subAttack.m_Extra4 = m_Extra4;

    m_lstSubAttacks.Add(subAttack);
}
// PC 0x0052B1B0
CSWSCombatRound::CSWSCombatRound(CSWSCreature* pCreature)
{
    m_nUnknown0 = 0;
    m_bRoundStarted = 0;
    m_bSpellCastRound = 0;
    m_nSpellCastRound = 0;
    m_bRoundPaused = 0;
    m_nUnknown940 = 0;
    m_oidRoundPausedBy = OBJECT_INVALID;
    m_nPauseTimer = 0;
    m_bInfinitePause = 0;
    m_pScheduledActions = new CExoLinkedList<CSWSCombatRoundAction>;
    m_nNumAOOs = 1;
    m_nNumCleaves = 1;
    m_pBaseCreature = pCreature;
    m_nNumActionsTaken = 0;
    m_nCurrentAttack = 0;
    m_bOffHandAttack = 0;
    m_nOnHandAttacks = 0;
    m_nOffHandAttacks = 0;
    m_nOffHandTaken = 0;
    m_nExtraTaken = 0;
    m_nAdditAttacks = 0;
    m_nEffectAttacks = 0;
    m_nParryIndex = 0;
    m_bDeflectArrow = 0;
    m_bWeaponSucks = 0;
    m_nTimer = 0;
    m_nUnknown948 = 0;
    m_nRoundLength = 0;
    m_nBleedTimer = 0;
    m_nOverlapAmount = 0;
    m_nParryActions = 0;
    m_oidNewAttackTarget = OBJECT_INVALID;
    m_nAttackId = 0;
    m_oidDodgeTarget = OBJECT_INVALID;
    m_bEngaged = 0;
    m_bMaster = 0;
    m_pCurrentAction = NULL;
    m_oidUnknown9CC = OBJECT_INVALID;
    m_nUnknown9D4 = 0;
    m_nCurrentActionType = 0;
    m_nAttackGroup = 255;
    memset(m_Attacks, 0, sizeof(m_Attacks));
}

unsigned short CSWSCombatRound::GetNewAttackID()
{
    if (m_nAttackId == ATTACK_ID_WRAP)
        m_nAttackId = 0;
    return m_nAttackId++;
}

void CSWSCombatRound::InsertSpecialAttack(unsigned short nAttack, int nIndex)
{
    m_lstSpecialAttacks.Add(nAttack);
    m_lstSpecialAttackIds.Add(GetNewAttackID());
}
// PC 0x0052B370
CSWSCombatRound::~CSWSCombatRound()
{
    if (m_pScheduledActions != NULL)
    {
        while (!m_pScheduledActions->IsEmpty())
        {
            CSWSCombatRoundAction* pAction = m_pScheduledActions->RemoveHead();
            if (pAction != NULL)
            {
                delete pAction;
            }
        }
        delete m_pScheduledActions;
        m_pScheduledActions = NULL;
    }

    if (m_pCurrentAction != NULL)
    {
        delete m_pCurrentAction;
        m_pCurrentAction = NULL;
    }
}
// PC 0x005299B0
CSWSCombatAttackData::~CSWSCombatAttackData()
{
}
// PC 0x0052B480
void CSWSCombatRound::StartCombatRound(unsigned long oidTarget, int bEngaged,
                                       int bMaster, unsigned short nFeat,
                                       int nArg5)
{
    m_bEngaged = bEngaged;
    m_bMaster = bMaster;

    if (bEngaged != 0 && bMaster == 0)
    {
        EndCombatRound(m_bRoundStarted);
    }

    if (m_bMaster != 0)
    {
        m_oidMaster = m_pBaseCreature->m_nId;
        if (m_bEngaged != 0)
        {
            m_oidMasterTarget = oidTarget;
            CSWSCreature* pTarget =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                    oidTarget);
            if (pTarget != NULL)
            {
                pTarget->m_pcCombatRound->m_oidMaster = m_oidMaster;
                pTarget->m_pcCombatRound->m_oidMasterTarget = m_oidMasterTarget;
                pTarget->m_pcCombatRound->m_nTimer = 0;
            }
        }
    }
    else
    {
        m_oidMaster = oidTarget;
    }

    m_bRoundStarted = 1;
    m_nTimer = 0;
    m_bRoundPaused = 0;
    m_nPauseTimer = 0;
    m_oidRoundPausedBy = OBJECT_INVALID;
    m_bSpellCastRound = 0;
    m_nSpellCastRound = 0;
    m_nUnknown940 = nArg5;
    m_nRoundLength = 3000;

    ClearAllAttacks();

    InitializeNumberOfAttacks(nFeat);

    if (m_pBaseCreature->m_nId != oidTarget)
    {
        m_oidUnknown9CC = oidTarget;
    }
    else
    {
        m_oidUnknown9CC = OBJECT_INVALID;
    }
    m_oidDodgeTarget = oidTarget;
    m_bDeflectArrow = 1;
    m_bWeaponSucks = 0;
    m_pBaseCreature->SetLockOrientationToObject(oidTarget, 0);
    m_pBaseCreature->m_nCombatMode = 3;
}
// PC 0x00529A90
CSWSCombatSubAttack CSWSCombatAttackData::GetTopSubAttack()
{
    CSWSCombatSubAttack subAttack;

    if (m_lstSubAttacks.num > 0)
    {
        subAttack = m_lstSubAttacks[0];
    }

    return subAttack;
}
// PC 0x00529AE0
void CSWSCombatAttackData::ResolveTopSubAttackToAttack()
{
    ResolveSubAttackToAttack(GetTopSubAttack());
}
// PC 0x00529B10
int CSWSCombatAttackData::IsSubAttackReady(int nTime)
{
    int bReady = 0;
    CSWSCombatSubAttack subAttack = GetTopSubAttack();

    if ((unsigned int)(m_nAnimationLength - nTime) > subAttack.m_nAnimationTime)
    {
        bReady = 1;
    }
    return bReady;
}
// PC 0x00529B50
void CSWSCombatRound::EndCombatRound(int bNotify)
{
    CSWSCreature* pCreature = m_pBaseCreature;

    m_bRoundStarted = 0;
    m_nTimer = 0;
    m_nRoundLength = 0;
    m_bRoundPaused = 0;
    m_oidRoundPausedBy = OBJECT_INVALID;
    m_nPauseTimer = 0;
    m_oidDodgeTarget = OBJECT_INVALID;
    m_nAdditAttacks = 0;
    m_nNumActionsTaken = 0;

    if (pCreature != NULL && pCreature->m_bBodyFuel != 0)
    {
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        int nForcePoints = pCreature->GetMaxForcePoints();
        pStats->m_nCurrentForcePoints = nForcePoints;
    }

    int bRunEndRoundScript = 1;
    if (m_bMaster != 0)
    {
        CSWSCreature* pSlave = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(m_oidMasterTarget);
        if (pSlave != NULL)
        {
            pSlave->m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
            pSlave->m_pcCombatRound->SetPauseTimer(0, 0);
            pSlave->m_pcCombatRound->m_bEngaged = 0;
            pSlave->m_pcCombatRound->m_oidMaster = OBJECT_INVALID;
        }
        else
        {
            CExoString sError;
            sError.Format("CSWSCombatRound::EndCombatRound - %x Combat Slave (%x) not found!",
                          m_pBaseCreature->GetId(), m_oidMasterTarget);
        }
    }

    int bTargetDead = 0;
    unsigned long oidNextTarget = OBJECT_INVALID;
    if (m_oidUnknown9CC != OBJECT_INVALID)
    {
        CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(m_oidUnknown9CC);
        if (pTarget != NULL)
        {
            if (pTarget->AsSWSCreature() != NULL)
            {
                CSWSCreature* pTargetCreature = pTarget->AsSWSCreature();
                if (pTargetCreature->GetDead() || pTargetCreature->GetDeadTemp())
                {
                    m_oidUnknown9CC = OBJECT_INVALID;
                    bTargetDead = 1;
                    if (m_bSpellCastRound)
                        oidNextTarget = m_pBaseCreature->m_oidAttemptedSpellTarget;
                    else
                        oidNextTarget = m_pBaseCreature->m_oidAttemptedAttackTarget;
                }
            }
            else if ((pTarget->AsSWSDoor() != NULL && pTarget->AsSWSDoor()->m_nOpenState != CSWSDoor::OPEN_STATE_CLOSED) ||
                     (pTarget->AsSWSPlaceable() != NULL && pTarget->AsSWSPlaceable()->m_bOpen != 0))
            {
                m_oidUnknown9CC = OBJECT_INVALID;
            }
        }
        else
        {
            m_oidUnknown9CC = OBJECT_INVALID;
        }
    }

    m_pBaseCreature->SetAnimation(CSWSObject::ANIMATION_COMBAT_READY);
    ClearAllAttacks();

    unsigned long oidTarget = m_pBaseCreature->m_oidAttemptedAttackTarget;
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        m_pBaseCreature->m_oidAttemptedAttackTarget = OBJECT_INVALID;
    else if (pTarget->AsSWSObject() != NULL && pTarget->AsSWSObject()->GetDead())
        m_pBaseCreature->m_oidAttemptedAttackTarget = OBJECT_INVALID;

    oidTarget = m_pBaseCreature->m_oidAttemptedSpellTarget;
    pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        m_pBaseCreature->m_oidAttemptedSpellTarget = OBJECT_INVALID;
    else if (pTarget->AsSWSObject() != NULL && pTarget->AsSWSObject()->GetDead())
        m_pBaseCreature->m_oidAttemptedSpellTarget = OBJECT_INVALID;

    oidTarget = m_pBaseCreature->m_oidAttackTarget;
    pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        m_pBaseCreature->m_oidAttackTarget = OBJECT_INVALID;
    else if (pTarget->AsSWSObject() != NULL && pTarget->AsSWSObject()->GetDead())
        m_pBaseCreature->m_oidAttackTarget = OBJECT_INVALID;

    oidTarget = m_pBaseCreature->m_oidSpellTarget;
    pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        m_pBaseCreature->m_oidSpellTarget = OBJECT_INVALID;
    else if (pTarget->AsSWSObject() != NULL && pTarget->AsSWSObject()->GetDead())
        m_pBaseCreature->m_oidSpellTarget = OBJECT_INVALID;

    oidTarget = m_pBaseCreature->m_oidGoingToBeAttackedBy;
    pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        m_pBaseCreature->m_oidGoingToBeAttackedBy = OBJECT_INVALID;
    else if (pTarget->AsSWSObject() != NULL && pTarget->AsSWSObject()->GetDead())
        m_pBaseCreature->m_oidGoingToBeAttackedBy = OBJECT_INVALID;

    m_pBaseCreature->ClearAttackerList();

    int bPlayerCharacter = 0;
    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    if (pPlayer != NULL)
    {
        unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pPlayer->GetId());
        if (m_pBaseCreature->GetId() == oidPlayer)
            bPlayerCharacter = 1;
    }
    if (!bPlayerCharacter)
    {
        m_pBaseCreature->SetCombatMode(0, 1);
    }

    CExoLinkedListNode* pAction = m_pBaseCreature->m_lstActions.GetHeadPos();
    if (pAction != NULL && bNotify &&
        m_pBaseCreature->m_lstActions.GetAtPos(pAction)->m_nActionType == CSWSObject::ACTION_MOVE_TO_POINT)
    {
        bRunEndRoundScript = 0;
    }

    if (bNotify && bRunEndRoundScript && m_nSpellCastRound == 0 &&
        m_nUnknown940 == 0 && !bPlayerCharacter &&
        !m_pBaseCreature->GetDead() && !m_pBaseCreature->GetDeadTemp())
    {
        g_pVirtualMachine->RunScript(&m_pBaseCreature->m_sScripts[CSWSCreature::SCRIPT_END_ROUND],
                                     m_pBaseCreature->GetId(), 1);
    }

    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    if (pClient != NULL && !pClient->GetPausedByCombat() && bNotify && bPlayerCharacter &&
        pClient->GetClientOptions()->m_bAutopauseEndOfCombatRound && pPlayer->m_bCombatMode)
    {
        pClient->SetAutoPaused(1, CClientExoApp::AUTOPAUSE_END_OF_COMBAT_ROUND);
    }

    if (m_nUnknown940 == 0 && bPlayerCharacter && bNotify)
    {
        m_pBaseCreature->CreateNewAttackActions(bTargetDead, m_bSpellCastRound, oidNextTarget);
    }

    m_pBaseCreature->m_oidLastHostileTarget = m_pBaseCreature->m_oidAttackTarget;
    m_pBaseCreature->m_nLastAttackAction = m_pBaseCreature->m_nCombatMode;
    if (m_bSpellCastRound)
    {
        m_pBaseCreature->m_nLastForcePowerUsed = m_pBaseCreature->m_nCombatSpellId;
        m_pBaseCreature->m_bIgnoreSpellReadyCount = 0;
        m_bSpellCastRound = 0;
    }
    if (m_pBaseCreature->m_nLastSpecialAttack != 0)
    {
        m_pBaseCreature->m_nLastCombatFeatUsed = m_pBaseCreature->m_nLastSpecialAttack;
    }
    m_nNumAOOs = 1;
    m_nNumCleaves = 1;
}
// PC 0x0052A140
void CSWSCombatRound::IncrementTimer(int nAmount)
{
    m_nTimer += nAmount;

    if (m_bRoundStarted != 0 && m_nTimer >= m_nRoundLength &&
        m_bSpellCastRound != 0)
    {
        if (m_pBaseCreature->m_lstActions.GetHead() == NULL ||
            m_pBaseCreature->m_lstActions.GetHead()->m_nActionType != 15)
        {
            EndCombatRound(1);
        }
    }
    else if (m_bRoundStarted != 0 && m_nTimer >= m_nRoundLength &&
        ((m_bEngaged != 0 && m_bMaster != 0) || m_bEngaged == 0) && m_bSpellCastRound == 0)
    {
        EndCombatRound(1);
    }
    else if (m_bRoundStarted != 0 && m_nTimer >= m_nRoundLength && m_bEngaged != 0 &&
        m_bMaster == 0)
    {
        CExoString sDebug;
        unsigned long oidMaster = m_oidMaster;
        CGameObject* pGameObject =
            g_pAppManager->m_pServerExoApp->GetGameObject(oidMaster);
        if (pGameObject == NULL)
        {
            CSWSCreature* pCreature = m_pBaseCreature;
            sDebug.Format("CSWSCombatRound::IncrementTimer - %s Master cannot "
                          "be found and round has expired; Resetting",
                          pCreature->m_sTag.CStr());
            m_oidMaster = OBJECT_INVALID;
            EndCombatRound(1);
        }
        else
        {
            unsigned long oidFoundMaster = pGameObject->m_nId;
            CSWSCreature* pCreature = m_pBaseCreature;
            sDebug.Format("CSWSCombatRound::IncrementTimer - %s Master IS "
                          "found (%x) and round has expired (%d %d); Resetting",
                          pCreature->m_sTag.CStr(),
                          oidFoundMaster, m_nTimer, m_nRoundLength);
            m_oidMaster = OBJECT_INVALID;
            EndCombatRound(1);
        }
    }
    else if (m_nTimer < 0)
    {
        CExoString sDebug;
        CSWSCreature* pCreature = m_pBaseCreature;
        sDebug.Format("CSWSCombatRound::IncrementTimer - %s Timer is negative "
                      "at %d; Ending combat round and resetting",
                      pCreature->m_sTag.CStr(), m_nTimer);
        EndCombatRound(1);
    }
    else if (m_bEngaged != 0 && m_bMaster == 0 &&
             m_oidMaster == m_pBaseCreature->m_nId)
    {
        m_bEngaged = 0;
        m_oidMaster = OBJECT_INVALID;
    }
}
// PC 0x0052A3B0
void CSWSCombatRound::DecrementPauseTimer(int nAmount)
{
    CExoString sDebug;

    if (g_pAppManager->m_pServerExoApp->GetGameObject(m_oidMaster) == NULL)
    {
        CSWSCreature* pCreature = m_pBaseCreature;
        sDebug.Format("CSWSCombatRound::DecrementPauseTimer - %s Master cannot "
                      "be found expire the round; Resetting",
                      pCreature->m_sTag.CStr());
        m_oidMaster = OBJECT_INVALID;
        EndCombatRound(1);
    }
    else
    {
        m_nPauseTimer -= nAmount;
        if (m_nPauseTimer <= 0 && m_bInfinitePause == 0)
        {
            m_nPauseTimer = 0;
            m_bRoundPaused = 0;
            m_oidRoundPausedBy = OBJECT_INVALID;
        }
    }
}
// PC 0x0052A490
void CSWSCombatRound::ClearAllSpecialAttacks()
{
    for (int i = m_nCurrentAttack;
         i < (unsigned char)(m_nOffHandAttacks + m_nOnHandAttacks); i++)
    {
        if (GetAttack(i)->m_nAttackType != 0)
        {
            GetAttack(i)->m_nAttackType = 0;
        }
    }

    m_lstSpecialAttacks.SetSize(0);
}
// PC 0x0052A520
CSWSItem* CSWSCombatRound::GetCurrentAttackWeapon(int nAttackType)
{
    int nType = nAttackType;
    if (nType == WEAPON_ATTACK_AUTOMATIC)
    {
        nType = GetWeaponAttackType();
    }

    if (nType == WEAPON_ATTACK_RIGHTHAND || nType == WEAPON_ATTACK_EXTRA_RIGHTHAND)
    {
        CSWInventory* pInventory = m_pBaseCreature->m_pInventory;
        return pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    }

    if (nType == WEAPON_ATTACK_LEFTHAND)
    {
        CSWItem* pItem =
            (CSWItem*)m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pItem != NULL &&
            g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())
                    ->m_nWeaponWield == 3)
        {
            return (CSWSItem*)pItem;
        }
        CSWInventory* pInventory = m_pBaseCreature->m_pInventory;
        return pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    }

    if (nType == WEAPON_ATTACK_CREATURE1)
    {
        CSWInventory* pInventory = m_pBaseCreature->m_pInventory;
        return pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON1);
    }

    if (nType == WEAPON_ATTACK_CREATURE2)
    {
        CSWInventory* pInventory = m_pBaseCreature->m_pInventory;
        return pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON2);
    }

    if (nType == WEAPON_ATTACK_CREATURE3)
    {
        CSWInventory* pInventory = m_pBaseCreature->m_pInventory;
        return pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON3);
    }

    if (nType == WEAPON_ATTACK_UNARMED || nType == WEAPON_ATTACK_EXTRA_UNARMED)
    {
        CSWInventory* pInventory = m_pBaseCreature->m_pInventory;
        return pInventory->GetItemInSlot(INVENTORY_SLOT_HANDS);
    }

    return NULL;
}
// PC 0x0052A650
int CSWSCombatRound::LoadCombatRound(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    CResList resList;
    CResStruct resStruct;
    int i;

    m_bRoundStarted =
        pRes->ReadFieldBYTE(pStruct, "RoundStarted", bSuccess, m_bRoundStarted);
    if (m_bRoundStarted == 0)
    {
        return 0;
    }

    m_bSpellCastRound = pRes->ReadFieldBYTE(pStruct, "SpellCastRound", bSuccess,
                                            m_bSpellCastRound);
    m_bDeflectArrow =
        pRes->ReadFieldBYTE(pStruct, "DeflectArrow", bSuccess, m_bDeflectArrow);
    m_bWeaponSucks =
        pRes->ReadFieldBYTE(pStruct, "WeaponSucks", bSuccess, m_bWeaponSucks);
    m_oidDodgeTarget = pRes->ReadFieldDWORD(pStruct, "DodgeTarget", bSuccess,
                                            m_oidDodgeTarget);
    m_oidNewAttackTarget = pRes->ReadFieldDWORD(pStruct, "NewAttackTarget",
                                                bSuccess, m_oidNewAttackTarget);
    m_bRoundPaused =
        pRes->ReadFieldBYTE(pStruct, "RoundPaused", bSuccess, m_bRoundPaused);
    m_oidRoundPausedBy = pRes->ReadFieldDWORD(pStruct, "RoundPausedBy",
                                              bSuccess, m_oidRoundPausedBy);
    m_bInfinitePause = pRes->ReadFieldBYTE(pStruct, "InfinitePause", bSuccess,
                                           m_bInfinitePause);
    m_nPauseTimer =
        pRes->ReadFieldINT(pStruct, "PauseTimer", bSuccess, m_nPauseTimer);
    m_bInfinitePause = 0;
    m_bEngaged = pRes->ReadFieldINT(pStruct, "Engaged", bSuccess, 0);
    m_bMaster = pRes->ReadFieldINT(pStruct, "Master", bSuccess, 0);
    m_oidMaster =
        pRes->ReadFieldDWORD(pStruct, "MasterID", bSuccess, OBJECT_INVALID);
    m_nTimer = pRes->ReadFieldINT(pStruct, "Timer", bSuccess, m_nTimer);
    m_nRoundLength =
        pRes->ReadFieldINT(pStruct, "RoundLength", bSuccess, m_nRoundLength);
    m_nOverlapAmount = pRes->ReadFieldINT(pStruct, "OverlapAmount", bSuccess,
                                          m_nOverlapAmount);
    m_nBleedTimer =
        pRes->ReadFieldINT(pStruct, "BleedTimer", bSuccess, m_nBleedTimer);
    m_nCurrentAttack = pRes->ReadFieldBYTE(pStruct, "CurrentAttack", bSuccess,
                                           m_nCurrentAttack);
    m_nAttackId =
        pRes->ReadFieldWORD(pStruct, "AttackID", bSuccess, m_nAttackId);
    m_nAttackGroup =
        pRes->ReadFieldBYTE(pStruct, "AttackGroup", bSuccess, m_nAttackGroup);
    m_nParryIndex =
        pRes->ReadFieldINT(pStruct, "ParryIndex", bSuccess, m_nParryIndex);
    m_nNumAOOs = pRes->ReadFieldINT(pStruct, "NumAOOs", bSuccess, m_nNumAOOs);
    m_nNumCleaves =
        pRes->ReadFieldINT(pStruct, "NumCleaves", bSuccess, m_nNumCleaves);
    m_nOnHandAttacks = pRes->ReadFieldINT(pStruct, "OnHandAttacks", bSuccess,
                                          m_nOnHandAttacks);
    m_nOffHandAttacks = pRes->ReadFieldINT(pStruct, "OffHandAttacks", bSuccess,
                                           m_nOffHandAttacks);
    m_nAdditAttacks =
        pRes->ReadFieldINT(pStruct, "AdditAttacks", bSuccess, m_nAdditAttacks);
    pRes->ReadFieldINT(pStruct, "EffectAttacks", bSuccess, m_nEffectAttacks);
    m_nEffectAttacks = 0;
    m_nParryActions =
        pRes->ReadFieldBYTE(pStruct, "ParryActions", bSuccess, m_nParryActions);
    m_nOffHandTaken =
        pRes->ReadFieldINT(pStruct, "OffHandTaken", bSuccess, m_nOffHandTaken);
    m_nExtraTaken =
        pRes->ReadFieldINT(pStruct, "ExtraTaken", bSuccess, m_nExtraTaken);

    if (pRes->GetList(&resList, pStruct, "AttackList") == 1 &&
        (int)pRes->GetListCount(&resList) > 0)
    {
        CSWSCombatAttackData* pAttack = m_Attacks;
        i = 0;
        do
        {
            if (pRes->GetListElement(&resStruct, &resList, i) == 1)
            {
                pAttack->LoadData(pRes, &resStruct);
            }
            i++;
            pAttack++;
        } while (i < (int)pRes->GetListCount(&resList));
    }

    if (pRes->GetList(&resList, pStruct, "SpecAttackList") == 1)
    {
        m_lstSpecialAttacks.SetSize(pRes->GetListCount(&resList));
        for (i = 0; i < m_lstSpecialAttacks.num; i++)
        {
            if (pRes->GetListElement(&resStruct, &resList, i) == 1)
            {
                unsigned short* pnSpecialAttack =
                    &m_lstSpecialAttacks[i];
                *pnSpecialAttack = pRes->ReadFieldWORD(
                    &resStruct, "SpecialAttack", bSuccess, 0);
            }
        }
    }

    if (pRes->GetList(&resList, pStruct, "SpecAttackIdList") == 1)
    {
        m_lstSpecialAttackIds.SetSize(pRes->GetListCount(&resList));
        for (i = 0; i < m_lstSpecialAttacks.num; i++)
        {
            if (pRes->GetListElement(&resStruct, &resList, i) == 1)
            {
                unsigned short* pnSpecialAttackId =
                    &m_lstSpecialAttackIds[i];
                *pnSpecialAttackId = pRes->ReadFieldWORD(
                    &resStruct, "SpecialAttackId", bSuccess, 0);
            }
        }
    }

    if (pRes->GetList(&resList, pStruct, "SchedActionList") == 1)
    {
        for (i = 0; i < (int)pRes->GetListCount(&resList); i++)
        {
            if (pRes->GetListElement(&resStruct, &resList, i) == 1)
            {
                CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;
                pAction->LoadData(pRes, &resStruct);
                m_pScheduledActions->AddTail(pAction);
            }
        }
    }

    return 1;
}
// PC 0x00528310
CSWSCombatRoundAction::CSWSCombatRoundAction()
{
    ClearData();
}
// PC 0x00527010
void CSWSCombatRoundAction::ClearData()
{
    m_nActionTimer = 0;
    m_nAnimationTime = 0;
    m_nAnimation = 0;
    m_nActionType = 0;
    m_nNumAttacks = 0;
    m_oidTarget = OBJECT_INVALID;
    m_bRetargettable = 1;
    m_nInventorySlot = 0;
    m_oidTargetRepository = OBJECT_INVALID;
    m_nSpellId = 0;
    m_aUnmapped28[0] = 0;
    m_aUnmapped28[1] = 0;
    m_aUnmapped28[2] = 0;
    m_aUnmapped28[3] = 0;
    m_oidSpellTarget = OBJECT_INVALID;
    m_nUnknown48 = 0;
    m_nUnknown4C = 0;
    m_nUnknown50 = 0;
    m_nEquipFlags = 0;
    m_nUnknown58 = 0;
    m_nSpecialAttack = 0;
    m_nUnknown60 = 0;
    m_oidSpellItem = OBJECT_INVALID;
    m_nUnknown68 = 0;
    m_nUnknown6C = 0;
    m_nUnknown70 = 0;
    m_nUnknown72 = 0;
    m_bCutsceneAttack = 0;
    m_nCutsceneArg1 = 0;
    m_nCutsceneArg2 = 4;
    m_nCutsceneArg3 = 0;
    m_nAttackFlags = 0;
}
// PC 0x00527090
int CSWSCombatRoundAction::SaveData(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldINT(pStruct, m_nActionTimer, "ActionTimer");
    pRes->WriteFieldWORD(pStruct, m_nAnimation, "Animation");
    pRes->WriteFieldINT(pStruct, m_nAnimationTime, "AnimationTime");
    pRes->WriteFieldINT(pStruct, m_nNumAttacks, "NumAttacks");
    pRes->WriteFieldBYTE(pStruct, m_nActionType, "ActionType");
    pRes->WriteFieldDWORD(pStruct, m_oidTarget, "Target");
    pRes->WriteFieldBYTE(pStruct, m_bRetargettable, "Retargettable");
    pRes->WriteFieldDWORD(pStruct, m_nInventorySlot, "InventorySlot");
    pRes->WriteFieldDWORD(pStruct, m_oidTargetRepository, "TargetRepository");
    return 1;
}
// PC 0x00527150
int CSWSCombatRoundAction::LoadData(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    m_nActionTimer = pRes->ReadFieldINT(pStruct, "ActionTimer", bSuccess, 0);
    m_nAnimation = pRes->ReadFieldWORD(pStruct, "Animation", bSuccess, 0);
    m_nAnimationTime =
        pRes->ReadFieldINT(pStruct, "AnimationTime", bSuccess, 0);
    m_nNumAttacks = pRes->ReadFieldINT(pStruct, "NumAttacks", bSuccess, 0);
    m_nActionType = pRes->ReadFieldBYTE(pStruct, "ActionType", bSuccess, 0);
    m_oidTarget = pRes->ReadFieldDWORD(pStruct, "Target", bSuccess, 0);
    m_bRetargettable =
        pRes->ReadFieldBYTE(pStruct, "Retargettable", bSuccess, 0);
    m_nInventorySlot =
        pRes->ReadFieldDWORD(pStruct, "InventorySlot", bSuccess, 0);
    m_oidTargetRepository =
        pRes->ReadFieldDWORD(pStruct, "TargetRepository", bSuccess, 0);
    return 1;
}
// PC 0x00527240
int CSWSCombatAttackData::GetDamage(unsigned short nDamageFlags)
{
    if (nDamageFlags & 0x10)
    {
        return m_nDamage[4];
    }
    if (nDamageFlags & 0x20)
    {
        return m_nDamage[5];
    }
    if (nDamageFlags & 0x40)
    {
        return m_nDamage[6];
    }
    if (nDamageFlags & 0x80)
    {
        return m_nDamage[7];
    }
    if (nDamageFlags & 0x100)
    {
        return m_nDamage[8];
    }
    if (nDamageFlags & 0x8)
    {
        return m_nDamage[3];
    }
    if (nDamageFlags & 0x200)
    {
        return m_nDamage[9];
    }
    if (nDamageFlags & 0x400)
    {
        return m_nDamage[10];
    }
    if (nDamageFlags & 0x800)
    {
        return m_nDamage[11];
    }
    if (nDamageFlags & 0x1000)
    {
        return m_nDamage[12];
    }
    if (nDamageFlags & 0x1)
    {
        return m_nDamage[0];
    }
    if (nDamageFlags & 0x2)
    {
        return m_nDamage[1];
    }
    if (nDamageFlags & 0x4)
    {
        return m_nDamage[2];
    }
    if (nDamageFlags & 0x2000)
    {
        return m_nDamage[13];
    }
    return 0;
}
// PC 0x005272F0
void CSWSCombatAttackData::SetBaseDamage(int nDamage)
{
    if (m_nDamage[BASE_DAMAGE_INDEX] > 0)
    {
        m_nDamage[BASE_DAMAGE_INDEX] += nDamage;
    }
    else
    {
        m_nDamage[BASE_DAMAGE_INDEX] = nDamage;
    }
}
// PC 0x00527320
int CSWSCombatAttackData::GetBaseDamage()
{
    return m_nDamage[BASE_DAMAGE_INDEX];
}
// PC 0x00527330
void CSWSCombatAttackData::SetDamage(unsigned short nDamageFlags, int nDamage)
{
    if (nDamage < 0)
    {
        nDamage = 0;
    }

    int nDamageIndex = Log_Base2(nDamageFlags);
    m_nDamage[nDamageIndex] = nDamage;
}
// PC 0x00527390
void CSWSCombatAttackData::AddDamage(unsigned short nDamageFlags, int nDamage,
                                     int bAddToBase)
{
    int nDamageIndex = Log_Base2(nDamageFlags);

    short nCurrent = m_nDamage[nDamageIndex];
    if (nCurrent > 0)
    {
        int nTotal = nDamage + nCurrent;
        if (nTotal < 1)
        {
            nTotal = 1;
        }
        m_nDamage[nDamageIndex] = nTotal;
    }
    else
    {
        SetDamage(nDamageFlags, nDamage);
    }

    if (bAddToBase != 0)
    {
        SetBaseDamage(nDamage);
    }
}
// PC 0x00527440
int CSWSCombatAttackData::GetTotalDamage(int bIncludeBase)
{
    int nTotal = 0;

    for (int i = 0; i < 14; i++)
    {
        if (m_nDamage[i] > -1)
        {
            nTotal += m_nDamage[i];
        }
    }

    return nTotal;
}
// PC 0x00528330
void CSWSCombatAttackData::ClearAttackData()
{
    m_nAttackGroup = ATTACK_GROUP_INVALID;
    m_nUnknown4 = 0;
    m_nAnimationLength = 0;
    m_nMissedBy = 0;
    for (int i = 0; i < DAMAGE_COUNT; i++)
    {
        m_nDamage[i] = -1;
    }
    m_nAttackResult = 0;
    m_nUnknown60 = 0;
    m_oidReactObject = OBJECT_INVALID;
    m_nReactionDelay = 0;
    m_nReactionAnimation = CREATURE_ANIMATION_COMBAT_READY;
    m_nReactionAnimationLength = 0;
    m_nConcealment = 0;
    m_nAttackType = 0;
    m_nAttackMode = 0;
    m_nUnknown66 = UNKNOWN66_UNSET;
    m_vRangedTarget = Vector(0.0f, 0.0f, 0.0f);
    m_nWeaponAttackType = 0;
    m_bSneakAttack = 0;
    m_bRangedAttack = 0;
    m_bKillingBlow = 0;
    m_bCoupDeGrace = 0;
    m_bCriticalThreat = 0;
    m_nUnknown54 = 0;
    m_bAttackDeflected = 0;
    m_oidAmmoItem = OBJECT_INVALID;
    m_sAttackDebugText = "";
    m_sDamageDebugText = "";
    for (int nEffect = 0; nEffect < m_lstEffects.num; nEffect++)
    {
        delete m_lstEffects[nEffect];
    }
    m_lstEffects.num = 0;
    m_lstUnknown94.num = 0;
    m_lstFeedbackMessages.num = 0;
    m_lstSubAttacks.num = 0;
    m_nUnknownB8 = 0;
    m_nUnknownBC = 0;
    m_Extra1.m_nUnknown0 = 0;
    m_Extra1.m_nUnknown1 = 0;
    m_Extra1.m_nUnknown2 = 0;
    m_Extra1.m_nStrengthModifier = 0;
    m_Extra1.m_nDexterityModifier = 0;
    m_Extra1.m_nUnknown5 = 0;
    m_Extra1.m_nSpecialAttackBonus = 0;
    m_Extra1.m_nBaseAttackBonus = 0;
    m_Extra1.m_nOffHand = 0;
    m_Extra1.m_nTwoWeaponPenalty = 0;
    m_Extra1.m_nWeaponSizeBonus = 0;
    m_Extra1.m_nTwoWeaponFeatBonus = 0;
    m_Extra1.m_nTwoWeaponFeat = 0;
    m_Extra1.m_nDuelingBonus = 0;
    m_Extra1.m_nDuelingFeat = 0;
    m_Extra1.m_nGoodAimBonus = 0;
    m_Extra1.m_nOffensiveTrainingBonus = 0;
    m_Extra1.m_nWeaponFocusBonus = 0;
    m_Extra1.m_nEffectAttackBonus = 0;
    m_Extra1.m_nUnknown18 = 0;
    m_Extra1.m_nUnknown1C = 0;
    m_Extra2.m_nUnknown0 = 0;
    m_Extra2.m_nUnknown4 = 0;
    m_Extra2.m_nUnknown8 = 0;
    m_Extra2.m_nUnknownC = 0;
    m_Extra2.m_nUnknown10 = 0;
    m_Extra2.m_nUnknown11 = 0;
    m_Flags.m_bUnknown0 = 0;
    m_Flags.m_nArmorModifier = 0;
    m_Flags.m_nDexterityModifier = 0;
    m_Flags.m_nBaseDexterityModifier = 0;
    m_Flags.m_nClassArmorModifier = 0;
    m_Flags.m_nNaturalArmorModifier = 0;
    m_Flags.m_nDodgeDeflectionModifier = 0;
    m_Flags.m_nFeatArmorModifier = 0;
    m_Flags.m_nDebilitatedModifier = 0;
    m_DamageBreakdown.m_nBaseDamage = 0;
    m_DamageBreakdown.m_nDamageBonus = 0;
    m_DamageBreakdown.m_nUnusedDamageFeedback = 0;
    m_DamageBreakdown.m_nSneakAttackDamage = 0;
    m_DamageBreakdown.m_nStrengthDamage = 0;
    m_DamageBreakdown.m_nWeaponSpecializationDamage = 0;
    m_DamageBreakdown.m_nUnknown6 = 0;
    m_DamageBreakdown.m_nUnknown7 = 0;
    m_DamageBreakdown.m_nUnknown8 = 0;
    m_DamageBreakdown.m_nUnknown9 = 0;
    m_DamageBreakdown.m_nUnknownA = 0;
    m_DamageBreakdown.m_nEffectDamageBonus = 0;
    for (int nDamage = 0; nDamage < 15; nDamage++)
    {
        m_DamageBreakdown.m_nUnknownDamage[nDamage] = -1;
    }
    m_Extra4.m_oidUnknown0 = OBJECT_INVALID;
    m_Extra4.m_nSavingThrowType = 0;
    m_Extra4.m_nState = 0;
    m_Extra4.m_nSaveVersusType = 0;
    m_Extra4.m_nSavingThrowRoll = 0;
    m_Extra4.m_nSavingThrowEffectBonus = 0;
    m_Extra4.m_nSavingThrowBonus = 0;
    m_Extra4.m_nSavingThrowTotal = 0;
    m_Extra4.m_nSavingThrowDC = 0;
    m_Extra4.m_nSavingThrowResult = 0xFF;
    m_Extra4.m_bStateImmune = 0;
    m_Extra4.m_nUnknown14 = 0;
    m_Extra5.m_nUnknown0 = 0;
    m_Extra5.m_nUnknown1 = 0;
    m_Extra5.m_nUnknown2 = 0;
    m_Extra5.m_nUnknown3 = 0;
    m_Extra5.m_nUnknown4 = 0;
    m_Extra5.m_nUnknown5 = 0;
    m_Extra5.m_nUnknown6 = 0;
    m_Extra5.m_nUnknown7 = 0;
    m_Extra5.m_nUnknown8 = 0;
}
// PC 0x00528670
void CSWSCombatAttackData::RemoveTopSubAttack()
{
    if (m_lstSubAttacks.num > 0)
    {
        m_lstSubAttacks.DelIndex(0);
    }
}
// PC 0x00528690
CSWSCombatSubAttack::CSWSCombatSubAttack()
{
    NwscombatroundTrace1();
}
// PC 0x005287C0
void CSWSCombatRound::DecrementRoundLength(int nAmount, int bClamp)
{
    if (m_bRoundStarted != 0)
    {
        if (m_nRoundLength - nAmount >= 0 ||
            1000 - m_nOverlapAmount < nAmount - m_nRoundLength)
        {
            if (m_nRoundLength - nAmount < 0 && bClamp != 0)
            {
                m_nRoundLength = nAmount;
                m_nBleedTimer = m_nBleedTimer - m_nRoundLength + nAmount;
            }
        }
        else
        {
            m_nRoundLength = nAmount + 1;
            m_nOverlapAmount = m_nOverlapAmount - m_nRoundLength + nAmount + 1;
        }

        CExoLinkedListNode* pPos = m_pScheduledActions->GetHeadPos();
        while (pPos != NULL)
        {
            CSWSCombatRoundAction* pAction =
                m_pScheduledActions->GetAtPos(pPos);
            if (m_nRoundLength > 0)
            {
                pAction->m_nActionTimer =
                    (int)((float)pAction->m_nActionTimer /
                          (float)m_nRoundLength *
                          (float)(m_nRoundLength - nAmount));
            }
            else
            {
                pAction->m_nActionTimer = 0;
            }
            m_pScheduledActions->GetNext(pPos);
        }

        int nLength = m_nRoundLength;
        float fRatio;
        if (nLength == 0)
        {
            fRatio = 1.0f;
        }
        else
        {
            fRatio = (float)m_nTimer / (float)m_nRoundLength;
        }
        m_nTimer = (int)((float)(nLength - nAmount) * fRatio);

        if (nLength - nAmount < 0)
        {
            m_nRoundLength = nAmount;
            m_nOverlapAmount = m_nOverlapAmount - m_nRoundLength + nAmount;
        }
        m_nRoundLength -= nAmount;
    }
}
// PC 0x00528900
int CSWSCombatRound::GetActionPending()
{
    return m_pScheduledActions != NULL &&
           m_pScheduledActions->GetHeadPos() != NULL &&
           m_nTimer >=
               m_pScheduledActions->GetAtPos(m_pScheduledActions->GetHeadPos())
                   ->m_nActionTimer;
}
// PC 0x00528940
int CSWSCombatRound::GetSpellActionPending()
{
    if (m_pScheduledActions == NULL)
    {
        return 0;
    }

    CExoLinkedListNode* pPos = m_pScheduledActions->GetHeadPos();
    if (pPos != NULL)
    {
        CSWSCombatRoundAction* pAction = m_pScheduledActions->GetAtPos(pPos);
        while (pPos != NULL)
        {
            if (pAction != NULL && m_nTimer >= pAction->m_nActionTimer &&
                pAction->m_nActionType == 2)
            {
                return 1;
            }
            pAction = m_pScheduledActions->GetNext(pPos);
        }
    }
    return 0;
}
// PC 0x005289B0
CSWSCombatRoundAction* CSWSCombatRound::GetAction()
{
    if (m_pScheduledActions == NULL)
    {
        return NULL;
    }

    CSWSCombatRoundAction* pAction = m_pScheduledActions->RemoveHead();
    if (pAction == NULL || m_nTimer < pAction->m_nActionTimer)
    {
        return NULL;
    }
    return pAction;
}
// PC 0x005289E0
void CSWSCombatRound::AddAction(CSWSCombatRoundAction* pAction, int nPosition)
{
    CExoLinkedListNode* pPos;

    if (m_pScheduledActions != NULL &&
        (unsigned long)m_pScheduledActions->Count() < 4)
    {
        if (nPosition == -1)
        {
            pPos = m_pScheduledActions->GetHeadPos();
            while (pPos != NULL)
            {
                if (pAction->m_nActionTimer <
                    m_pScheduledActions->GetAtPos(pPos)->m_nActionTimer)
                {
                    m_pScheduledActions->AddBefore(pAction, pPos);
                    return;
                }
                m_pScheduledActions->GetNext(pPos);
            }
            m_pScheduledActions->AddTail(pAction);
            return;
        }

        pPos = m_pScheduledActions->GetHeadPos();
        int i = 0;
        while (pPos != NULL && i < nPosition)
        {
            m_pScheduledActions->GetNext(pPos);
            i++;
        }

        if (pPos != NULL)
        {
            m_pScheduledActions->AddBefore(pAction, pPos);
        }
        else
        {
            m_pScheduledActions->AddTail(pAction);
        }
    }
    else if (pAction != NULL)
    {
        delete pAction;
    }
}
// PC 0x00528AF0
void CSWSCombatRound::RemoveAllActions()
{
    if (m_pScheduledActions != NULL &&
        !m_pScheduledActions->IsEmpty())
    {
        do
        {
            CSWSCombatRoundAction* pAction = m_pScheduledActions->RemoveHead();
            if (pAction != NULL)
            {
                delete pAction;
            }
        } while (!m_pScheduledActions->IsEmpty());
    }
}
// PC 0x00528B30
int CSWSCombatRound::RemoveLastAction()
{
    int bRemoved = 0;

    if (m_pScheduledActions != NULL &&
        m_pScheduledActions->GetHeadPos() != NULL)
    {
        CSWSCombatRoundAction* pAction = m_pScheduledActions->RemoveTail();
        if (pAction != NULL)
        {
            delete pAction;
        }
        bRemoved = 1;
    }

    return bRemoved;
}
// PC 0x00528C00
void CSWSCombatRound::ClearAllAttacks()
{
    for (int i = 0; i < 5; i++)
    {
        GetAttack(i)->ClearAttackData();
    }
}
// PC 0x00528C30
void CSWSCombatRound::AddCutsceneAttackAction(unsigned long oidTarget,
                                              int nArg1, int nArg2, int nArg3)
{
    CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;

    pAction->m_nActionType = 1;
    pAction->m_oidTarget = oidTarget;
    pAction->m_nAnimation = 10009;
    pAction->m_nAnimationTime = (g_nCombatStretch + 3000) / 2;
    pAction->m_nCutsceneArg1 = nArg1;
    pAction->m_bCutsceneAttack = 1;
    pAction->m_nCutsceneArg2 = nArg2;
    pAction->m_nCutsceneArg3 = nArg3;
    AddAction(pAction, -1);

    if (m_pBaseCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID)
    {
        m_pBaseCreature->m_oidAttemptedAttackTarget = oidTarget;
    }
}
// PC 0x00528CD0
void CSWSCombatRound::AddAttackAction(unsigned long oidTarget,
                                      unsigned short nSpecialAttack,
                                      int nAttackFlags, int nArg4,
                                      int nNumAttacks)
{
    CGameObject* pGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        if (pGameObject->AsSWSObject()->GetDead() != 0)
            return;

        int bValidTarget = 1;
        if (pGameObject->AsSWSDoor() != NULL &&
            pGameObject->AsSWSDoor()->m_nOpenState != CSWSDoor::OPEN_STATE_CLOSED)
        {
            bValidTarget = 0;
        }
        if (bValidTarget)
        {
            CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;

            if (nSpecialAttack != 0)
            {
                pAction->m_nActionType = CSWSCombatRoundAction::ACTION_SPECIAL_ATTACK;
                pAction->m_nSpecialAttack = nSpecialAttack;
            }
            else
            {
                pAction->m_nActionType = CSWSCombatRoundAction::ACTION_ATTACK;
            }

            pAction->m_nAttackFlags = nAttackFlags;
            pAction->m_nAnimation = ATTACK_ANIMATION_DEFAULT;
            pAction->m_nAnimationTime = (g_nCombatStretch + 3000) / 2;
            pAction->m_nNumAttacks = nNumAttacks;
            pAction->m_oidTarget = oidTarget;
            AddAction(pAction, ACTION_POSITION_BY_TIME);

            if (m_pBaseCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID)
            {
                m_pBaseCreature->m_oidAttemptedAttackTarget = oidTarget;
            }
        }
    }
}
// PC 0x00528E90
void CSWSCombatRound::AddSWSpellAction(unsigned long oidCreator, int nArg2,
                                       int nArg3, int nArg4, int nArg5,
                                       Vector vTarget, unsigned long oidItem,
                                       int nArg8, int nArg9,
                                       unsigned char nArg10, int nArg11,
                                       int nArg12, unsigned char nArg13,
                                       int nAttackFlags)
{
    CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;

    pAction->m_nActionType = 9;
    pAction->m_nAnimation = 10017;
    pAction->m_nAnimationTime = 500;
    pAction->m_nAttackFlags = nAttackFlags;
    pAction->m_nSpellId = oidCreator;
    pAction->m_aUnmapped28[0] = nArg2;
    pAction->m_aUnmapped28[1] = nArg3;
    pAction->m_aUnmapped28[2] = nArg4;
    pAction->m_aUnmapped28[3] = nArg5;
    pAction->m_vTarget = vTarget;
    pAction->m_oidSpellTarget = oidItem;
    pAction->m_nUnknown48 = nArg8;
    pAction->m_nUnknown4C = nArg9;
    pAction->m_nUnknown50 = nArg10;
    pAction->m_nEquipFlags = nArg11;
    pAction->m_nSpecialAttack = nArg12;
    pAction->m_nUnknown60 = nArg13;

    if (m_pBaseCreature->m_oidAttemptedSpellTarget == OBJECT_INVALID)
    {
        m_pBaseCreature->m_oidAttemptedSpellTarget = oidItem;
    }

    AddAction(pAction, -1);
}
// PC 0x00528FC0
void CSWSCombatRound::AddSWItemSpellAction(unsigned long oidCreator, int nArg2,
                                           int nArg3, Vector vTarget,
                                           unsigned long oidItem, int nArg6,
                                           int nAttackFlags)
{
    CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;

    pAction->m_nAttackFlags = nAttackFlags;
    pAction->m_oidSpellItem = oidCreator;
    pAction->m_nUnknown68 = nArg2;
    pAction->m_nUnknown6C = nArg3;
    pAction->m_vTarget = vTarget;
    pAction->m_nActionType = 10;
    pAction->m_oidSpellTarget = oidItem;
    pAction->m_nUnknown48 = nArg6;

    if (m_pBaseCreature->m_oidAttemptedSpellTarget == OBJECT_INVALID)
    {
        m_pBaseCreature->m_oidAttemptedSpellTarget = oidItem;
    }

    AddAction(pAction, -1);
}
// PC 0x00529060
void CSWSCombatRound::AddCutsceneMoveActions(unsigned long oidTarget,
                                             Vector vTarget, int nArg3)
{
    CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;

    pAction->m_nEquipFlags = nArg3;
    pAction->m_oidTarget = oidTarget;
    pAction->m_vTarget = vTarget;
    pAction->m_nActionType = 12;

    AddAction(pAction, -1);
}
// PC 0x005290D0
void CSWSCombatRound::RemoveSpellAction()
{
    if (m_pScheduledActions != NULL)
    {
        CExoLinkedListNode* pPos = m_pScheduledActions->GetHeadPos();
        while (pPos != NULL)
        {
            CSWSCombatRoundAction* pAction =
                m_pScheduledActions->GetAtPos(pPos);
            if (pAction != NULL && pAction->m_nActionType == 2)
            {
                CExoLinkedListNode* pRemove = pPos;
                m_pScheduledActions->GetNext(pPos);
                m_pScheduledActions->Remove(pRemove);
                delete pAction;
            }
            else
            {
                m_pScheduledActions->GetNext(pPos);
            }
        }
    }
}
// PC 0x00529160
void CSWSCombatRound::AddEquipAction(unsigned long oidItem, unsigned long nSlot,
                                     int nArg3, int nArg4)
{
    if (g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem) != NULL)
    {
        DecrementRoundLength(1500, 1);

        CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;
        pAction->m_nAttackFlags = nArg4;
        pAction->m_nActionType = 6;
        pAction->m_nActionTimer = 0;
        pAction->m_nAnimationTime = 1500;
        pAction->m_oidTarget = oidItem;
        pAction->m_nInventorySlot = nSlot;
        pAction->m_nEquipFlags = nArg3;

        if (m_pScheduledActions->GetHeadPos() != NULL)
        {
            m_pScheduledActions->AddBefore(pAction,
                                           m_pScheduledActions->GetHeadPos());
        }
        else
        {
            m_pScheduledActions->AddTail(pAction);
        }
    }
}
// PC 0x00529210
void CSWSCombatRound::AddUnequipAction(unsigned long oidItem,
                                       unsigned long nSlot, int nArg3)
{
    CSWSItem* pItem =
        g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem == NULL || m_pBaseCreature->CanUnEquipWeapon(pItem) != 0)
    {
        DecrementRoundLength(1500, 1);

        CSWSCombatRoundAction* pAction = new CSWSCombatRoundAction;
        pAction->m_nActionType = 7;
        pAction->m_nActionTimer = 0;
        pAction->m_nAnimationTime = 1500;
        pAction->m_oidTarget = oidItem;
        pAction->m_oidTargetRepository = nSlot;
        pAction->m_nEquipFlags = nArg3;

        if (m_pScheduledActions->GetHeadPos() != NULL)
        {
            m_pScheduledActions->AddBefore(pAction,
                                           m_pScheduledActions->GetHeadPos());
        }
        else
        {
            m_pScheduledActions->AddTail(pAction);
        }
    }
}
// PC 0x005292D0
int CSWSCombatRound::GetWeaponAttackType()
{
    int bCreatureWeapons = HasCreatureWeapons();
    int bOffHand = m_bOffHandAttack;
    int bExtraAttack;

    if (m_nAdditAttacks != 0 || m_nEffectAttacks != 0)
    {
        bExtraAttack = (m_nCurrentAttack + 1) > m_nOnHandAttacks;
    }
    else
    {
        bExtraAttack = 0;
    }

    if (bCreatureWeapons == 0)
    {
        if (bOffHand == 0 && bExtraAttack == 0)
        {
            return m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL
                       ? 1
                       : 7;
        }
        else if (bOffHand != 0)
        {
            return 2;
        }
        else
        {
            return m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL
                       ? 6
                       : 8;
        }
    }

    return m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON1) != NULL ? 3 : 0;
}
// PC 0x00529380
void CSWSCombatRound::UpdateAttackTargetForAllActions(unsigned long oidTarget)
{
    if (m_pScheduledActions != NULL)
    {
        CExoLinkedListNode* pPos = m_pScheduledActions->GetHeadPos();
        while (pPos != NULL)
        {
            CSWSCombatRoundAction* pAction =
                m_pScheduledActions->GetAtPos(pPos);
            if (pAction->m_nActionType == 1 && pAction->m_bRetargettable == 1)
            {
                pAction->m_oidTarget = oidTarget;
            }
            m_pScheduledActions->GetNext(pPos);
        }
    }
}
// PC 0x005293F0
int CSWSCombatRound::SaveCombatRound(CResGFF* pRes, CResStruct* pStruct)
{
    CResList resList;
    CResStruct resStruct;

    pRes->WriteFieldBYTE(pStruct, m_bRoundStarted, "RoundStarted");
    pRes->WriteFieldBYTE(pStruct, m_bSpellCastRound, "SpellCastRound");
    pRes->WriteFieldBYTE(pStruct, m_bDeflectArrow, "DeflectArrow");
    pRes->WriteFieldBYTE(pStruct, m_bWeaponSucks, "WeaponSucks");
    pRes->WriteFieldDWORD(pStruct, m_oidDodgeTarget, "DodgeTarget");
    pRes->WriteFieldDWORD(pStruct, m_oidNewAttackTarget, "NewAttackTarget");
    pRes->WriteFieldINT(pStruct, m_bEngaged, "Engaged");
    pRes->WriteFieldINT(pStruct, m_bMaster, "Master");
    pRes->WriteFieldDWORD(pStruct, m_oidMaster, "MasterID");
    pRes->WriteFieldBYTE(pStruct, m_bRoundPaused, "RoundPaused");
    pRes->WriteFieldDWORD(pStruct, m_oidRoundPausedBy, "RoundPausedBy");
    pRes->WriteFieldBYTE(pStruct, m_bInfinitePause, "InfinitePause");
    pRes->WriteFieldINT(pStruct, m_nPauseTimer, "PauseTimer");
    pRes->WriteFieldINT(pStruct, m_nTimer, "Timer");
    pRes->WriteFieldINT(pStruct, m_nRoundLength, "RoundLength");
    pRes->WriteFieldINT(pStruct, m_nOverlapAmount, "OverlapAmount");
    pRes->WriteFieldINT(pStruct, m_nBleedTimer, "BleedTimer");
    pRes->WriteFieldBYTE(pStruct, m_nCurrentAttack, "CurrentAttack");
    pRes->WriteFieldWORD(pStruct, m_nAttackId, "AttackID");
    pRes->WriteFieldBYTE(pStruct, m_nAttackGroup, "AttackGroup");
    pRes->WriteFieldINT(pStruct, m_nParryIndex, "ParryIndex");
    pRes->WriteFieldINT(pStruct, m_nNumAOOs, "NumAOOs");
    pRes->WriteFieldINT(pStruct, m_nNumCleaves, "NumCleaves");
    pRes->WriteFieldINT(pStruct, m_nOnHandAttacks, "OnHandAttacks");
    pRes->WriteFieldINT(pStruct, m_nOffHandAttacks, "OffHandAttacks");
    pRes->WriteFieldINT(pStruct, m_nAdditAttacks, "AdditAttacks");
    pRes->WriteFieldINT(pStruct, m_nEffectAttacks, "EffectAttacks");
    pRes->WriteFieldBYTE(pStruct, m_nParryActions, "ParryActions");
    pRes->WriteFieldINT(pStruct, m_nOffHandTaken, "OffHandTaken");
    pRes->WriteFieldINT(pStruct, m_nExtraTaken, "ExtraTaken");

    pRes->AddList(&resList, pStruct, "AttackList");
    CSWSCombatAttackData* pAttack = m_Attacks;
    for (int i = 5; i != 0; i--)
    {
        pRes->AddListElement(&resStruct, &resList, 0xAAEE);
        pAttack->SaveData(pRes, &resStruct);
        pAttack++;
    }

    pRes->AddList(&resList, pStruct, "SpecAttackList");
    for (int i = 0; i < m_lstSpecialAttacks.num; i++)
    {
        pRes->AddListElement(&resStruct, &resList, 0xDAEE);
        pRes->WriteFieldWORD(&resStruct, m_lstSpecialAttacks[i],
                             "SpecialAttack");
    }

    pRes->AddList(&resList, pStruct, "SpecAttackIdList");
    for (int j = 0; j < m_lstSpecialAttackIds.num; j++)
    {
        pRes->AddListElement(&resStruct, &resList, 0xDAAE);
        pRes->WriteFieldWORD(&resStruct, m_lstSpecialAttackIds[j],
                             "SpecialAttackId");
    }

    pRes->AddList(&resList, pStruct, "SchedActionList");
    CExoLinkedListNode* pPos = m_pScheduledActions->GetHeadPos();
    for (CExoLinkedListNode* pNode = pPos; pPos != NULL; pNode = pPos)
    {
        CSWSCombatRoundAction* pAction = m_pScheduledActions->GetAtPos(pNode);
        if (pAction != NULL)
        {
            pRes->AddListElement(&resStruct, &resList, 0xDAEA);
            pAction->SaveData(pRes, &resStruct);
        }
        m_pScheduledActions->GetNext(pPos);
    }

    return 1;
}
// PC 0x005274D0
int CSWSCombatAttackData::SaveData(CResGFF* pRes, CResStruct* pStruct)
{
    CResList resList;
    CResStruct resStruct;

    pRes->WriteFieldBYTE(pStruct, m_nAttackGroup, "AttackGroup");
    pRes->WriteFieldWORD(pStruct, m_nAnimationLength, "AnimationLength");
    pRes->WriteFieldDWORD(pStruct, m_nMissedBy, "MissedBy");
    pRes->WriteFieldBYTE(pStruct, m_nAttackResult, "AttackResult");
    pRes->WriteFieldDWORD(pStruct, m_oidReactObject, "ReactObject");
    pRes->WriteFieldWORD(pStruct, m_nReactionDelay, "ReaxnDelay");
    pRes->WriteFieldWORD(pStruct, m_nReactionAnimation, "ReaxnAnimation");
    pRes->WriteFieldWORD(pStruct, m_nReactionAnimationLength,
                         "ReaxnAnimLength");
    pRes->WriteFieldBYTE(pStruct, m_nConcealment, "Concealment");
    pRes->WriteFieldWORD(pStruct, m_nAttackType, "AttackType");
    pRes->WriteFieldBYTE(pStruct, m_nAttackMode, "AttackMode");
    pRes->WriteFieldINT(pStruct, m_bRangedAttack, "RangedAttack");
    pRes->WriteFieldINT(pStruct, m_bSneakAttack, "SneakAttack");
    pRes->WriteFieldBYTE(pStruct, m_nWeaponAttackType, "WeaponAttackType");
    pRes->WriteFieldFLOAT(pStruct, m_vRangedTarget.m_fX, "RangedTargetX");
    pRes->WriteFieldFLOAT(pStruct, m_vRangedTarget.m_fY, "RangedTargetY");
    pRes->WriteFieldFLOAT(pStruct, m_vRangedTarget.m_fZ, "RangedTargetZ");

    pRes->AddList(&resList, pStruct, "DamageList");
    for (int i = 0; i < 15; i++)
    {
        pRes->AddListElement(&resStruct, &resList, 0xDAEE);
        pRes->WriteFieldSHORT(&resStruct, m_nDamage[i], "DamageValue");
    }

    pRes->WriteFieldBYTE(pStruct, m_bKillingBlow, "KillingBlow");
    pRes->WriteFieldBYTE(pStruct, m_bCoupDeGrace, "CoupDeGrace");
    pRes->WriteFieldBYTE(pStruct, m_bCriticalThreat, "CriticalThreat");
    pRes->WriteFieldBYTE(pStruct, m_bAttackDeflected, "AttackDeflected");
    pRes->WriteFieldDWORD(pStruct, m_oidAmmoItem, "AmmoItem");
    pRes->WriteFieldCExoString(pStruct, m_sAttackDebugText, "AttackDebugText");
    pRes->WriteFieldCExoString(pStruct, m_sDamageDebugText, "DamageDebugText");
    return 1;
}
// PC 0x00527710
int CSWSCombatAttackData::LoadData(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    CResStruct resStruct;
    CResList resList;

    m_nAttackGroup = pRes->ReadFieldBYTE(pStruct, "AttackGroup", bSuccess, 0);
    m_nAnimationLength =
        pRes->ReadFieldWORD(pStruct, "AnimationLength", bSuccess, 0);
    m_nMissedBy = pRes->ReadFieldDWORD(pStruct, "MissedBy", bSuccess, 0);
    m_nAttackResult = pRes->ReadFieldBYTE(pStruct, "AttackResult", bSuccess, 0);
    m_oidReactObject =
        pRes->ReadFieldDWORD(pStruct, "ReactObject", bSuccess, 0);
    m_nReactionDelay = pRes->ReadFieldWORD(pStruct, "ReaxnDelay", bSuccess, 0);
    m_nReactionAnimation =
        pRes->ReadFieldWORD(pStruct, "ReaxnAnimation", bSuccess, 0);
    m_nReactionAnimationLength =
        pRes->ReadFieldWORD(pStruct, "ReaxnAnimLength", bSuccess, 0);
    m_nConcealment = pRes->ReadFieldBYTE(pStruct, "Concealment", bSuccess, 0);
    m_nAttackType = pRes->ReadFieldWORD(pStruct, "AttackType", bSuccess, 0);
    m_nAttackMode = pRes->ReadFieldBYTE(pStruct, "AttackMode", bSuccess, 0);
    m_bRangedAttack = pRes->ReadFieldINT(pStruct, "RangedAttack", bSuccess, 0);
    m_bSneakAttack = pRes->ReadFieldINT(pStruct, "SneakAttack", bSuccess, 0);
    m_nWeaponAttackType =
        pRes->ReadFieldBYTE(pStruct, "WeaponAttackType", bSuccess, 0);
    m_vRangedTarget.m_fX =
        pRes->ReadFieldFLOAT(pStruct, "RangedTargetX", bSuccess, 0.0f);
    m_vRangedTarget.m_fY =
        pRes->ReadFieldFLOAT(pStruct, "RangedTargetY", bSuccess, 0.0f);
    m_vRangedTarget.m_fZ =
        pRes->ReadFieldFLOAT(pStruct, "RangedTargetZ", bSuccess, 0.0f);

    if (pRes->GetList(&resList, pStruct, "DamageList") == 1)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&resList); i++)
        {
            if (pRes->GetListElement(&resStruct, &resList, i) == 1)
            {
                m_nDamage[i] = pRes->ReadFieldSHORT(&resStruct, "DamageValue",
                                                    bSuccess, 0);
            }
        }
    }

    m_bKillingBlow = pRes->ReadFieldBYTE(pStruct, "KillingBlow", bSuccess, 0);
    m_bCoupDeGrace = pRes->ReadFieldBYTE(pStruct, "CoupDeGrace", bSuccess, 0);
    m_bCriticalThreat =
        pRes->ReadFieldBYTE(pStruct, "CriticalThreat", bSuccess, 0);
    m_bAttackDeflected =
        pRes->ReadFieldBYTE(pStruct, "AttackDeflected", bSuccess, 0);
    m_oidAmmoItem = pRes->ReadFieldDWORD(pStruct, "AmmoItem", bSuccess, 0);

    {
        CExoString sAttackDefault("");
        m_sAttackDebugText = pRes->ReadFieldCExoString(
            pStruct, "AttackDebugText", bSuccess, sAttackDefault);
    }

    {
        CExoString sDamageDefault("");
        m_sDamageDebugText = pRes->ReadFieldCExoString(
            pStruct, "DamageDebugText", bSuccess, sDamageDefault);
    }
    return 1;
}
// PC 0x00527AA0
void CSWSCombatAttackData::ResolveSubAttackToAttack(
    const CSWSCombatSubAttack& subAttack)
{
    m_nAttackResult = subAttack.m_nAttackResult;
    m_nAttackType = subAttack.m_nAttackType;
    m_nWeaponAttackType = subAttack.m_nWeaponAttackType;
    m_vRangedTarget = subAttack.m_vRangedTarget;
    m_nReactionDelay = subAttack.m_nReactionDelay;
    m_nUnknown4 = subAttack.m_nUnknown10;

    for (int i = 0; i < 15; i++)
    {
        m_nDamage[i] = subAttack.m_nDamage[i];
    }

    m_Extra1 = subAttack.m_Extra1;
    m_Extra2 = subAttack.m_Extra2;
    m_Flags = subAttack.m_Flags;
    m_DamageBreakdown = subAttack.m_DamageBreakdown;
    m_Extra4 = subAttack.m_Extra4;
}
// PC 0x00527B70
void CSWSCombatRound::SetSpellCastRound(int nSpellId, int nRound)
{
    m_nSpellCastRound = nRound;
    m_bSpellCastRound = 1;
    m_pBaseCreature->m_nCombatMode = 4;
    m_pBaseCreature->m_nCombatSpellId = nSpellId;
}
// PC 0x00527BB0
void CSWSCombatRound::SetRoundPaused(int bPaused, unsigned long oidPauser)
{
    m_bRoundPaused = bPaused;
    m_oidRoundPausedBy = oidPauser;

    if (m_bInfinitePause != 0 && bPaused == 0)
    {
        m_bInfinitePause = 0;
    }
}
// PC 0x00527BE0
void CSWSCombatRound::SetPauseTimer(int nTime, int bInfinite)
{
    m_bInfinitePause = bInfinite;

    if (bInfinite != 0)
    {
        m_nPauseTimer = nTime;
    }
    else
    {
        if (nTime == 0)
        {
            m_nPauseTimer = 0;
        }

        if (m_nPauseTimer > 0)
        {
            m_nPauseTimer += nTime;
        }
        else
        {
            m_nPauseTimer = nTime;
        }
    }
}
// PC 0x00527C30
int CSWSCombatRound::CheckActionLengthAtTime(unsigned long oidAction,
                                             int nLength, int nTime)
{
    if (m_bRoundPaused == 1 && m_oidRoundPausedBy != oidAction &&
        nTime + m_nTimer < m_nTimer + m_nPauseTimer)
    {
        return 0;
    }

    if (m_bRoundStarted != 1)
    {
        return 1;
    }

    return (m_nRoundLength - m_nOverlapAmount - m_nTimer - nTime + 1000) >=
           nLength;
}
// PC 0x00527CB0
void CSWSCombatRound::SetOffHandAttack(int bOffHand)
{
    m_bOffHandAttack = bOffHand;
}
// PC 0x00527CC0
int CSWSCombatRound::GetOffHandAttack()
{
    return m_bOffHandAttack;
}
// PC 0x00528B60
int CSWSCombatRound::CalculateOnHandAttacks(unsigned short nFeat)
{
    m_nOnHandAttacks =
        m_nEffectAttacks + m_pBaseCreature->m_pStats->GetAttacksPerRound();

    if (nFeat == 11)
    {
        m_nOnHandAttacks++;
    }
    else if (nFeat == 91)
    {
        m_nOnHandAttacks++;
    }
    else if (nFeat == 53)
    {
        m_nOnHandAttacks++;
    }
    else if (nFeat == 30)
    {
        m_nOnHandAttacks++;
    }
    else if (nFeat == 92)
    {
        m_nOnHandAttacks++;
    }
    else if (nFeat == 26)
    {
        m_nOnHandAttacks++;
    }

    return m_nOnHandAttacks;
}
// PC 0x00527CD0
int CSWSCombatRound::CalculateOffHandAttacks()
{
    BOOL bOffHand = FALSE;

    CSWSItem* pItem =
        m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    if (pItem != NULL)
    {
        CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
        if (pBaseItem->m_nWeaponSize - m_pBaseCreature->m_nCreatureSize <= 0)
        {
            CSWSItem* pOffItem =
                m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            if (pOffItem != NULL &&
                g_pRules->m_pBaseItems->GetBaseItem(pOffItem->GetBaseItemType())->m_nWeaponType != 0 &&
                g_pRules->m_pBaseItems->GetBaseItem(pOffItem->GetBaseItemType())->m_nWeaponWield != 0)
            {
                bOffHand = TRUE;
            }
        }
        else
        {
            if (pItem->GetBaseItem()->m_nWeaponWield == 3)
            {
                bOffHand = TRUE;
            }
        }
    }

    m_nOffHandAttacks = bOffHand;
    return bOffHand;
}
// PC 0x00527D90
unsigned char CSWSCombatRound::GetCurrentAction()
{
    return m_nCurrentActionType;
}
// PC 0x00527DA0
void CSWSCombatRound::SetCurrentAction(unsigned char nAction)
{
    m_nCurrentActionType = nAction;
}
// PC 0x00529810
void CSWSCombatRound::DEBUGDumpCombatRoundToScreen(int nArg1,
                                                   const CExoString& sMessage)
{
    sMessage.CStr();

    CExoLinkedListNode* pPos = m_pScheduledActions->GetHeadPos();
    if (pPos != NULL)
    {
        for (CSWSCombatRoundAction* pAction =
                 m_pScheduledActions->GetAtPos(pPos);
             pAction != NULL; pAction = m_pScheduledActions->GetNext(pPos))
        {
        }
    }
}
// PC 0x00527DB0
CSWSCombatAttackData* CSWSCombatRound::GetAttack(int nAttack)
{
    if (nAttack < 0)
    {
        EXOXBOXASSERT(0);
        nAttack = 0;
    }

    EXOXBOXASSERT(nAttack <= 5);
    return &m_Attacks[nAttack];
}
// PC 0x00527DD0
int CSWSCombatRound::ResolveChoreographedState(unsigned long oidTarget)
{
    BOOL bResult = FALSE;
    CSWItem* pItem =
        (CSWItem*)m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    if (pItem != NULL)
    {
        char nWieldType = pItem->GetBaseItem()->m_nWeaponWield;
        if (pItem->GetBaseItem()->m_nRangedWeapon <= 0 && nWieldType != 1 &&
            nWieldType != 8)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CGameObject* pObject = pServer->GetGameObject(oidTarget);
            if (pObject != NULL && pObject->AsSWSCreature() != NULL)
            {
                CSWSCreature* pTarget = pObject->AsSWSCreature();
                CSWInventory* pInventory = pTarget->m_pInventory;
                pItem = (CSWItem*)pInventory->GetItemInSlot(
                    INVENTORY_SLOT_RIGHTHAND);
                if (pItem != NULL)
                {
                    nWieldType = pItem->GetBaseItem()->m_nWeaponWield;
                    if (pItem->GetBaseItem()->m_nRangedWeapon <= 0 &&
                        nWieldType != 1 && nWieldType != 8)
                    {
                        bResult = TRUE;
                    }
                }
            }
        }
    }
    return bResult;
}
// PC 0x00527F20
int CSWSCombatRound::ResolveEngagedState(unsigned long oidTarget)
{
    BOOL bResult = FALSE;

    CGameObject* pObject =
        g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature->m_pcCombatRound->m_bRoundStarted == 0 &&
            pCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID)
        {
            bResult = TRUE;
        }

        unsigned long oidBaseCreature = m_pBaseCreature->m_nId;
        if (pCreature->m_oidAttemptedAttackTarget == oidBaseCreature ||
            pCreature->m_oidAttemptedSpellTarget == oidBaseCreature)
        {
            bResult = TRUE;
        }

        if (pCreature->m_pcCombatRound->m_bRoundStarted != 0 &&
            pCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID)
        {
            bResult = TRUE;
        }

        CSWCCreature* pCharacter =
            g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
        if (pCharacter != NULL &&
            oidTarget == pCharacter->GetServerCreature()->m_nId)
        {
            if (pCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID)
            {
                if (pCreature->m_oidAttemptedSpellTarget ==
                    pCreature->m_oidAttemptedAttackTarget)
                {
                    bResult = FALSE;
                }
            }

            if (pCreature->m_oidAttemptedAttackTarget != OBJECT_INVALID &&
                pCreature->m_oidAttemptedAttackTarget != m_pBaseCreature->m_nId)
            {
                bResult = FALSE;
            }
            else if (pCreature->m_oidAttemptedSpellTarget != OBJECT_INVALID &&
                     pCreature->m_oidAttemptedSpellTarget != m_pBaseCreature->m_nId)
            {
                bResult = FALSE;
            }
            else if (pCreature->m_oidAttemptedMoveToTarget != OBJECT_INVALID &&
                     pCreature->m_oidAttemptedMoveToTarget != m_pBaseCreature->m_nId)
            {
                bResult = FALSE;
            }
        }

        if (pCreature->IsDebilitated())
        {
            return FALSE;
        }
    }

    return bResult;
}
// PC 0x00528050
int CSWSCombatRound::ResolveMasterState(unsigned long oidTarget, int& bMaster)
{
    BOOL bResult = FALSE;

    if (bMaster != 0)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CGameObject* pObject = pServer->GetGameObject(oidTarget);
        if (pObject != NULL && pObject->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (pCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID)
            {
                bResult = TRUE;
            }

            if (pCreature->m_oidAttemptedAttackTarget ==
                    m_pBaseCreature->m_nId &&
                pCreature->m_pcCombatRound->m_bMaster == 0)
            {
                bResult = TRUE;
            }

            if (m_pBaseCreature->m_bInParty != 0)
            {
                bResult = TRUE;
            }
            else
            {
                CSWCCreature* pCharacter =
                    g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
                if (pCharacter != NULL)
                {
                    pCharacter->GetServerCreature();
                }
            }
        }
    }

    return bResult;
}
// PC 0x00528110
int CSWSCombatRound::HasCreatureWeapons()
{
    return m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) == NULL &&
           m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND) == NULL &&
           (m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON1) != NULL ||
            m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON2) != NULL ||
            m_pBaseCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON3) != NULL);
}
// PC 0x005281A0
int CSWSCombatRound::GetTotalAttacks()
{
    return m_nOnHandAttacks + m_nOffHandAttacks;
}
// PC 0x00528230
void CSWSCombatSubAttackThreat::Clear()
{
    m_nUnknown0 = 0;
    m_nUnknown4 = 0;
    m_nUnknown8 = 0;
    m_nUnknownC = 0;
    m_nUnknown10 = 0;
    m_nUnknown11 = 0;
}

#ifndef _XBOX
template <> void CExoArrayList<CSWSCombatSubAttack>::DelIndex(int nIndex)  // PC 0x00528250
{
    num--;
    for (; nIndex < num; nIndex++)
    {
        element[nIndex] = element[nIndex + 1];
    }
}
#endif

static inline void NwscombatroundTrace1()
{
}
