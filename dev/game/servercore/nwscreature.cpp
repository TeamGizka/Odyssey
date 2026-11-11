#include "gameobjectarray.h"
#include "nwscreature.h"
#include "swgui.h"
#include "nwsobject.h"
#include <stdlib.h>
#include <math.h>
#include "appmanager.h"
#include "serverexoapp.h"
#include "serverexoappinternal.h"
#include "serveraimaster.h"
#include "nwscombatround.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwsitem.h"
#include "nwsstore.h"
#include "nwitem.h"
#include "nwmessage.h"
#include "nwsplayer.h"
#include "nwsparty.h"
#include "factionmanager.h"
#include "nwsplaceable.h"
#include "nwsdoor.h"
#include "nwstrigger.h"
#include "nwsarea.h"
#include "nwsambientsound.h"
#include "worldtimer.h"
#include "nwrules.h"
#include "nwfeat.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwmessage.h"
#include "nwccreature.h"
#include "swparty.h"
#include "clientexoapp.h"
#include "partytables.h"
#include "serverinfo.h"
#include "guiingame.h"
#include "resgff.h"
#include "swguiabilitieschargen.h"
#include "nwscript.h"
#include "pathfindinfo.h"
#include "nwsmodule.h"
#include "nwcmodule.h"
#include "nwjournal.h"
#include "nwportrait.h"
#include "nwsscriptevent.h"
#include "nwcreaturelevelstats.h"
#include "nwsspellimpact.h"
#include "nwcanimbase.h"
#include "clientoptions.h"
#include "nwsforcedaction.h"
#include "nwskill.h"
#include "nwsencounter.h"
#include "nwspell.h"
#include "nwclass.h"
#include "nwrace.h"

static inline void NwscreatureTrace1();
#define NWSCREATURE_TRACE() NwscreatureTrace1()

int g_bEnableCombatDebugging = 0;
int g_bEnableSavingThrowDebugging = 1;
int g_bEnableMovementSpeedDebugging = 1;
float fPartySprintFactor = 1.0f;
int RenderHP = 0;
int RenderQA = 0;
int RenderDEV = 0;
int ShowPath = 0;

int HurtPlayers = 0;
int RenderCombat = 0;
int ZeroForce = 0;

int g_nCreatureUnused = 0;

int g_nIncreaseDamage = 1;
int bDisableBumping = 1;
float g_fInjuredLevel = 0.2f;
const float CSWSCreature::MOVE_AWAY_RADIUS = 40.0f;
const int CSWSCreature::XP_CALCULATIONS_HENCHMEN_ALLOWED = 1;
const float CSWSCreature::XP_CALCULATIONS_PC_BONUS_AMOUNT = 0.1f;
const float CSWSCreature::XP_CALCULATIONS_HENCHMEN_BONUS_AMOUNT = 0.05f;
const unsigned char CSWSCreature::XP_CALCULATIONS_MAX_XP_SCALE = 200;

#include "swmain.h"
// PC 0x004EAF70  Xbox 0x0016E530
int CSWSCreaturePartyFollowInfo::Save(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldDWORD(pStruct, m_oidFollowObject, "FollowObject");
    pRes->WriteFieldVector(pStruct, m_vFollowLocation, "FollowLocation");
    pRes->WriteFieldVector(pStruct, m_vLastLeaderPosition, "LastLeaderPos");
    pRes->WriteFieldVector(pStruct, m_vLastFollowerPosition, "LastFollowerPos");
    pRes->WriteFieldFLOAT(pStruct, m_fMaxSpeed, "MaxSpeed");
    pRes->WriteFieldBYTE(pStruct, m_bStickToPosition, "StickToPos");
    pRes->WriteFieldDWORD(pStruct, m_nResult, "Result");
    pRes->WriteFieldDWORD(pStruct, m_nTimeElapsed, "TimeElapsed");
    return pRes->WriteFieldBYTE(pStruct, m_bInSafetyRange, "InSafetyRange");
}
// PC 0x004EB020  Xbox 0x0016E5E0
int CSWSCreaturePartyFollowInfo::Load(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    m_oidFollowObject = pRes->ReadFieldDWORD(pStruct, "FollowObject", bSuccess, GetFollowObject());
    m_vFollowLocation = pRes->ReadFieldVector(pStruct, "FollowLocation", bSuccess,
                                              m_vFollowLocation);
    m_vLastLeaderPosition = pRes->ReadFieldVector(pStruct, "LastLeaderPos", bSuccess,
                                                  m_vLastLeaderPosition);
    m_vLastFollowerPosition = pRes->ReadFieldVector(pStruct, "LastFollowerPos", bSuccess,
                                                    m_vLastFollowerPosition);
    m_fMaxSpeed = pRes->ReadFieldFLOAT(pStruct, "MaxSpeed", bSuccess, GetMaxSpeed());
    m_bStickToPosition = pRes->ReadFieldBYTE(pStruct, "StickToPos", bSuccess, GetStickToPosition());
    m_nResult = pRes->ReadFieldDWORD(pStruct, "Result", bSuccess, GetResult());
    m_nTimeElapsed = pRes->ReadFieldDWORD(pStruct, "TimeElapsed", bSuccess, GetTimeElapsed());
    return m_bInSafetyRange = pRes->ReadFieldBYTE(pStruct, "InSafetyRange", bSuccess,
                                                  GetInSafetyRange());
}
// PC 0x004F79E0
inline CSWSCreaturePartyFollowInfo::CSWSCreaturePartyFollowInfo() throw()
{
    Clear();
}
// PC 0x004F0020  Xbox 0x00173950
inline void CSWSCreaturePartyFollowInfo::Clear()
{
    NWSCREATURE_TRACE();
    Vector vZero;

    m_vFollowLocation = vZero;
    m_vLastLeaderPosition = vZero;
    m_vLastFollowerPosition = vZero;
    m_oidFollowObject = OBJECT_INVALID;
    m_fMaxSpeed = 0.0f;
    m_bStickToPosition = 0;
    m_nResult = CSWSObject::ACTION_COMPLETE;
    m_nTimeElapsed = 0;
    m_bInSafetyRange = 1;
}
// PC 0x004EB160  Xbox 0x0016E720
void CSWSCreature::SetId(unsigned long oidCreature)
{
    unsigned long oidPrevious = m_nId;

    AddToVisibleList(oidCreature, 1, 1, 0, 0);
    m_nId = oidCreature;
    RemoveFromVisibleList(oidPrevious);
}
// PC 0x004EB190  Xbox 0x0016E750
int CSWSCreature::GetActivity(int nActivity)
{
    return (nActivity & m_nActivity) != 0;
}
// PC 0x0063E7F0
int CSWSCreature::GetUseMonkAbilities()
{
    return 0;
}
// PC 0x0063E7C0
int CSWSCreature::UseLoreOnItem(unsigned long oidItem)
{
    return 0;
}
// PC 0x0063E7C0
int CSWSCreature::LearnScroll(unsigned long oidItem)
{
    return 0;
}
// PC 0x004F7A10  Xbox 0x0017B770
CSWSCreature::CSWSCreature(unsigned long oidCreature, int bFlag)
    : CSWSObject(CGameObject::OBJECT_TYPE_CREATURE, oidCreature, bFlag)
{
    m_bInParty = 0;
    m_nSoundSetFile = -1;
    m_pStats = new CSWSCreatureStats(this);
    m_pPathfindInformation = new CPathfindInformation;
    m_pPathfindInformation->m_oidPathfindCreature = GetId();
    m_pcCombatRound = new CSWSCombatRound(this);
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddObject(this, 0);
    m_nAmbientAnimationState = AMBIENT_STATE_NORMAL;
    m_bCombatState = 0;
    SetAnimation(ANIMATION_COMBAT_READY);
    for (int i = 0; i < NUM_SCRIPTS; ++i)
        m_sScripts[i] = "default";
    m_nLastHeartbeatDay = 0;
    m_nLastHeartbeatTime = 0;
    m_nHeartbeatInterval = rand() % CREATURE_HEARTBEAT_RANDOM_INTERVAL + CREATURE_HEARTBEAT_BASE_INTERVAL;
    m_nLastVisibilityDay = 0;
    m_nLastVisibilityTime = 0;
    m_nVisibilityInterval = rand() % CREATURE_VISIBILITY_RANDOM_INTERVAL + CREATURE_VISIBILITY_BASE_INTERVAL;
    m_pInventory = new CSWInventory;
    m_pcItemRepository = new CItemRepository(m_nId, 0);
    memset(&m_cAppearanceInfo, 0, sizeof(m_cAppearanceInfo));
    m_cAppearanceInfo.m_oidAppearanceLeftHand = OBJECT_INVALID;
    m_cAppearanceInfo.m_oidAppearanceRightHand = OBJECT_INVALID;
    m_cAppearanceInfo.m_oidAppearanceArmour = OBJECT_INVALID;
    m_cAppearanceInfo.m_oidAppearanceHead = OBJECT_INVALID;
    m_cAppearanceInfo.m_nAppearancePart6 = 1;
    m_cAppearanceInfo.m_bUseBackupHead = 0;
    m_cAppearanceInfo.m_nAppearanceHead = 0xFF;
    m_oidLockOrientationToObject = CGameObjectArray::INVALIDOBJID;
    m_nLastSpellProjectileTime = 0;
    m_nUnknown228 = 0;
    m_nInventoryIndex = 0;
    m_nInventoryContainerIndex = 0;
    m_oidInventoryContainer = CGameObjectArray::INVALIDOBJID;
    m_fClientPathTime = 1.0f;
    m_fMovementRateFactor = 1.0f;
    m_nAIStateFlags = AI_STATE_ALL;
    m_nActivity = 0;
    m_bInDialog = 0;
    m_bJumped = 0;
    m_bLightsaberThrow = 0;
    m_bUpdateMovingOrientation = 0;
    m_bDebilitated = 0;
    m_nLastMineCheckDay = 0;
    m_nLastMineCheckTime = 0;
    m_nUnknown368 = 0;
    m_nUnknown36C = 0;
    m_nDisarmable = 1;
    m_nDetectMode = 0;
    m_nStealthMode = 0;
    m_nActiveCombatMode = 0;
    m_nDesiredCombatMode = 0;
    m_nCombatStateTimer = 0;
    m_nActionQueueUpdateDay = 0;
    m_nActionQueueUpdateTime = 0;
    m_nUnknown4D8 = 0;
    m_bResolveInitiative = 1;
    m_oidDisappearTarget = OBJECT_INVALID;
    m_vDisappearPosition = Vector();
    m_bWorkRemainingCompleteSent = 1;
    m_bCreationScriptFired = 0;
    m_nUnknown9F8 = 0;
    m_nAIState = 0;
    m_nCurrentAction = ACTION_TYPE_INVALID;
    m_oidAITarget = OBJECT_INVALID;
    m_nUnknown84 = OBJECT_INVALID;
    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_pCreatureList = NULL;
    m_oidSpawnedBy = OBJECT_INVALID;
    m_nEffectSpellId = -1;
    AddToVisibleList(GetId(), 1, 1, 0, 0);
    m_nLastPerceptionDay = 0;
    m_nLastPerceptionTime = 0;
    m_nAttackMovementTimer = CREATURE_ATTACK_MOVEMENT_INTERVAL;
    m_nInjuredAnimationTimer = 1000;
    m_bTimedAnimation = 0;
    m_nTimedAnimationRemaining = 0;
    m_nCustomScriptEventId = -1;
    m_nVisionFlags = 0;
    m_bSilenced = 0;
    m_bHasted = 0;
    m_bSlowed = 0;
    m_nGuardStanceFeat = 0;
    m_bAssuredHit = 0;
    m_bAssuredDeflection = 0;
    m_bHasAssuredDeflection = 0;
    m_bTaunted = 0;
    m_nPoisonState = 0;
    m_bDiseased = 0;
    m_nPoisonEffectID = -1;
    m_bHasLegs = 1;
    m_bHasArms = 1;
    m_nCreatureSize = 0;
    m_sModelType = "";
    m_fWeaponScale = 0.0f;
    m_fPreferredAttackDistance = 0.0f;
    m_oidCounterSpellTarget = OBJECT_INVALID;
    m_bPickPocketAnimationPlayed = 0;
    m_bPickPocketTransferStarted = 0;
    m_bHealAnimationPlayed = 0;
    m_bMineActionStarted = 0;
    m_bLockActionStarted = 0;
    m_bUnlockObjectAnimationPlayed = 0;
    m_bDropAnimationPlayed = 0;
    m_bPickupAnimationPlayed = 0;
    m_nUnknown990 = 0;
    m_nLastStealthRollDay = 0;
    m_nLastStealthRollTime = 0;
    m_nMoveSilentlyCheckRoll = rand() % 10 + 1;
    m_nHideCheckRoll = rand() % 10 + 1;
    m_nSpotCheckRoll = rand() % 10 + 1;
    m_nListenCheckRoll = rand() % 10 + 1;
    for (int nSkill = 0; nSkill < CREATURE_SKILL_COOLDOWN_COUNT; nSkill++)
        m_anSkillCooldowns[nSkill] = 0;
    m_nExcitedStateTimer = 0;
    m_nMovementSpeedDebugTimer = 0;
    m_nUnknown994 = 0;
    m_nUnknown998 = 0;
    m_nUnknown99C = 0;
    m_nUnknown9A0 = 0;
    m_oidAttackTarget = OBJECT_INVALID;
    m_oidAttemptedMovementTarget = OBJECT_INVALID;
    m_oidAttemptedAttackTarget = OBJECT_INVALID;
    m_oidAttemptedMoveToTarget = OBJECT_INVALID;
    m_vLastAttackPosition = Vector();
    m_oidAttemptedSpellTarget = OBJECT_INVALID;
    m_oidSpellTarget = OBJECT_INVALID;
    m_oidGoingToBeAttackedBy = OBJECT_INVALID;
    m_oidLastHostileTarget = OBJECT_INVALID;
    m_nCombatMode = -1;
    m_nLastAttackAction = -1;
    m_nCombatSpellId = -1;
    m_nLastForcePowerUsed = -1;
    m_nLastSpecialAttack = 0;
    m_nLastCombatFeatUsed = 0;
    m_nUnknown560 = 0;
    m_bLastForcePowerSuccessful = -1;
    m_nUnknown8E8 = 0;
    m_nSpecialWalkAnimation = 0;
    m_bInvitedToParty = 0;
    m_oidInvitedBy = OBJECT_INVALID;
    m_bNewAttackTarget = 0;
    m_bIgnoreSpellReadyCount = 0;
    m_pJournal = NULL;
    m_lScriptVars.m_pCreature = this;
    m_nGold = 0;
    m_pInvitationsOffered = NULL;
    m_pInvitationsIgnored = NULL;
    m_nAreaWaitState = 0;
    m_nUnknown948 = -1;
    m_nUnknown94C = 0xFF;
    m_nUnknown94D = 0;
    m_bEncounterCountRemoved = 0;
    m_nCasterLevelOverride = -1;
    m_bCasterLevelOverride = 0;
    m_oidSpellCasterItem = OBJECT_INVALID;
    m_oidLastMineDetected = OBJECT_INVALID;
    m_bItemCastStarted = 0;
    m_fListenCheckDistance = 0.0f;
    m_fSpotCheckDistance = 0.0f;
    m_nEquippedWeight = 0;
    m_nEncumbranceState = 0;
    m_bComputeWeightCarried = 1;
    m_nPerceptionUpdateCount = rand() % 100;
    m_nHeartbeatUpdateCount = rand() % 100;
    m_bRestorePlotOnDownload = 0;
    m_bPMIsDisguised = 0;
    m_nPMAppearance = 0;
    m_nUnknown39C = 0;
    m_nUnknown344 = 0;
    m_nCharacterSheetDisplayCount = 0;
    m_nBodyBag = 0;
    m_bUnlockObjectFacingSet = 0;
    m_nUnknownA78 = 0;
    m_nUnknownA7C = 0;
    m_pFollowInfo = NULL;
    m_nAnimationState = -1;
    m_nNextAttacker = 0;
    m_pAttackerList = NULL;
    SetFadeOutTime(8000);
    m_fForcedMovementDistance = 0.0f;
    m_fForcedMovementHeight = 0.0f;
    m_bBodyFuel = 0;
    m_bRun = 0;
    m_bVisibleListUpdatePending = 0;
    m_nAttackedPartyLeaderState = 0;
    SetActivity(ACTIVITY_DETECT, 1, 0);
    m_nUnknown22C = 0;
    m_nLastInstantEquipLeft = 0;
    m_nLastInstantEquipRight = 0;
    m_nUseItemTimer = 0;
    m_fLightsaberThrowRate = 37.0f;
    m_bForceEquipClothing = 0;
    m_nStealthModeValue = 0;
    m_nItemUseState = 0;
    m_bDisableInjuredAnim = 0;
}
// PC 0x004F8410  Xbox 0x0017C1F0
CSWSCreature::~CSWSCreature()
{
    CSWSFaction* pFaction;
    CSWSModule* pModule;
    CSWSCreatureStats* pStats;
    if (GetArea() != NULL)
    {
        RemoveFromArea(0);
    }

    int nItemCount = m_pcItemRepository->m_lstItems.num;
    for (int nItem = nItemCount - 1; nItem >= 0; nItem--)
    {
        CSWSItem* pItem = m_pcItemRepository->ItemListGetItem(nItem);
        if (pItem != NULL)
        {
            delete pItem;
        }
    }

    for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; nSlot++)
    {
        CSWSItem* pItem = m_pInventory->GetItemInSlot(1 << nSlot);
        if (pItem != NULL)
        {
            delete pItem;
        }
    }

    pFaction = GetFaction();
    if (pFaction != NULL && m_nAreaWaitState == 0)
    {
        pFaction->RemoveMember(GetId());
        pStats = m_pStats;
        pStats->m_nFactionId = -1;
    }
    if (m_pStats != NULL)
    {
        delete m_pStats;
        m_pStats = NULL;
    }

    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->RemoveObject(this);
    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        pModule->RemoveObjectFromLookupTable(m_sTag, GetId());
    }
    if (m_pPathfindInformation != NULL)
    {
        delete m_pPathfindInformation;
        m_pPathfindInformation = NULL;
    }
    if (m_pcCombatRound != NULL)
    {
        delete m_pcCombatRound;
        m_pcCombatRound = NULL;
    }
    if (m_pcItemRepository != NULL)
    {
        delete m_pcItemRepository;
        m_pcItemRepository = NULL;
    }
    if (m_pInventory != NULL)
    {
        delete m_pInventory;
        m_pInventory = NULL;
    }
    if (m_pCreatureList != NULL)
    {
        delete m_pCreatureList;
        m_pCreatureList = NULL;
    }
    if (m_lstVisibilityList.num > 0)
    {
        for (int nNode = m_lstVisibilityList.num - 1; nNode >= 0; nNode--)
        {
            CSWVisibilityNode* pNode = m_lstVisibilityList[nNode];
            m_lstVisibilityList.DelIndex(nNode);
            if (pNode != NULL)
            {
                delete pNode;
            }
        }
    }
    if (m_pJournal != NULL)
    {
        delete m_pJournal;
        m_pJournal = NULL;
    }
    if (m_pFollowInfo != NULL)
    {
        delete m_pFollowInfo;
        m_pFollowInfo = NULL;
    }
    if (m_pAttackerList != NULL)
    {
        delete m_pAttackerList;
        m_pAttackerList = NULL;
    }
}
// PC 0x004F8770  Xbox 0x0017C550
void CSWSCreature::ActionManager(unsigned long nAction)
{
    CServerExoApp* pServer;

    pServer = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer = pServer->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL)
    {
        ComputeAIState();
        if ((nAction == 1 || nAction == 2) && m_nAIState < 10)
        {
            ((CSWSObject*)this)->ClearAllActions(1);
        }
        if (nAction == 8 || nAction == 4)
        {
            ((CSWSObject*)this)->ClearAllActions(1);
        }
    }
}
// PC 0x004F87D0  Xbox 0x0017C5B0
void CSWSCreature::ActivityManager(unsigned long nAction)
{
    if (nAction != ACTIVITY_ACTION_COUNTERSPELL)
    {
        SetActivity(ACTIVITY_GROUP_HIGH, 0, 0);
    }
    if (nAction == ACTIVITY_ACTION_COUNTERSPELL)
    {
        ClearActivities(ACTIVITY_GROUP_BASIC);
    }
    else if (nAction == ACTIVITY_ACTION_CLEAR_MODES)
    {
        ClearActivities(ACTIVITY_GROUP_ALL_LOW);
    }
}
// PC 0x004F00A0  Xbox 0x001739D0
void CSWSCreature::SetActivity(int nActivity, int bFlag, int nArg)
{
    if ((nActivity & m_bInDialog) == nActivity)
    {
        return;
    }
    if (((nActivity & ACTIVITY_GROUP_LOW) && bFlag == 1) ||
        (nActivity == ACTIVITY_GROUP_LOW && bFlag == 0))
    {
        ClearActivities(ACTIVITY_GROUP_LOW);
    }
    if (((nActivity & ACTIVITY_GROUP_HIGH) && bFlag == 1) ||
        (nActivity == ACTIVITY_GROUP_HIGH && bFlag == 0))
    {
        ClearActivities(ACTIVITY_GROUP_HIGH);
    }
    int nChanged;
    if (bFlag == 1)
    {
        nChanged = (m_nActivity & nActivity) == 0;
        m_nActivity |= nActivity;
    }
    else
    {
        nChanged = m_nActivity & nActivity;
        m_nActivity &= ~nActivity;
    }
    if (nChanged)
    {
        if (nActivity & ACTIVITY_STEALTH)
        {
            if (bFlag == 1 && !(m_nActivity & (ACTIVITY_DIALOG | ACTIVITY_REST)))
            {
                SetStealthMode(1, nArg);
            }
            else if (bFlag == 0)
            {
                SetStealthMode(0, nArg);
            }
            else
            {
                m_nActivity &= ~nActivity;
            }
        }
        if (nActivity & ACTIVITY_DETECT)
        {
            SetDetectMode(bFlag != 0);
        }
        if ((nActivity & ACTIVITY_DIALOG) && bFlag == 0)
        {
            StopDialog();
        }
    }
}
// PC 0x004EB1B0  Xbox 0x0016E770
void CSWSCreature::ClearActivities(int nActivity)
{
    if ((nActivity & ACTIVITY_STEALTH) && (m_nActivity & ACTIVITY_STEALTH) &&
        (m_bInDialog & ACTIVITY_STEALTH) != ACTIVITY_STEALTH)
    {
        SetActivity(ACTIVITY_STEALTH, 0, 0);
    }
    if ((nActivity & ACTIVITY_DIALOG) && (m_nActivity & ACTIVITY_DIALOG) &&
        (m_bInDialog & ACTIVITY_DIALOG) != ACTIVITY_DIALOG)
    {
        SetActivity(ACTIVITY_DIALOG, 0, 0);
    }
    if ((nActivity & ACTIVITY_REST) && (m_nActivity & ACTIVITY_REST) &&
        (m_bInDialog & ACTIVITY_REST) != ACTIVITY_REST)
    {
        SetActivity(ACTIVITY_REST, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_4) && (m_nActivity & ACTIVITY_BIT_4) &&
        (m_bInDialog & ACTIVITY_BIT_4) != ACTIVITY_BIT_4)
    {
        SetActivity(ACTIVITY_BIT_4, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_5) && (m_nActivity & ACTIVITY_BIT_5) &&
        (m_bInDialog & ACTIVITY_BIT_5) != ACTIVITY_BIT_5)
    {
        SetActivity(ACTIVITY_BIT_5, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_6) && (m_nActivity & ACTIVITY_BIT_6) &&
        (m_bInDialog & ACTIVITY_BIT_6) != ACTIVITY_BIT_6)
    {
        SetActivity(ACTIVITY_BIT_6, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_7) && (m_nActivity & ACTIVITY_BIT_7) &&
        (m_bInDialog & ACTIVITY_BIT_7) != ACTIVITY_BIT_7)
    {
        SetActivity(ACTIVITY_BIT_7, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_8) && (m_nActivity & ACTIVITY_BIT_8) &&
        (m_bInDialog & ACTIVITY_BIT_8) != ACTIVITY_BIT_8)
    {
        SetActivity(ACTIVITY_BIT_8, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_9) && (m_nActivity & ACTIVITY_BIT_9) &&
        (m_bInDialog & ACTIVITY_BIT_9) != ACTIVITY_BIT_9)
    {
        SetActivity(ACTIVITY_BIT_9, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_10) && (m_nActivity & ACTIVITY_BIT_10) &&
        (m_bInDialog & ACTIVITY_BIT_10) != ACTIVITY_BIT_10)
    {
        SetActivity(ACTIVITY_BIT_10, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_11) && (m_nActivity & ACTIVITY_BIT_11) &&
        (m_bInDialog & ACTIVITY_BIT_11) != ACTIVITY_BIT_11)
    {
        SetActivity(ACTIVITY_BIT_11, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_12) && (m_nActivity & ACTIVITY_BIT_12) &&
        !(m_bInDialog & ACTIVITY_BIT_12))
    {
        SetActivity(ACTIVITY_BIT_12, 0, 0);
    }
    if ((nActivity & ACTIVITY_BIT_13) && (m_nActivity & ACTIVITY_BIT_13) &&
        !(m_bInDialog & ACTIVITY_BIT_13))
    {
        SetActivity(ACTIVITY_BIT_13, 0, 0);
    }
}
// PC 0x004EB3A0  Xbox 0x0016E960
float CSWSCreature::ComputeSpellRange(CExoString sRange, unsigned long oidTarget)
{
    float fRange = 0.0f;

    if (sRange == "P")
        sRange = "T";

    if (sRange == "P")
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_PERSONAL];
    else if (sRange == "T")
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_TOUCH];
    else if (sRange == "S")
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_SHORT];
    else if (sRange == "M")
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_MEDIUM];
    else if (sRange == "L")
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_LONG];
    else if (sRange == "W")
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_WEAPON];

    fRange += GetPathfindInformation()->GetCreaturePersonalSpace() - 0.1f;
    CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()
        ->GetCreatureByGameObjectID(oidTarget);
    if (pTarget != NULL)
        fRange += pTarget->GetPathfindInformation()->GetCreaturePersonalSpace() - 0.1f;

    return fRange;
}
// PC 0x004EB510  Xbox 0x0016EAF0
int CSWSCreature::AddDriveAction(unsigned short nGroupId,
                                 const Vector& vPosition, int nDirection,
                                 int nDriveMode, int nFlags, int nUnknown)
{
    int nArea;
    int nState;
    char bInterruptable;

    nArea = (int)((CSWSObject*)this)->GetArea();
    if (nArea == 0)
    {
        return nArea;
    }

    nState = m_nUnknown8E8;
    bInterruptable = 1;
    if (nState != 0)
    {
        nFlags &= 0xFFFDu;
    }

    ((CSWSObject*)this)
        ->AddAction(0x33u, nGroupId, 2u, (Vector*)&vPosition, 2u,
                    &((Vector*)&vPosition)->m_fY, 1u, &nDirection, 1u,
                    &nDriveMode, 1u, &nFlags, 1u, &nUnknown, 1u,
                    &bInterruptable, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004EB5A0  Xbox 0x0016EB80
int CSWSCreature::AddPathfindingWaitActionToFront(unsigned short nGroupId)
{
    float fWaitTime = 0.3f;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;

    if (pServer->GetClientObjectByObjectId(m_nId) != 0)
    {
        fWaitTime = 0.1f;
    }

    ((CSWSObject*)this)
        ->AddActionToFront(0x1E, nGroupId, 2, &fWaitTime, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004EB620  Xbox 0x0016EC10
int CSWSCreature::AddPickUpItemActions(unsigned long oidItem,
                                      unsigned long oidRepository,
                                      unsigned char nPanel)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer->GetItemByGameObjectID(oidItem) != NULL &&
        ((CSWSObject*)this)->GetArea() != NULL && m_bCommandable != 0)
    {
        ((CSWSObject*)this)->AddAction(
            CSWSObject::ACTION_PICK_UP_ITEM, CSWSObject::ACTION_GROUP_NEW,
            CSWSObject::ACTION_PARAM_OBJECT, &oidItem,
            CSWSObject::ACTION_PARAM_OBJECT, &oidRepository,
            CSWSObject::ACTION_PARAM_INTEGER, &nPanel,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    return 0;
}
// PC 0x004EB6B0  Xbox 0x0016ECA0
void CSWSCreature::ResetUpdateTimes(unsigned long nDay, unsigned long nTime)
{
    m_nLastHeartbeatDay = nDay;
    m_nLastHeartbeatTime = nTime;
    m_nLastPerceptionDay = nDay;
    m_nLastPerceptionTime = nTime;
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
}
// PC 0x004EB6E0  Xbox 0x0016ECD0
void CSWSCreature::SpawnInHeartbeatPerception(int bForce)
{
    unsigned long nDay = 0, nTime = 0;
    unsigned long nElapsedDay = 0, nElapsedTime = 0;
    unsigned long nPerceptionElapsedDay = 0, nPerceptionElapsedTime = 0;
    CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastHeartbeatDay, m_nLastHeartbeatTime, &nElapsedDay, &nElapsedTime);
    int bCreationScriptFired = m_bCreationScriptFired;
    if (bCreationScriptFired == 0)
    {
        CExoString& sSpawn = m_sScripts[SCRIPT_SPAWN];
        g_pVirtualMachine->RunScript(&sSpawn, m_nId, 1);
        if (m_oidSpawnedBy == OBJECT_INVALID && m_bInParty == 0)
            m_bCreationScriptFired = 1;
    }
    if ((bForce == 1 && nElapsedTime >= m_nHeartbeatInterval) || m_bCreationScriptFired == 0)
    {
        ++m_nHeartbeatUpdateCount;
        if (m_nAILevel != CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE || m_nHeartbeatUpdateCount >= (m_nHeartbeatInterval >> CREATURE_HEARTBEAT_COUNT_SHIFT) ||
            m_bCreationScriptFired == 0)
        {
            m_nHeartbeatUpdateCount = 0;
            if ((m_bCreationScriptFired == 0 || m_nLastHeartbeatTime != 0) &&
                GetDead() == 0 && GetIsPCDying() == 0)
            {
                CExoString& sHeartbeat = m_sScripts[CREATURE_SCRIPT_HEARTBEAT];
                g_pVirtualMachine->RunScript(&sHeartbeat, m_nId, 1);
                unsigned char nStateScript = 0;
                switch (m_bDebilitated)
                {
                case CSWSEffectListHandler::STATE_CONFUSION:
                    nStateScript = CREATURE_STATE_SCRIPT_CONFUSION;
                    break;
                case CSWSEffectListHandler::STATE_FEAR:
                    nStateScript = CREATURE_STATE_SCRIPT_FEAR;
                    break;
                case CREATURE_STATE_SCRIPT_STATE_11:
                    nStateScript = CREATURE_STATE_SCRIPT_OTHER;
                    break;
                }
                if (nStateScript != 0)
                {
                    CExoString sScript;
                    g_pRules->Get2DArrays()->GetStateScripts()->GetCExoStringEntry(
                        nStateScript, "SCRIPTNAME", &sScript);
                    g_pVirtualMachine->RunScript(&sScript, m_nId, 1);
                    m_bCommandable = 0;
                }
                CleanInvitationLists();
            }
            m_nLastHeartbeatDay = nDay;
            m_nLastHeartbeatTime = nTime;
            m_nHeartbeatInterval = rand() % CREATURE_HEARTBEAT_RANDOM_INTERVAL + CREATURE_HEARTBEAT_BASE_INTERVAL;
        }
    }
    g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastVisibilityDay, m_nLastVisibilityTime, &nElapsedDay, &nElapsedTime);
    if (m_bCreationScriptFired == 0 ||
        (m_bInParty == 0 && ((m_nAreaWaitState != 0 && bForce == 1 && nElapsedTime >= m_nVisibilityInterval) ||
                            (m_nAreaWaitState == 0 && bForce == 1 && nElapsedTime >= CREATURE_IDLE_PERCEPTION_INTERVAL))))
    {
        UpdateVisibleList(1);
        m_nVisibilityInterval = rand() % CREATURE_VISIBILITY_RANDOM_INTERVAL + CREATURE_VISIBILITY_BASE_INTERVAL;
    }
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastPerceptionDay, m_nLastPerceptionTime,
        &nPerceptionElapsedDay, &nPerceptionElapsedTime);
    if (m_bCreationScriptFired == 0 ||
        (m_nAreaWaitState != 0 && bForce == 1 && nPerceptionElapsedTime >= CREATURE_ACTIVE_PERCEPTION_INTERVAL) ||
        (m_nAreaWaitState == 0 && bForce == 1 && nPerceptionElapsedTime >= CREATURE_IDLE_PERCEPTION_INTERVAL))
    {
        ++m_nPerceptionUpdateCount;
        if (m_nAILevel != CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE || (int)m_nPerceptionUpdateCount >= CREATURE_PERCEPTION_UPDATE_LIMIT || m_bCreationScriptFired == 0)
        {
            m_nPerceptionUpdateCount = rand() % CREATURE_PERCEPTION_RANDOM_COUNT;
            if (m_nLastPerceptionTime != 0 || m_bCreationScriptFired == 0)
            {
                if (m_bInParty != 0)
                    UpdateVisibleList(0);
                else
                    UpdateVisibleList(2);
            }
            m_nLastPerceptionDay = nDay;
            m_nLastPerceptionTime = nTime;
            if (GetArea() != NULL && GetArea()->m_nPlayersInArea == 0 && m_nAILevel != CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE)
                pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE);
        }
    }
    if (m_bCreationScriptFired == 0)
        m_bCreationScriptFired = 1;
}
// PC 0x004EBB30  Xbox 0x0016F120
int CSWSCreature::RangeWeaponEquipped()
{
    CSWSItem* pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);

    if (pItem != NULL)
    {
        int bRanged =
            g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())
                ->m_nRangedWeapon > 0;
        return bRanged > 0;
    }

    return 0;
}
// PC 0x004F0310  Xbox 0x00173C40
int CSWSCreature::ReplaceEquipItemActions(CSWSItem* pItem, unsigned long nSlot,
                                         int bInstant, int nEquipFlags)
{
    unsigned long oidItem = pItem->m_nId;
    int bReplaced = 0;
    CExoLinkedListNode* pPos;
    if (bInstant == 0 && m_pcCombatRound->m_pScheduledActions != NULL)
    {
        pPos = m_pcCombatRound->m_pScheduledActions->GetHeadPos();
        while (pPos != NULL)
        {
            CSWSCombatRoundAction* pAction = m_pcCombatRound->m_pScheduledActions->GetAtPos(pPos);
            if (pAction != NULL && pAction->m_nActionType == CSWSCombatRoundAction::ACTION_EQUIP_ITEM &&
                (pAction->m_nInventorySlot == nSlot || pAction->m_oidTarget == oidItem))
            {
                bReplaced = 1;
                pAction->m_nInventorySlot = nSlot;
                pAction->m_oidTarget = oidItem;
                break;
            }
            m_pcCombatRound->m_pScheduledActions->GetNext(pPos);
        }
    }
    if (bReplaced == 0)
    {
        pPos = m_lstActions.GetHeadPos();
        while (pPos != NULL)
        {
            CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);
            if (pAction != NULL && pAction->m_nActionType == ACTION_EQUIP_ITEM)
            {
                unsigned long oidQueuedItem;
                unsigned long nQueuedSlot;
                memcpy(&oidQueuedItem,
                       &pAction->m_apParam[CSWSObjectActionNode::EQUIP_ITEM_PARAMETER],
                       sizeof(oidQueuedItem));
                memcpy(&nQueuedSlot,
                       &pAction->m_apParam[CSWSObjectActionNode::EQUIP_SLOT_PARAMETER],
                       sizeof(nQueuedSlot));
                if (nQueuedSlot == nSlot || oidQueuedItem == oidItem)
                {
                    bReplaced = 1;
                    pAction->m_apParam[CSWSObjectActionNode::EQUIP_SLOT_PARAMETER] = (void*)nSlot;
                    pAction->m_apParam[CSWSObjectActionNode::EQUIP_ITEM_PARAMETER] = (void*)oidItem;
                    pAction->m_apParam[CSWSObjectActionNode::EQUIP_FLAGS_PARAMETER] = (void*)nEquipFlags;
                    break;
                }
            }
            m_lstActions.GetNext(pPos);
        }
    }
    return bReplaced;
}
// PC 0x004F0420  Xbox 0x00173D50
int CSWSCreature::AddEquipItemActions(CSWSItem* pItem,
                                     unsigned long nInventorySlot,
                                     int bAddToFront, int bIgnoreCombatRound,
                                     int nEquipFlags, int nArg6,
                                     int bSkipReplace)
{
    if (m_pInventory != NULL &&
        (nInventorySlot == INVENTORY_SLOT_ARMOUR ||
         nInventorySlot == INVENTORY_SLOT_HEAD ||
         nInventorySlot == INVENTORY_SLOT_RIGHTHAND ||
         nInventorySlot == INVENTORY_SLOT_LEFTHAND) &&
        pItem != NULL && m_pInventory->GetItemInInventory(pItem) != 0)
    {
        return 0;
    }
    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    unsigned long oidItem = pItem->GetId();
    if (m_bCommandable == 0)
    {
        if (pPlayer != NULL && pMessage != NULL)
        {
            pMessage->SendServerToPlayerInventory_EquipCancel(
                pPlayer->m_nPlayerId, oidItem, nInventorySlot);
        }
        return 0;
    }
    if (m_bCombatState == 1 && m_nItemUseState == COMBAT_STATE_NOTIFY &&
        nInventorySlot == INVENTORY_SLOT_ARMOUR)
    {
        if (pPlayer != NULL && pMessage != NULL)
        {
            pMessage->SendServerToPlayerInventory_EquipCancel(
                pPlayer->m_nPlayerId, oidItem, nInventorySlot);
        }
        SendFeedbackMessage(FEEDBACK_CANNOT_EQUIP_ARMOUR_IN_COMBAT, NULL);
        return 0;
    }
    if (bSkipReplace == 0 &&
        ReplaceEquipItemActions(pItem, nInventorySlot, bIgnoreCombatRound,
                                nEquipFlags) != 0)
    {
        return 1;
    }
    if (m_bCombatState == 1 && m_nItemUseState == COMBAT_STATE_NOTIFY &&
        bIgnoreCombatRound == 0)
    {
        m_pcCombatRound->AddEquipAction(oidItem, nInventorySlot, nEquipFlags,
                                       nArg6);
        return 1;
    }
    if (bAddToFront != 0)
    {
        AddActionToFront(CSWSObject::ACTION_EQUIP_ITEM,
            CSWSObject::ACTION_GROUP_NEW, CSWSObject::ACTION_PARAM_OBJECT,
            &oidItem, CSWSObject::ACTION_PARAM_INTEGER, &nInventorySlot,
            CSWSObject::ACTION_PARAM_INTEGER, &nEquipFlags,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    AddAction(CSWSObject::ACTION_EQUIP_ITEM,
        CSWSObject::ACTION_GROUP_NEW, CSWSObject::ACTION_PARAM_OBJECT,
        &oidItem, CSWSObject::ACTION_PARAM_INTEGER, &nInventorySlot,
        CSWSObject::ACTION_PARAM_INTEGER, &nEquipFlags,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F05D0  Xbox 0x00173EF0
int CSWSCreature::ReplaceUnequipItemActions(CSWSItem* pItem,
                                            unsigned long nSlot,
                                            int bUseActionQueue, int nFlags)
{
    unsigned long oidItem = pItem->GetId();
    int bReplaced = 0;
    CExoLinkedListNode* pPosition;

    if (bUseActionQueue == 0 && m_pcCombatRound->m_pScheduledActions != NULL)
    {
        pPosition = m_pcCombatRound->m_pScheduledActions->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSCombatRoundAction* pAction =
                m_pcCombatRound->m_pScheduledActions->GetAtPos(pPosition);
            if (pAction != NULL &&
                pAction->m_nActionType == CSWSCombatRoundAction::ACTION_UNEQUIP_ITEM &&
                pAction->m_oidTarget == oidItem)
            {
                bReplaced = 1;
                pAction->m_oidTarget = oidItem;
                break;
            }
            m_pcCombatRound->m_pScheduledActions->GetNext(pPosition);
        }
    }

    if (bReplaced == 0)
    {
        pPosition = m_lstActions.GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPosition);
            if (pAction != NULL && pAction->m_nActionType == ACTION_UNEQUIP_ITEM &&
                (unsigned long)pAction->m_apParam[CSWSObjectActionNode::UNEQUIP_ITEM_PARAMETER] == oidItem)
            {
                bReplaced = 1;
                pAction->m_apParam[CSWSObjectActionNode::UNEQUIP_ITEM_PARAMETER] = (void*)oidItem;
                pAction->m_apParam[CSWSObjectActionNode::UNEQUIP_FLAGS_PARAMETER] = (void*)nFlags;
                break;
            }
            m_lstActions.GetNext(pPosition);
        }
    }
    return bReplaced;
}
// PC 0x004F06D0  Xbox 0x00173FF0
int CSWSCreature::AddUnequipActions(CSWSItem* pItem,
                                     unsigned long oidRepository,
                                     int bFront, int bInstant, int nFlags)
{
    unsigned long oidItem = pItem->GetId();
    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    if (m_bCommandable == 0)
    {
        return 0;
    }
    if (m_bCombatState == 1 && m_nItemUseState == COMBAT_STATE_NOTIFY)
    {
        CSWSItem* pArmour = m_pInventory->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
        if (pArmour != NULL && pArmour->GetId() == oidItem)
        {
            if (pPlayer != NULL && pMessage != NULL)
            {
                pMessage->SendServerToPlayerInventory_UnequipCancel(
                    pPlayer->m_nPlayerId, oidItem);
            }
            SendFeedbackMessage(FEEDBACK_CANNOT_UNEQUIP_ARMOUR_IN_COMBAT, NULL);
            return 0;
        }
    }
    if (ReplaceUnequipItemActions(pItem, oidRepository, bInstant, nFlags) == 0)
    {
        if (m_bCombatState == 1 && m_nItemUseState == COMBAT_STATE_NOTIFY && bInstant == 0)
        {
            m_pcCombatRound->AddUnequipAction(oidItem, oidRepository, nFlags);
            return 1;
        }
        int nActionFlags = nFlags;
        if (bFront)
        {
            AddActionToFront(ACTION_UNEQUIP_ITEM, ACTION_GROUP_NEW,
                             ACTION_PARAM_OBJECT, &oidItem,
                             ACTION_PARAM_OBJECT, &oidRepository,
                             ACTION_PARAM_INTEGER, &nActionFlags,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0);
        }
        else
        {
            AddAction(ACTION_UNEQUIP_ITEM, ACTION_GROUP_NEW,
                      ACTION_PARAM_OBJECT, &oidItem,
                      ACTION_PARAM_OBJECT, &oidRepository,
                      ACTION_PARAM_INTEGER, &nActionFlags,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0);
        }
    }
    return 1;
}
// PC 0x004F0840  Xbox 0x00174160
int CSWSCreature::AddDropItemActions(unsigned long oidItem,
                                    const Vector& vPosition,
                                    int bDropSingleItem)
{
    CSWSItem* pArmour;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer->GetItemByGameObjectID(oidItem) == NULL ||
        GetArea() == NULL)
    {
        return 0;
    }
    if (m_bCombatState == 1)
    {
        pArmour = m_pInventory->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
        if (pArmour != NULL && pArmour->m_nId == oidItem)
        {
            SendFeedbackMessage(FEEDBACK_CANNOT_DROP_ARMOUR_IN_COMBAT, NULL);
            return 0;
        }
    }
    if (m_bCommandable == 0)
    {
        return 0;
    }
    AddAction(CSWSObject::ACTION_DROP_ITEM, CSWSObject::ACTION_GROUP_NEW,
              CSWSObject::ACTION_PARAM_OBJECT, &oidItem,
              CSWSObject::ACTION_PARAM_FLOAT, (Vector*)&vPosition,
              CSWSObject::ACTION_PARAM_FLOAT, (void*)&vPosition.m_fY,
              CSWSObject::ACTION_PARAM_FLOAT, (void*)&vPosition.m_fZ,
              CSWSObject::ACTION_PARAM_INTEGER, &bDropSingleItem,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F0900  Xbox 0x00174220
int CSWSCreature::AddHealActions(unsigned long oidTarget, unsigned long oidItem,
                                 int nItemProperty)
{
    int bFirstAction;

    if (m_pStats->GetCanUseSkill(CSWRules::SKILL_TREAT_INJURY) == 0)
    {
        SendFeedbackMessage(FEEDBACK_CANNOT_USE_SKILL, NULL);
        return 0;
    }
    if (m_bCommandable == 0)
    {
        return 0;
    }
    bFirstAction = 1;
    ((CSWSObject*)this)
        ->AddAction(0x38, 0xFFFF, 3, &oidTarget, 3, &oidItem, 1, &nItemProperty,
                    1, &bFirstAction, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0);
    return 1;
}
// PC 0x004F0990  Xbox 0x001742B0
void CSWSCreature::MergeItem(CSWSItem* pItem, CSWSItem* pMergeItem)
{
    CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
    if (pBaseItem->m_nMaxStackSize > 1)
    {
        if (pItem->MergeItem(pMergeItem))
        {
            RemoveItem(pMergeItem, FALSE, FALSE, NULL);
            delete pMergeItem;
        }
        else if (GetInventory()->GetItemInInventory(pItem) == 1)
        {
            int bFound = FALSE;
            unsigned long nSlotFlag;
            for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; nSlot++)
            {
                CSWSItem* pEquippedItem = GetInventory()->GetItemInSlot(1 << nSlot);
                if (pEquippedItem != NULL && pEquippedItem->GetId() == pItem->GetId())
                {
                    bFound = TRUE;
                    nSlotFlag = 1 << nSlot;
                    break;
                }
            }
            if (bFound == TRUE)
            {
                AddEquipItemActions(pMergeItem, nSlotFlag, 0, 0, 0, 0, 0);
            }
        }
    }
}
void CSWSCreature::SplitItem(CSWSItem* pItem, int nCount)  // PC 0x004F0A60  Xbox 0x00174380
{
    CItemRepository* pRepository = GetItemRepository(TRUE);
    if (pItem->m_oidPossessor != m_nId)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CGameObject* pPossessor = pServer->GetGameObject(pItem->m_oidPossessor);
        if (pPossessor->AsSWSItem() == NULL)
        {
            return;
        }
        CSWSItem* pContainer = pPossessor->AsSWSItem();
        if (pContainer->m_oidPossessor == m_nId &&
            pContainer->m_pRepository != NULL)
        {
            pRepository = pContainer->m_pRepository;
        }
    }

    CSWSItem* pSplitItem = pItem->SplitItem(nCount);
    if (pSplitItem != NULL)
    {
        if (!pRepository->AddItem(&pSplitItem, FALSE, FALSE, TRUE))
        {
            pSplitItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                  m_vPosition.m_fZ, TRUE);
        }
    }
}
// PC 0x004F0B20  Xbox 0x00174460
int CSWSCreature::EvaluateLock(CSWSObject* pObject, int bLocked)
{
    CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    if (pObject->AsSWSDoor() != NULL)
    {
        CSWSDoor* pDoor = pObject->AsSWSDoor();
        if (pDoor->m_bLocked == bLocked)
        {
            return TRUE;
        }
        if (pDoor->m_sKeyName.IsEmpty())
        {
            return FALSE;
        }
        unsigned long oidKey = GetItemRepository(TRUE)->FindItemWithTag(pDoor->m_sKeyName);
        CSWSItem* pKey = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidKey);
        if (pKey != NULL)
        {
            if ((unsigned char)pDoor->m_bAutoRemoveKey)
            {
                pAIMaster->AddEventDeltaTime(0, 0, GetId(), pKey->GetId(),
                    CServerAIMaster::EVENT_DECREMENT_STACKSIZE, NULL);
            }
            pDoor->m_bLocked = bLocked;
            SendFeedbackMessage(FEEDBACK_KEY_USED, NULL);
            return TRUE;
        }
        for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
        {
            pKey = GetInventory()->GetItemInSlot(1 << nSlot);
            if (pKey != NULL && pKey->m_sTag == pDoor->m_sKeyName)
            {
                if ((unsigned char)pDoor->m_bAutoRemoveKey)
                {
                    pAIMaster->AddEventDeltaTime(0, 0, GetId(), pKey->GetId(),
                        CServerAIMaster::EVENT_DECREMENT_STACKSIZE, NULL);
                }
                pDoor->m_bLocked = bLocked;
                SendFeedbackMessage(FEEDBACK_KEY_USED, NULL);
                return TRUE;
            }
        }
    }
    else if (pObject->AsSWSPlaceable() != NULL)
    {
        CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
        if (pPlaceable->m_bLocked == bLocked)
        {
            return TRUE;
        }
        if (pPlaceable->m_sKeyName.IsEmpty())
        {
            return FALSE;
        }
        unsigned long oidKey = GetItemRepository(TRUE)->FindItemWithTag(pPlaceable->m_sKeyName);
        CSWSItem* pKey = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidKey);
        if (pKey != NULL)
        {
            if (pPlaceable->m_bAutoRemoveKey)
            {
                pAIMaster->AddEventDeltaTime(0, 0, GetId(), pKey->GetId(),
                    CServerAIMaster::EVENT_DECREMENT_STACKSIZE, NULL);
            }
            pPlaceable->m_bLocked = bLocked;
            SendFeedbackMessage(FEEDBACK_KEY_USED, NULL);
            return TRUE;
        }
        for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
        {
            pKey = GetInventory()->GetItemInSlot(1 << nSlot);
            if (pKey != NULL && pKey->m_sTag == pPlaceable->m_sKeyName)
            {
                if (pPlaceable->m_bAutoRemoveKey)
                {
                    pAIMaster->AddEventDeltaTime(0, 0, GetId(), pKey->GetId(),
                        CServerAIMaster::EVENT_DECREMENT_STACKSIZE, NULL);
                }
                pPlaceable->m_bLocked = bLocked;
                SendFeedbackMessage(FEEDBACK_KEY_USED, NULL);
                return TRUE;
            }
        }
    }
    return FALSE;
}
// PC 0x004F0D70  Xbox 0x001746F0
void CSWSCreature::SetAnimation(int nRequestedAnimation)
{
    int nAnimation = nRequestedAnimation;

    if (nAnimation == CREATURE_ANIMATION_COMBAT_READY ||
        nAnimation == CREATURE_ANIMATION_COMBAT_IDLE)
    {
        switch (m_nAmbientAnimationState)
        {
        case AMBIENT_STATE_2: nAnimation = CREATURE_ANIMATION_AMBIENT_2; break;
        case AMBIENT_STATE_5: nAnimation = CREATURE_ANIMATION_AMBIENT_5; break;
        case AMBIENT_STATE_9: nAnimation = CREATURE_ANIMATION_AMBIENT_9; break;
        case AMBIENT_STATE_10: nAnimation = CREATURE_ANIMATION_AMBIENT_10; break;
        case AMBIENT_STATE_13: nAnimation = CREATURE_ANIMATION_AMBIENT_13; break;
        case AMBIENT_STATE_4: nAnimation = CREATURE_ANIMATION_AMBIENT_4; break;
        case AMBIENT_STATE_3: nAnimation = CREATURE_ANIMATION_AMBIENT_3; break;
        case AMBIENT_STATE_14: nAnimation = CREATURE_ANIMATION_AMBIENT_14; break;
        case AMBIENT_STATE_6: nAnimation = CREATURE_ANIMATION_AMBIENT_6; break;
        case AMBIENT_STATE_7: nAnimation = CREATURE_ANIMATION_AMBIENT_7; break;
        case AMBIENT_STATE_8: nAnimation = CREATURE_ANIMATION_AMBIENT_8; break;
        case AMBIENT_STATE_11: nAnimation = CREATURE_ANIMATION_AMBIENT_11; break;
        case AMBIENT_STATE_12: nAnimation = CREATURE_ANIMATION_AMBIENT_12; break;
        default:
            nAnimation = int(m_bCombatState != 0) + CREATURE_ANIMATION_COMBAT_IDLE;
            break;
        }
    }
    else if (nAnimation == CREATURE_ANIMATION_DEFAULT)
        nAnimation = CREATURE_ANIMATION_COMBAT_IDLE;

    if (g_pAppManager->GetServerExoApp()->IsInjured(GetId()) &&
        (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
             CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
         (g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0) != NULL &&
          g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId()) !=
              g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)->GetObjectId())))
    {
        if (nAnimation == CREATURE_ANIMATION_WALK)
            nAnimation = CREATURE_ANIMATION_INJURED_WALK;
        else if (nAnimation == CREATURE_ANIMATION_RUN)
            nAnimation = CREATURE_ANIMATION_INJURED_RUN;
    }
    else if ((nAnimation == CREATURE_ANIMATION_WALK ||
              nAnimation == CREATURE_ANIMATION_RUN) &&
             (m_nActivity & ACTIVITY_STEALTH))
        nAnimation = CREATURE_ANIMATION_STEALTH_WALK;

    if ((!GetDead() && !GetDeadTemp()) ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_4 ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_3 ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_14)
    {
        CSWCCreature* pClientCreature = GetClientCreature();
        if (pClientCreature == NULL ||
            !pClientCreature->AnimationFireAndForget(nAnimation) ||
            m_nAnimation != nAnimation)
            SetDirtyFlag(DIRTY_ANIMATION_INDEX);
        m_nAnimation = nAnimation;
    }
}
// PC 0x004F0F90  Xbox 0x00174910
int CSWSCreature::GetPauseReadyAnimation()
{
    CSWCCreature* pClientCreature = GetClientCreature();

    if (pClientCreature != NULL)
    {
        return pClientCreature->GetPauseReadyAnimation();
    }
    return m_bCombatState ? 10001 : 10000;
}
// PC 0x004F8A50  Xbox 0x0017C830
int CSWSCreature::AddMoveToPointActionToFront(
    unsigned short nGroupId, const Vector& vPosition, unsigned long oidArea, unsigned long oidTarget, int bRun,
    float fRange, float fTimeout, int bUsePath, int nMovementRate, int bUnknown1, int bClientMoving, int bUnknown2, int bUnknown3,
    const Vector& vOrientation, unsigned long nUnknown4, unsigned long nUnknown5, int bUnknown6)
{
#ifdef _XBOX
    m_nMoveToPointElapsed = -1;
#endif
    int nMoveFlags = bRun | (bUsePath << MOVE_USE_PATH_SHIFT);
    if (fTimeout != 0.0f)
    {
        nMoveFlags |= MOVE_TIMEOUT_MASK;
    }
    nMoveFlags |= (bUnknown1 & MOVE_OPTION1_INPUT_MASK) << MOVE_OPTION1_SHIFT;
    nMoveFlags |= bUnknown2 << MOVE_OPTION2_SHIFT;
    if (bClientMoving != 0)
    {
        nMoveFlags |= (bClientMoving & MOVE_CLIENT_INPUT_MASK) << MOVE_CLIENT_SHIFT;
    }
    nMoveFlags |= (bUnknown3 & MOVE_OPTION3_INPUT_MASK) << MOVE_OPTION3_SHIFT;
    nMoveFlags |= (bUnknown6 & MOVE_OPTION6_INPUT_MASK) << MOVE_OPTION6_SHIFT;

    CPathfindInformation* pPathfindInformation = m_pPathfindInformation;
    m_nAnimationState = ANIMATION_STATE_MOVE_PENDING;
    pPathfindInformation->m_nField_26C = 0;
    pPathfindInformation->m_nField_270 = 0;
    pPathfindInformation->m_nField_274 = 0;

    AddActionToFront(
        ACTION_MOVE_TO_POINT, nGroupId,
        ACTION_PARAM_FLOAT, (void*)&vPosition.m_fX,
        ACTION_PARAM_FLOAT, (void*)&vPosition.m_fY,
        ACTION_PARAM_FLOAT, (void*)&vPosition.m_fZ,
        ACTION_PARAM_OBJECT, &oidArea,
        ACTION_PARAM_OBJECT, &oidTarget,
        ACTION_PARAM_INTEGER, &nMoveFlags,
        ACTION_PARAM_FLOAT, &fRange,
        ACTION_PARAM_INTEGER, &nMovementRate,
        ACTION_PARAM_FLOAT, &fTimeout,
        ACTION_PARAM_FLOAT, (void*)&vOrientation.m_fX,
        ACTION_PARAM_FLOAT, (void*)&vOrientation.m_fY,
        ACTION_PARAM_INTEGER, &nUnknown4,
        ACTION_PARAM_INTEGER, &nUnknown5);
    SetLockOrientationToObject(OBJECT_INVALID, 0);
    return 1;
}
// PC 0x004F8B60  Xbox 0x0017C950
int CSWSCreature::AddMoveToPointAction(
    unsigned short nGroupId, const Vector& vPosition, unsigned long oidArea, unsigned long oidTarget, int bRun, float fRange, float fTimeout,
    int bUsePath, int nMovementRate, int bUnknown1, int bClientMoving, int bUnknown2, int bUnknown3, const Vector& vOrientation, unsigned long nUnknown4, unsigned long nUnknown5, int bUnknown6)
{
#ifdef _XBOX
    m_nMoveToPointElapsed = -1;
#endif
    int nFlags;
    CPathfindInformation* pPathfind;

    nFlags = bRun | (bUsePath << 1);
    if (fTimeout != 0.0f)
    {
        nFlags |= 4;
    }
    nFlags |= (bUnknown2 | ((bUnknown1 & 7) << 1)) << 3;
    if (bClientMoving)
    {
        nFlags |= (bClientMoving & 3) << 7;
    }
    nFlags |= ((bUnknown3 & 1) | ((bUnknown6 & 1) << 1)) << 9;

    pPathfind = m_pPathfindInformation;
    m_nAnimationState = ANIMATION_STATE_MOVE_PENDING;
    pPathfind->m_nField_26C = 0;
    pPathfind->m_nField_270 = 0;
    pPathfind->m_nField_274 = 0;

    AddAction(1, nGroupId, 2, (Vector*)&vPosition, 2,
              &((Vector*)&vPosition)->m_fY, 2, &((Vector*)&vPosition)->m_fZ, 3,
              &oidArea, 3, &oidTarget, 1, &nFlags, 2, &fRange, 1,
              &nMovementRate, 2, &fTimeout, 2, (Vector*)&vOrientation, 2,
              &((Vector*)&vOrientation)->m_fY, 1, &nUnknown4, 1, &nUnknown5);
    SetLockOrientationToObject(OBJECT_INVALID, 0);
    return 1;
}
// PC 0x004F8C70  Xbox 0x0017CA70
int CSWSCreature::AddItemCastSpellActions(unsigned long oidItem,
    int nItemProperty, int nItemOption, Vector vTarget, unsigned long oidTarget,
    int bItemOption, int bTargetLocation, int bCombatAction, int bInstant)
{
    int bPathfinding = TRUE;
    if (!bInstant)
    {
        m_pcCombatRound->AddSWItemSpellAction(oidItem, nItemProperty, nItemOption,
            vTarget, oidTarget, bTargetLocation, bItemOption);
        if (!m_bCombatState && m_pcCombatRound->GetActionPending())
        {
            ActionManager(4);
        }
        if (GetNodeById(ACTION_COMBAT) == NULL)
        {
            int bCombat = TRUE;
            AddAction(ACTION_COMBAT, ACTION_GROUP_NEW,
                ACTION_PARAM_INTEGER, &bCombat,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        return TRUE;
    }
    m_oidAttemptedSpellTarget = OBJECT_INVALID;
    m_oidSpellTarget = OBJECT_INVALID;
    float fX = vTarget.m_fX;
    float fY = vTarget.m_fY;
    float fZ = vTarget.m_fZ;
    if (oidTarget != OBJECT_INVALID)
    {
        CSWSObject* pTarget = (CSWSObject*)g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
        if (pTarget == NULL || pTarget->GetArea() != GetArea())
            return FALSE;
        vTarget = pTarget->m_vPosition;
        if (pTarget->AsSWSCreature() != NULL && pTarget->GetId() != GetId() && !GetInParty())
        {
            CSWVisibilityNode* pVisible = GetVisibleListElement(oidTarget);
            if (pVisible == NULL ||
                pVisible->m_nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED ||
                (pVisible->m_bInvisible && !pVisible->m_bSeen))
                return FALSE;
        }
    }
    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
        return FALSE;
    CSWItemProperty* pProperty = pItem->GetActiveProperty(nItemProperty);

    unsigned long nSpellId;
    if (pProperty != NULL)
        nSpellId = pProperty->m_nSubtype;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell == NULL || GetArea() == NULL || !m_bCommandable)
        return FALSE;
    unsigned long oidArea = GetArea()->GetId();
    float fRange = ComputeSpellRange(pSpell->m_sRange, oidTarget);
    int bRun = TRUE;
    int bMoveAdded = FALSE;
    unsigned short nGroupId = ACTION_GROUP_NEW;
    int bCheckRange = TRUE;
    if (!bCombatAction)
    {
        Vector vDelta = vTarget - m_vPosition;
        if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
            vDelta.m_fZ * vDelta.m_fZ > fRange * fRange)
        {
            AddMoveToPointAction(ACTION_GROUP_NEW, vTarget, oidArea, oidTarget,
                bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, Vector(), 0, 0, bPathfinding);
            bMoveAdded = TRUE;
            nGroupId = ACTION_GROUP_LAST;
        }
        if (oidTarget != OBJECT_INVALID)
        {
            int bInstantCheck = FALSE;
            AddAction(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bCheckRange,
                ACTION_PARAM_INTEGER, &bInstantCheck,
                ACTION_PARAM_INTEGER, &bPathfinding,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
            if (!bMoveAdded)
                nGroupId = ACTION_GROUP_LAST;
            AddAction(ACTION_CHANGE_FACING_OBJECT, nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        else
        {
            AddAction(ACTION_CHECK_MOVE_TO_POINT_RADIUS, nGroupId,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                ACTION_PARAM_OBJECT, &oidArea,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bPathfinding,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
            if (!bMoveAdded)
                nGroupId = ACTION_GROUP_LAST;
            AddAction(ACTION_CHANGE_FACING_POINT, nGroupId,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        AddAction(ACTION_ITEM_CAST_SPELL, nGroupId,
            ACTION_PARAM_OBJECT, &oidItem,
            ACTION_PARAM_INTEGER, &nItemProperty,
            ACTION_PARAM_INTEGER, &nItemOption,
            ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX,
            ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL);
    }
    else
    {
        AddActionToFront(ACTION_ITEM_CAST_SPELL, ACTION_GROUP_NEW,
            ACTION_PARAM_OBJECT, &oidItem,
            ACTION_PARAM_INTEGER, &nItemProperty,
            ACTION_PARAM_INTEGER, &nItemOption,
            ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX,
            ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL);
        if (oidTarget != OBJECT_INVALID)
        {
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, ACTION_GROUP_LAST,
                ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
            int bInstantCheck = FALSE;
            AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, ACTION_GROUP_LAST,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bCheckRange,
                ACTION_PARAM_INTEGER, &bInstantCheck,
                ACTION_PARAM_INTEGER, &bPathfinding,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        else
        {
            AddActionToFront(ACTION_CHANGE_FACING_POINT, ACTION_GROUP_LAST,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
            AddActionToFront(ACTION_CHECK_MOVE_TO_POINT_RADIUS, ACTION_GROUP_LAST,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                ACTION_PARAM_OBJECT, &oidArea,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bPathfinding,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL, 0, NULL);
        }
        Vector vDelta = vTarget - m_vPosition;
        float fDistanceSquared = vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
            vDelta.m_fZ * vDelta.m_fZ;
        if (fDistanceSquared > fRange * fRange)
        {
            AddMoveToPointActionToFront(ACTION_GROUP_LAST, vTarget, oidArea, oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, Vector(), 0, 0, bPathfinding);
            if (GetInParty() && fDistanceSquared > 0.25f)
            {
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                int nIndex = pClient->GetSWParty()->GetIndex(
                    pClient->ServerToClientObjectId(GetId()));
                if (nIndex >= 0)
                {
#ifdef _XBOX
                    CSWPartyMemberData* pMember = pClient->GetSWParty()->GetMemberData(nIndex); pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_APPROACH_TARGET);
#else
                    CSWParty* pParty = pClient->GetSWParty(); pParty->m_aPartyMemberData[nIndex].SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_APPROACH_TARGET);
#endif
                }
            }
        }
    }
    if (!bTargetLocation)
        m_oidAttemptedSpellTarget = oidTarget;
    else
        m_oidAttemptedSpellTarget = GetArea()->GetId();
    return TRUE;
}
// PC 0x004F9460  Xbox 0x0017D280
int CSWSCreature::AddCastSpellActions(unsigned long nSpellId, int nMultiClass,
    int nSpellLevel, int nMetamagic, int bOverride, Vector vTarget,
    unsigned long oidTarget, int nActionFlags, int bTargetLocation,
    int bAddToFront, int bFake, unsigned char nProjectilePath, int bInstant,
    int nFeat, unsigned char nSpellLikeClass, int bDirect)
{
    if (!bDirect)
    {
        m_pcCombatRound->AddSWSpellAction(nSpellId, nMultiClass, nSpellLevel,
            nMetamagic, bOverride, vTarget, oidTarget, bTargetLocation, bFake,
            nProjectilePath, bInstant, nFeat, nSpellLikeClass, nActionFlags);
        if (!m_bCombatState && !m_pcCombatRound->GetActionPending())
        {
            ActionManager(4);
        }
        if (GetNodeById(ACTION_COMBAT) != NULL)
            return TRUE;
        int bCombat = TRUE;
        AddAction(ACTION_COMBAT, ACTION_GROUP_NEW,
            ACTION_PARAM_INTEGER, &bCombat,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL,
            0, NULL);
        return TRUE;
    }

    m_oidAttemptedSpellTarget = OBJECT_INVALID;
    m_oidSpellTarget = OBJECT_INVALID;
    int nSpellLikeClassValue = nSpellLikeClass;
    if (!bFake && !bOverride && nMultiClass != CASTING_CLASS_INNATE_FF)
    {
        CSWSCreatureStats* pStats = m_pStats;
        if (nMultiClass == CASTING_CLASS_INNATE_FE)
        {
            if (!pStats->GetIsInSpellLikeAbilityList(nSpellId))
                return FALSE;
        }
        else if (!pStats->GetSpellUsesLeft(nSpellId, nMultiClass, nSpellLevel, nMetamagic))
            return FALSE;
    }
    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (!bFake && pObject != NULL && pObject->AsSWSCreature() != NULL &&
        pObject->GetId() != GetId() && !GetInParty())
    {
        CSWVisibilityNode* pVisible = GetVisibleListElement(oidTarget);
        if (pVisible == NULL ||
            pVisible->m_nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED ||
            (pVisible->m_bInvisible && !pVisible->m_bSeen))
            return FALSE;
    }
    float fX = vTarget.m_fX;
    float fY = vTarget.m_fY;
    float fZ = vTarget.m_fZ;
    if (oidTarget != OBJECT_INVALID)
    {
        CGameObject* pTargetObject;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidTarget, &pTargetObject) != CGameObjectArray::SUCCESS)
            return FALSE;
        if (pTargetObject->m_nObjectType <= OBJECT_TYPE_AREA)
            return FALSE;
        CSWSArea* pTargetArea = ((CSWSObject*)pTargetObject)->GetArea();
        if (pTargetArea != GetArea())
            return FALSE;
        vTarget = ((CSWSObject*)pTargetObject)->m_vPosition;
    }
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell == NULL || GetArea() == NULL || !m_bCommandable)
        return FALSE;
    unsigned long oidArea = GetArea()->GetId();
    float fRange = ComputeSpellRange(pSpell->m_sRange, oidTarget);
    int bRun = TRUE;
    int nProjectileFlags = nProjectilePath;
    if (bFake)
        nProjectileFlags |= SWSOBJECT_CAST_SPELL_FAKE_FLAG;
    if (bInstant)
        nProjectileFlags |= SWSOBJECT_CAST_SPELL_INSTANT_FLAG;
    int bMoveAdded = FALSE;
    unsigned short nGroupId = ACTION_GROUP_NEW;
    int bCheckRange = TRUE;
    if (bInstant == TRUE)
    {
        if (bAddToFront)
        {
            AddActionToFront(ACTION_CAST_SPELL, ACTION_GROUP_NEW,
                ACTION_PARAM_INTEGER, &nSpellId,
                ACTION_PARAM_INTEGER, &nMultiClass,
                ACTION_PARAM_INTEGER, &nSpellLevel,
                ACTION_PARAM_INTEGER, &nMetamagic,
                ACTION_PARAM_INTEGER, &bOverride,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                ACTION_PARAM_INTEGER, &nProjectileFlags,
                ACTION_PARAM_INTEGER, &nFeat,
                ACTION_PARAM_INTEGER, &nSpellLikeClassValue,
                0, NULL);
        }
        else
        {
            AddAction(ACTION_CAST_SPELL, ACTION_GROUP_NEW,
                ACTION_PARAM_INTEGER, &nSpellId,
                ACTION_PARAM_INTEGER, &nMultiClass,
                ACTION_PARAM_INTEGER, &nSpellLevel,
                ACTION_PARAM_INTEGER, &nMetamagic,
                ACTION_PARAM_INTEGER, &bOverride,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                ACTION_PARAM_INTEGER, &nProjectileFlags,
                ACTION_PARAM_INTEGER, &nFeat,
                ACTION_PARAM_INTEGER, &nSpellLikeClassValue,
                0, NULL);
        }
        return TRUE;
    }
    if (!bAddToFront)
    {
        int bPathfinding = TRUE;
        Vector vDelta = vTarget - m_vPosition;
        if (vDelta * vDelta > fRange * fRange)
        {
            AddMoveToPointAction(ACTION_GROUP_NEW, vTarget, oidArea, oidTarget,
                bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, Vector(), 0, 0, bPathfinding);
            bMoveAdded = TRUE;
            nGroupId = ACTION_GROUP_LAST;
            int bInstantCheck = TRUE;
            if (oidTarget != OBJECT_INVALID)
            {
                AddAction(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, ACTION_GROUP_LAST,
                    ACTION_PARAM_OBJECT, &oidTarget,
                    ACTION_PARAM_INTEGER, &bRun,
                    ACTION_PARAM_FLOAT, &fRange,
                    ACTION_PARAM_FLOAT, &fRange,
                    ACTION_PARAM_INTEGER, &bCheckRange,
                    ACTION_PARAM_INTEGER, &bInstantCheck,
                    ACTION_PARAM_INTEGER, &bPathfinding,
                    0, NULL,
                    0, NULL,
                    0, NULL,
                    0, NULL,
                    0, NULL,
                    0, NULL);
            }
            else
            {
                AddAction(ACTION_CHECK_MOVE_TO_POINT_RADIUS, ACTION_GROUP_LAST,
                    ACTION_PARAM_FLOAT, &fX,
                    ACTION_PARAM_FLOAT, &fY,
                    ACTION_PARAM_FLOAT, &fZ,
                    ACTION_PARAM_OBJECT, &oidArea,
                    ACTION_PARAM_OBJECT, &oidTarget,
                    ACTION_PARAM_INTEGER, &bRun,
                    ACTION_PARAM_FLOAT, &fRange,
                    0, NULL,
                    0, NULL,
                    0, NULL,
                    0, NULL,
                    0, NULL,
                    0, NULL);
            }
        }
        if (oidTarget != OBJECT_INVALID)
        {
            AddAction(ACTION_CHANGE_FACING_OBJECT, nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        else
        {
            AddAction(ACTION_CHANGE_FACING_POINT, nGroupId,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        if (!bMoveAdded)
            nGroupId = ACTION_GROUP_LAST;
        AddAction(ACTION_CAST_SPELL, nGroupId,
            ACTION_PARAM_INTEGER, &nSpellId,
            ACTION_PARAM_INTEGER, &nMultiClass,
            ACTION_PARAM_INTEGER, &nSpellLevel,
            ACTION_PARAM_INTEGER, &nMetamagic,
            ACTION_PARAM_INTEGER, &bOverride,
            ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX,
            ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ,
            ACTION_PARAM_INTEGER, &nProjectileFlags,
            ACTION_PARAM_INTEGER, &nFeat,
            ACTION_PARAM_INTEGER, &nSpellLikeClassValue,
            0, NULL);
    }
    else
    {
        int bPathfinding = TRUE;
        AddActionToFront(ACTION_CAST_SPELL, ACTION_GROUP_NEW,
            ACTION_PARAM_INTEGER, &nSpellId,
            ACTION_PARAM_INTEGER, &nMultiClass,
            ACTION_PARAM_INTEGER, &nSpellLevel,
            ACTION_PARAM_INTEGER, &nMetamagic,
            ACTION_PARAM_INTEGER, &bOverride,
            ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX,
            ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ,
            ACTION_PARAM_INTEGER, &nProjectileFlags,
            ACTION_PARAM_INTEGER, &nFeat,
            ACTION_PARAM_INTEGER, &nSpellLikeClassValue,
            0, NULL);
        int bInstantCheck = FALSE;
        if (oidTarget != OBJECT_INVALID)
        {
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, ACTION_GROUP_LAST,
                ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
            AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, ACTION_GROUP_LAST,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bCheckRange,
                ACTION_PARAM_INTEGER, &bInstantCheck,
                ACTION_PARAM_INTEGER, &bPathfinding,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);
        }
        else
        {
            AddActionToFront(ACTION_CHANGE_FACING_POINT, ACTION_GROUP_LAST,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL);

            AddActionToFront(ACTION_CHECK_MOVE_TO_POINT_RADIUS, ACTION_GROUP_LAST,
                ACTION_PARAM_FLOAT, &fX,
                ACTION_PARAM_FLOAT, &fY,
                ACTION_PARAM_FLOAT, &fZ,
                ACTION_PARAM_OBJECT, &oidArea,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_FLOAT, &bPathfinding,
                0, NULL,
                0, NULL,
                0, NULL,
                0, NULL, 0, NULL);
        }
        Vector vDelta = vTarget - m_vPosition;
        float fDistanceSquared = vDelta * vDelta;
        if (fDistanceSquared > fRange * fRange)
        {
            AddMoveToPointActionToFront(ACTION_GROUP_LAST, vTarget, oidArea, oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, Vector(), 0, 0, bPathfinding);
            if (GetInParty() && fDistanceSquared > 0.25f)
            {
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                unsigned long oidClient = pClient->ServerToClientObjectId(GetId());
                int nIndex = pClient->GetSWParty()->GetIndex(oidClient);
                if (nIndex >= 0)
                {
#ifdef _XBOX
                    CSWPartyMemberData* pMember = pClient->GetSWParty()->GetMemberData(nIndex); pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_APPROACH_TARGET);
#else
                    CSWParty* pParty = pClient->GetSWParty(); pParty->m_aPartyMemberData[nIndex].SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_APPROACH_TARGET);
#endif
                }
            }
        }
    }
    if (!bTargetLocation)
        m_oidAttemptedSpellTarget = oidTarget;
    else
        m_oidAttemptedSpellTarget = GetArea()->GetId();
    return TRUE;
}
// PC 0x004F9DA0  Xbox 0x0017DBD0
int CSWSCreature::AddMineActions(unsigned long oidMine, unsigned char nAction,
                                 unsigned long oidTarget, const Vector& vTarget,
                                 unsigned long oidArea)
{
    float fX, fY, fZ;
    if (!m_bCommandable)
        return FALSE;
    if (nAction == ACTION_SET_MINE && GetArea() != NULL &&
        !GetArea()->GetPlayerCanSetMines())
        return FALSE;
    ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
    CGameObject* pMine = g_pAppManager->GetServerExoApp()->GetGameObject(oidMine);
    if (pMine == NULL)
        return FALSE;
    if (nAction == ACTION_DISARM_MINE || nAction == ACTION_RECOVER_MINE)
    {
        if (pMine->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER &&
            !pMine->AsSWSTrigger()->m_bTrapDisarmable &&
            pMine->AsSWSTrigger()->GetTrapFlag() == TRUE)
            return FALSE;
        if (pMine->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR &&
            !pMine->AsSWSDoor()->GetDisarmable() &&
            pMine->AsSWSDoor()->GetTrapped() == TRUE)
            return FALSE;
        if (pMine->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE &&
            !pMine->AsSWSPlaceable()->m_bTrapDisarmable &&
            pMine->AsSWSPlaceable()->GetTrapFlag() == TRUE)
            return FALSE;
    }
    int bCanUseSkill = m_pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS);
    if (nAction == ACTION_SET_MINE && !bCanUseSkill)
        return FALSE;
    else if (nAction != ACTION_SET_MINE && !bCanUseSkill)
        return FALSE;
    if (nAction == ACTION_SET_MINE)
    {
        fX = vTarget.m_fX;
        fY = vTarget.m_fY;
        fZ = vTarget.m_fZ;
        AddAction(ACTION_SET_MINE, ACTION_GROUP_NEW,
                  ACTION_PARAM_OBJECT, &oidMine, ACTION_PARAM_OBJECT, &oidTarget,
                  ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY,
                  ACTION_PARAM_FLOAT, &fZ,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        return TRUE;
    }
    else
    {
        AddAction(nAction, ACTION_GROUP_NEW, ACTION_PARAM_OBJECT, &oidMine,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    return TRUE;
}
// PC 0x004F9FB0  Xbox 0x0017DE20
int CSWSCreature::AddCutsceneAttackActions(unsigned long nArg1, int nArg2,
                                           int nArg3, int nArg4)
{
    m_pcCombatRound->AddCutsceneAttackAction(nArg1, nArg2, nArg3, nArg4);
    if (m_bCombatState == 0)
    {
        ActionManager(4);
    }
    if (((CSWSObject*)this)->GetNodeById(0x3F) == 0)
    {
        nArg4 = 1;
        ((CSWSObject*)this)
            ->AddAction(0x3F, 0xFFFF, 1, &nArg4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    return 1;
}
// PC 0x004FA050  Xbox 0x0017DEC0
int CSWSCreature::AddCutsceneMoveActions(unsigned long nArg1,
                                         const Vector& vArg2, int nArg3)
{
    m_pcCombatRound->AddCutsceneMoveActions(nArg1, vArg2, nArg3);
    if (m_bCombatState == 0)
    {
        ActionManager(4);
    }
    if (((CSWSObject*)this)->GetNodeById(0x3F) == 0)
    {
        nArg3 = 1;
        ((CSWSObject*)this)
            ->AddAction(0x3F, 0xFFFF, 1, &nArg3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    return 1;
}
// PC 0x004FA100  Xbox 0x0017DF70
void CSWSCreature::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                            int bLoadingGame, int bUpdateSubAreas)
{
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
        ->GetClientObjectByObjectId(m_nId);
    if (GetArea() != pArea || bLoadingGame == 1)
    {
        RemoveFromArea(0);
    }

    CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    CGameObjectArray* pObjectArray = g_pAppManager->GetServerExoApp()->GetObjectArray();
    CGameObject* pObject;
    for (int i = 0; i < m_lstAreaOfEffectObjects.num; i++)
    {
        if (pObjectArray->GetGameObject(m_lstAreaOfEffectObjects[i], &pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->AsSWSAreaOfEffectObject() != NULL)
        {
            pObject->AsSWSAreaOfEffectObject()->AddToArea(pArea, fX, fY, fZ, 1);
        }
    }

    if (pArea != NULL)
    {
        if (GetArea() != pArea && pArea->GetId() != m_oidDisappearTarget &&
            pPlayer != NULL)
        {
            CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
            if (pMessage != NULL)
            {
                m_oidDisappearTarget = pArea->GetId();
                m_vDisappearPosition = Vector(fX, fY, fZ);
                m_bWorkRemainingCompleteSent = 0;
                pMessage->SendServerToPlayerArea_ClientArea(
                    pPlayer, pArea, fX, fY, fZ, m_vOrientation, 0);
                SetArea(NULL);
                SetPosition(Vector(fX, fY, fZ), 0, 1, 0);
                return;
            }
        }

        SetPosition(Vector(fX, fY, fZ), 1, 1, 0);
        if (GetArea() != pArea || bLoadingGame == 1)
        {
            pArea->AddObjectToArea(GetId(), !bLoadingGame);
        }
        SetArea(pArea);
        if (m_nAILevel == CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE &&
            pArea->m_nPlayersInArea > 0 && pAIMaster != NULL)
        {
            pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_OCCUPIED_AREA_CREATURE);
        }
        if (m_nAILevel == CServerAIMaster::AI_LEVEL_OCCUPIED_AREA_CREATURE &&
            pArea->m_nPlayersInArea == 0 && pAIMaster != NULL)
        {
            pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE);
        }
        if (bUpdateSubAreas)
        {
            UpdateSubareasOnJumpPosition(m_vPosition, pArea->GetId(),
                !g_pAppManager->GetServerExoApp()->GetLoadFromSaveGame());
        }
    }
}
// PC 0x004F0FD0  Xbox 0x00174950
void CSWSCreature::RemoveFromArea(int bLoading)
{
    if (GetArea() != NULL)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
    }

    if (!bLoading)
    {
        ClearVisibleList();
        if (GetArea() != NULL)
        {
            CSWSArea* pArea = GetArea();
            CServerAIMaster* pAIMaster =
                g_pAppManager->GetServerExoApp()->GetServerAIMaster();
            for (int i = 0; i < pArea->m_lstObjectIds.num; i++)
            {
                CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()
                    ->GetCreatureByGameObjectID(pArea->m_lstObjectIds[i]);
                if (pCreature != NULL)
                {
                    CSWVisibilityNode* pNode = pCreature->GetVisibleListElement(GetId());
                    if (pNode != NULL)
                    {
                        if (pNode->m_bHeard)
                        {
                            CScriptEvent* pEvent = new CScriptEvent;
                            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                            pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_LOST_HEARING);
                            pEvent->SetObjectID(0, GetId());
                            pAIMaster->AddEventDeltaTime(0, 0, GetId(), pCreature->GetId(),
                                CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                        }
                        if (pNode->m_bSeen)
                        {
                            CScriptEvent* pEvent = new CScriptEvent;
                            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                            pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_LOST_SIGHT);
                            pEvent->SetObjectID(0, m_nId);
                            pAIMaster->AddEventDeltaTime(0, 0, GetId(), pCreature->GetId(),
                                CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                        }
                        pCreature->RemoveFromVisibleList(m_nId);
                    }
                }
            }
        }
    }

    CGameObjectArray* pObjectArray = g_pAppManager->GetServerExoApp()->GetObjectArray();
    CGameObject* pObject;
    for (int j = 0; j < m_lstAreaOfEffectObjects.num; j++)
    {
        unsigned char nResult = pObjectArray->GetGameObject(m_lstAreaOfEffectObjects[j], &pObject);
        if (nResult == CGameObjectArray::SUCCESS &&
            pObject->AsSWSAreaOfEffectObject() != NULL)
        {
            pObject->AsSWSAreaOfEffectObject()->RemoveFromArea();
        }
    }
    SetArea(NULL);
    SetPosition(Vector(0.0f, 0.0f, 0.0f), TRUE, TRUE, FALSE);
}
// PC 0x004FA390  Xbox 0x0017E230
void CSWSCreature::UpdateMineCheck()
{
    Vector vNearest;
    Vector vPosition;
    unsigned long nDay, nTime, nElapsedDay, nElapsedTime;
    g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastMineCheckDay, m_nLastMineCheckTime, &nElapsedDay, &nElapsedTime);
    unsigned long nInterval;
    float fDistanceSquared;
    if (GetDetectMode() == 0)
    {
        nInterval = CREATURE_MINE_CHECK_SLOW_INTERVAL;
        fDistanceSquared = NWSCREATURE_MINE_CHECK_NEAR_DISTANCE_SQUARED;
    }
    else
    {
        nInterval = CREATURE_MINE_CHECK_FAST_INTERVAL;
        fDistanceSquared = NWSCREATURE_MINE_CHECK_FAR_DISTANCE_SQUARED;
    }
    if (nElapsedTime >= nInterval)
    {
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->m_nLastEvent[CREATURE_LAST_EVENT_MINE_CHECK] = 1;
        CSWSArea* pArea = GetArea();
        if (pArea != NULL)
        {
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
            vPosition = m_vPosition;
            int bInParty = m_bInParty;
            for (int nObject = 0; nObject < pArea->m_lstTrapObjectIds.num; ++nObject)
            {
                CGameObject* pObject = pServer->GetGameObject(pArea->m_lstTrapObjectIds.element[nObject]);
                if (pObject == NULL)
                    continue;
                int nRoll;
                if (GetDetectMode() == 1 || GetDetectMode() == 2)
                    nRoll = (unsigned short)g_pRules->RollDice(1, CREATURE_MINE_DETECT_DIE) + CREATURE_MINE_DETECT_BONUS;
                else
                    nRoll = (unsigned short)g_pRules->RollDice(1, CREATURE_MINE_DETECT_DIE);
                GetClientCreature();
                if (pObject->AsSWSTrigger() != NULL)
                {
                    CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
                    pTrigger->CalculateNearestPoint(vPosition, &vNearest);
                    if (pTrigger->m_lstTrapTargets.Contains(GetId()) == 0 &&
                        pTrigger->m_bTrapDetectable == 1)
                    {
                        Vector vDelta;
                        vDelta = vPosition - vNearest;
                        if (vDelta * vDelta <= fDistanceSquared &&
                            pTrigger->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION &&
                            pTrigger->GetFactionId() != GetFactionId())
                        {
                            int nDC = pTrigger->m_nTrapDetectDC;
                            if (nRoll + m_pStats->GetSkillRank(CSWRules::SKILL_AWARENESS, NULL, 0) >= nDC ||
                                pTrigger->m_bTrapFlagged == 1)
                            {
                                CSWCCMessageData* pData = NULL;
                                if (pTrigger->m_bTrapFlagged == 0)
                                {
                                    pData = new CSWCCMessageData;
                                    pData->SetObjectID(0, GetId());
                                    pData->SetObjectID(1, pTrigger->GetId());
                                    pData->SetInteger(0, nRoll);
                                    pData->SetInteger(1, m_pStats->GetSkillRank(CSWRules::SKILL_AWARENESS, NULL, 0));
                                    pData->SetInteger(2, pTrigger->m_nTrapDetectDC);
                                    BroadcastMineDetectionData(pData);
                                    delete pData;
                                }
                                if (bInParty == 1)
                                {
                                    for (int i = 0; i < pParty->GetNumCharacters(); ++i)
                                    {
                                        CSWCCreature* pMember = pParty->GetCharacter(i);
                                        unsigned long oidMember = pServer->ClientToServerObjectId(pMember->GetId());
                                        pTrigger->m_lstTrapTargets.AddUnique(oidMember);
                                    }
                                }
                                else
                                    pTrigger->m_lstTrapTargets.AddUnique(GetId());
                            }
                        }
                    }
                }
                if (pObject->AsSWSDoor() != NULL)
                {
                    CSWSDoor* pDoor = pObject->AsSWSDoor();
                    if (pDoor->m_lstTrapDetectedBy.Contains(GetId()) == 0 &&
                        pDoor->m_bTrapDetectable == 1)
                    {
                        Vector vDelta;
                        vDelta = vPosition - pDoor->m_vPosition;
                        float fDistance = vDelta * vDelta;
                        if (fDistance <= fDistanceSquared &&
                            pObject->AsSWSDoor()->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION &&
                            pObject->AsSWSDoor()->GetFactionId() != GetFactionId())
                        {
                            unsigned char nDC = pDoor->m_nTrapDetectDC;
                            if (nRoll + m_pStats->GetSkillRank(CSWRules::SKILL_AWARENESS, NULL, 0) >= nDC ||
                                (unsigned char)pDoor->m_bTrapFlagged == 1)
                            {
                                if (bInParty == 1)
                                {
                                    for (int i = 0; i < pParty->GetNumCharacters(); ++i)
                                    {
                                        CSWCCreature* pMember = pParty->GetCharacter(i);
                                        unsigned long oidMember = pServer->ClientToServerObjectId(pMember->GetId());
                                        pDoor->m_lstTrapDetectedBy.AddUnique(oidMember);
                                    }
                                }
                                else
                                    pDoor->m_lstTrapDetectedBy.AddUnique(GetId());
                            }
                        }
                    }
                }
                if (pObject->AsSWSPlaceable() != NULL)
                {
                    CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
                    if (pPlaceable->m_lstTrapDetectedBy.Contains(GetId()) == 0 &&
                        pPlaceable->m_bTrapDetectable == 1)
                    {
                        Vector vDelta;
                        vDelta = vPosition - pPlaceable->m_vPosition;
                        float fDistance = vDelta * vDelta;
                        if (fDistance <= fDistanceSquared &&
                            pObject->AsSWSPlaceable()->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION &&
                            pObject->AsSWSPlaceable()->GetFactionId() != GetFactionId())
                        {
                            unsigned char nDC = pPlaceable->m_nTrapDetectDC;
                            if (nRoll + m_pStats->GetSkillRank(CSWRules::SKILL_AWARENESS, NULL, 0) >= nDC ||
                                (unsigned char)pPlaceable->m_bTrapFlagged == 1)
                            {
                                if (bInParty == 1)
                                {
                                    for (int i = 0; i < pParty->GetNumCharacters(); ++i)
                                    {
                                        CSWCCreature* pMember = pParty->GetCharacter(i);
                                        unsigned long oidMember = pServer->ClientToServerObjectId(pMember->GetId());
                                        pPlaceable->m_lstTrapDetectedBy.AddUnique(oidMember);
                                    }
                                }
                                else
                                    pPlaceable->m_lstTrapDetectedBy.AddUnique(GetId());
                            }
                        }
                    }
                }
            }
        }
        m_nLastMineCheckDay = nDay;
        m_nLastMineCheckTime = nTime;
    }
}
// PC 0x004FDE40  Xbox 0x00181980
int CSWSCreature::AddAttackActions(unsigned long oidTarget,
                                  unsigned short nSpecialAttack,
                                  int nAttackFlags, int bNewAttackTarget,
                                  int bClearActions, int bAddToFront,
                                  int bQueueAction, int nArg8, int nArg9,
                                  int nArg10, int nArg11, int nArg12,
                                  int nArg13, int nArg14, int nArg15)
{
    if (bQueueAction == 0)
    {
        if (nAttackFlags != 0 && bClearActions != 0)
        {
            ClearAllActions(1);
            m_pcCombatRound->RemoveAllActions();
            m_pcCombatRound->ClearAllAttacks();
        }
        m_pcCombatRound->AddAttackAction(oidTarget, nSpecialAttack,
            nAttackFlags, CSWSCombatRound::ATTACK_ACTION_TIME, 1);
        if (m_bCombatState == 0)
        {
            ActionManager(4);
        }
        if (GetNodeById(ACTION_COMBAT) == NULL)
        {
            int bCombat = TRUE;
            AddAction(ACTION_COMBAT, ACTION_GROUP_NEW,
                ACTION_PARAM_INTEGER, &bCombat,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    else
    {
        ComputeAIState();
        if (m_bCommandable == 0)
            return 0;
        CSWVisibilityNode* pVisible = GetVisibleListElement(oidTarget);
        if (pVisible != NULL &&
            pVisible->m_nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED)
            return 0;
        if (oidTarget != OBJECT_INVALID)
        {
            CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
            if (pObject != NULL && pObject->AsSWSCreature() != NULL &&
                GetAIStateReputation(oidTarget) == AI_STATE_REPUTATION_FRIEND)
            {
                SendFeedbackMessage(FEEDBACK_CANNOT_ATTACK_FRIEND, NULL);
                return 0;
            }
        }
        if (bClearActions != 0)
            ClearAllActions(1);
        if (oidTarget != OBJECT_INVALID)
        {
            if (bAddToFront != 0)
            {
                AddActionToFront(ACTION_PHYSICAL_ATTACKS, ACTION_GROUP_NEW,
                    ACTION_PARAM_INTEGER, &nArg12, ACTION_PARAM_OBJECT, &oidTarget,
                    ACTION_PARAM_INTEGER, &nArg8, ACTION_PARAM_INTEGER, &nArg9,
                    ACTION_PARAM_INTEGER, &nArg10, ACTION_PARAM_INTEGER, &nArg11,
                    ACTION_PARAM_INTEGER, &nSpecialAttack, ACTION_PARAM_INTEGER, &nArg13,
                    ACTION_PARAM_INTEGER, &nArg14, ACTION_PARAM_INTEGER, &nArg15,
                    0, 0, 0, 0, 0, 0);
            }
            else
            {
                AddAction(ACTION_PHYSICAL_ATTACKS, ACTION_GROUP_NEW,
                    ACTION_PARAM_INTEGER, &nArg12, ACTION_PARAM_OBJECT, &oidTarget,
                    ACTION_PARAM_INTEGER, &nArg8, ACTION_PARAM_INTEGER, &nArg9,
                    ACTION_PARAM_INTEGER, &nArg10, ACTION_PARAM_INTEGER, &nArg11,
                    ACTION_PARAM_INTEGER, &nSpecialAttack, ACTION_PARAM_INTEGER, &nArg13,
                    ACTION_PARAM_INTEGER, &nArg14, ACTION_PARAM_INTEGER, &nArg15,
                    0, 0, 0, 0, 0, 0);
            }
            CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(oidTarget);
            if (pTarget != NULL && (pTarget->GetIsPC() == 1 ||
                g_pAppManager->GetServerExoApp()->GetPartyTable()
                    ->GetIsLeader(pTarget->GetId()) == 1))
                pTarget->m_oidGoingToBeAttackedBy = GetId();
            BroadcastCombatStateToParty(GetId());
            if (pTarget != NULL)
                ResolveInitiative();
            if (m_bCombatState == 0 && GetIsPC() == 1)
            {
                if (rand() % 4 < 3)
                    BroadcastBattleCry();
            }
            else if (GetInParty() == 1 && rand() % 10 == 0)
                BroadcastBattleCry();
            if (m_oidAttemptedAttackTarget == OBJECT_INVALID)
                m_oidAttemptedAttackTarget = oidTarget;
            m_bNewAttackTarget = bNewAttackTarget;
        }
        CSWSDoor* pDoor = g_pAppManager->GetServerExoApp()->GetDoorByGameObjectID(oidTarget);
        if (pDoor != NULL && pDoor->m_bPlotObject == 1)
        {
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
    }
    return 1;
}
// PC 0x004FE210  Xbox 0x00181D50
void CSWSCreature::AIUpdate()
{
    if (HurtPlayers != 0 && m_bInParty != 0)
    {
        HurtPlayers = 0;
        SetCurrentHitPoints(1);
    }
    if (RenderAi != 0)
    {
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        CSWSCreature* pPlayer = pServer->GetCreatureByGameObjectID(pServer->GetPlayerCreatureId());
        m_sTag.CStr();
        if (pPlayer != NULL)
            magnitude(m_vPosition - pPlayer->m_vPosition);
    }

    unsigned __int64 nUpdateStart = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CServerAIMaster* pAIMaster = pServer->GetServerAIMaster();
    CWorldTimer* pWorldTimer = pServer->GetWorldTimer();
    if ((m_pStats->GetIsPC() == 1 || m_bInParty != 0) &&
        m_nAILevel != CServerAIMaster::AI_LEVEL_PARTY_CREATURE)
        pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_PARTY_CREATURE);
    SpawnInHeartbeatPerception(1);

    unsigned long nDay = 0;
    unsigned long nTime = 0;
    unsigned long nElapsedDay = 0;
    unsigned long nElapsedTime = 0;
    pServer->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    pWorldTimer->SubtractWorldTimes(nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime,
                                  &nElapsedDay, &nElapsedTime);
    m_nElapsedTime = nElapsedTime;
    UpdateCombatRoundTimer();
    UpdateExcitedStateTimer();
    UpdateEffectList(nDay, nTime);
    RunActions(nDay, nTime, nUpdateStart);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;

    if (GetArea() != NULL)
    {
        CSWSRoom* pRoom = NULL;
        Vector vPosition(m_vPosition);
        vPosition.m_fZ = GetArea()->ComputeHeight(m_vPosition, 1, &pRoom);
        if (pRoom == NULL && m_bInParty != 0)
        {
            if (GetId() != g_pAppManager->GetServerExoApp()->GetPlayerCreatureId() &&
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState == 0 &&
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 == 0)
            {
                unsigned long oidSelf = GetId();
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                unsigned long oidClient = pClient->ServerToClientObjectId(oidSelf);
                int nIndex = pClient->GetSWParty()->GetIndex(oidClient);
                if (nIndex != -1)
                {
                    CSWPartyMemberData* pMember = &g_pAppManager->GetClientExoApp()->GetSWParty()->m_aPartyMemberData[nIndex];
                    if (pMember != NULL)
                    {
                        vPosition = pMember->m_vFollowPosition;
                        if (GetArea()->ComputeSafeLocation(vPosition, NWSCREATURE_PARTY_SAFE_LOCATION_RADIUS,
                            GetPathfindInformation(), 1, &vPosition) != 0)
                            vPosition.m_fZ = GetArea()->ComputeHeight(m_vPosition, 1, &pRoom);
                    }
                }
            }
        }
        if (m_nAnimationState == ANIMATION_STATE_FORCE_PUSH ||
            m_nAnimationState == ANIMATION_STATE_FORCE_JUMP)
        {
            float fHeight = m_fForcedMovementHeight;
            vPosition.m_fZ += fHeight;
        }
        SetPosition(vPosition, 1, 0, 0);
    }

    ComputeAIState();
    if (GetArea() != NULL)
        UpdateMineCheck();
    if (m_bCombatState == 1)
    {
        if (m_nItemUseState == COMBAT_STATE_NOTIFY)
            SetActivity(ACTIVITY_STEALTH, 0, 0);
        if (m_nCombatStateTimer <= 0)
            SetCombatState(0, m_nItemUseState);
        else
            m_nCombatStateTimer -= m_nElapsedTime;
    }
    if ((unsigned long)m_nUseItemTimer > 0)
    {
        if (m_nElapsedTime >= (unsigned long)m_nUseItemTimer)
            m_nUseItemTimer = 0;
        else
            m_nUseItemTimer -= m_nElapsedTime;
    }
    if (m_bCombatState == 1)
    {
        if (m_nAttackMovementTimer <= 0)
        {
            GetDead();
            m_nAttackMovementTimer = CREATURE_ATTACK_MOVEMENT_INTERVAL;
        }
        m_nAttackMovementTimer -= m_nElapsedTime;
    }

    if (GetDead() == 0 && GetDeadTemp() == 0 && m_bInParty != 0)
    {
        int bRegenerate = m_bCombatState == 0 || m_nItemUseState == COMBAT_STATE_EQUIPMENT;
        float fHealthRate = 0.0f;
#ifdef _XBOX
        CTwoDimArrays* pArraysForRegeneration = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pArraysForRegeneration->m_pRegeneration);
        pArraysForRegeneration->m_pRegeneration->GetFLOATEntry(bRegenerate,
#else
        g_pRules->m_p2DArrays->m_pRegeneration->GetFLOATEntry(bRegenerate,
#endif
            CTwoDimArrays::COL2DA_REGENERATION_HEALTHREGEN, &fHealthRate);
        float fHealthRegen = max(NWSCREATURE_REGEN_MINIMUM,
                                 GetMaxHitPoints(1) * (fHealthRate * NWSCREATURE_REGEN_PERCENT_SCALE));
        float fHealthGain = fHealthRegen * (m_nElapsedTime * NWSCREATURE_SECONDS_PER_MILLISECOND);
        CSWSCreatureStats* pHealthStats = m_pStats;
        float fHitPoints = (short)GetCurrentHitPoints(1) +
            (short)pHealthStats->AdjustHPAccumulatorBy(fHealthGain);
        SetCurrentHitPoints((int)min(GetMaxHitPoints(1), fHitPoints));
        short nTemporaryForcePoints = m_pStats->m_nTemporaryForcePoints;
        int nMaxForce = nTemporaryForcePoints + (short)GetMaxForcePoints();
        if ((short)GetMaxForcePoints() > 0)
        {
            float fForceRate = 0.0f;
            g_pRules->m_p2DArrays->m_pRegeneration->GetFLOATEntry(bRegenerate,
                CTwoDimArrays::COL2DA_REGENERATION_FORCEREGEN, &fForceRate);
            float fForceRegen = max(NWSCREATURE_REGEN_MINIMUM,
                                    nMaxForce * (fForceRate * NWSCREATURE_REGEN_PERCENT_SCALE));
            float fForceGain = fForceRegen * (m_nElapsedTime * NWSCREATURE_SECONDS_PER_MILLISECOND);
            int nForce = (short)(m_pStats->m_nCurrentForcePoints + m_pStats->m_nTemporaryForcePoints) +
                (short)m_pStats->AdjustFPAccumulatorBy(fForceGain);
            if (ZeroForce != 0)
            {
                ZeroForce = 0;
                nForce = 0;
            }
            if (nMaxForce < nForce)
                nForce = nMaxForce;
            m_pStats->m_nCurrentForcePoints = nForce;
        }
    }
    if (RenderHP != 0)
    {
        m_sTag.CStr();
        GetCurrentHitPoints(0);
        GetMaxHitPoints(1);
        GetMaxForcePoints();
    }

    unsigned long oidPlayer = 0;
    if (g_pAppManager->GetClientExoApp() != NULL &&
        g_pAppManager->GetClientExoApp()->GetSWParty() != NULL)
    {
        if (g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0) != NULL)
        {
            CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
            oidPlayer = pPlayer->GetId();
            if (oidPlayer != 0)
                oidPlayer = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pPlayer->GetId());
        }
    }
    if ((RenderQA != 0 || RenderDEV != 0) && GetId() == oidPlayer &&
        g_pAppManager->GetClientExoApp()->GetModule() != NULL)
    {
        CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
        pModule->m_sModuleName.CStr();
    }

    if (m_bTimedAnimation == 1)
    {
        if (m_nTimedAnimationRemaining <= 0)
        {
            m_bTimedAnimation = 0;
            m_nTimedAnimationRemaining = 0;
        }
        else
            m_nTimedAnimationRemaining -= m_nElapsedTime;
    }
    if (m_nAnimation == CREATURE_ANIMATION_RUN ||
        m_nAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10087 ||
        m_nAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10086)
    {
        if (m_nInjuredAnimationTimer <= 0)
            m_nInjuredAnimationTimer = CREATURE_INJURED_ANIMATION_INTERVAL;
        else
            m_nInjuredAnimationTimer -= m_nElapsedTime;
    }
    if (m_nAreaWaitState == 1)
    {
        unsigned long nPauseDay;
        unsigned long nPauseTime;
        unsigned long nQueueElapsedDay;
        unsigned long nQueueElapsedTime;
        pServer->GetPauseTimer()->GetWorldTime(&nPauseDay, &nPauseTime);
        pWorldTimer->SubtractWorldTimes(nPauseDay, nPauseTime,
            m_nActionQueueUpdateDay, m_nActionQueueUpdateTime, &nQueueElapsedDay, &nQueueElapsedTime);
        if (nQueueElapsedTime >= CREATURE_PAUSED_QUEUE_INTERVAL)
        {
            UpdateActionQueue();
            m_nActionQueueUpdateDay = nPauseDay;
            m_nActionQueueUpdateTime = nPauseTime;
        }
    }
    for (int i = 0; i < CREATURE_SKILL_COOLDOWN_COUNT; i++)
    {
        if (m_anSkillCooldowns[i] > 0)
        {
            if (m_anSkillCooldowns[i] > m_nElapsedTime)
                m_anSkillCooldowns[i] -= m_nElapsedTime;
            else
                m_anSkillCooldowns[i] = 0;
        }
    }
    if (g_bEnableMovementSpeedDebugging == 1)
    {
        if (m_nMovementSpeedDebugTimer > m_nElapsedTime)
            m_nMovementSpeedDebugTimer -= m_nElapsedTime;
        else
            m_nMovementSpeedDebugTimer = CREATURE_MOVEMENT_DEBUG_INTERVAL;
    }
    if (m_nUnknown344 == 1)
    {
        m_pStats->UpdateCombatInformation();
        m_nUnknown344 = 0;
    }
    if (m_bCombatState == 1)
    {
        if (m_nAILevel == CServerAIMaster::AI_LEVEL_OCCUPIED_AREA_CREATURE ||
            m_nAILevel == CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE)
            pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_NONPARTY_CREATURE);
    }
    else if (m_nAILevel == CServerAIMaster::AI_LEVEL_NONPARTY_CREATURE && GetArea() != NULL)
    {
        if (GetArea()->m_nPlayersInArea == 0)
            pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE);
        else
            pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_OCCUPIED_AREA_CREATURE);
    }
    if (GetArea() != NULL)
    {
        if (GetArea()->m_bStealthXPEnabled != 0)
        {
            if ((unsigned long)m_nAttackedPartyLeaderState <= 0)
                return;
            if ((unsigned long)m_nAttackedPartyLeaderState > m_nElapsedTime && m_nAttackedPartyLeaderState != -1)
            {
                m_nAttackedPartyLeaderState -= m_nElapsedTime;
                return;
            }
            unsigned long nExperience = GetArea()->m_nStealthXPCurrent;
            if (nExperience < GetArea()->m_nStealthXPLoss)
                nExperience = 0;
            else
                nExperience -= GetArea()->m_nStealthXPLoss;
            GetArea()->SetCurrentStealthXP(nExperience);
        }
        m_nAttackedPartyLeaderState = 0;
    }
}
// PC 0x004FEB90  Xbox 0x00182760
int CSWSCreature::EquipItem(unsigned long nSlot, CSWSItem* pItem,
                          int bApplyProperties, int bLoading, int bInstant)
{
    if (m_pInventory->GetItemInInventory(pItem))
    {
        UnequipItem(pItem);
    }
    if (bApplyProperties)
    {
        pItem->ApplyItemProperties(this, nSlot, bLoading);
    }
    m_pInventory->PutItemInSlot(nSlot, pItem);
    ComputeArmourClass(pItem, TRUE, FALSE);
    m_pStats->UpdateCombatInformation();
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState == 0 &&
        bInstant == 0 && bLoading == 0 &&
        (nSlot == INVENTORY_SLOT_RIGHTHAND || nSlot == INVENTORY_SLOT_LEFTHAND))
    {
        SetCombatState(TRUE, COMBAT_STATE_EQUIPMENT);
    }
    int nEquippedWeight = m_nEquippedWeight;
    m_nEquippedWeight = nEquippedWeight + pItem->GetWeight();
    CScriptEvent* pEvent = new CScriptEvent;
    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_EQUIP_ITEM;
    pEvent->SetObjectID(0, pItem->GetId());
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
        0, 0, GetId(), pModule->GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    return TRUE;
}
// PC 0x004FAA70  Xbox 0x0017E980
int CSWSCreature::UnequipItem(CSWSItem* pItem)
{
    pItem->RemoveItemProperties(this, m_pInventory->GetSlotFromItem(pItem));
    m_pInventory->RemoveItem(pItem);
    ComputeArmourClass(pItem, TRUE, FALSE);
    int nEquippedWeight = m_nEquippedWeight;
    m_nUnknown344 = TRUE;
    m_nEquippedWeight = nEquippedWeight - pItem->GetWeight();
    if (pItem->GetBaseItem() != NULL &&
        pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_STEALTH_DEVICE &&
        m_nStealthMode == TRUE)
    {
        ToggleMode(MODE_STEALTH);
    }
    return TRUE;
}
// PC 0x004FAB00  Xbox 0x0017EA10
int CSWSCreature::ClearAction(CSWSObjectActionNode* pNode, int bRemove)
{
    if (pNode->m_nField70 == 0)
        return 0;
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();

    CServerExoAppInternal* pInternal = g_pAppManager->GetServerExoApp()->m_pcExoAppInternal;
    CSWSPlayer* pPlayer = pInternal->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL && pMessage != NULL)
    {
        if (pNode->m_nActionType == ACTION_EQUIP_ITEM)
        {
            unsigned long oidItem;
            memcpy(&oidItem, &pNode->m_apParam[0], sizeof(oidItem));
            unsigned long nSlot;
            memcpy(&nSlot, &pNode->m_apParam[1], sizeof(nSlot));
            pMessage->SendServerToPlayerInventory_EquipCancel(pPlayer->m_nPlayerId,
                oidItem, nSlot);
        }
        else if (pNode->m_nActionType == ACTION_PICK_UP_ITEM)
        {
            unsigned long oidItem;
            memcpy(&oidItem, &pNode->m_apParam[0], sizeof(oidItem));
            pMessage->SendServerToPlayerInventory_PickupCancel(pPlayer->m_nPlayerId,
                oidItem);
        }
        else if (pNode->m_nActionType == ACTION_DROP_ITEM)
        {
            unsigned long oidItem;
            memcpy(&oidItem, &pNode->m_apParam[0], sizeof(oidItem));
            pMessage->SendServerToPlayerInventory_DropCancel(pPlayer->m_nPlayerId, oidItem);
        }
        else if (pNode->m_nActionType == ACTION_UNEQUIP_ITEM)
        {
            unsigned long oidItem;
            memcpy(&oidItem, &pNode->m_apParam[0], sizeof(oidItem));
            pMessage->SendServerToPlayerInventory_UnequipCancel(pPlayer->m_nPlayerId,
                oidItem);
        }
    }
    if (pNode->m_nActionType == ACTION_REST)
    {
        CancelRest(REST_CANCEL_ACTION);
    }
    else if (pNode->m_nActionType == ACTION_PHYSICAL_ATTACKS)
    {
        m_oidAttackTarget = OBJECT_INVALID;
        m_oidAttemptedAttackTarget = OBJECT_INVALID;
        SetAnimation(ANIMATION_IDLE);
        ProcessPendingCombatActions();
    }
    else if (pNode->m_nActionType == ACTION_MOVE_TO_POINT)
    {
        m_pPathfindInformation->m_oidPathfindTargetCreature = OBJECT_INVALID;
        m_pPathfindInformation->m_oidUseTarget = OBJECT_INVALID;
        m_nAnimationState = ANIMATION_STATE_WALK;
    }
    else if (pNode->m_nActionType == ACTION_CAST_SPELL)
    {
        m_oidAttemptedSpellTarget = OBJECT_INVALID;
        m_oidSpellTarget = OBJECT_INVALID;
        m_pcCombatRound->RemoveSpellAction();
        m_bIgnoreSpellReadyCount = 0;
        SetAnimation(ANIMATION_IDLE);
        ProcessPendingCombatActions();
    }
    else if (pNode->m_nActionType == ACTION_ITEM_CAST_SPELL)
    {
        m_oidAttemptedSpellTarget = OBJECT_INVALID;
        m_oidSpellTarget = OBJECT_INVALID;
        m_pcCombatRound->RemoveSpellAction();
        SetAnimation(ANIMATION_IDLE);
        m_bItemCastStarted = 0;
        ProcessPendingCombatActions();
    }
    else if (pNode->m_nActionType == ACTION_COUNTER_SPELL)
    {
        SetCombatMode(0, 1);
    }
    else if (pNode->m_nActionType == ACTION_DROP_ITEM)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bDropAnimationPlayed = 0;
    }
    else if (pNode->m_nActionType == ACTION_PICK_UP_ITEM)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bPickupAnimationPlayed = 0;
    }
    else if (pNode->m_nActionType == ACTION_SET_MINE ||
             pNode->m_nActionType == ACTION_DISARM_MINE ||
             pNode->m_nActionType == ACTION_RECOVER_MINE ||
             pNode->m_nActionType == ACTION_EXAMINE_MINE ||
             pNode->m_nActionType == ACTION_FLAG_MINE)
    {
        SetAnimation(ANIMATION_IDLE);
        StopGuiTimingBar();
        m_bMineActionStarted = 0;
    }
    else if (pNode->m_nActionType == ACTION_PICK_POCKET)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bPickPocketAnimationPlayed = 0;
        m_bPickPocketTransferStarted = 0;
    }
    else if (pNode->m_nActionType == ACTION_HEAL)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bHealAnimationPlayed = 0;
    }
    else if (pNode->m_nActionType == ACTION_UNLOCK_OBJECT)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bUnlockObjectAnimationPlayed = 0;
        StopGuiTimingBar();
    }
    else if (pNode->m_nActionType == ACTION_LOCK_OBJECT)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bLockActionStarted = 0;
        StopGuiTimingBar();
    }
    else if (pNode->m_nActionType == ACTION_OPEN_DOOR)
    {
        SetAnimation(ANIMATION_IDLE);
        m_bOpenDoorStarted = 0;
    }
    else if (pNode->m_nActionType == ACTION_FOLLOW_LEADER)
    {
        if (m_bInParty != 0)
        {
            unsigned long oidClient = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId()); int nIndex = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(oidClient);
            if (nIndex >= 0 && nIndex <= 2) {
#ifdef _XBOX
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetMemberData(nIndex)->Reset();
#else
                CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty(); EXOLOG("CSWSCreature::ClearAction: party %p", &pParty); pParty->m_aPartyMemberData[nIndex].Reset();
#endif
            }
        }
    }
    else if (pNode->m_nActionType == ACTION_DIALOG_OBJECT)
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
    }
    else if (pNode->m_nActionType == ACTION_RESUME_DIALOG)
    {
        m_nDialogPending = 0;
    }
    else if (pNode->m_nActionType == ACTION_USE_OBJECT)
    {
        unsigned long oidPlaceable = (unsigned long)pNode->m_apParam[0];
        CSWSPlaceable* pPlaceable = g_pAppManager->m_pServerExoApp->GetPlaceableByGameObjectID(oidPlaceable);
        if (pPlaceable != NULL && pPlaceable->m_bInventoryOpening != 0)
        {
            pPlaceable->m_bInventoryOpening = 0;
            if (pPlaceable->m_nAnimation == PLACEABLE_ANIMATION_OPEN)
                pPlaceable->SetAnimation(PLACEABLE_ANIMATION_CLOSED);
            pPlaceable->m_fSyncHeight = 1.0f;
        }
    }
    if (pMessage != NULL && pNode->m_nActionType == ACTION_MOVE_TO_POINT && bRemove != 0)
        SetAnimation(ANIMATION_IDLE);
    if (GetLockOrientationToObject() != CGameObjectArray::INVALIDOBJID)
    {
        GetLockOrientationToObject();
        SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID, 0);
    }
    if (bRemove == 1)
    {
        m_nActionStartDay = 0;
        m_nActionStartTime = 0;
    }
    return 1;
}
// PC 0x004FAF20  Xbox 0x0017EE30
void CSWSCreature::UpdateCombatRoundTimer()
{
    if (m_pcCombatRound != NULL)
    {
        if (!g_pAppManager->GetServerExoApp()->GetPauseState(
                CServerExoAppInternal::PAUSE_TYPE_COMBAT_MASK))
        {
            if (m_pcCombatRound->m_bRoundPaused)
            {
                if (!m_pcCombatRound->m_bInfinitePause)
                {
                    CSWSCombatAttackData* pAttack = m_pcCombatRound->GetAttack(
                        m_pcCombatRound->m_nCurrentAttack);
                    int nTimeRemaining = m_pcCombatRound->m_nPauseTimer - m_nElapsedTime;
                    if (nTimeRemaining > 0)
                    {
                        m_pcCombatRound->DecrementPauseTimer(m_nElapsedTime);
                        if (pAttack->m_lstSubAttacks.num > 0)
                        {
                            int nPauseTime = m_pcCombatRound->m_nPauseTimer;
                            if (pAttack->IsSubAttackReady(nPauseTime))
                            {
                                pAttack->ResolveTopSubAttackToAttack();
                                SignalDamage(pAttack->m_bRangedAttack, pAttack->m_oidReactObject);
                                pAttack->RemoveTopSubAttack();
                            }
                        }
                    }
                    else
                    {
                        while (pAttack->m_lstSubAttacks.num > 0)
                        {
                            pAttack->ResolveTopSubAttackToAttack();
                            SignalDamage(pAttack->m_bRangedAttack, pAttack->m_oidReactObject);
                            pAttack->RemoveTopSubAttack();
                        }
                        int nElapsedTime = -nTimeRemaining;
                        ResetCombatAnimations(nElapsedTime);
                        if (m_pcCombatRound->m_bMaster)
                        {
                            unsigned long oidMasterTarget = m_pcCombatRound->m_oidMasterTarget;
                            CSWSCreature* pMaster = g_pAppManager->GetServerExoApp()
                                ->GetCreatureByGameObjectID(oidMasterTarget);
                            if (pMaster != NULL)
                                pMaster->ResetCombatAnimations(nElapsedTime);
                        }
                        unsigned long oidTarget = m_pcCombatRound->m_oidUnknown9CC;
                        CGameObject* pTarget = g_pAppManager->GetServerExoApp()
                            ->GetGameObject(oidTarget);
                        if (pTarget != NULL && pTarget->AsSWSObject() != NULL)
                        {
                            CSWSObject* pObject = pTarget->AsSWSObject();
                            if (pObject->GetDead() ||
                                (pObject->AsSWSCreature() != NULL &&
                                 pObject->AsSWSCreature()->GetDeadTemp()))
                                m_pcCombatRound->EndCombatRound(1);
                        }
                    }
                }
                else
                    m_pcCombatRound->DecrementPauseTimer(m_nElapsedTime);
            }
            else
                m_pcCombatRound->IncrementTimer(m_nElapsedTime);
        }
    }
}
// PC 0x004F1250  Xbox 0x00174BD0
int CSWSCreature::ResetCombatAnimations(int nElapsedTime)
{
    int bAnimationChanged = 0;

    m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
    m_pcCombatRound->SetPauseTimer(0, 0);
    m_pcCombatRound->IncrementTimer(nElapsedTime);
    m_pcCombatRound->SetCurrentAction(0);
    m_bResolveInitiative = 0;

    if (!GetDead() && !GetIsPCDying() && !GetDeadTemp() &&
        AnimationStationary(GetAnimation()) && !IsDebilitated() &&
        m_pcCombatRound->m_nUnknown940 == 0)
    {
        SetAnimation(GetPauseReadyAnimation());
        bAnimationChanged = 1;
    }
    return bAnimationChanged;
}

inline unsigned char CSWSCreature::GetStealthMode()
{
    return m_nStealthMode;
}
// PC 0x004FECE0  Xbox 0x001828B0
void CSWSCreature::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nDay,
                              unsigned long nTime)
{
    if (m_bCreationScriptFired == 0)
        SpawnInHeartbeatPerception(0);
    if (nEventId == CServerAIMaster::EVENT_APPLY_EFFECT)
    {
        if (pEventData != NULL)
        {
            CGameEffect* pEffect = static_cast<CGameEffect*>(pEventData);
            int bLinked = 0;
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_LINK)
            {
                bLinked = 1;
                m_bApplyingLinkedEffect = 1;
            }
            ApplyEffect(pEffect, 0, bLinked);
            if (bLinked != 0)
                m_bApplyingLinkedEffect = 0;
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_REMOVE_EFFECT)
    {
        if (pEventData != NULL)
            RemoveEffectById(*static_cast<unsigned __int64*>(pEventData));
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_REMOVE_FROM_AREA)
    {
        if (pEventData != NULL)
            RemoveFromArea((int)pEventData);
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_PLAY_ANIMATION)
    {
        if (pEventData != NULL && GetDead() == 0 && GetIsPCDying() == 0 && GetDeadTemp() == 0)
            SetAnimation((int)pEventData);
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
            g_pVirtualMachine->RunScriptSituation(pEventData, GetId(), 1);
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_BROADCAST_AOO)
    {
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_BROADCAST_SAFE_PROJECTILE)
    {
        {
            CSWSCombatAttackData* pAttack = static_cast<CSWSCombatAttackData*>(pEventData);
            if (pAttack == NULL)
                return;
            CSWSCreature* pAttacker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(pAttack->m_oidReactObject);
            if (pAttacker != NULL && pTarget != NULL)
            {
                CSWSItem* pAmmo = static_cast<CSWSItem*>(g_pAppManager->GetServerExoApp()->GetGameObject(pAttack->m_oidAmmoItem));
                if (pAmmo != NULL)
                {
                    CSWBaseItem* pBaseItem = pAmmo->GetBaseItem();
                    BroadcastSafeProjectile(oidCaller, pTarget->GetId(), pAttacker->m_vPosition,
                        pTarget->m_vPosition, pAttack->m_nReactionDelay,
                        pAttack->m_nAttackResult, 2, pBaseItem->m_nAmmunitionType, 0, 0);
                }
            }
            delete pAttack;
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_FEEDBACK_MESSAGE)
    {
        {
            CSWCCMessageData* pData = static_cast<CSWCCMessageData*>(pEventData);
            if (pData == NULL)
                return;
            CSWSCreature* pCaller = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            if (pData->m_nType == 1)
            {
                BroadcastSavingThrowData(pData);
                if (pCaller != NULL)
                    pCaller->BroadcastSavingThrowData(pData);
                delete pData;
            }
            else if (pData->m_nType == 3)
            {
                if (pCaller != NULL)
                    pCaller->SendFeedbackMessage((unsigned short)pData->GetInteger(0), pData);
            }
            else if (pData->m_nType == 4)
                SendFeedbackMessage((unsigned short)pData->GetInteger(0), pData);
            else
            {
                CSWCCMessageData* pCopy = new CSWCCMessageData;
                pData->CopyTo(pCopy);
                SendFeedbackMessage((unsigned short)pData->GetInteger(0), pData);
                if (pCaller != NULL)
                    pCaller->SendFeedbackMessage((unsigned short)pCopy->GetInteger(0), pCopy);
                else
                    delete pCopy;
            }
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_ON_MELEE_ATTACKED)
    {
        {
            CSWSItem* pWeapon = NULL;
            CSWSCreature* pAttacker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            if (pEventData != NULL)
            {
                CSWSCombatAttackData* pAttack = static_cast<CSWSCombatAttackData*>(pEventData);
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, oidCaller);
                pData->SetObjectID(1, GetId());
                if (g_bEnableCombatDebugging != 0)
                    pData->SetString(0, pAttack->m_sAttackDebugText);
                delete pData;
                m_nAttackType = pAttack->m_nAttackType;
                m_nAttackMode = pAttack->m_nAttackMode;
                if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_RIGHTHAND ||
                    pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND)
                    pWeapon = pAttacker->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND)
                    pWeapon = pAttacker->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
                         pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
                    pWeapon = pAttacker->GetInventory()->GetItemInSlot(INVENTORY_SLOT_HANDS);
                else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE1)
                    pWeapon = pAttacker->GetInventory()->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON1);
                else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE2)
                    pWeapon = pAttacker->GetInventory()->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON2);
                else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE3)
                    pWeapon = pAttacker->GetInventory()->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON3);
            }
            m_oidLastAttacker = oidCaller;
            SetLastHostileActor(oidCaller, 0);
            if (pAttacker != NULL)
            {
                pAttacker->BroadcastCombatStateToParty(GetId());
                pAttacker->RemoveCombatInvisibilityEffects();
                if (pWeapon != NULL)
                    pAttacker->m_oidLastWeaponUsed = pWeapon->GetId();
                else
                    pAttacker->m_oidLastWeaponUsed = OBJECT_INVALID;
            }
            BroadcastCombatStateToParty(oidCaller);
            ResolveInitiative();
            if (GetDead() == 0 && GetDeadTemp() == 0)
                g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ATTACKED], GetId(), 1);
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_SPELL_IMPACT ||
             nEventId == CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT)
    {
        {
            CSWSSpellScriptData* pSpell = static_cast<CSWSSpellScriptData*>(pEventData);
            if (pSpell != NULL && pSpell->m_oidCaster == oidCaller)
            {
                m_nLastSpellId = pSpell->m_nSpellId;
                m_oidSpellTargetObject = pSpell->m_oidTarget;
                SetSpellTargetPosition(pSpell->m_vTargetPosition);
                m_oidSpellCasterItem = pSpell->m_oidItem;
                if (pSpell->m_oidTarget != OBJECT_INVALID)
                {
                    CGameObject* pObject;
                    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(pSpell->m_oidTarget, &pObject) == CGameObjectArray::SUCCESS &&
                        pObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
                    {
                        CSWSArea* pArea = static_cast<CSWSObject*>(pObject)->GetArea();
                        if (GetArea() == pArea)
                            SetSpellTargetPosition(static_cast<CSWSObject*>(pObject)->m_vPosition);
                    }
                }
                SetEffectSpellId(pSpell->m_nSpellId);
                m_nSavingThrowSpellId = pSpell->m_nSpellId;
                if (nEventId == CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT)
                {
                    m_bCasterLevelOverride = 1;
                    m_nCasterLevelOverride = pSpell->m_nUnknown28;
                    CalculateLastSpellProjectileTime(0);
                }
                g_pVirtualMachine->RunScript(&pSpell->m_sScript, GetId(), 1);
                SetEffectSpellId(-1);
                m_nSavingThrowSpellId = -1;
                m_bCasterLevelOverride = 0;
                m_nCasterLevelOverride = -1;
            }

            operator delete(pEventData);
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        {
            if (GetIsPC() == 1)
                EXOLOG("Creature: %x Tag: %s Received Destroy Event but cannot be destroyed, 'cuz it's a PC!",
                       GetId(), m_sTag.CStr());
            if (m_nEC == 0)
                EXOLOG("Creature: %x Tag: %s Received Destroy Event but cannot be destroyed, 'cuz it's not destroyable!",
                       GetId(), m_sTag.CStr());

            if (m_bInParty != 0)
                EXOLOG("Creature: %x Tag: %s Received Destroy Event but cannot be destroyed, 'cuz it's in the party!",
                       GetId(), m_sTag.CStr());
            if (GetIsPC() != 0 || m_nEC == 0 || m_bInParty != 0)
                return;
            if (GetDead() != 0)
            {
                unsigned long oidCreature = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId());
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                unsigned long oidBodyBag = SpawnBodyBag();
                unsigned long oidClientBodyBag = pClient->ServerToClientObjectId(oidBodyBag);
                CSWCCreature* pClientCreature = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidCreature);
                if (pClientCreature != NULL)
                    pClientCreature->m_oidParentCorpse = oidClientBodyBag;
            }
            if (pEventData == NULL)
            {
                int nFadeDelay;
                int bFadeDelay = g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(m_cAppearanceInfo.m_nAppearance,
                    "FadeDelayOnDeath", &nFadeDelay);
                if (bFadeDelay != 0)
                {
                    SetFadeOutTime(nFadeDelay);
                    SetKeepCorpse(0);
                }
                else
                    SetKeepCorpse(1);
            }
            RemoveFromArea(0);
            if (GetArea() != NULL)
            {
                CExoArrayList<unsigned long>* pTraps = &GetArea()->m_lstTrapObjectIds;
                for (int i = 0; i < pTraps->num; ++i)
                {
                    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject((*pTraps)[i]);
                    if (pObject != NULL)
                    {
                        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                        {
                            if (pObject->AsSWSTrigger()->GetCreatureList()->Contains(GetId()))
                                pObject->AsSWSTrigger()->GetCreatureList()->Remove(GetId());
                        }
                        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                        {
                            if (pObject->AsSWSDoor()->GetCreatureList()->Contains(GetId()))
                                pObject->AsSWSDoor()->GetCreatureList()->Remove(GetId());
                        }
                        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                        {
                            if (pObject->AsSWSPlaceable()->GetCreatureList()->Contains(GetId()))
                                pObject->AsSWSPlaceable()->GetCreatureList()->Remove(GetId());
                        }
                    }
                }
            }
            if (m_bEncounterCountRemoved == 0)
            {
                CSWSEncounter* pEncounter = g_pAppManager->GetServerExoApp()->GetEncounterByGameObjectID(m_oidSpawnedBy);
                if (pEncounter != NULL)
                {
                    pEncounter->RemoveFromActiveCreatureCount(m_pStats->GetChallengeRating(), 0);
                    m_bEncounterCountRemoved = 1;
                }
            }
            delete this;
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_FORCED_ACTION)
    {
        {
            CSWSForcedAction* pAction = static_cast<CSWSForcedAction*>(pEventData);
            unsigned short nGroupId = pAction->m_nGroupId;
            if (CancelAction(0, nGroupId) == 1 && pAction->m_nActionId == ACTION_MOVE_TO_POINT)
                ForceMoveToPoint(pAction);
            delete pAction;
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        {
            CScriptEvent* pEvent = static_cast<CScriptEvent*>(pEventData);
            switch (pEvent->m_nType)
            {
            case CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION:
                m_oidLastPerceived = pEvent->GetObjectID(0);
                if (pEvent->GetInteger(0) == CScriptEvent::PERCEPTION_EVENT_LOST_HEARING)
                {
                    m_bLastPerceptionInaudible = 1;
                    m_bLastPerceptionSeen = 0;
                    m_bLastPerceptionHeard = 0;
                    m_bLastPerceptionVanished = 0;
                }
                else if (pEvent->GetInteger(0) == CScriptEvent::PERCEPTION_EVENT_GAINED_HEARING)
                {
                    m_bLastPerceptionHeard = 1;
                    m_bLastPerceptionInaudible = 0;
                    m_bLastPerceptionSeen = 0;
                    m_bLastPerceptionVanished = 0;
                }
                else if (pEvent->GetInteger(0) == CScriptEvent::PERCEPTION_EVENT_LOST_SIGHT)
                {
                    m_bLastPerceptionHeard = 0;
                    m_bLastPerceptionInaudible = 0;
                    m_bLastPerceptionSeen = 0;
                    m_bLastPerceptionVanished = 1;
                }
                else if (pEvent->GetInteger(0) == CScriptEvent::PERCEPTION_EVENT_GAINED_SIGHT)
                {
                    m_bLastPerceptionInaudible = 0;
                    m_bLastPerceptionSeen = 1;
                    m_bLastPerceptionHeard = 0;
                    m_bLastPerceptionVanished = 0;
                }
                if (m_bInParty != 0)
                {
                    unsigned long oidClient = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId());
                    if (g_pAppManager->GetClientExoApp()->GetSWParty()->IsPartyMember(oidClient))
                        g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_NOTICE], GetId(), 1);
                }
                else
                    g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_NOTICE], GetId(), 1);
                break;
            case CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED:
                if (g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId()) == NULL)
                {
                    m_oidBlockingDoor = oidCaller;
                    g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_BLOCKED], GetId(), 1);
                }
                break;
            case CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE:
                m_oidLastSpeaker = oidCaller;
                m_nMatchedStringsNum = pEvent->GetInteger(1);
                m_sLastListenText = pEvent->GetString(0);
                g_pAppManager->GetServerExoApp()->SetLastConversation(pEvent->GetString(0));
                m_nConversationType = pEvent->GetInteger(2);
                g_pAppManager->GetServerExoApp()->SetActionConversation(1);
                g_pAppManager->GetServerExoApp()->SetIgnoreConversationStartRange(pEvent->GetInteger(3));
                ClearMatchedExpressionStrings();
                for (int i = 1; i < pEvent->GetInteger(0); ++i)
                    AddMatchedExpressionString(pEvent->GetString(i));
                if (m_sScripts[SCRIPT_DIALOGUE].IsEmpty() || m_sScripts[SCRIPT_DIALOGUE] == "default")
                    m_sScripts[SCRIPT_DIALOGUE] = "k_hen_dialogue01";
                m_sScripts[SCRIPT_DIALOGUE].CStr();
                g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_DIALOGUE], GetId(), 1);
                break;
            case CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT:
                m_nCustomScriptEventId = pEvent->GetInteger(0);
                g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_USER_DEFINED], GetId(), 1);
                break;
            case CSWSSCRIPTEVENT_EVENTTYPE_ON_SPELLCASTAT:
                {
                    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                    unsigned long oidSource = pEvent->GetObjectID(0);
                    CGameObject* pSource = pServer->GetGameObject(oidSource);
                    if (pSource != NULL && pSource->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                        m_n1D8 = pEvent->GetObjectID(0);
                    else if (pSource != NULL && pSource->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
                    {
                        CSWSAreaOfEffectObject* pAreaEffect = pSource->AsSWSAreaOfEffectObject();
                        CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(pAreaEffect->GetCreator());
                        if (pCreator != NULL)
                            m_n1D8 = pCreator->GetId();
                        else
                            m_n1D8 = OBJECT_INVALID;
                    }
                    else
                        m_n1D8 = OBJECT_INVALID;
                    m_n1DC = pEvent->GetInteger(0);
                    m_n1E0 = pEvent->GetInteger(1);
                    if (m_n1E0 == 1)
                        SetLastHostileActor(m_n1D8, 0);
                    if (GetDead() == 0 && GetDeadTemp() == 0)
                        g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_SPELL_AT], GetId(), 1);
                    unsigned long oidCaster = m_n1D8;
                    CSWSCreature* pCaster = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaster);
                    if (pCaster != NULL)
                    {
                        if (m_n1E0 != 0)
                        {
                            pCaster->BroadcastCombatStateToParty(GetId());
                            pCaster->ResolveInitiative();
                            pCaster->SetExcitedState(2);
                            pCaster->RemoveCombatInvisibilityEffects();
                            BroadcastCombatStateToParty(pCaster->GetId());
                            ResolveInitiative();
                            SetExcitedState(1);
                            oidCaster = m_n1D8;
                            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AdjustTargetAndWitnessReputations(GetId(), oidCaster, 0);
                        }
                        else
                        {
                            oidCaster = m_n1D8;
                            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AdjustTargetAndWitnessReputations(GetId(), oidCaster, 3);
                        }
                    }
                }
                break;
            case CSWSSCRIPTEVENT_EVENTTYPE_ON_INVENTORY_DISTURBED:
                m_oidLastDisturbed = oidCaller;
                SetLastHostileActor(oidCaller, 0);
                m_n178 = pEvent->GetInteger(0);
                m_oidInventoryDisturbItem = pEvent->GetObjectID(0);
                g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_DISTURBED], GetId(), 1);
                break;
            }
            delete pEvent;
        }
        return;
    }
    else if (nEventId == CServerAIMaster::EVENT_CONTROLLER_RUMBLE)
    {
        g_pAppManager->GetClientExoApp()->PlayRumblePattern((int)pEventData);
        return;
    }
}
// PC 0x004FFAD0  Xbox 0x00183740
float CSWSCreature::MaxCleaveRange(unsigned long oidTarget, int nAttackType,
                                   int bOffHand)
{
    return MaxAttackRange(oidTarget, nAttackType, bOffHand) + 2.0f;
}
// PC 0x004FB0F0  Xbox 0x0017EFF0
float CSWSCreature::MaxAttackRange(unsigned long oidTarget, int nAttackType,
                                   int bOffHand)
{
    CSWSItem* pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);

    if (pItem != NULL)
    {
        int bRanged = pItem->GetBaseItem()->m_nRangedWeapon > 0;
        if (bRanged > 0)
        {
            if (bOffHand)
            {
                return 20.0f;
            }

            float fRange = pItem->GetBaseItem()->m_fMaxAttackRange;
            if (fRange == 0.0f)
            {
                return 30.0f;
            }
            return fRange;
        }
    }

    return DesiredAttackRange(oidTarget, nAttackType) + 0.5f;
}
// PC 0x004F1310  Xbox 0x00174C90
float CSWSCreature::DesiredAttackRange(unsigned long oidTarget, int nAttackType)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    float fRange = 0.0f;
    CGameObject* pObject = pServer->GetGameObject(oidTarget);

    if (pObject != NULL)
    {
        if (pObject->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (nAttackType != 0)
            {
                float fTargetPersonalSpace = pCreature->m_pPathfindInformation->m_fStepScaleY;
                fRange = fTargetPersonalSpace + m_pPathfindInformation->m_fStepScaleY + 0.7f;
            }
            else
            {
                fRange = 1.6f;
                float fHitRadius = 0.0f;
                g_pRules->m_p2DArrays->m_pAppearance->GetFLOATEntry(
                    m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_HITRADIUS,
                    &fHitRadius);
                fRange += fHitRadius;
                g_pRules->m_p2DArrays->m_pAppearance->GetFLOATEntry(
                    pCreature->m_cAppearanceInfo.m_nAppearance,
                    CTwoDimArrays::COL2DA_APPEARANCETABLE_HITRADIUS, &fHitRadius);
                fRange += fHitRadius;
            }
        }
        else
        {
            if (pObject->AsSWSDoor() != NULL)
            {
                Vector vPosition = pObject->AsSWSDoor()->m_vPosition;
                GetUseRange(oidTarget, vPosition, fRange, 0);
            }
            else if (pObject->AsSWSPlaceable() != NULL)
            {
                Vector vPosition = pObject->AsSWSPlaceable()->m_vPosition;
                GetUseRange(oidTarget, vPosition, fRange, 0);
            }
            else
            {
                fRange = 1.5f;
            }
        }
    }
    return fRange;
}
// PC 0x004F1460  Xbox 0x00174E80
void CSWSCreature::ComputeAIState()
{
    CExoLinkedListNode* pPos;
    CSWSObjectActionNode* pAction;
    unsigned long oidTarget;

    m_nAIState = 0;
    m_oidAITarget = OBJECT_INVALID;
    pPos = m_lstActions.GetHeadPos();
    while (pPos != NULL)
    {
        pAction = m_lstActions.GetAtPos(pPos);
        oidTarget = OBJECT_INVALID;
        if (pAction->m_nActionType == 12)
        {
            oidTarget = (unsigned long)pAction->m_apParam[0];
        }
        ComputeAIStateOnAction(pAction->m_nActionType, oidTarget);
        m_lstActions.GetNext(pPos);
    }
    if (m_nCurrentAction != -1)
    {
        ComputeAIStateOnAction(m_nCurrentAction, m_nUnknown84);
    }
}
// PC 0x004EBB70  Xbox 0x0016F160
void CSWSCreature::ComputeAIStateOnAction(int nAction, unsigned long oidTarget)
{
    if (nAction == ACTION_EQUIP_ITEM)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 12;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_UNEQUIP_ITEM)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 13;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_PICK_UP_ITEM)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 10;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_DROP_ITEM)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 11;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_CAST_SPELL || nAction == ACTION_ITEM_CAST_SPELL)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 14;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_OPEN_DOOR)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 15;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_CLOSE_DOOR)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 16;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_DISARM_MINE)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 17;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_RECOVER_MINE)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 18;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_FLAG_MINE)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 19;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_EXAMINE_MINE)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 20;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_SET_MINE)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 21;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_GIVE_ITEM)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 22;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_TAKE_ITEM)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 23;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_UNLOCK_OBJECT)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 24;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_LOCK_OBJECT)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 25;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_USE_OBJECT)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 26;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_ANIMAL_EMPATHY)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 27;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_REST)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 28;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_PICK_POCKET)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 29;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
    else if (nAction == ACTION_HEAL)
    {
        if (m_nAIState < 10)
        {
            m_nAIState = 30;
            m_oidAITarget = OBJECT_INVALID;
        }
    }

    if (nAction == ACTION_PHYSICAL_ATTACKS)
    {
        if (m_nAIState == 0 || m_nAIState == 1)
        {
            m_nAIState = 2;
            m_oidAITarget = oidTarget;
        }
    }
    if ((nAction == ACTION_MOVE_TO_POINT || nAction == ACTION_DRIVE) && m_nAIState == 0)
    {
        m_nAIState = 1;
        m_oidAITarget = OBJECT_INVALID;
    }
    if (nAction == ACTION_COUNTER_SPELL)
    {
        if (m_nAIState == 0 || m_nAIState == 1)
        {
            m_nAIState = 2;
            m_oidAITarget = OBJECT_INVALID;
        }
    }
}
// PC 0x004EBF20  Xbox 0x0016F510
void CSWSCreature::ReadScriptsFromGff(CResGFF* pRes, CResStruct* pStruct)
{
    CExoString sScript;
    int bSuccess;
    {
        CResRef cDefault(m_sScripts[0]);
        pRes->ReadFieldCResRef(pStruct, "ScriptHeartbeat", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[0] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[1]);
        pRes->ReadFieldCResRef(pStruct, "ScriptOnNotice", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[1] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[2]);
        pRes->ReadFieldCResRef(pStruct, "ScriptSpellAt", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[2] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[3]);
        pRes->ReadFieldCResRef(pStruct, "ScriptAttacked", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[3] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[4]);
        pRes->ReadFieldCResRef(pStruct, "ScriptDamaged", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[4] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[5]);
        pRes->ReadFieldCResRef(pStruct, "ScriptDisturbed", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[5] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[6]);
        pRes->ReadFieldCResRef(pStruct, "ScriptEndRound", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[6] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[7]);
        pRes->ReadFieldCResRef(pStruct, "ScriptDialogue", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[7] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[8]);
        pRes->ReadFieldCResRef(pStruct, "ScriptSpawn", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[8] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[9]);
        pRes->ReadFieldCResRef(pStruct, "ScriptRested", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[9] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[10]);
        pRes->ReadFieldCResRef(pStruct, "ScriptDeath", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[10] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[11]);
        pRes->ReadFieldCResRef(pStruct, "ScriptUserDefine", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[11] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[12]);
        pRes->ReadFieldCResRef(pStruct, "ScriptOnBlocked", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[12] = sScript;
    }
    {
        CResRef cDefault(m_sScripts[13]);
        pRes->ReadFieldCResRef(pStruct, "ScriptEndDialogue", bSuccess, cDefault).CopyToString(sScript);
        m_sScripts[13] = sScript;
    }
}
// PC 0x004F14F0  Xbox 0x00174F10
void CSWSCreature::UpdateEffectPtrs()
{
    int nPreviousType = CSWSEffectListHandler::MAX_EFFECTS;

    for (int nEffect = 0; nEffect < m_lstEffects.num; nEffect++)
    {
        int nType = m_lstEffects[nEffect]->m_nType;

        if (nType != nPreviousType)
        {
            if (nType == CSWSEffectListHandler::EFFECT_AC_INCREASE)
            {
                m_pStats->m_nACEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE)
            {
                m_pStats->m_nAttackEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION)
            {
                m_pStats->m_nDamageReductionEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE)
            {
                m_pStats->m_nDamageResistanceEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
            {
                m_pStats->m_nDamageIncreaseEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SET_AI_STATE)
            {
                m_pStats->m_nSetAIStateEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_EFFECT_ICON)
            {
                m_pStats->m_nEffectIconEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_INVISIBILITY)
            {
                m_pStats->m_nInvisibilityEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SANCTUARY)
            {
                m_pStats->m_nSanctuaryEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_ABILITY_INCREASE)
            {
                m_pStats->m_nAbilityEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE)
            {
                m_pStats->m_nForceResistanceEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SEEINVISIBLE)
            {
                m_pStats->m_nSeeInvisibleEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_ULTRAVISION)
            {
                m_pStats->m_nTrueSeeingEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_MISS_CHANCE)
            {
                m_pStats->m_nMissChanceEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_CONCEALMENT)
            {
                m_pStats->m_nConcealmentEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_BLINDNESS)
            {
                m_pStats->m_nBlindnessEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SETWALKANIMATION)
            {
                m_pStats->m_nSpecialWalkEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION)
            {
                m_pStats->m_nSpellLevelAbsorptionEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SPELL_IMMUNITY)
            {
                m_pStats->m_nSpellImmunityEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_SHIELD)
            {
                m_pStats->m_nDamageShieldEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SKILL_INCREASE)
            {
                m_pStats->m_nSkillDecreaseEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE)
            {
                m_pStats->m_nSavingThrowEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_HASTE)
            {
                m_pStats->m_nHasteEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_IMMUNITY)
            {
                m_pStats->m_nImmunityEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED)
            {
                m_pStats->m_nLimitMovementSpeedEffectIndex = nEffect;
            }
            else if (nType == CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE)
            {
                m_pStats->m_nMovementSpeedDecreaseEffectIndex = nEffect;
            }
        }
        nPreviousType = nType;
    }
}
// PC 0x004FB180  Xbox 0x0017F090
int CSWSCreature::LoadFollowInfo(CResGFF* pResGFF, CResStruct* pResStruct)
{
    CResStruct cFollowInfo;
    int nResult = pResGFF->GetStructFromStruct(&cFollowInfo, pResStruct,
                                                "FollowInfo");

    if (nResult != 0)
    {
        nResult = (int)m_pFollowInfo;
        if (nResult == NULL)
        {
            m_pFollowInfo = new CSWSCreaturePartyFollowInfo;
            return m_pFollowInfo->Load(pResGFF, &cFollowInfo);
        }
    }

    return nResult;
}
// PC 0x004FB1E0  Xbox 0x0017F130
void CSWSCreature::ApplyDeathExperience()
{
    CSWPartyTable* pParty = g_pAppManager->GetServerExoApp()->GetPartyTable();
    if (GetIsPC() || (pParty != NULL && pParty->GetIsMember(GetId())))
        return;
    if (g_pAppManager->GetServerExoApp()->GetFactionManager()->
            GetNPCFactionReputation(GetFactionId(), 0) > 10)
        return;

    int nPercentXP = 0;
    float fExperience = CalculateDeathExperience();
#ifdef _XBOX
    CTwoDimArrays* pArraysForNPC = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pArraysForNPC->m_pNPC);
    C2DA* pNPCTable = pArraysForNPC->m_pNPC;
#else
    C2DA* pNPCTable = g_pRules->Get2DArrays()->m_pNPC;
#endif
    pNPCTable->GetINTEntry(10, "PercentXP", &nPercentXP);
    if (nPercentXP > 0)
    {
        int nMembers = pParty->m_nNumMembers;
        double fBonus = 1.0f + (nPercentXP / 100.0f) * nMembers;
        fExperience *= fBonus;
    }
    int nExperience = (int)ceil(fExperience);
    pParty->DistributeExperience(nExperience, FALSE);

    CGameObject* pKiller;
    CSWSCreature* pCreature = NULL;
    unsigned long oidKiller = m_n154;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->
            GetGameObject(oidKiller, &pKiller) == CGameObjectArray::SUCCESS)
    {
        if (pKiller->AsSWSTrigger() != NULL &&
            pKiller->AsSWSTrigger()->m_oidCreator != OBJECT_INVALID)
        {
            unsigned long oidCreator = pKiller->AsSWSTrigger()->m_oidCreator;
            pKiller = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidCreator);
        }
        if (pKiller != NULL)
            pCreature = pKiller->AsSWSCreature();
    }
    if (pCreature == NULL)
    {
        pCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreature();
        if (pCreature != NULL)
        {
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, GetId());
            pMessageData->SetObjectID(1, pCreature->GetId());
            pMessageData->SetInteger(0, (int)fExperience);
            pCreature->BroadcastDeathDataToParty(pMessageData);
            delete pMessageData;
        }
    }
    else
    {
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        pMessageData->SetObjectID(0, GetId());
        pMessageData->SetObjectID(1, pCreature->GetId());
        pMessageData->SetInteger(0, (int)fExperience);
        pCreature->BroadcastDeathDataToParty(pMessageData);
        delete pMessageData;
    }
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    pClient->AddFloatyText(pClient->ServerToClientObjectId(GetId()),
                          CClientExoApp::FLOATY_TEXT_XP, nExperience);
}
// PC 0x004F17D0  Xbox 0x001754A0
void CSWSCreature::LoadJournal(CResGFF* pRes, CResStruct* pStruct)
{
    CSWSJournal* pJournal = GetJournal();
    if (pJournal == NULL || pJournal->m_lstEntries.num != 0)
        return;

    int bSuccess;
    CResList lstEntries;
    if (pRes->GetList(&lstEntries, pStruct, "JNL_Entries") == 0)
        return;

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
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetPartyTable()->GetPartyJournal()->SetState(sPlotID, nState, 1);
        unsigned long nDate =
            pRes->ReadFieldDWORD(&stEntry, "JNL_Date", bSuccess, 0);
        pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetPartyTable()->GetPartyJournal()->SetDate(sPlotID, nDate, 1);
        unsigned long nTime =
            pRes->ReadFieldDWORD(&stEntry, "JNL_Time", bSuccess, 0);
        pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetPartyTable()->GetPartyJournal()->SetTime(sPlotID, nTime, 1);
    }
}
// PC 0x004F19E0  Xbox 0x001756B0
float CSWSCreature::CalculateDeathExperience()
{
    CSWSPlayer* pPlayer =
        (CSWSPlayer*)g_pAppManager->GetServerExoApp()->GetPlayerList()->GetHead();
    unsigned long oidPC = pPlayer->m_oidPCObject;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            oidPC);
    if (pCreature != NULL)
    {
        unsigned long nExperience = pCreature->GetStats()->m_nExperience;
        int nLevel;
        for (int nIndex = CSWRules::EXPERIENCE_TABLE_LAST_INDEX; nIndex >= 0; --nIndex)
        {
            if (nExperience >= (unsigned long)g_pRules->GetExperienceForLevel((unsigned char)nIndex))
            {
                nLevel = nIndex + 1;
                break;
            }
        }

        int nChallengeRating = (int)m_pStats->m_fChallengeRating;
        float fExperience = 0.0f;
#ifdef _XBOX
        CTwoDimArrays* pArraysForXpBase = g_pRules->Get2DArrays();
        EXOXBOXASSERT(pArraysForXpBase->m_pXpBase);
        pArraysForXpBase->m_pXpBase->GetFLOATEntry(
#else
        g_pRules->Get2DArrays()->m_pXpBase->GetFLOATEntry(
#endif
            nLevel - 1, nChallengeRating + 1, &fExperience);
        float fPercentXP = 0.0f;
        {
            CExoString sColumn("PercentXP");
#ifdef _XBOX
            CTwoDimArrays* pArraysForNPC = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pArraysForNPC->m_pNPC);
            C2DA* pNPCTable = pArraysForNPC->m_pNPC;
#else
            C2DA* pNPCTable = g_pRules->Get2DArrays()->m_pNPC;
#endif
            pNPCTable->GetFLOATEntry(
                CSWPartyTable::PARTY_XP_ROW, sColumn, &fPercentXP);
        }
        return fExperience * (fPercentXP * 0.01f);
    }
    return 0.0f;
}
// PC 0x004F1B20  Xbox 0x00175810
float CSWSCreature::GetWalkRate()
{
    float fWalkRate;
    float fFactor;

    fWalkRate = m_pStats->m_fWalkRate;
    fFactor = GetMovementRateFactor();
    fWalkRate = fWalkRate * fFactor * 1000.0f;
    if (fWalkRate <= 100.0f)
    {
        return 0.0f;
    }

    unsigned long oidServer = m_nId;
    unsigned long oidClient = g_pAppManager->m_pClientExoApp->ServerToClientObjectId(oidServer);
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetCreatureByGameObjectID(oidClient);
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    if ((m_nActivity & ACTIVITY_STEALTH) != 0 && pCreature != NULL)
    {
        return pCreature->m_pAppearance->m_fDriveAnimWalk * 1000.0f;
    }
    return fWalkRate;
}
// PC 0x004F1BE0  Xbox 0x001758D0
float CSWSCreature::GetRunRate()
{
    float fRunRate;
    float fFactor;

    fRunRate = m_pStats->m_fRunRate;
    fFactor = GetMovementRateFactor();
    fRunRate = fRunRate * fFactor * 1000.0f;
    return max(1000.0f, fRunRate);
}
#ifndef _XBOX
float CSWSCreature::GetMovementRateFactor()  // PC 0x004EC370  Xbox 0x0016F960
{
    float fMovementRateFactor;

    fMovementRateFactor = m_fMovementRateFactor;
    if (fMovementRateFactor < 0.125f)
    {
        return 0.125f;
    }

    if (fMovementRateFactor > 1.5f)
    {
        return 1.5f;
    }

    return fMovementRateFactor;
}
// PC 0x004EC3B0  Xbox 0x0016F9A0
void CSWSCreature::SetMovementRateFactor(float fMovementRateFactor)
{
    if (fMovementRateFactor < 0.125f)
    {
        m_fMovementRateFactor = 0.125f;
        return;
    }

    if (fMovementRateFactor > 1.5f)
    {
        m_fMovementRateFactor = 1.5f;
        return;
    }

    m_fMovementRateFactor = fMovementRateFactor;
}
#endif
void CSWSCreature::BroadcastDamageDataToParty(CSWCCMessageData* pMessageData,  // PC 0x004EC400  Xbox 0x0016FA50
                                            int bHealing)
{
    unsigned long* pMembers;
    int nMember;
    CSWSPlayer* pPlayer;
    CSWSObject* pGameObject;
    CServerExoApp* pServer;

    GetFaction()->GetMemberList(&pMembers);
    for (nMember = 0; nMember < GetFaction()->m_lstMembers.num; nMember++)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        pPlayer = pServer->GetClientObjectByObjectId(pMembers[nMember]);
        if (pPlayer != NULL)
        {
            pGameObject = pPlayer->GetGameObject();
            if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
            {
                Vector vDifference = pGameObject->m_vPosition - m_vPosition;
                if (vDifference.m_fX * vDifference.m_fX +
                    vDifference.m_fY * vDifference.m_fY +
                    vDifference.m_fZ * vDifference.m_fZ <=
                    DAMAGE_BROADCAST_RANGE_SQUARED)
                {
                    if (!bHealing)
                    {
                        g_pAppManager->m_pServerExoApp->GetSWSMessage()
                            ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                                CSWSMessage::CC_MESSAGE_DAMAGE, pMessageData, NULL);
                    }
                    else
                    {
                        g_pAppManager->m_pServerExoApp->GetSWSMessage()
                            ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                                CSWSMessage::CC_MESSAGE_DAMAGE_WITHOUT_ATTACKER, pMessageData, NULL);
                    }
                }
            }
        }
    }
}
// PC 0x004EC500  Xbox 0x0016FB70
void CSWSCreature::BroadcastDeathDataToParty(CSWCCMessageData* pMessageData)
{
    unsigned long* pMembers;
    int nMember;
    CSWSPlayer* pPlayer;
    CSWSObject* pGameObject;
    CServerExoApp* pServer;
    unsigned long nPlayerId;

    if (GetFaction() != NULL)
    {
        GetFaction()->GetMemberList(&pMembers);
        for (nMember = 0; nMember < GetFaction()->m_lstMembers.num; nMember++)
        {
            pServer = g_pAppManager->GetServerExoApp();
            pPlayer = pServer->GetClientObjectByObjectId(pMembers[nMember]);
            if (pPlayer != NULL)
            {
                pGameObject = pPlayer->GetGameObject();
                if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
                {
                    Vector vDifference = pPlayer->GetGameObject()->m_vPosition - m_vPosition;
                    if (vDifference.m_fX * vDifference.m_fX +
                        vDifference.m_fY * vDifference.m_fY +
                        vDifference.m_fZ * vDifference.m_fZ <=
                        NWSCREATURE_DEATH_BROADCAST_DISTANCE_SQUARED)
                    {
                        nPlayerId = pPlayer->m_nPlayerId;
                        g_pAppManager->GetServerExoApp()->GetSWSMessage()
                            ->SendServerToPlayerCCMessage(nPlayerId,
                                CSWSMessage::CC_MESSAGE_DEATH, pMessageData, NULL);
                    }
                }
            }
        }
    }
}
// PC 0x004EC610  Xbox 0x0016FC90
void CSWSCreature::BroadcastFloatyData(CSWCCMessageData* pMessageData)
{
    unsigned long* pMembers;
    int nMember;
    CSWSPlayer* pPlayer;
    CSWSObject* pGameObject;
    unsigned long nPlayerId;

    GetFaction()->GetMemberList(&pMembers);
    for (nMember = 0; nMember < GetFaction()->m_lstMembers.num; nMember++)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pPlayer = pServer->GetClientObjectByObjectId(pMembers[nMember]);
        if (pPlayer != NULL)
        {
            pGameObject = pPlayer->GetGameObject();
            if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
            {
                Vector vDifference = pPlayer->GetGameObject()->m_vPosition -
                                     m_vPosition;
                if (vDifference.m_fX * vDifference.m_fX +
                    vDifference.m_fY * vDifference.m_fY +
                    vDifference.m_fZ * vDifference.m_fZ <=
                    FLOATY_DATA_RANGE_SQUARED)
                {
                    nPlayerId = pPlayer->m_nPlayerId;
                    g_pAppManager->m_pServerExoApp->GetSWSMessage()
                        ->SendServerToPlayerCCMessage(nPlayerId,
                            CSWSMessage::CC_MESSAGE_FEEDBACK, pMessageData, NULL);
                }
            }
        }
    }
}
// PC 0x004EC700  Xbox 0x0016FDA0
void CSWSCreature::BroadcastSkillData(CSWCCMessageData* pMessageData)
{
    CSWSPlayer* pPlayer;
    unsigned long nPlayerId;

    pPlayer =
        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL)
    {
        nPlayerId = pPlayer->m_nPlayerId;
        g_pAppManager->m_pServerExoApp->GetSWSMessage()
            ->SendServerToPlayerCCMessage(nPlayerId, 9, pMessageData, NULL);
    }
    if (pMessageData != NULL)
    {
        delete pMessageData;
    }
}
// PC 0x004EC760  Xbox 0x0016FE00
void CSWSCreature::BroadcastSavingThrowData(CSWCCMessageData* pMessageData)
{
    CSWSPlayer* pPlayer;
    CServerExoApp* pServer;
    unsigned long nPlayerId;

    pPlayer =
        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        nPlayerId = pPlayer->m_nPlayerId;
        pServer->GetSWSMessage()
            ->SendServerToPlayerCCMessage(nPlayerId, 7, pMessageData, NULL);
    }
}
// PC 0x004EC7A0  Xbox 0x0016FE40
void CSWSCreature::BroadcastStealthDetectionData(CSWCCMessageData* pMessageData)
{
    unsigned long* pMembers;
    int nMember;
    CSWSPlayer* pPlayer;
    CSWSObject* pGameObject;
    CServerExoApp* pServer;
    unsigned long nPlayerId;

    GetFaction()->GetMemberList(&pMembers);
    for (nMember = 0; nMember < GetFaction()->m_lstMembers.num; nMember++)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        pPlayer = pServer->GetClientObjectByObjectId(pMembers[nMember]);
        if (pPlayer != NULL)
        {
            pGameObject = pPlayer->GetGameObject();
            if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
            {
                nPlayerId = pPlayer->m_nPlayerId;
                g_pAppManager->m_pServerExoApp->GetSWSMessage()
                    ->SendServerToPlayerCCMessage(nPlayerId, 16, pMessageData,
                                                  NULL);
            }
        }
    }
}
// PC 0x004EC840  Xbox 0x0016FEE0
void CSWSCreature::BroadcastMineDetectionData(CSWCCMessageData* pMessageData)
{
    unsigned long* pMembers;
    int nMember;
    CSWSPlayer* pPlayer;
    CSWSObject* pGameObject;
    CServerExoApp* pServer;
    unsigned long nPlayerId;

    GetFaction()->GetMemberList(&pMembers);
    for (nMember = 0; nMember < GetFaction()->m_lstMembers.num; nMember++)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        pPlayer = pServer->GetClientObjectByObjectId(pMembers[nMember]);
        if (pPlayer != NULL)
        {
            pGameObject = pPlayer->GetGameObject();
            if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
            {
                nPlayerId = pPlayer->m_nPlayerId;
                g_pAppManager->m_pServerExoApp->GetSWSMessage()
                    ->SendServerToPlayerCCMessage(nPlayerId, 19, pMessageData,
                                                  NULL);
            }
        }
    }
}
// PC 0x004EC8E0  Xbox 0x0016FF80
void CSWSCreature::BroadcastBattleCry()
{
    if (GetArea() != NULL)
    {
        unsigned long oidPlayerCreature = g_pAppManager->GetServerExoApp()
            ->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
            ->GetClientObjectByObjectId(oidPlayerCreature);
        if (pPlayer != NULL)
        {
            CSWSObject* pGameObject = pPlayer->GetGameObject();
            if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
            {
                Vector vDifference = pGameObject->m_vPosition - m_vPosition;
                if (vDifference.m_fX * vDifference.m_fX +
                    vDifference.m_fY * vDifference.m_fY +
                    vDifference.m_fZ * vDifference.m_fZ <=
                    BATTLE_CRY_RANGE_SQUARED)
                {
                    int nBattleCry = rand() % BATTLE_CRY_COUNT;
                    int nVoiceChat;
                    if (nBattleCry == 0)
                        nVoiceChat = BATTLE_CRY_VOICE_1;
                    else if (nBattleCry == 1)
                        nVoiceChat = BATTLE_CRY_VOICE_2;
                    else if (nBattleCry == 2)
                        nVoiceChat = BATTLE_CRY_VOICE_3;
                    else if (nBattleCry == 3)
                        nVoiceChat = BATTLE_CRY_VOICE_4;
                    else
                        nVoiceChat = BATTLE_CRY_VOICE_5;

                    unsigned long oidCreature = m_nId;
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()
                        ->SendServerToPlayerVoiceChat_Play(pPlayer,
                            oidCreature, nVoiceChat);
                }
            }
        }
    }
}
// PC 0x004EC9E0  Xbox 0x00170090
void CSWSCreature::BroadcastAttackSummaryToParty(CSWCCMessageData* pMessageData)
{
    CSWSFaction* pFaction = GetFaction();
    if (pFaction != NULL)
    {
        unsigned long* pMembers = NULL;
        int nMembers = pFaction->GetMemberList(&pMembers);
        for (int nMember = 0; nMember < nMembers; nMember++)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                ->GetClientObjectByObjectId(pMembers[nMember]);
            if (pPlayer != NULL)
            {
                CSWSObject* pGameObject = pPlayer->GetGameObject();
                if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
                {
                    Vector vDifference = pGameObject->m_vPosition - m_vPosition;
                    if (vDifference.m_fX * vDifference.m_fX +
                        vDifference.m_fY * vDifference.m_fY +
                        vDifference.m_fZ * vDifference.m_fZ <=
                        ATTACK_SUMMARY_RANGE_SQUARED)
                    {
                        unsigned long nPlayerId = pPlayer->m_nPlayerId;
                        g_pAppManager->GetServerExoApp()->GetSWSMessage()
                            ->SendServerToPlayerCCMessage(nPlayerId,
                                CSWSMessage::CC_MESSAGE_ATTACK_SUMMARY,
                                pMessageData, NULL);
                    }
                }
            }
        }
    }
}
// PC 0x004ECAE0  Xbox 0x001701A0
void CSWSCreature::BroadcastBreakdownMessage(unsigned char nMessageType,
                                            CSWCCMessageData* pMessageData)
{
    CSWSFaction* pFaction = GetFaction();
    if (pFaction != NULL)
    {
        unsigned long* pMembers = NULL;
        int nMemberCount = pFaction->GetMemberList(&pMembers);
        for (int nMember = 0; nMember < nMemberCount; nMember++)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                ->GetClientObjectByObjectId(pMembers[nMember]);
            if (pPlayer != NULL)
            {
                CSWSObject* pGameObject = pPlayer->GetGameObject();
                if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
                {
                    Vector vDifference = pGameObject->m_vPosition - m_vPosition;
                    if (vDifference.m_fX * vDifference.m_fX +
                        vDifference.m_fY * vDifference.m_fY +
                        vDifference.m_fZ * vDifference.m_fZ <=
                        BREAKDOWN_BROADCAST_RANGE_SQUARED)
                    {
                        unsigned long nPlayerId = pPlayer->m_nPlayerId;
                        g_pAppManager->GetServerExoApp()->GetSWSMessage()
                            ->SendServerToPlayerCCMessage(nPlayerId,
                                nMessageType, pMessageData, NULL);
                    }
                }
            }
        }
    }
}
// PC 0x004ECBE0  Xbox 0x001702B0
void CSWSCreature::BroadcastAttackBreakdown(unsigned long oidTarget,
                                            CSWCCMessageData* pMessageData)
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;

    BroadcastBreakdownMessage(20, pMessageData);

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(oidTarget);

    if (pCreature != NULL)
    {
        pCreature->BroadcastBreakdownMessage(20, pMessageData);
    }
}
// PC 0x004ECC20  Xbox 0x001702F0
void CSWSCreature::BroadcastThreatBreakdown(unsigned long oidTarget,
                                            CSWCCMessageData* pMessageData)
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;

    BroadcastBreakdownMessage(21, pMessageData);

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(oidTarget);

    if (pCreature != NULL)
    {
        pCreature->BroadcastBreakdownMessage(21, pMessageData);
    }
}
// PC 0x004ECC60  Xbox 0x00170330
void CSWSCreature::BroadcastDefenseBreakdown(unsigned long oidTarget,
                                             CSWCCMessageData* pMessageData)
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;

    BroadcastBreakdownMessage(22, pMessageData);

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(oidTarget);

    if (pCreature != NULL)
    {
        pCreature->BroadcastBreakdownMessage(22, pMessageData);
    }
}
// PC 0x004ECCA0  Xbox 0x00170370
void CSWSCreature::BroadcastDamageBreakdown(unsigned long oidTarget,
                                            CSWCCMessageData* pMessageData)
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;

    BroadcastBreakdownMessage(23, pMessageData);

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(oidTarget);

    if (pCreature != NULL)
    {
        pCreature->BroadcastBreakdownMessage(23, pMessageData);
    }
}
// PC 0x004ECCE0  Xbox 0x001703B0
void CSWSCreature::BroadcastEffectApplicationBreakdown(
    unsigned long oidTarget, CSWCCMessageData* pMessageData)
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;

    BroadcastBreakdownMessage(25, pMessageData);

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(oidTarget);

    if (pCreature != NULL)
    {
        pCreature->BroadcastBreakdownMessage(25, pMessageData);
    }
}
// PC 0x004ECD20  Xbox 0x001703F0
void CSWSCreature::BroadcastDeflectBreakdown(unsigned long oidTarget,
                                             CSWCCMessageData* pMessageData)
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;

    BroadcastBreakdownMessage(26, pMessageData);

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(oidTarget);

    if (pCreature != NULL)
    {
        pCreature->BroadcastBreakdownMessage(26, pMessageData);
    }
}
// PC 0x004ECD60  Xbox 0x00170430
void CSWSCreature::BroadcastPoisoned(CSWCCMessageData* pMessageData)
{
    CSWSFaction* pFaction = GetFaction();
    if (pFaction != NULL)
    {
        unsigned long* pMembers = NULL;
        int nMembers = pFaction->GetMemberList(&pMembers);
        for (int nMember = 0; nMember < nMembers; nMember++)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                ->GetClientObjectByObjectId(pMembers[nMember]);
            if (pPlayer != NULL)
            {
                CSWSObject* pGameObject = pPlayer->GetGameObject();
                if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
                {
                    Vector vDifference = pGameObject->m_vPosition - m_vPosition;
                    if (vDifference.m_fX * vDifference.m_fX +
                        vDifference.m_fY * vDifference.m_fY +
                        vDifference.m_fZ * vDifference.m_fZ <=
                        POISON_BROADCAST_RANGE_SQUARED)
                    {
                        unsigned long nPlayerId = pPlayer->m_nPlayerId;
                        g_pAppManager->GetServerExoApp()->GetSWSMessage()
                            ->SendServerToPlayerCCMessage(nPlayerId,
                                CSWSMessage::CC_MESSAGE_POISONED,
                                pMessageData, NULL);
                    }
                }
            }
        }
    }
}
// PC 0x004ECE60  Xbox 0x00170540
int CSWSCreature::ResolveInitiative()
{
    m_bResolveInitiative = m_bInParty == 0;
    return m_bResolveInitiative;
}
// PC 0x004ECE80  Xbox 0x00170560
void CSWSCreature::ComputeArmourClass(CSWSItem* pItem, int bEquipping,
                                      int bLoading)
{
    CSWSItem* pArmour;
    CSWBaseItem* pBaseItem;
    unsigned char nBaseAC;

    pArmour = m_pInventory->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
    nBaseAC = 0;
    if (pArmour != NULL)
    {
        pBaseItem = pArmour->GetBaseItem();
        if (pBaseItem != NULL)
        {
            nBaseAC = pBaseItem->m_nBaseAC;
        }
    }
    m_pStats->m_nACArmorBase = nBaseAC;
}
// PC 0x004ECEE0  Xbox 0x001705C0
int CSWSCreature::UseFeat(unsigned short nFeat, unsigned short nAction,
                         unsigned long oidTarget, unsigned long nTime)
{
    int bUsed = 0;
    if (m_pStats->HasFeat(nFeat))
    {
        int nSuccessor = nFeat;
        unsigned short nSelectedFeat;
        do
        {
            nSelectedFeat = nSuccessor;
            nSuccessor = g_pRules->GetFeat(nSuccessor)->m_nSuccessor;
        }
        while (nSuccessor > 0 && m_pStats->HasFeat(nSuccessor));

        switch (nSelectedFeat)
        {
        case CSWFeat::FEAT_CRITICAL_STRIKE:
        case CSWFeat::FEAT_FLURRY:
        case CSWFeat::FEAT_IMPROVED_POWER_ATTACK:
        case CSWFeat::FEAT_IMPROVED_POWER_BLAST:
        case CSWFeat::FEAT_IMPROVED_CRITICAL_STRIKE:
        case CSWFeat::FEAT_IMPROVED_SNIPER_SHOT:
        case CSWFeat::FEAT_MULTI_SHOT:
        case CSWFeat::FEAT_POWER_ATTACK:
        case CSWFeat::FEAT_POWER_BLAST:
        case CSWFeat::FEAT_RAPID_SHOT:
        case CSWFeat::FEAT_SNIPER_SHOT:
        case CSWFeat::FEAT_WHIRLWIND_ATTACK:
        case CSWFeat::FEAT_MASTER_SNIPER_SHOT:
        case CSWFeat::FEAT_MASTER_CRITICAL_STRIKE:
        case CSWFeat::FEAT_MASTER_POWER_BLAST:
        case CSWFeat::FEAT_MASTER_POWER_ATTACK:
        case CSWFeat::FEAT_IMPROVED_FLURRY:
        case CSWFeat::FEAT_IMPROVED_RAPID_SHOT:
            m_pcCombatRound->AddAttackAction(oidTarget, 0, nSelectedFeat,
                                             CSWSCombatRound::FEAT_ATTACK_TIME, 1);
            bUsed = 1;
            break;
        }
        switch (nSelectedFeat)
        {
        case CSWFeat::FEAT_ADVANCED_GUARD_STANCE:
        case CSWFeat::FEAT_MASTER_GUARD_STANCE:
        case CSWFeat::FEAT_GUARD_STANCE:
            if (m_nGuardStanceFeat != 0)
                m_nGuardStanceFeat = 0;
            else
                m_nGuardStanceFeat = nSelectedFeat;
            break;
        }
    }
    return bUsed;
}
// PC 0x004ECFF0  Xbox 0x001706E0
int CSWSCreature::AddAppearActions()
{
    if (m_bCommandable == 0)
    {
        return 0;
    }
    ((CSWSObject*)this)
        ->AddAction(ACTION_APPEAR, ACTION_GROUP_NEW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    ((CSWSObject*)this)->SetGroupInterruptable(m_nLastGroupId, 0);
    return 1;
}
// PC 0x004ED060  Xbox 0x00170750
int CSWSCreature::AddDisappearActions(int bRemoveFromArea)
{
    if (m_bCommandable == 0)
        return 0;
    AddAction(ACTION_DISAPPEAR, ACTION_GROUP_NEW,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    SetGroupInterruptable(m_nLastGroupId, 0);
    if (!bRemoveFromArea)
    {
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, DISAPPEAR_ANIMATION_DURATION, GetId(), GetId(),
            CServerAIMaster::EVENT_DESTROY_OBJECT, (void*)bRemoveFromArea);
    }
    else
    {
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, DISAPPEAR_ANIMATION_DURATION, GetId(), GetId(),
            CServerAIMaster::EVENT_REMOVE_FROM_AREA, (void*)bRemoveFromArea);
    }
    return 1;
}
// PC 0x004ED110  Xbox 0x00170800
void CSWSCreature::UpdateExcitedStateTimer()
{
    int nTimer = m_nExcitedStateTimer;

    if (nTimer != 0)
    {
        unsigned long nElapsed = m_nElapsedTime;
        if (nTimer <= nElapsed)
        {
            CSWSPlayer* pPlayer;

            m_nExcitedStateTimer = 0;
            pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_nId);
            if (GetArea() != NULL && pPlayer != NULL &&
                GetArea()->m_pAmbientSound != NULL)
            {
                CSWSArea* pArea = GetArea();
                pArea->m_pAmbientSound->PlayBattleMusic(0);
            }
        }
        else
        {
            nTimer -= nElapsed;
            m_nExcitedStateTimer = nTimer;
        }
    }}

inline void CSWActionNode::Clear()
{
    m_nActionId = 0xFFFF;
    m_nGroupActionId = 0xFFFF;
    m_nActionType = 0xFFFF;
    m_vLocation = Vector();
    m_oidTarget = OBJECT_INVALID;
    m_nParameter = -1;
}
// PC 0x004ED190  Xbox 0x00170880
int CSWSCreature::GetRelativeWeaponSize(CSWSItem* pItem)
{
    int nBaseItem = pItem->m_nBaseItem;
    int nCreatureWeaponSize = m_nCreatureSize;
    int nRelativeWeaponSize =
        g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponSize -
        nCreatureWeaponSize;

    if (nRelativeWeaponSize < -2 || nRelativeWeaponSize > 1)
    {
        return -10;
    }

    return nRelativeWeaponSize;
}
// PC 0x004ED1D0  Xbox 0x001708C0
short CSWSCreature::GetArmorClass()
{
    short nArmorClass = SW_BASE_ARMOR_CLASS;
    for (int nClass = 0; nClass < m_pStats->m_nClassCount; ++nClass)
    {
        unsigned char nClassType = m_pStats->GetClass(nClass);
        CSWClass* pClass = &g_pRules->m_pClasses[nClassType];
        CSWSCreatureStats* pStats = m_pStats;
        nArmorClass += (short)pClass->GetArmorBonus(pStats->GetClassLevel(nClass, 0));
    }

    CSWSCreatureStats* pStats = m_pStats;
    nArmorClass += (short)(char)(pStats->m_nACArmorBonus - pStats->m_nACArmorPenalty + pStats->m_nACArmorBase)
                + (short)(char)(pStats->m_nACShieldBonus - pStats->m_nACShieldPenalty + pStats->m_nACShieldBase)
                + (short)(char)(pStats->m_nACDeflectionBonus - pStats->m_nACDeflectionPenalty);
    int nDodgeBonus = min(SW_MAX_AC_DODGE_BONUS,
                          pStats->m_nACDodgeBonus - pStats->m_nACDodgePenalty);
    nArmorClass += (short)(char)(pStats->GetACNaturalBase() + pStats->m_nACNaturalBonus - pStats->m_nACNaturalPenalty)
                + (short)(char)nDodgeBonus;
    nArmorClass += (short)m_pStats->GetDEXMod(1);
    nArmorClass += (short)m_pStats->GetTotalACFeatMod();
    if (IsDebilitated())
    {
        nArmorClass -= SW_DEBILITATED_AC_PENALTY;
    }
    return nArmorClass;
}
// PC 0x004ED310  Xbox 0x00170A00
short CSWSCreature::GetMaxHitPoints(int bIncludeFeats)
{
    int nBonus = 0;
    if (GetStats()->m_bIsPC == 1 &&
        g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nControlledNPC == -1)
    {
        for (int nLevel = 0; nLevel < GetStats()->GetLevel(0); ++nLevel)
        {
            CSWLevelStats* pLevelStats = GetStats()->GetLevelStats(nLevel);
            int nHitPoints = GetStats()->GetCONMod() +
                             pLevelStats->m_nHitDie;
            nHitPoints = max(1, nHitPoints);
            if (bIncludeFeats)
            {
                if (GetStats()->HasFeat(CSWFeat::FEAT_MASTER_TOUGHNESS))
                    nHitPoints += 2;
                else if (GetStats()->HasFeat(CSWFeat::FEAT_TOUGHNESS))
                    ++nHitPoints;
            }
            nBonus += nHitPoints;
        }
        return nBonus;
    }
    else
    {
        if (bIncludeFeats)
        {
            if (GetStats()->HasFeat(CSWFeat::FEAT_MASTER_TOUGHNESS))
                nBonus = 2;
            else if (GetStats()->HasFeat(CSWFeat::FEAT_TOUGHNESS))
                nBonus = 1;
        }
        if (GetStats()->HasFeat(CSWFeat::FEAT_WOOKIE_ENDURANCE))
            nBonus += 2;
        if (CSWSObject::GetMaxHitPoints(0) +
            GetStats()->GetCONMod() * GetStats()->GetLevel(0) <
            GetStats()->GetLevel(0))
            return GetStats()->GetLevel(0) * nBonus + GetStats()->GetLevel(0);
        return CSWSObject::GetMaxHitPoints(0) +
            (GetStats()->GetCONMod() + nBonus) * GetStats()->GetLevel(0);
    }
}
// PC 0x004ED4C0  Xbox 0x00170D50
void CSWSCreature::SetPortraitId(unsigned short nPortraitId)
{
    CExoString sPortraitPrefix("po_");

    if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
    {
        CResRef cPortrait("");
        SetPortrait(cPortrait);
    }
    else
    {
        CExoString sBaseResRef;
        {
            CExoString sColumn("BaseResRef");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPortraits = p2DArrays->m_pPortrait;
            pPortraits->GetCExoStringEntry(nPortraitId, sColumn, &sBaseResRef);
        }
        sBaseResRef = sBaseResRef;
        {
            CResRef cPortrait((char*)sBaseResRef.CStr());
            SetPortrait(cPortrait);
        }
    }

    m_nPortraitId = nPortraitId;
}
// PC 0x004ED5E0  Xbox 0x00170E70
unsigned long CSWSCreature::GetDamageFlags()
{
    CSWSCombatAttackData* pAttack;
    CSWSItem* pWeapon;
    int nAttackType;

    pAttack = m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
    nAttackType = pAttack->m_nWeaponAttackType;
    pWeapon = m_pcCombatRound->GetCurrentAttackWeapon(nAttackType);

    if (pWeapon != NULL)
    {
        return pWeapon->GetDamageFlags();
    }

    return CSWSEffectListHandler::DAMAGE_FLAG_BLUDGEONING;
}
// PC 0x004ED620  Xbox 0x00170EB0
void CSWSCreature::UpdateAppearanceForEquippedItems()
{
    CSWSItem* pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    unsigned long oidLeftHand = pItem != NULL ? pItem->m_nId :
        CGameObjectArray::INVALIDOBJID;

    pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    unsigned long oidRightHand = pItem != NULL ? pItem->m_nId :
        CGameObjectArray::INVALIDOBJID;

    pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
    unsigned long oidArmour = pItem != NULL ? pItem->m_nId :
        CGameObjectArray::INVALIDOBJID;

    pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_HEAD);
    unsigned long oidHead = pItem != NULL ? pItem->m_nId :
        CGameObjectArray::INVALIDOBJID;

    if (oidLeftHand != m_cAppearanceInfo.m_oidAppearanceLeftHand ||
        oidRightHand != m_cAppearanceInfo.m_oidAppearanceRightHand ||
        oidArmour != m_cAppearanceInfo.m_oidAppearanceArmour ||
        oidHead != m_cAppearanceInfo.m_oidAppearanceHead)
    {
        m_cAppearanceInfo.m_oidAppearanceLeftHand = oidLeftHand;
        m_cAppearanceInfo.m_oidAppearanceRightHand = oidRightHand;
        m_cAppearanceInfo.m_oidAppearanceArmour = oidArmour;
        m_cAppearanceInfo.m_oidAppearanceHead = oidHead;
    }
}

int CSWSCreature::RequestBuy(unsigned long oidItem, unsigned long oidStore)
{
    CSWSItem* pItem =
        g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
    {
        return FALSE;
    }
    CSWSStore* pStore =
        g_pAppManager->m_pServerExoApp->GetStoreByGameObjectID(oidStore);
    if (pStore == NULL)
    {
        return FALSE;
    }
    if (pStore->GetItemInInventory(oidItem) == 0)
    {
        return FALSE;
    }
    if (pItem->m_pRepository != NULL &&
        pItem->m_pRepository->GetNumItems() != 0)
    {
        SendFeedbackMessage(FEEDBACK_UNKNOWN_100, NULL);
        return FALSE;
    }
    if ((unsigned long)m_nGold < pStore->CalculateItemSellPrice(pItem, m_nId))
    {
        SendFeedbackMessage(FEEDBACK_UNKNOWN_102, NULL);
        return FALSE;
    }
    return TRUE;
}

int CSWSCreature::RequestSell(unsigned long oidItem, unsigned long oidStore)
{
    CSWSItem* pItem =
        g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
    {
        return FALSE;
    }
    if (g_pAppManager->m_pServerExoApp->GetStoreByGameObjectID(oidStore) ==
        NULL)
    {
        return FALSE;
    }
    if (pItem->m_pRepository != NULL &&
        pItem->m_pRepository->GetNumItems() != 0)
    {
        SendFeedbackMessage(FEEDBACK_UNKNOWN_100, NULL);
        return FALSE;
    }
    if (pItem->m_bPlotObject == 1)
    {
        SendFeedbackMessage(FEEDBACK_UNKNOWN_099, NULL);
        return FALSE;
    }
    if (GetItemRepository(TRUE)->GetItemInRepository(pItem, 0) != 0)
    {
        return TRUE;
    }
    if (m_pInventory->GetItemInInventory(pItem) == 1)
    {
        return TRUE;
    }
    CSWSItem* pContainer = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
        pItem->m_oidPossessor);
    if (pContainer == NULL)
    {
        return FALSE;
    }
    return GetItemRepository(TRUE)->GetItemInRepository(pContainer, 0) != 0;
}
// PC 0x004ED6E0  Xbox 0x00170F70
void CSWSCreature::UpdatePersonalSpace()
{
    float fSpace = 0.6f;
    C2DA* pTable;

    pTable = g_pRules->m_p2DArrays->m_pAppearance;
    int bFound = pTable->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_PERSPACE,
            &fSpace);
    CPathfindInformation* pInfo = m_pPathfindInformation;
    if (bFound == 1)
        pInfo->m_fStepScale = fSpace;
    else
        pInfo->m_fStepScale = 0.6f;
    pInfo->ComputeStepTolerance();

    pTable = g_pRules->m_p2DArrays->m_pAppearance;
    if (pTable->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_CREPERSPACE,
            &fSpace) == 1)
        m_pPathfindInformation->m_fStepScaleY = fSpace;
    else
        m_pPathfindInformation->m_fStepScaleY = fSpace;

    pTable = g_pRules->m_p2DArrays->m_pAppearance;
    bFound = pTable->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_CAMERASPACE,
            &fSpace);
    if (bFound == 1)
        m_pPathfindInformation->m_fStepScaleZ = fSpace;
    else
        m_pPathfindInformation->m_fStepScaleZ =
            m_pPathfindInformation->GetCreaturePersonalSpace();

    pTable = g_pRules->m_p2DArrays->m_pAppearance;
    if (pTable->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_HITDIST,
            &fSpace) == 1)
        m_pPathfindInformation->m_fHitDistance = fSpace;
    else
        m_pPathfindInformation->m_fHitDistance = 0.5f;

    pTable = g_pRules->m_p2DArrays->m_pAppearance;
    if (pTable->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_HEIGHT,
            &fSpace) == 1)
        m_pPathfindInformation->m_fHalfStep = fSpace;
    else
        m_pPathfindInformation->m_fHalfStep = 1.0f;
}

void CSWSCreature::SaveFollowInfo(CResGFF* pRes, CResStruct* pStruct)
{
    if (m_pFollowInfo != NULL)
    {
        CResStruct stFollow;
        pRes->AddStructToStruct(&stFollow, pStruct, "FollowInfo", 0);
        m_pFollowInfo->Save(pRes, &stFollow);
    }
}
// PC 0x004ED850  Xbox 0x00171200
void CSWSCreature::UpdateAppearanceDependantInfo(int bUpdateModel)
{
    int nHasLimbs;
    g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
        m_cAppearanceInfo.m_nAppearance, "haslegs", &nHasLimbs);
    m_bHasLegs = nHasLimbs;
    g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
        m_cAppearanceInfo.m_nAppearance, "hasarms", &nHasLimbs);
    m_bHasArms = nHasLimbs;

    int bDisableInjuredAnim;
    g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
        m_cAppearanceInfo.m_nAppearance,
        CTwoDimArrays::COL2DA_APPEARANCETABLE_DISABLEINJUREDANIM, &bDisableInjuredAnim);
    m_bDisableInjuredAnim = bDisableInjuredAnim;

    int nSize;
    g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
        m_cAppearanceInfo.m_nAppearance,
        CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY, &nSize);
    m_nCreatureSize = nSize;

    CExoString sModelType;
    g_pRules->Get2DArrays()->GetAppearance()->GetCExoStringEntry(
        m_cAppearanceInfo.m_nAppearance,
        CTwoDimArrays::COL2DA_APPEARANCETABLE_MODELTYPE, &sModelType);
    m_sModelType = sModelType;

    float fValue;
    if (g_pRules->Get2DArrays()->GetAppearance()->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, "weaponscale", &fValue) == 1)
        m_fWeaponScale = fValue;
    else
        m_fWeaponScale = 1.0f;

    if (bUpdateModel)
        m_pStats->SetMovementRate(CSWSCreatureStats::MOVEMENT_SPEED_DEFAULT);

    g_pRules->Get2DArrays()->GetAppearance()->GetFLOATEntry(
        m_cAppearanceInfo.m_nAppearance, "PREFATCKDIST", &fValue);
    m_fPreferredAttackDistance = fValue;
    m_nUnknown344 = 1;
}
// PC 0x004EDAE0  Xbox 0x00171490
unsigned char CSWSCreature::GetDetectMode()
{
    if (GetAnimation() == 10000 && m_nDetectMode != 1)
    {
        return 2;
    }
    return m_nDetectMode;
}
// PC 0x004EDB00  Xbox 0x001714B0
CSWSJournal* CSWSCreature::GetJournal()
{
    return g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyJournal();
}
// PC 0x004EDB20  Xbox 0x001714D0
int CSWSCreature::GetIsWeaponEffective(unsigned long oidTarget, int bOffHand)
{
    CSWSObject* pTarget;
    CGameObject* pObject;
    int nDamage;
    CServerExoApp* pServer;

    pTarget = NULL;
    pServer = g_pAppManager->m_pServerExoApp;
    pObject = pServer->GetGameObject(oidTarget);
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        pTarget = pObject->AsSWSObject();
        if (pTarget != NULL && pTarget->m_bPlotObject == 1)
        {
            return 0;
        }
    }
    nDamage = m_pStats->GetDamageRoll(pTarget, bOffHand, 0, 0, 1, 0);
    return nDamage + CalculateMaxElementalDamage(pTarget, bOffHand) > 0;
}
// PC 0x004F1C40  Xbox 0x00175930
void CSWSCreature::PostProcess()
{
    CServerAIMaster* pAIMaster =
        g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    if (GetStats()->GetIsPC() == TRUE)
    {
        unsigned long nSlot = 1;
        pAIMaster->SetAILevel(this, CServerAIMaster::AI_LEVEL_PARTY_CREATURE);
        for (int nSlots = CSWInventory::TOTAL_SLOT_COUNT; nSlots != 0; --nSlots)
        {
            CSWSItem* pItem = m_pInventory->GetItemInSlot(nSlot);
            if (pItem != NULL)
                pItem->SetDropable(TRUE);
            nSlot *= 2;
        }
        for (unsigned int nItem = 0;
             nItem < (unsigned int)GetItemRepository(TRUE)->GetNumItems(); ++nItem)
        {
            CSWSItem* pItem = GetItemRepository(TRUE)->ItemListGetItem(nItem);
            pItem->SetDropable(TRUE);
        }
    }

    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(
        m_sTag, GetId());
    m_nAreaWaitState = GetStats()->GetIsPC();
    CFactionManager* pFactionManager =
        g_pAppManager->GetServerExoApp()->GetFactionManager();
    if (GetIsPC() == TRUE)
    {
        if (pFactionManager != NULL && pFactionManager->m_pPlayer != NULL)
            pFactionManager->m_pPlayer->AddMember(GetId(), FALSE);
        SetOriginalFactionId(0);
    }
    else
    {
        int nFactionId = m_pStats->m_nFactionId;
        m_pStats->m_nFactionId = INVALID_STANDARD_FACTION;
        CSWSFaction* pFaction = pFactionManager->GetFaction(nFactionId);
        if (pFaction != NULL)
        {
            pFaction->AddMember(GetId(), FALSE);
            SetOriginalFactionId(nFactionId);
        }
        else
        {
            CExoString sError;
            sError.Format("Cannot set creature %s to faction %d because faction does not exist! Setting to Hostile1.",
                          m_sTag.CStr(), nFactionId);
            pFaction = pFactionManager->GetFaction(STANDARD_FACTION_HOSTILE_1);
            EXOXBOXASSERT(pFaction);
            pFaction->AddMember(GetId(), FALSE);
            SetOriginalFactionId(STANDARD_FACTION_HOSTILE_1);
        }
    }

    UpdatePersonalSpace();
    m_n1EC = TRUE;
    CGameEffect* pEffect = new CGameEffect(TRUE);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_RACIAL_TYPE;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_INNATE |
        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(GetId());
    pEffect->SetInteger(0, m_pStats->m_nRace);
    ApplyEffect(pEffect, FALSE, FALSE);
    m_n1EC = FALSE;
    if (GetCurrentHitPoints(FALSE) < 1)
    {
        SetAnimation(CREATURE_ANIMATION_AMBIENT_3);
        m_bCommandable = FALSE;
        if (!GetIsPC() || GetCurrentHitPoints(FALSE) <= -10)
            m_nAIStateFlags = 0;
    }
    if (GetIsPC() == TRUE)
        m_nF0 = TRUE;
    UpdateAppearanceDependantInfo(FALSE);
}
// PC 0x004F1EF0  Xbox 0x00175C20
int CSWSCreature::CancelAction(unsigned short nActionId,
                              unsigned short nGroupId)
{
    CExoLinkedList<CSWSObjectActionNode>* pActions = &m_lstActions;
    CSWSObjectActionNode* pAction;
    CExoLinkedListNode* pPos;

    if (pActions != NULL)
    {
        pAction = pActions->GetHead();
        pPos = pActions->GetHeadPos();
    }

    int bCancelled = 0;
    while (pAction != NULL)
    {
        if (pAction->m_nGroupId == nGroupId)
        {
            CExoLinkedListNode* pRemovePos = pPos;
            pAction = pActions->GetNext(pPos);
            CSWSObjectActionNode* pRemoveAction = pActions->GetAtPos(pRemovePos);
            if (ClearAction(pRemoveAction,
                            pRemoveAction == pActions->GetHead()) == 1)
            {
                delete pActions->Remove(pRemovePos);
                bCancelled = 1;
            }
        }
        else
        {
            pAction = pActions->GetNext(pPos);
        }
    }
    return bCancelled;
}
// PC 0x004EDBA0  Xbox 0x00171550
int CSWSCreature::ForceMoveToPoint(CSWSForcedAction* pForcedAction)
{
    CSWSObjectActionNode* pAction = new CSWSObjectActionNode;
    int bWalkStraightLineToPoint = 1;
    float fRange = 1.0f;
    float fTimeout = 0.0f;
    unsigned long oidTarget;

    pAction->m_nField70 = 1;
    if (pForcedAction->m_oidTargetObject == OBJECT_INVALID)
    {
        pAction->m_nActionType = ACTION_JUMP_TO_POINT;
        pAction->m_nGroupId = pForcedAction->m_nGroupId;
        oidTarget = pForcedAction->m_oidTargetArea;
        AddActionNodeParameter(pAction, 0, ACTION_PARAM_FLOAT,
                              (Vector*)&pForcedAction->GetTargetLocation());
        AddActionNodeParameter(pAction, 1, ACTION_PARAM_FLOAT,
                              &((Vector*)&pForcedAction->GetTargetLocation())->m_fY);
        AddActionNodeParameter(pAction, 2, ACTION_PARAM_FLOAT,
                              &((Vector*)&pForcedAction->GetTargetLocation())->m_fZ);
        AddActionNodeParameter(pAction, 3, ACTION_PARAM_OBJECT, &oidTarget);
        AddActionNodeParameter(pAction, 4, ACTION_PARAM_INTEGER,
                              &bWalkStraightLineToPoint);
        AddActionNodeParameter(pAction, 5, ACTION_PARAM_FLOAT, &fRange);
        AddActionNodeParameter(pAction, 6, ACTION_PARAM_FLOAT, &fTimeout);
        AIActionJumpToPoint(pAction);
    }
    else
    {
        pAction->m_nActionType = ACTION_JUMP_TO_OBJECT;
        pAction->m_nGroupId = pForcedAction->m_nGroupId;
        oidTarget = pForcedAction->m_oidTargetObject;
        AddActionNodeParameter(pAction, 0, ACTION_PARAM_OBJECT, &oidTarget);
        AddActionNodeParameter(pAction, 1, ACTION_PARAM_INTEGER,
                              &bWalkStraightLineToPoint);
        AIActionJumpToObject(pAction);
    }
    delete pAction;
    return 1;
}
// PC 0x004EDD60  Xbox 0x00171720
int CSWSCreature::GetGold()
{
    if (m_bInParty != 0)
    {
        return g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nGold;
    }

    return m_nGold;
}
// PC 0x004F1FD0  Xbox 0x00175D00
int CSWSCreature::DoSpotDetection(CSWSCreature* pCreature, int bInvisible)
{
    int bDetected = TRUE;
    int nCheck = 0;
    int nSpotRank = 0;
    int nHideRank = 0;
    int nObserverStationary = 0;
    int nTargetStationary = 0;
    int nDistancePenalty = 0;
    int nBehindPenalty = 0;
    int nCombatPenalty = 0;
    Vector vPosition = m_vPosition;
    Vector vTarget = pCreature->m_vPosition;
    vPosition.m_fZ += 1.0f;
    vTarget.m_fZ += 1.0f;
    if (GetBlind())
        return 0;
    if (bInvisible)
        return 0;
    Vector vDelta = vPosition - vTarget;
    float fRange = GetSpotCheckDistance();
    if (vDelta * vDelta > fRange * fRange)
        return 0;
    unsigned char nVisionFlags = m_nVisionFlags;
    if (nVisionFlags & VISION_TRUESEEING)
        return 1;
    if ((unsigned char)pCreature->GetStats()->GetSkillRank(
            CSWSkill::SKILL_STEALTH, AsSWSObject(), 0) == 0 ||
        pCreature->m_nStealthMode == 0)
        return 1;

    if (pCreature->m_nStealthMode == 1)
    {
        nCheck -= pCreature->GetStats()->GetSkillRank(
            CSWSkill::SKILL_STEALTH, AsSWSObject(), 0);
        nCheck -= pCreature->m_nHideCheckRoll;
        nHideRank = pCreature->GetStats()->GetSkillRank(
            CSWSkill::SKILL_STEALTH, AsSWSObject(), 0);
    }
    if (GetDetectMode() == 1 || GetDetectMode() == 2)
    {
        nCheck += GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS,
                                          pCreature->AsSWSObject(), 0);
        nSpotRank = GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS,
                                            pCreature->AsSWSObject(), 0);
    }
    else
    {
        nCheck += GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS,
                                          pCreature->AsSWSObject(), 0) / 2;
        nSpotRank = GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS,
                                            pCreature->AsSWSObject(), 0) / 2;
    }
    nCheck += m_nSpotCheckRoll;
    GetClientCreature();
    CSWSPlayer* pClient = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
    CSWSPlayer* pTargetClient = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pCreature->GetId());
    if (pClient == NULL && pTargetClient != NULL)
    {
        Vector vDirection = pCreature->m_vPosition - m_vPosition;
        if (vDirection * vDirection > 0.0)
            vDirection = normalize(vDirection);
        else
            vDirection = m_vOrientation;
        if (vDirection * normalize(m_vOrientation) <= -0.707f)
        {
            nCheck -= 5;
            nBehindPenalty = -5;
        }
    }
    if (m_bCombatState)
    {
        nCheck -= 10;
        nCombatPenalty = -10;
    }
    if (pCreature->AnimationStationary(pCreature->GetAnimation()))
    {
        nCheck -= 5;
        nTargetStationary = 5;
    }
    if (AnimationStationary(GetAnimation()))
    {
        nCheck += 5;
        nObserverStationary = 5;
    }
    float fDistance = magnitude(m_vPosition - pCreature->m_vPosition) * 0.333333343f;
    if (fDistance > 2.0f)
    {
        int nPenalty = (int)(fDistance - 2.0f);
        nCheck -= nPenalty;
        nDistancePenalty = -nPenalty;
    }
    if (nCheck <= 0)
    {
        bDetected = FALSE;
    }
    else
    {
        CSWCCMessageData* pData = new CSWCCMessageData;
        pData->SetObjectID(0, GetId());
        pData->SetObjectID(1, pCreature->GetId());
        pData->SetInteger(0, 1);
        pData->SetInteger(1, m_nSpotCheckRoll);
        pData->SetInteger(2, pCreature->m_nHideCheckRoll);
        pData->SetInteger(3, nSpotRank);
        pData->SetInteger(4, nHideRank);
        pData->SetInteger(5, nObserverStationary);
        pData->SetInteger(6, nTargetStationary);
        pData->SetInteger(7, nDistancePenalty);
        pData->SetInteger(8, nBehindPenalty);
        pData->SetInteger(9, 0);
        pData->SetInteger(10, nCombatPenalty);
        if (g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId()) != NULL)
            BroadcastStealthDetectionData(pData);
        else if (g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pCreature->GetId()) != NULL)
            pCreature->BroadcastStealthDetectionData(pData);
        delete pData;
    }
    return bDetected;
}
// PC 0x004F2520  Xbox 0x00176380
void CSWSCreature::BroadcastVoiceChat(unsigned char nVoiceChat)
{
    if (GetArea() != NULL)
    {
        CExoArrayList<unsigned long>* pObjects = &GetArea()->m_lstObjectIds;
        for (int nObject = 0; nObject < pObjects->num; nObject++)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSPlayer* pPlayer = pServer
                ->GetClientObjectByObjectId((*pObjects)[nObject]);
            if (pPlayer != NULL)
            {
                CSWSObject* pGameObject = pPlayer->GetGameObject();
                if (pGameObject != NULL && m_oidArea == pGameObject->m_oidArea)
                {
                    Vector vDifference = pGameObject->m_vPosition - m_vPosition;
                    if (vDifference.m_fX * vDifference.m_fX +
                        vDifference.m_fY * vDifference.m_fY +
                        vDifference.m_fZ * vDifference.m_fZ <=
                        VOICE_CHAT_RANGE_SQUARED)
                    {
                        pServer = g_pAppManager->m_pServerExoApp;
                        unsigned long oidCreature = m_nId;
                        pServer->GetSWSMessage()
                            ->SendServerToPlayerVoiceChat_Play(pPlayer,
                                oidCreature, nVoiceChat);
                    }
                }
            }
        }
    }
}
// PC 0x004F2610  Xbox 0x00176480
void CSWSCreature::SetCombatState(int bCombatState, unsigned char bFeedback)
{
    int nFaction = GetFaction()->m_nIndex;
    if (nFaction == STANDARD_FACTION_NEUTRAL)
        bCombatState = FALSE;

    if (m_bCombatState != bCombatState && bCombatState == TRUE &&
        bFeedback == COMBAT_STATE_NOTIFY && !GetInParty() && !GetIsPC() &&
        rand() % 10 < 3)
    {
        BroadcastBattleCry();
    }

    m_bCombatState = bCombatState;
    if (bCombatState)
    {
        m_nCombatStateTimer = 8000;
        if (m_nItemUseState != COMBAT_STATE_NOTIFY)
            m_nItemUseState = bFeedback;
    }
    else
    {
        m_nCombatStateTimer = 0;
        m_nItemUseState = 0;
        m_bResolveInitiative = TRUE;
        m_nCombatMode = -1;
        m_nLastAttackAction = -1;
        m_oidLastHostileTarget = OBJECT_INVALID;
        SetLastHostileActor(OBJECT_INVALID, TRUE);
        m_nCombatSpellId = -1;
        m_nLastForcePowerUsed = -1;
        m_nLastSpecialAttack = 0;
        m_nLastCombatFeatUsed = 0;
        m_pcCombatRound->ClearAllAttacks();
        m_pcCombatRound->RemoveAllActions();
        m_oidAttemptedAttackTarget = OBJECT_INVALID;
        m_oidAttackTarget = OBJECT_INVALID;
        m_oidAttemptedSpellTarget = OBJECT_INVALID;
        m_oidSpellTarget = OBJECT_INVALID;
        m_pcCombatRound->m_oidUnknown9CC = OBJECT_INVALID;

        CSWSCombatRound* pCombatRound = m_pcCombatRound;
        if (pCombatRound->m_bRoundStarted)
        {
            CExoString sWarning;
            CExoString sTargetTag;
            CGameObject* pTarget = g_pAppManager->GetServerExoApp()->
                GetGameObject(pCombatRound->GetMasterID());
            if (pTarget != NULL && pTarget->AsSWSObject() != NULL)
            {
                sTargetTag = pTarget->AsSWSObject()->m_sTag;
                if (sTargetTag.IsEmpty())
                    sTargetTag = "NULL";
            }
            sWarning.Format("WARNING:  %x %s %d %d %d %d %x %s %x",
                GetId(), m_sTag.CStr(), pCombatRound->GetTimer(),
                pCombatRound->GetRoundLength(), pCombatRound->GetEngaged(),
                pCombatRound->GetMasterState(), pCombatRound->GetMasterID(),
                sTargetTag.CStr(), pTarget);
            m_pcCombatRound->EndCombatRound(FALSE);
        }
    }

    if (GetClientCreature() != NULL && bFeedback == COMBAT_STATE_NOTIFY)
        GetClientCreature()->SetCombatState(bCombatState);
}
// PC 0x004F28D0  Xbox 0x00176740
void CSWSCreature::RecomputeAmbientAnimationState()
{
    if (GetDead() || GetIsPCDying() || GetDeadTemp())
    {
        if (m_nAmbientAnimationState != AMBIENT_STATE_4 &&
            m_nAmbientAnimationState != AMBIENT_STATE_3 &&
            m_nAmbientAnimationState != AMBIENT_STATE_14)
        {
            SetAmbientAnimationState(AMBIENT_STATE_4);
        }
    }
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_SLEEP)
        SetAmbientAnimationState(AMBIENT_STATE_11);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_STUN)
        SetAmbientAnimationState(AMBIENT_STATE_2);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_CHOKE)
        SetAmbientAnimationState(AMBIENT_STATE_5);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_FORCE_PUSH)
        SetAmbientAnimationState(AMBIENT_STATE_9);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_VARIANT_12)
        SetAmbientAnimationState(AMBIENT_STATE_10);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_FORCE_JUMP_TARGET)
        SetAmbientAnimationState(AMBIENT_STATE_13);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_WHIRLWIND)
        SetAmbientAnimationState(AMBIENT_STATE_6);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_HORRIFIED)
        SetAmbientAnimationState(AMBIENT_STATE_7);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_DROID_STUN)
        SetAmbientAnimationState(AMBIENT_STATE_8);
    else if (m_bDebilitated == CSWSEffectListHandler::STATE_HOLD)
        SetAmbientAnimationState(AMBIENT_STATE_12);
    else
        SetAmbientAnimationState(AMBIENT_STATE_NORMAL);

    SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
}
// PC 0x004F2A50  Xbox 0x001768C0
int CSWSCreature::ToggleMode(unsigned char nMode)
{
    if (GetDead() || GetIsPCDying() || GetDeadTemp())
    {
        return 0;
    }

    if (nMode == MODE_STEALTH)
    {
        if (m_pStats->GetCanUseSkill(CSWRules::SKILL_STEALTH))
        {
            if (m_nStealthMode == 1)
            {
                SetActivity(ACTIVITY_STEALTH, 0, 0);
            }
            else if (m_bCombatState)
            {
                SendFeedbackMessage(FEEDBACK_CANNOT_STEALTH_IN_COMBAT, NULL);
            }
            else
            {
                SetActivity(ACTIVITY_STEALTH, 1, 0);
            }
        }
    }
    else if (nMode == MODE_PARRY)
    {
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_PARRY)
            SetCombatMode(0, 1);
        else
            SetCombatMode(SWSOBJECT_COMBAT_MODE_PARRY, 0);
    }
    else if (nMode == MODE_POWER_ATTACK)
    {
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK)
            SetCombatMode(0, 1);
        else
            SetCombatMode(SWSOBJECT_COMBAT_MODE_POWER_ATTACK, 0);
    }
    else if (nMode == MODE_IMPROVED_POWER_ATTACK)
    {
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK)
            SetCombatMode(0, 1);
        else
            SetCombatMode(SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK, 0);
    }
    else if (nMode == MODE_COUNTERSPELL)
    {
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL)
            SetCombatMode(0, 1);
        else
            SetCombatMode(SWSOBJECT_COMBAT_MODE_COUNTERSPELL, 0);
    }
    else if (nMode == MODE_FLURRY_OF_BLOWS)
    {
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
            SetCombatMode(0, 1);
        else
            SetCombatMode(SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS, 0);
    }
    else if (nMode == MODE_RAPID_SHOT)
    {
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
            SetCombatMode(0, 1);
        else
            SetCombatMode(SWSOBJECT_COMBAT_MODE_RAPID_SHOT, 0);
    }
    return 1;
}
// PC 0x004FFAF0  Xbox 0x001751F0
void CSWSCreature::UpdateAttributesOnEffect(CGameEffect* pEffect)
{
    if (pEffect == NULL ||
        (pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_ABILITY_INCREASE &&
         pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_ABILITY_DECREASE))
        return;

    int nAbility = pEffect->GetInteger(0);
    if (nAbility == ABILITY_STRENGTH)
    {
        m_pStats->UpdateSTRModifier();
        m_nUnknown344 = 1;
    }
    if (nAbility == ABILITY_DEXTERITY)
    {
        m_pStats->UpdateDEXModifier();
        m_nUnknown344 = 1;
    }
    if (nAbility == ABILITY_CONSTITUTION)
    {
        GetIsPCDying();
        int bWasDead = GetDead();
        int bWasDeadTemp = GetDeadTemp();
        short nMaxHitPoints = GetMaxHitPoints(1);
        m_pStats->UpdateCONModifier();
        SetCurrentHitPoints(GetCurrentHitPoints(1) - nMaxHitPoints + GetMaxHitPoints(1));
        if ((!bWasDead && GetDead()) || (!bWasDeadTemp && GetDeadTemp()))
        {
            CGameEffect* pDeath = new CGameEffect(1);
            pDeath->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
            pDeath->m_nSubType =
                (pDeath->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pDeath->SetCreator(pEffect->GetCreator());
            pDeath->SetInteger(1, 1);
            CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
            pAIMaster->AddEventDeltaTime(0, 0, pDeath->GetCreator(), GetId(),
                                         CServerAIMaster::EVENT_APPLY_EFFECT, pDeath);
        }
    }
    if (nAbility == ABILITY_INTELLIGENCE)
    {
        m_pStats->UpdateINTModifier();
        m_pStats->AdjustSpellUsesPerDay();
    }
    if (nAbility == ABILITY_WISDOM)
    {
        int nMaxForcePoints = GetMaxForcePoints();
        m_pStats->UpdateWISModifier();
        CSWSCreatureStats* pStats = m_pStats;
        int nForcePoints = pStats->m_nCurrentForcePoints;
        pStats->m_nCurrentForcePoints = nForcePoints - nMaxForcePoints + GetMaxForcePoints();
        m_pStats->AdjustSpellUsesPerDay();
    }
    if (nAbility == ABILITY_CHARISMA)
    {
        int nMaxForcePoints = GetMaxForcePoints();
        m_pStats->UpdateCHAModifier();
        CSWSCreatureStats* pStats = m_pStats;
        int nForcePoints = pStats->m_nCurrentForcePoints;
        pStats->m_nCurrentForcePoints = nForcePoints - nMaxForcePoints + GetMaxForcePoints();
        m_pStats->AdjustSpellUsesPerDay();
    }
}
// PC 0x004FFDA0  Xbox 0x00183760
void CSWSCreature::ReadItemsFromGff(CResGFF* pRes, CResStruct* pStruct,
                                    int bReadObjectIds, int bLoadInventory,
                                    int bLoadFromTemplate)
{
    CSWSItem* pItem;
    unsigned long oidItem = OBJECT_INVALID;
    unsigned long nSlot;
    if (m_nCreatureSize == 0)
    {
        int nSizeCategory;
        g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
            m_cAppearanceInfo.m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY,
            &nSizeCategory);
        m_nCreatureSize = nSizeCategory;
    }

    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    int bPartyRepository = 0;
    if (pPartyTable != NULL && pPartyTable->m_bInventoryDirty != 0)
        bPartyRepository = 1;
    m_bForceEquipClothing = 0;

    CResList lstItems;
    CResStruct stItem;
    int bSuccess;
    if (pRes->GetList(&lstItems, pStruct, "Equip_ItemList") == 1)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&lstItems); i++)
        {
            pRes->GetListElement(&stItem, &lstItems, i);
            if (bReadObjectIds != 0)
                oidItem = pRes->ReadFieldDWORD(&stItem, "ObjectId", bSuccess,
                                               OBJECT_INVALID);

            if (g_pAppManager->m_pServerExoApp->GetGameObject(oidItem) == NULL)
            {
                pItem = new CSWSItem(oidItem);
                if (pItem == NULL)
                    continue;
                if (bLoadFromTemplate != 0)
                {
                    CResRef cDefault("");
                    CResRef cTemplate = pRes->ReadFieldCResRef(
                        &stItem, "EquippedRes", bSuccess, cDefault);
                    if (bSuccess == 0 || pItem->LoadFromTemplate(cTemplate) == 0)
                    {
                        delete pItem;
                        pItem = NULL;
                    }
                    else
                    {
                        int bDropable = pRes->ReadFieldBYTE(
                            &stItem, "Dropable", bSuccess, 0);
                        pItem->SetDropable(bDropable);
                    }
                }
                else if (pItem->LoadItem(pRes, &stItem) == 0)
                {
                    delete pItem;
                    pItem = NULL;
                }

                if (pItem != NULL)
                {
                    nSlot = pRes->GetElementType(&stItem);
                    pItem->SetPossessor(GetId(), bLoadInventory, 0,
                                        OBJECT_INVALID);
                    if (CanEquipItem(pItem, &nSlot, 1, 1, 1))
                        EquipItem(nSlot, pItem, 1, 1, 0);
                    else
                    {
                        if (nSlot == 2)
                            m_bForceEquipClothing = 1;
                        GetItemRepository(bPartyRepository)->AddItem(
                            &pItem, 1, 0, 0);
                    }
                }
            }
            else
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
                if (pItem->GetPossessor() != GetId())
                    continue;
                nSlot = pRes->GetElementType(&stItem);
                pItem->SetPossessor(GetId(), bLoadInventory, 0,
                                    OBJECT_INVALID);
                if (CanEquipItem(pItem, &nSlot, 1, 1, 1))
                    EquipItem(nSlot, pItem, 1, 1, 0);
                else
                    GetItemRepository(1)->AddItem(&pItem, 1, 0, 1);
            }
        }
    }

    if (pRes->GetList(&lstItems, pStruct, "ItemList") == 1)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&lstItems); i++)
        {
            pRes->GetListElement(&stItem, &lstItems, i);
            if (bReadObjectIds != 0)
                oidItem = pRes->ReadFieldDWORD(&stItem, "ObjectId", bSuccess,
                                               OBJECT_INVALID);

            if (g_pAppManager->GetServerExoApp()->GetGameObject(oidItem) == NULL)
            {
                pItem = new CSWSItem(oidItem);
                if (pItem == NULL)
                    continue;
                if (bLoadFromTemplate != 0)
                {
                    CResRef cDefault("");
                    CResRef cTemplate = pRes->ReadFieldCResRef(
                        &stItem, "InventoryRes", bSuccess, cDefault);
                    if (bSuccess == 0 || pItem->LoadFromTemplate(cTemplate) == 0)
                    {
                        delete pItem;
                        pItem = NULL;
                    }
                    else
                    {
                        int bDropable = pRes->ReadFieldBYTE(
                            &stItem, "Dropable", bSuccess, 0);
                        pItem->SetDropable(bDropable);
                    }
                }
                else if (pItem->LoadItem(pRes, &stItem) == 0)
                {
                    delete pItem;
                    pItem = NULL;
                }

                if (pItem != NULL)
                {
                    if (GetItemRepository(1)->AddItem(&pItem, 0, 0, 1) != 0 &&
                        pItem != NULL)
                        pItem->SetPossessor(GetId(), bLoadInventory, 0,
                                            OBJECT_INVALID);
                }
            }
            else
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
                if (pItem->GetPossessor() != GetId())
                    continue;
                GetItemRepository(1)->AddItem(&pItem, 0, 0, 1);
            }
        }
    }
}
// PC 0x00500350  Xbox 0x00183D90
int CSWSCreature::LoadCreature(CResGFF* pRes, CResStruct* pStruct, int nAreaId)
{
    int bSuccess;
    m_oidArea = pRes->ReadFieldDWORD(pStruct, "AreaId", bSuccess, 0);
    CSWSCreatureStats* pStats = m_pStats;
    if (pStats->ReadStatsFromGff(pRes, pStruct, &m_cAppearanceInfo) != 0)
        return 0;

    pRes->ReadFieldBYTE(pStruct, "DetectMode", bSuccess, 0);
    SetActivity(ACTIVITY_DETECT, 1, 0);

    unsigned char nStealthMode =
        pRes->ReadFieldBYTE(pStruct, "StealthMode", bSuccess, 0);
    SetActivity(ACTIVITY_STEALTH, nStealthMode, 0);

    m_nCreatureSize = pRes->ReadFieldINT(pStruct, "CreatureSize", bSuccess, 3);
    m_nEC = pRes->ReadFieldBYTE(pStruct, "IsDestroyable", bSuccess, 1);
    m_nF0 = pRes->ReadFieldBYTE(pStruct, "IsRaiseable", bSuccess, 1);
    m_nF4 = pRes->ReadFieldBYTE(pStruct, "DeadSelectable", bSuccess, 1);

    unsigned char nAmbientState =
        pRes->ReadFieldBYTE(pStruct, "AmbientAnimState", bSuccess, 0);
    if (nAmbientState == AMBIENT_STATE_4 ||
        nAmbientState == AMBIENT_STATE_3)
    {
        if (m_nAnimation == 0x279b || m_nAnimation == 0x2785 ||
            m_nAnimation == 0x28a0)
            nAmbientState = AMBIENT_STATE_14;
    }
    m_nAmbientAnimationState = nAmbientState;

    SetAnimation(pRes->ReadFieldINT(pStruct, "Animation", bSuccess, ANIMATION_IDLE));

    int bCreationScriptFired =
        pRes->ReadFieldBYTE(pStruct, "CreatnScrptFird", bSuccess, 0);
    if (bSuccess == 1)
        m_bCreationScriptFired = bCreationScriptFired;
    else if (nAreaId == 1)
        m_bCreationScriptFired = 1;

    ReadScriptsFromGff(pRes, pStruct);
    int bLoadInventory = nAreaId == 0;
    ReadItemsFromGff(pRes, pStruct, 1, bLoadInventory, 0);
    m_pStats->ReadSpellsFromGff(pRes, pStruct);

    m_bPMIsDisguised = pRes->ReadFieldBYTE(
        pStruct, "PM_IsDisguised", bSuccess, 0);
    if (m_bPMIsDisguised != 0)
        m_nPMAppearance = pRes->ReadFieldWORD(
            pStruct, "PM_Appearance", bSuccess, 0);

    m_bListening = pRes->ReadFieldBYTE(pStruct, "Listening", bSuccess, 0);
    LoadListenData(pRes, pStruct);
    PostProcess();
    LoadFollowInfo(pRes, pStruct);
    LoadObjectState(pRes, pStruct);
    return 1;
}
// PC 0x00500610  Xbox 0x00184050
int CSWSCreature::SaveCreature(CResGFF* pRes, CResStruct* pStruct)
{
    int bWasInParty = m_bInParty;
    int bInParty = 0;
    SetInParty(bInParty, 1);
    m_pStats->SaveStats(pRes, pStruct);
    pRes->WriteFieldBYTE(pStruct, m_nDetectMode, "DetectMode");
    pRes->WriteFieldBYTE(pStruct, m_nStealthMode, "StealthMode");
    pRes->WriteFieldINT(pStruct, m_nCreatureSize, "CreatureSize");
    pRes->WriteFieldBYTE(pStruct, m_nEC, "IsDestroyable");
    pRes->WriteFieldBYTE(pStruct, m_nF0, "IsRaiseable");
    pRes->WriteFieldBYTE(pStruct, m_nF4, "DeadSelectable");

    {
        CResRef cScript(m_sScripts[0]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptHeartbeat");
    }
    {
        CResRef cScript(m_sScripts[1]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptOnNotice");
    }
    {
        CResRef cScript(m_sScripts[2]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptSpellAt");
    }
    {
        CResRef cScript(m_sScripts[3]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptAttacked");
    }
    {
        CResRef cScript(m_sScripts[4]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptDamaged");
    }
    {
        CResRef cScript(m_sScripts[5]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptDisturbed");
    }
    {
        CResRef cScript(m_sScripts[6]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptEndRound");
    }
    {
        CResRef cScript(m_sScripts[7]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptDialogue");
    }
    {
        CResRef cScript(m_sScripts[8]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptSpawn");
    }
    {
        CResRef cScript(m_sScripts[9]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptRested");
    }
    {
        CResRef cScript(m_sScripts[10]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptDeath");
    }
    {
        CResRef cScript(m_sScripts[11]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptUserDefine");
    }
    {
        CResRef cScript(m_sScripts[12]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptOnBlocked");
    }
    {
        CResRef cScript(m_sScripts[13]);
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptEndDialogue");
    }

    CResList lstItems;
    CResStruct stItem;
    pRes->AddList(&lstItems, pStruct, "Equip_ItemList");
    unsigned long nSlot = 1;
    for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; i++)
    {
        CSWSItem* pItem = m_pInventory->GetItemInSlot(nSlot);
        if (pItem != NULL)
        {
            pRes->AddListElement(&stItem, &lstItems, nSlot);
            pRes->WriteFieldDWORD(&stItem, pItem->GetId(), "ObjectId");
            pItem->SaveItem(pRes, &stItem);
        }
        nSlot *= 2;
    }

    pRes->AddList(&lstItems, pStruct, "ItemList");
    for (unsigned long i = 0; i < GetItemRepository(1)->GetNumItems(); i++)
    {
        CSWSItem* pItem = GetItemRepository(1)->ItemListGetItem(i);
        pRes->AddListElement(&stItem, &lstItems, 0);
        pRes->WriteFieldDWORD(&stItem, pItem->GetId(), "ObjectId");
        pItem->SaveItem(pRes, &stItem);
    }

    CResList lstPerception;
    CResStruct stPerception;
    pRes->AddList(&lstPerception, pStruct, "PerceptionList");
    for (int i = 0; i < m_lstVisibilityList.num; i++)
    {
        CSWVisibilityNode* pNode = m_lstVisibilityList[i];
        if (pNode != NULL)
        {
            pRes->AddListElement(&stPerception, &lstPerception, 0);
            pRes->WriteFieldDWORD(&stPerception, pNode->m_oidObject,
                                  "ObjectId");
            unsigned char nPerception =
                pNode->m_bSeen | (pNode->m_bHeard << 1) |
                (pNode->m_nSanctuary << 2) | (pNode->m_bInvisible << 1);
            pRes->WriteFieldBYTE(&stPerception, nPerception,
                                 "PerceptionData");
        }
    }

    CResStruct stCombatRound;
    pRes->AddStructToStruct(&stCombatRound, pStruct, "CombatRoundData", 0xcada);
    if (m_pcCombatRound->m_bRoundStarted == 1)
        m_pcCombatRound->SaveCombatRound(pRes, &stCombatRound);

    pRes->WriteFieldDWORD(pStruct, m_oidArea, "AreaId");
    pRes->WriteFieldBYTE(pStruct, m_nAmbientAnimationState, "AmbientAnimState");
    pRes->WriteFieldINT(pStruct, m_nAnimation, "Animation");
    pRes->WriteFieldBYTE(pStruct, m_bCreationScriptFired, "CreatnScrptFird");
    pRes->WriteFieldBYTE(pStruct, m_bPMIsDisguised, "PM_IsDisguised");
    if (m_bPMIsDisguised == 1)
        pRes->WriteFieldWORD(pStruct, m_nPMAppearance, "PM_Appearance");
    pRes->WriteFieldBYTE(pStruct, m_bListening, "Listening");
    SaveListenData(pRes, pStruct);
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "XPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "YPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "ZPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fX, "XOrientation");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fY, "YOrientation");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fZ, "ZOrientation");
    pRes->WriteFieldINT(pStruct, m_nUnknown22C, "JoiningXP");
    SetInParty(bWasInParty, 1);

    SaveFollowInfo(pRes, pStruct);
    SaveObjectState(pRes, pStruct);
    return 1;
}
// PC 0x00500C00  Xbox 0x00184670
int CSWSCreature::DoStealthDetection(CSWSCreature* pCreature, int bCheckSpot,
                                     int* pnSpotDetected,
                                     int* pnListenDetected, int bFlag)
{
    int bSpotDetected;
    CSWSArea* pArea;
    int bListenDetected;

    bSpotDetected = 0;
    if (pCreature == NULL)
    {
        *pnSpotDetected = 0;
        *pnListenDetected = 0;
        return 0;
    }
    pArea = pCreature->GetArea();
    if (pArea != GetArea())
    {
        *pnSpotDetected = 0;
        *pnListenDetected = 0;
        return 0;
    }
    bListenDetected = DoListenDetection(pCreature, bFlag);
    if (bCheckSpot != 0)
    {
        bSpotDetected = DoSpotDetection(pCreature, bFlag);
    }
    *pnListenDetected = bListenDetected;
    *pnSpotDetected = bSpotDetected;
    return bSpotDetected != 0 || bListenDetected != 0;
}
// PC 0x004FB4B0  Xbox 0x0017F3C0
int CSWSCreature::DoListenDetection(CSWSCreature* pCreature, int bInvisible)
{
    Vector vHit1(0.0f, 0.0f, 0.0f);
    Vector vHit2(0.0f, 0.0f, 0.0f);
    int nCheck = 0;
    int bDetected = TRUE;
    if (pCreature->m_bSilenced)
        return 0;
    Vector vPosition = m_vPosition;
    Vector vTarget = pCreature->m_vPosition;
    vPosition.m_fZ += 1.5f;
    vTarget.m_fZ += 1.5f;
    Vector vDelta = vPosition - vTarget;
    if (bInvisible || GetBlind())
    {
        float fAttackRange = MaxAttackRange(pCreature->GetId(), 0, 0);
        if (vDelta * vDelta > fAttackRange * fAttackRange)
            return 0;
        CSWVisibilityNode* pVisible = GetVisibleListElement(pCreature->GetId());
        if (pVisible != NULL && pVisible->m_nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED)
            return 0;
    }
    float fRange = GetListenCheckDistance();
    if (vDelta * vDelta > fRange * fRange)
        return 0;
    unsigned long oidHit1;
    unsigned long oidHit2;
    GetArea()->ClearLineOfSight(vPosition, vTarget, &vHit1, &oidHit1,
                                OBJECT_INVALID, OBJECT_INVALID, 0, 0);
    GetArea()->ClearLineOfSight(vTarget, vPosition, &vHit2, &oidHit2,
                                OBJECT_INVALID, OBJECT_INVALID, 0, 0);
    float fObstruction;
    if (oidHit1 == oidHit2)
    {
        if (oidHit1 != OBJECT_INVALID && oidHit2 != OBJECT_INVALID)
            fObstruction = 0.4f * 0.4f;
        else
            fObstruction = (vHit2 - vHit1) * (vHit2 - vHit1);
    }
    else
    {
        if (oidHit1 != OBJECT_INVALID)
        {
            CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidHit1);
            if (pObject != NULL && pObject->AsSWSObject() != NULL)
                vHit1 = pObject->AsSWSObject()->m_vPosition;
        }
        if (oidHit2 != OBJECT_INVALID)
        {

            CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidHit1);
            if (pObject != NULL && pObject->AsSWSObject() != NULL)
                vHit2 = pObject->AsSWSObject()->m_vPosition;
        }
        fObstruction = (vHit2 - vHit1) * (vHit2 - vHit1);
    }
    if (fObstruction > 0.0f)
    {
        if ((GetArea()->m_nFlags & 1) || (GetArea()->m_nFlags & 2))
        {
            CPathfindInformation* pInfo = GetArea()->m_pAreaPathfindInformation;
            pInfo->SetEndX(m_vPosition.m_fX);
            pInfo->SetEndY(m_vPosition.m_fY);
            pInfo->SetStartX(pCreature->m_vPosition.m_fX);
            pInfo->SetStartY(pCreature->m_vPosition.m_fY);
            if (GetArea()->PlotSoundPath(pInfo) == 1)
                nCheck = -2;
            else
                return 0;
        }
        else
            nCheck = -5 * (int)sqrt(fObstruction);
    }
    if ((unsigned char)pCreature->GetStats()->GetSkillRank(
            CSWSkill::SKILL_STEALTH, AsSWSObject(), 0) == 0 ||
        pCreature->m_nStealthMode == 0)
        return 1;
    if (pCreature->m_nStealthMode == 1)
    {
        nCheck -= pCreature->GetStats()->GetSkillRank(CSWSkill::SKILL_STEALTH, AsSWSObject(), 0);
        nCheck -= pCreature->m_nMoveSilentlyCheckRoll;
    }
    if (GetDetectMode() == 1 || GetDetectMode() == 2)
        nCheck += GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS,
                                          pCreature->AsSWSObject(), 0);
    else
        nCheck += GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS,
                                          pCreature->AsSWSObject(), 0) / 2;
    unsigned char nListenRoll = m_nListenCheckRoll;
    nCheck += GetArea()->m_nModListenCheck + nListenRoll;
    GetClientCreature();
    if (m_bCombatState)
        nCheck -= 10;
    if (pCreature->AnimationStationary(pCreature->GetAnimation()))
        nCheck -= 5;
    if (AnimationStationary(GetAnimation()))
        nCheck += 5;
    nCheck += (int)(magnitude(m_vPosition - pCreature->m_vPosition) * -0.333333343f);
    if (pCreature->m_nCreatureSize == 1)
        nCheck -= 8;
    else if (pCreature->m_nCreatureSize == 2)
        nCheck -= 4;
    else if (pCreature->m_nCreatureSize == 4)
        nCheck += 4;
    else if (pCreature->m_nCreatureSize == 5)
        nCheck += 8;
    nCheck += GetStats()->GetFavoredEnemyBonus(pCreature);
    if (bDetected == TRUE)
    {
        if (nCheck <= 0)
            bDetected = FALSE;
    }
    return bDetected;
}
// PC 0x004FB9E0  Xbox 0x0017F960
void CSWSCreature::BroadcastSpellCast(unsigned long nSpell, int nCastResult,
    unsigned char nClass, unsigned short nFeat, unsigned long oidTarget)
{
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    CSWSArea* pArea = GetArea();
    if (pArea != NULL)
    {
        CExoArrayList<unsigned long>* pObjects = &pArea->m_lstObjectIds;
        if (pObjects != NULL)
        {
            m_lstCreatureRequests.SetSize(0);
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(CSWCCMessageData::SPELL_DATA_CASTER, m_nId);
            pMessageData->SetObjectID(CSWCCMessageData::SPELL_DATA_ITEM, OBJECT_INVALID);
            pMessageData->SetObjectID(CSWCCMessageData::SPELL_DATA_TARGET, oidTarget);
            pMessageData->SetInteger(CSWCCMessageData::SPELL_DATA_SPELL, nSpell);
            pMessageData->SetInteger(CSWCCMessageData::SPELL_DATA_CAST,
                CSWCCMessageData::SPELL_DATA_CAST_STARTED);
            for (int nObject = 0; nObject < pObjects->num; nObject++)
            {
                CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()
                    ->GetCreatureByGameObjectID((*pObjects)[nObject]);
                if (pCreature != NULL)
                {
                    Vector vDifference = pCreature->m_vPosition - m_vPosition;
                    float fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_LONG];
                    if (!(vDifference * vDifference > fRange * fRange))
                    {
                        if (pSpell->m_nUserType == CSWSpell::USER_TYPE_FORCE_POWER &&
                            pSpell->m_nAltMessage == 0)
                        {
                            CSWVisibilityNode* pVisible = pCreature->GetVisibleListElement(GetId());
                            if (pVisible == NULL || !pVisible->m_bSeen)
                                continue;
                        }
                        else
                        {
                            m_lstCreatureRequests.Add(pCreature->m_nId);
                        }
                    }
                }
            }
            delete pMessageData;
        }
    }
}
// PC 0x004FBBE0  Xbox 0x0017FB70
void CSWSCreature::BroadcastCombatStateToParty(unsigned long oidTarget)
{
    CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()
        ->GetCreatureByGameObjectID(oidTarget);
    unsigned long oidAttackTarget = OBJECT_INVALID;
    unsigned long oidSpellTarget = OBJECT_INVALID;
    if (pTarget != NULL)
    {
        oidAttackTarget = pTarget->m_oidAttemptedAttackTarget;
        oidSpellTarget = pTarget->m_oidAttemptedSpellTarget;
    }

    if (GetReputation(oidTarget, FALSE) <= MAX_HOSTILE_REPUTATION ||
        oidTarget == GetId())
    {
        if (oidAttackTarget == GetId() || oidSpellTarget == GetId())
            SetCombatState(TRUE, COMBAT_STATE_NOTIFY);
        else
            SetCombatState(TRUE, COMBAT_STATE_EQUIPMENT);
    }

    if (!m_bPlotObject)
    {
        unsigned long* pMembers;
        GetFaction()->GetMemberList(&pMembers);
        for (int nMember = 0; nMember < GetFaction()->m_lstMembers.num; nMember++)
        {
            CGameObject* pGameObject = g_pAppManager->GetServerExoApp()
                ->GetGameObject(pMembers[nMember]);
            if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
            {
                CSWSObject* pObject = pGameObject->AsSWSObject();
                Vector vDifference = m_vPosition - pObject->m_vPosition;
                float fDistance = 0.0f;
                if (GetInParty())
                    fDistance = NWSCREATURE_COMBAT_STATE_PARTY_RANGE;
                else if (pObject->AsSWSCreature() != NULL)
                    fDistance = pObject->AsSWSCreature()->GetSpotCheckDistance();
                else
                {
                    CExoString sColumn("PrimaryRange");
                    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
                    EXOXBOXASSERT(pArrays->m_pRanges);
                    C2DA* pRanges = pArrays->m_pRanges;
                    pRanges->GetFLOATEntry(CTwoDimArrays::RANGES_COMBAT_BROADCAST_ROW,
                        sColumn, &fDistance);
                }

                if (vDifference * vDifference <= fDistance * fDistance &&
                    pObject->AsSWSCreature()->GetReputation(oidTarget, FALSE) <=
                        MAX_HOSTILE_REPUTATION)
                {
                    if (oidAttackTarget == pObject->GetId() ||
                        oidSpellTarget == pObject->GetId())
                        pObject->AsSWSCreature()->SetCombatState(TRUE, COMBAT_STATE_NOTIFY);
                    else
                        pObject->AsSWSCreature()->SetCombatState(TRUE, COMBAT_STATE_EQUIPMENT);
                    pObject->AsSWSCreature()->SetExcitedState(
                        CTwoDimArrays::EXCITED_DURATION_COMBAT_ROW);
                }
            }
        }
    }
}
// PC 0x004FBE40  Xbox 0x0017FE10
int CSWSCreature::UseSkill(unsigned char nSkill, unsigned char nSubSkill,
                         unsigned long oidTarget, const Vector& vTarget,
                         unsigned long oidArea, unsigned long oidItemUsed,
                         int bInstant)
{
    CGameObject* pObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidTarget, &pObject) != CGameObjectArray::SUCCESS && equal(vTarget, Vector()))
        return 1;

    if (!m_pStats->GetCanUseSkill(nSkill))
    {
        SendFeedbackMessage(FEEDBACK_CANNOT_USE_SKILL, NULL);
        return 0;
    }
    if (pObject != NULL && pObject->AsSWSCreature() != NULL &&
        g_pRules->m_pSkills[nSkill].m_bHostileSkill &&
        GetAIStateReputation(oidTarget) == AI_STATE_REPUTATION_FRIEND)
    {
        SendFeedbackMessage(FEEDBACK_CANNOT_ATTACK_FRIEND, NULL);
        return 0;
    }

    switch (nSkill)
    {
    case CSWRules::SKILL_DEMOLITIONS:
        if (nSubSkill == CSWSkill::SUBSKILL_FLAGTRAP)
            AddMineActions(oidTarget, ACTION_FLAG_MINE, OBJECT_INVALID, vTarget, oidArea);
        else if (nSubSkill == CSWSkill::SUBSKILL_RECOVERTRAP)
            AddMineActions(oidTarget, ACTION_RECOVER_MINE, OBJECT_INVALID, vTarget, oidArea);
        else if (nSubSkill == CSWSkill::SUBSKILL_EXAMINETRAP)
            AddMineActions(oidTarget, ACTION_EXAMINE_MINE, OBJECT_INVALID, vTarget, oidArea);
        else
            AddMineActions(oidTarget, ACTION_DISARM_MINE, OBJECT_INVALID, vTarget, oidArea);
        break;
    case CSWRules::SKILL_TREAT_INJURY:
        AddHealActions(oidTarget, oidItemUsed, bInstant);
        break;
    case CSWRules::SKILL_STEALTH:
        if (m_pStats->GetCanUseSkill(CSWRules::SKILL_STEALTH))
            ToggleMode(MODE_STEALTH);
        break;
    case CSWRules::SKILL_SECURITY:
        ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
        AddUnlockObjectAction(oidTarget, oidItemUsed, bInstant);
        break;
    case CSWRules::SKILL_AWARENESS:
        ToggleMode(0);
        break;
    }
    return 1;
}
// PC 0x004FC210  Xbox 0x00180090
int CSWSCreature::UseItem(unsigned long oidItem, unsigned char nItemProperty,
                          unsigned char nItemOption, unsigned long oidTarget,
                          const Vector& vTarget, unsigned long oidArea,
                          int bItemOption)
{
    int bTargetLocation = FALSE;
    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pItem == NULL)
        return FALSE;
    if (!CanUseItem(pItem, FALSE))
        return FALSE;
    CSWItemProperty* pProperty = pItem->GetActiveProperty(nItemProperty);
    int nResult = FALSE;
    if (pProperty != NULL)
    {
        if ((pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
             (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType))) &&
            pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
        {
            if ((unsigned char)pProperty->m_bUseable)
            {
                if (pTarget != NULL && pTarget->AsSWSCreature() != NULL)
                {
                    CSWVisibilityNode* pVisible = GetVisibleListElement(oidTarget);
                    if (pVisible != NULL &&
                        pVisible->m_nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED)
                        return ACTION_COMPLETE;
                }
                CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()
                    ->GetBaseItem(pItem->m_nBaseItem);
                if (oidTarget == OBJECT_INVALID)
                    bTargetLocation = TRUE;
                nResult = AddItemCastSpellActions(oidItem, nItemProperty, nItemOption,
                    vTarget, oidTarget, bItemOption, bTargetLocation,
                    m_bCombatState && GetNodeById(ACTION_PHYSICAL_ATTACKS) != NULL,
                    FALSE);
            }
        }
        else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE)
            return UseSkill(CSWRules::SKILL_SECURITY, 0, oidTarget, vTarget,
                            oidArea, oidItem, nItemProperty);
        else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_TRAP)
            return AddMineActions(pItem->GetId(), ACTION_SET_MINE,
                                  oidTarget, vTarget, oidArea);
    }
    return nResult;
}
// PC 0x004FC4C0  Xbox 0x00180380
void CSWSCreature::GetNearestObjects(float fHostileRange, float fRange,
                                     CExoArrayList<unsigned long>& aForward,
                                     CExoArrayList<unsigned long>& aNearest)
{
    int nIndex = 0;
    CSWSArea* pArea = GetArea();
    for (nIndex = 0; nIndex < pArea->m_lstObjectIds.num; ++nIndex)
    {
        if (pArea->m_lstObjectIds[nIndex] == GetId())
            break;
    }
    aNearest.SetSize(0);
    if (nIndex >= pArea->m_lstObjectIds.num)
        return;

    CExoArrayList<float> aAngles;
    CExoArrayList<float> aNearestDistances;
    CExoArrayList<float> aForwardDistances;
    Vector vDelta;
    int bHostile;
    float fAngle;
    float fDistanceSquared;
    Vector vFacing = normalize(m_vOrientation);
    Vector vViewOrigin = vFacing * -NWSCREATURE_NEAREST_VIEW_ORIGIN_OFFSET + m_vPosition;
    float fFacing = Yaw(vFacing);
    int bLeft = TRUE;
    int bRight = TRUE;
    for (int nOffset = 1; bLeft || bRight; ++nOffset)
    {
        if (bLeft)
        {
            if (nIndex - nOffset >= 0)
            {
                unsigned long oidObject = pArea->m_lstObjectIds[nIndex - nOffset];
                if (oidObject != GetId())
                {
                    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
                    if (ValidNearestObjectType(pObject, bHostile))
                    {
                        vDelta = pObject->AsSWSObject()->m_vPosition - m_vPosition;
                        fDistanceSquared = vDelta * vDelta;
                        float fObjectRange = bHostile ? fHostileRange : fRange;
                        if (fDistanceSquared < fObjectRange * fObjectRange)
                        {
                            fAngle = Yaw(vDelta) - fFacing;
                            while (fAngle < 0.0f)
                                fAngle += 360.0f;
                            while (fAngle > 359.0f)
                                fAngle -= 360.0f;
                            int nInsert = 0;
                            while (nInsert < aAngles.num)
                            {
                                if (fAngle < aAngles[nInsert])
                                    break;
                                ++nInsert;
                            }
                            if (nInsert < aAngles.num)
                            {
                                aAngles.Insert(fAngle, nInsert);
                                aNearestDistances.Insert(fDistanceSquared, nInsert);
                                aNearest.Insert(oidObject, nInsert);
                            }
                            else
                            {
                                aAngles.Add(fAngle);
                                aNearestDistances.Add(fDistanceSquared);
                                aNearest.Add(oidObject);
                            }
                            if (vDelta * m_vOrientation >= 0.0f)
                            {
                                vDelta = pObject->AsSWSObject()->m_vPosition - vViewOrigin;
                                vDelta.Normalize();
                                if (vDelta * m_vOrientation > NWSCREATURE_NEAREST_FRONT_DOT)
                                {
                                    int nForwardInsert;
                                    for (nForwardInsert = 0; nForwardInsert < aForwardDistances.num; ++nForwardInsert)
                                    {
                                        if (fDistanceSquared < aForwardDistances[nForwardInsert])
                                            break;
                                    }
                                    if (nForwardInsert < aForwardDistances.num)
                                    {
                                        aForwardDistances.Insert(fDistanceSquared, nForwardInsert);
                                        aForward.Insert(oidObject, nForwardInsert);
                                    }
                                    else
                                    {
                                        aForwardDistances.Add(fDistanceSquared);
                                        aForward.Add(oidObject);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
                bLeft = FALSE;
        }
        if (bRight)
        {
            if (nIndex + nOffset < pArea->m_lstObjectIds.num)
            {
                unsigned long oidObject = pArea->m_lstObjectIds[nIndex + nOffset];
                if (oidObject != GetId())
                {
                    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
                    if (ValidNearestObjectType(pObject, bHostile))
                    {
                        vDelta = pObject->AsSWSObject()->m_vPosition - m_vPosition;
                        fDistanceSquared = vDelta * vDelta;
                        float fObjectRange = bHostile ? fHostileRange : fRange;
                        if (fDistanceSquared < fObjectRange * fObjectRange)
                        {
                            fAngle = Yaw(vDelta) - fFacing;
                            while (fAngle < 0.0f)
                                fAngle += 360.0f;
                            while (fAngle > 359.0f)
                                fAngle -= 360.0f;
                            int nInsert = 0;
                            while (nInsert < aAngles.num)
                            {
                                if (fAngle < aAngles[nInsert])
                                    break;
                                ++nInsert;
                            }
                            if (nInsert < aAngles.num)
                            {
                                aAngles.Insert(fAngle, nInsert);
                                aNearestDistances.Insert(fDistanceSquared, nInsert);
                                aNearest.Insert(oidObject, nInsert);
                            }
                            else
                            {
                                aAngles.Add(fAngle);
                                aNearestDistances.Add(fDistanceSquared);
                                aNearest.Add(oidObject);
                            }
                            if (vDelta * m_vOrientation >= 0.0f)
                            {
                                vDelta = pObject->AsSWSObject()->m_vPosition - vViewOrigin;
                                vDelta.Normalize();
                                if (vDelta * m_vOrientation > NWSCREATURE_NEAREST_FRONT_DOT)
                                {
                                    int nForwardInsert;
                                    for (nForwardInsert = 0; nForwardInsert < aForwardDistances.num; ++nForwardInsert)
                                    {
                                        if (fDistanceSquared < aForwardDistances[nForwardInsert])
                                            break;
                                    }
                                    if (nForwardInsert < aForwardDistances.num)
                                    {
                                        aForwardDistances.Insert(fDistanceSquared, nForwardInsert);
                                        aForward.Insert(oidObject, nForwardInsert);
                                    }
                                    else
                                    {
                                        aForwardDistances.Add(fDistanceSquared);
                                        aForward.Add(oidObject);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
                bRight = FALSE;
        }
    }
}
// PC 0x004F2C30  Xbox 0x00176AA0
int CSWSCreature::ValidNearestObjectType(CGameObject* pObject, int& bHostile)
{
    if (pObject == NULL)
        return FALSE;

    int bValid = FALSE;
    bHostile = FALSE;
    switch (pObject->m_nObjectType)
    {
    case SW_OBJECT_TYPE_CREATURE:
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (pCreature->GetDead() || pCreature->GetDeadTemp())
                break;
            CSWVisibilityNode* pNode = GetVisibleListElement(pCreature->GetId());
            if (pNode != NULL && pNode->m_bSeen)
                bValid = TRUE;
            else if (DoSpotDetection(pCreature, FALSE))
                bValid = TRUE;
            if (bValid)
            {
                if (GetAIStateReputation(pCreature->GetId()) == AI_STATE_REPUTATION_ENEMY)
                    bHostile = TRUE;
            }
        }
        break;
    case SW_OBJECT_TYPE_PLACEABLE:
        {
            CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
            if (pPlaceable == NULL || !pPlaceable->m_bUseable)
                break;
            bHostile = pPlaceable->m_bHostile &&
                pPlaceable->GetReputation(m_nId, FALSE) <= MAX_HOSTILE_REPUTATION;
            bValid = TRUE;
        }
        break;
    case SW_OBJECT_TYPE_DOOR:
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            if (pDoor == NULL || pDoor->m_nOpenState || pDoor->m_bStatic)
                break;
            bValid = TRUE;
        }
        break;
    case SW_OBJECT_TYPE_TRIGGER:
        {
            CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
            if (pTrigger == NULL || !pTrigger->m_bMineTrigger)
                break;
            if (!pTrigger->m_lstTrapTargets.Contains(m_nId) &&
                pTrigger->GetReputation(m_nId, FALSE) < MIN_FRIENDLY_REPUTATION &&
                pTrigger->m_nFaction != m_pStats->m_nFactionId)
                break;
            bValid = TRUE;
        }
        break;
    default:
        bValid = FALSE;
        break;
    }
    return bValid;
}
// PC 0x004F2DE0  Xbox 0x00176C50
unsigned long CSWSCreature::GetNearestEnemy(float fRange, unsigned long oidExclude,
                                           unsigned long oidRelativeTo)
{
    if (GetArea() == NULL)
        return OBJECT_INVALID;

    CExoArrayList<unsigned long>* pObjects = &GetArea()->m_lstObjectIds;
    int nIndex;
    for (nIndex = 0; nIndex < pObjects->num; nIndex++)
    {
        if ((*pObjects)[nIndex] == GetId())
            break;
    }
    if (nIndex >= pObjects->num)
        return OBJECT_INVALID;

    float fNearest = fRange;
    int bLeft = 1;
    int bRight = 1;
    unsigned long oidNearest = OBJECT_INVALID;
    Vector vHit;
    unsigned long oidHit;
    for (int nOffset = 0; nIndex - nOffset >= 0 ||
         nIndex + nOffset < pObjects->num; nOffset++)
    {
        if (bLeft)
        {
            if (nIndex - nOffset >= 0)
            {
                unsigned long oidObject = (*pObjects)[nIndex - nOffset];
                CGameObject* pObject = g_pAppManager->GetServerExoApp()->
                    GetGameObject(oidObject);
                if (pObject != NULL && oidObject != GetId() && oidObject != oidExclude)
                {
                    int bEnemy;
                    if (oidRelativeTo != OBJECT_INVALID)
                    {
                        bEnemy = pObject->AsSWSCreature() != NULL &&
                            pObject->AsSWSCreature()->GetAIStateReputation(oidRelativeTo) ==
                                AI_STATE_REPUTATION_ENEMY;
                    }
                    else
                    {
                        bEnemy = GetAIStateReputation(oidObject) == AI_STATE_REPUTATION_ENEMY;
                    }
                    if (pObject->AsSWSCreature() != NULL && bEnemy &&
                        !pObject->AsSWSCreature()->GetDead() &&
                        !pObject->AsSWSCreature()->GetIsPCDying() &&
                        !pObject->AsSWSCreature()->GetDeadTemp())
                    {
                        CSWVisibilityNode* pVisible = GetVisibleListElement(pObject->GetId());
                        if (pVisible != NULL && pVisible->m_bSeen)
                        {
                            Vector vOrigin;
                            if (oidRelativeTo != OBJECT_INVALID)
                            {
                                CSWSCreature* pRelativeTo = g_pAppManager->GetServerExoApp()->
                                    GetCreatureByGameObjectID(oidRelativeTo);
                                if (pRelativeTo != NULL)
                                    vOrigin = pRelativeTo->m_vPosition;
                            }
                            else
                            {
                                vOrigin = m_vPosition;
                            }
                            float fDistance = magnitude(vOrigin -
                                pObject->AsSWSCreature()->m_vPosition);
                            fDistance = fDistance - GetPathfindInformation()->GetCreaturePersonalSpace() -
                                pObject->AsSWSCreature()->GetPathfindInformation()->GetCreaturePersonalSpace();
                            if (fDistance < fNearest)
                            {
                                Vector vStart = m_vPosition;
                                vStart.m_fZ += 1.5f;
                                Vector vEnd = pObject->AsSWSCreature()->m_vPosition;
                                vEnd.m_fZ += 1.5f;
                                if (GetArea() != NULL && GetArea()->ClearLineOfSight(
                                        vStart, vEnd, &vHit, &oidHit, GetId(), oidObject, 1, 0))
                                {
                                    fNearest = fDistance;
                                    oidNearest = oidObject;
                                }
                            }
                        }
                    }
                    if (pObject->AsSWSObject() != NULL &&
                        m_vPosition.m_fX - pObject->AsSWSObject()->m_vPosition.m_fX > fNearest)
                    {
                        bLeft = 0;
                    }
                }
            }
            else
            {
                bLeft = 0;
            }
        }
        if (bRight)
        {
            if (nIndex + nOffset < pObjects->num)
            {
                unsigned long oidObject = (*pObjects)[nIndex + nOffset];
                CGameObject* pObject = g_pAppManager->GetServerExoApp()->
                    GetGameObject(oidObject);
                if (pObject != NULL && oidObject != GetId() && oidObject != oidExclude)
                {
                    int bEnemy;
                    if (oidRelativeTo != OBJECT_INVALID)
                    {
                        bEnemy = pObject->AsSWSCreature() != NULL &&
                            pObject->AsSWSCreature()->GetAIStateReputation(oidRelativeTo) ==
                                AI_STATE_REPUTATION_ENEMY;
                    }
                    else
                    {
                        bEnemy = GetAIStateReputation(oidObject) == AI_STATE_REPUTATION_ENEMY;
                    }
                    if (pObject->AsSWSCreature() != NULL && bEnemy &&
                        !pObject->AsSWSCreature()->GetDead() &&
                        !pObject->AsSWSCreature()->GetIsPCDying() &&
                        !pObject->AsSWSCreature()->GetDeadTemp())
                    {
                        CSWVisibilityNode* pVisible = GetVisibleListElement(pObject->GetId());
                        if (pVisible != NULL && pVisible->m_bSeen)
                        {
                            Vector vOrigin;
                            if (oidRelativeTo != OBJECT_INVALID)
                            {
                                CSWSCreature* pRelativeTo = g_pAppManager->GetServerExoApp()->
                                    GetCreatureByGameObjectID(oidRelativeTo);
                                if (pRelativeTo != NULL)
                                    vOrigin = pRelativeTo->m_vPosition;
                            }
                            else
                            {
                                vOrigin = m_vPosition;
                            }
                            float fDistance = magnitude(vOrigin -
                                pObject->AsSWSCreature()->m_vPosition);
                            fDistance = fDistance - GetPathfindInformation()->GetCreaturePersonalSpace() -
                                pObject->AsSWSCreature()->GetPathfindInformation()->GetCreaturePersonalSpace();
                            if (fDistance < fNearest)
                            {
                                Vector vStart = vOrigin;
                                vStart.m_fZ += 1.5f;
                                Vector vEnd = pObject->AsSWSCreature()->m_vPosition;
                                vEnd.m_fZ += 1.5f;
                                if (GetArea() != NULL && GetArea()->ClearLineOfSight(
                                        vStart, vEnd, &vHit, &oidHit, GetId(), oidObject, 1, 0))
                                {
                                    fNearest = fDistance;
                                    oidNearest = oidObject;
                                }
                            }
                        }
                    }
                    if (pObject->AsSWSObject() != NULL &&
                        pObject->AsSWSObject()->m_vPosition.m_fX - m_vPosition.m_fX > fNearest)
                    {
                        bRight = 0;
                    }
                }
            }
            else
            {
                bRight = 0;
            }
        }
    }
    return oidNearest;
}
// PC 0x004F34A0  Xbox 0x001773B0
void CSWSCreature::SetLockOrientationToObject(unsigned long oidTarget, int bForce)
{
    if (bForce || (!GetDead() && !GetDeadTemp()))
    {
        if (oidTarget == CGameObjectArray::INVALIDOBJID &&
            m_oidLockOrientationToObject != CGameObjectArray::INVALIDOBJID &&
            m_oidLockOrientationToObject != m_nId)
        {
            CGameObject* pObject;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            if (pServer->GetObjectArray()->GetGameObject(
                    m_oidLockOrientationToObject, &pObject) == CGameObjectArray::SUCCESS &&
                pObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            {
                Vector vDirection = ((CSWSObject*)pObject)->m_vPosition - m_vPosition;
                vDirection = normalize(vDirection);
                SetOrientation(vDirection);
            }
        }
        m_oidLockOrientationToObject = oidTarget;
    }
}
// PC 0x004F3580  Xbox 0x001774C0
void CSWSCreature::SetExcitedState(unsigned char nState)
{
    int nDuration;

    bool bFailed;
    {
        CExoString sColumn("DURATION");
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pArrays->m_pExcitedDuration);
        C2DA* pTable = pArrays->m_pExcitedDuration;
        bFailed = pTable->GetINTEntry(nState, sColumn, &nDuration) == 0;
    }
    if (bFailed)
        return;

    if ((unsigned long)nDuration > m_nExcitedStateTimer)
    {
        m_nExcitedStateTimer = nDuration;
        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->
            GetClientObjectByObjectId(GetId());
        if (GetArea() != NULL && pPlayer != NULL &&
            GetArea()->m_pAmbientSound != NULL &&
            GetNearestEnemy(30.0f, OBJECT_INVALID, OBJECT_INVALID) != OBJECT_INVALID)
        {
            CSWSArea* pArea = GetArea();
            pArea->m_pAmbientSound->PlayBattleMusic(1);
        }
    }
}
// PC 0x004F3690  Xbox 0x001775D0
void CSWSCreature::CancelRest(unsigned short nFeedback)
{
    SetAnimation(ANIMATION_IDLE);
    SendFeedbackMessage(nFeedback, NULL);
    if ((m_bInDialog & ACTIVITY_REST) == 0)
    {
        m_nActivity &= ~ACTIVITY_REST;
    }
    if (m_nAreaWaitState != 0)
    {
        CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_REST;
        pEvent->SetInteger(0, CScriptEvent::REST_EVENT_CANCELLED);
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventAbsoluteTime(
            0, 0, GetId(), pModule->GetId(),
            CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        StopGuiTimingBar();
    }
}
// PC 0x004F3770  Xbox 0x001776B0
int CSWSCreature::GetItemCount(int bDropableOnly, int bRepositoryOnly,
                             int bNonPlotOnly)
{
    int nCount = 0;
    if (bRepositoryOnly == 0 && m_pInventory != NULL)
    {
        for (unsigned int nSlot = 0;
             nSlot < CSWInventory::EQUIPMENT_SLOT_COUNT; ++nSlot)
        {
            CSWInventory* pInventory = m_pInventory;
            CSWSItem* pItem = pInventory->GetItemInSlot(1 << nSlot);
            if (pItem != NULL &&
                (bDropableOnly == 0 || pItem->m_bDropable) &&
                (bNonPlotOnly == 0 || pItem->m_bPlotObject == 0))
            {
                ++nCount;
            }
        }
    }

    CItemRepository* pRepository = GetItemRepository(TRUE);
    if (pRepository != NULL)
    {
        for (unsigned int nItem = 0; nItem < pRepository->m_lstItems.num;
             ++nItem)
        {
            CSWSItem* pItem = pRepository->ItemListGetItem(nItem);
            if (pItem != NULL &&
                (bDropableOnly == 0 || pItem->m_bDropable) &&
                (bNonPlotOnly == 0 || pItem->m_bPlotObject == 0))
            {
                ++nCount;
            }
        }
    }
    return nCount;
}
// PC 0x004F3830  Xbox 0x00177790
void CSWSCreature::DoDamage(int nDamage, int bSuppressFeedback)
{
    int nDamageAmount;
    CSWPartyTable* pParty = g_pAppManager->GetServerExoApp()->GetPartyTable();
    float fMultiplier;
    if ((pParty != NULL && pParty->GetIsMember(GetId())) || GetIsPC())
    {
        int nDifficulty = g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nDifficulty;
        int bFound;
        {
            CExoString sColumn("MULTIPLIER");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pDifficultyOptions);
            C2DA* pTable = p2DArrays->m_pDifficultyOptions;
            bFound = pTable->GetFLOATEntry(nDifficulty, sColumn, &fMultiplier);
        }
        if (bFound)
        {
            nDamageAmount = (int)(nDamage * fMultiplier);
        }
        else
        {
            nDamageAmount = nDamage;
        }
    }
    else
    {
        nDamageAmount = nDamage;
    }

    if (!GetInParty() && g_nIncreaseDamage > 1)
    {
        nDamageAmount *= g_nIncreaseDamage;
    }

    short nTemporaryHitPoints = m_nTemporaryHitPoints;
    if (nTemporaryHitPoints > 0)
    {
        int nRemainingHitPoints = 0;
        int i = 0;
        CExoArrayList<CGameEffect*>* pEffects = &AsSWSCreature()->m_lstEffects;
        if (pEffects->num > 0)
        {
            for (i = m_pStats->m_nEffectIndex12C;
                 i < pEffects->num; i++)
            {
                CGameEffect* pEffect = (*pEffects)[i];
                if ((*pEffects)[i]->GetEffectType() > CSWSEffectListHandler::EFFECT_TEMPORARY_HITPOINTS)
                {
                    break;
                }
                if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_TEMPORARY_HITPOINTS)
                {
                    int nHitPoints = pEffect->GetInteger(0);
                    if (nHitPoints > nDamageAmount)
                    {
                        nHitPoints -= nDamageAmount;
                        nDamageAmount = 0;
                        pEffect->SetInteger(0, nHitPoints);
                        nRemainingHitPoints += nHitPoints;
                    }
                    else
                    {
                        unsigned __int64 nEffectId = pEffect->m_nID;
                        nDamageAmount -= nHitPoints;
                        AsSWSCreature()->RemoveEffectById(nEffectId);
                    }
                }
            }
        }
        m_nTemporaryHitPoints = nRemainingHitPoints;
    }

    if (g_pAppManager->GetServerExoApp()->GetDifficultyOption(
            CSWRules::DIFFICULTY_PC_MINIMUM_HP) == 1 &&
        GetIsPC() == 1 && GetCurrentHitPoints(1) - nDamageAmount < 1)
    {
        nDamageAmount = GetCurrentHitPoints(1) - 1;
        if (nDamageAmount < 0)
        {
            nDamageAmount = 0;
        }
    }

    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    unsigned long oidClient = pClient->ServerToClientObjectId(GetId());
    if (pClient->GetPlayerCreatureId() == oidClient && !bSuppressFeedback)
    {
        pClient->AddFloatyText(oidClient, CClientExoApp::FLOATY_TEXT_DAMAGE, nDamageAmount);
    }
    CSWSObject::DoDamage(nDamageAmount);
}
// PC 0x004F3A60  Xbox 0x001779D0
int CSWSCreature::EquipMostEffectiveArmor()
{
    if (m_pInventory == NULL || m_pcItemRepository == NULL)
        return FALSE;

    CSWSItem* pBestArmour = NULL;
    unsigned long nBestAC = 0;
    CSWSItem* pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_ARMOUR);
    CSWSItem* pOldArmour = pItem;
    if (pItem != NULL &&
        g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType()) != NULL)
    {
        pBestArmour = pItem;
        nBestAC = pItem->m_nBaseAC;
        for (int nProperty = 0; nProperty < pItem->m_nPassiveProperties; ++nProperty)
        {
            CSWItemProperty* pProperty = pItem->GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                pItem->GetIsUpgradeActive(pProperty->m_nUpgradeType) &&
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS)
                nBestAC += pProperty->m_nCostValue;
        }
    }
    for (unsigned long nItem = 0; nItem < m_pcItemRepository->GetNumItems(); ++nItem)
    {
        pItem = m_pcItemRepository->ItemListGetItem(nItem);
        if (pItem != NULL)
        {
            unsigned long nSlot = INVENTORY_SLOT_ARMOUR;
            if (pItem->GetBaseItem()->m_nItemType >= CSWBaseItem::ITEM_TYPE_ARMOR_FIRST &&
                pItem->GetBaseItem()->m_nItemType <= CSWBaseItem::ITEM_TYPE_ARMOR_LAST &&
                CanEquipItem(pItem, &nSlot, FALSE, FALSE, TRUE))
            {
                if (g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType()) != NULL)
                {
                    unsigned long nAC = pItem->m_nBaseAC;
                    for (int nProperty = 0; nProperty < pItem->m_nPassiveProperties; ++nProperty)
                    {
                        CSWItemProperty* pProperty = pItem->GetPassiveProperty(nProperty);
                        if (pProperty != NULL &&
                            pItem->GetIsUpgradeActive(pProperty->m_nUpgradeType) &&
                            pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS)
                            nAC += pProperty->m_nCostValue;
                    }
                    if (nAC > nBestAC)
                    {
                        pBestArmour = pItem;
                        nBestAC = nAC;
                    }
                }
            }
        }
    }
    if (pBestArmour != NULL)
    {
        if (pOldArmour != pBestArmour)
            AddEquipItemActions(pBestArmour, INVENTORY_SLOT_ARMOUR, FALSE, FALSE, FALSE, FALSE, FALSE);
        return TRUE;
    }
    return FALSE;
}
// PC 0x004F3DB0  Xbox 0x00177D20
void CSWSCreature::AddGold(int nGold, int bFeedback)
{
    int nNewGold = GetGold() + nGold;
    if (nNewGold > MAX_GOLD)
    {
        nNewGold = MAX_GOLD;
    }

    int nGoldAdded = nNewGold - GetGold();
    SetGold(nNewGold);

    if (bFeedback == 1 && nGoldAdded != 0)
    {
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        pMessageData->SetInteger(0, nGoldAdded);
        SendFeedbackMessage(FEEDBACK_GOLD_ADDED, pMessageData);
    }
}
// PC 0x004FCE90  Xbox 0x00180DF0
int CSWSCreature::AddCounterSpellActions(unsigned long oidTarget)
{
    Vector vTarget;
    if (oidTarget == OBJECT_INVALID)
        return 0;

    CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()
        ->GetCreatureByGameObjectID(oidTarget);
    if (pTarget == NULL)
        return 0;
    if (GetArea() == NULL)
        return 0;
    if (oidTarget == GetId())
        return 0;
    if (pTarget->GetArea() == NULL)
        return 0;
    {
        CSWSArea* pArea = pTarget->GetArea();
        if (pArea == NULL)
            return ACTION_IN_PROGRESS;

        unsigned long oidArea = pArea->GetId();
        vTarget = pTarget->m_vPosition;
        float fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_LONG];
        int bRun = 1;
        int bCheckRange = 1;
        if (!(m_nAIStateFlags & AI_STATE_CAN_MOVE))
            return 0;
        {
            AddActionToFront(ACTION_COUNTER_SPELL, ACTION_GROUP_NEW,
                ACTION_PARAM_OBJECT, &oidTarget,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, ACTION_GROUP_LAST,
                ACTION_PARAM_OBJECT, &oidTarget,
                ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bCheckRange,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            AddMoveToPointActionToFront(ACTION_GROUP_LAST, vTarget, oidArea,
                OBJECT_INVALID, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
                Vector(), 0, 0, 0);
            m_oidCounterSpellTarget = oidTarget;
            return 1;
        }
    }
    return 0;
}
// PC 0x004FD070  Xbox 0x00180FD0
int CSWSCreature::AddUseTalentAtLocationActions(
    int nTalentType, int nTalentId, const Vector& vTarget,
    unsigned char nMultiClass, unsigned long oidItem, int nItemProperty,
    unsigned char nSpellOption11, unsigned char nSpellOption2)
{
    unsigned long oidPowerItem;
    int nPowerItemProperty;

    if (m_bCommandable != 0 && GetArea() != NULL &&
        GetArea()->GetId() != OBJECT_INVALID)
    {
        if (nTalentType == CScriptTalent::TALENT_TYPE_SPELL)
        {
            if (oidItem != OBJECT_INVALID &&
                nItemProperty != ITEM_PROPERTY_INVALID)
            {
                ClearAllActions(1);
                UseItem(oidItem, nItemProperty, 0, OBJECT_INVALID, vTarget,
                        GetArea()->GetId(), 0);
                return 1;
            }
            if (m_pStats->GetItemDetailsForPower(
                    nTalentId, oidPowerItem, nPowerItemProperty) == 1)
            {
                ClearAllActions(1);
                UseItem(oidPowerItem, nPowerItemProperty, 0, OBJECT_INVALID,
                        vTarget, GetArea()->GetId(), 0);
                return 1;
            }
            AddCastSpellActions(nTalentId, nMultiClass, 0, nSpellOption2, 0,
                                vTarget, OBJECT_INVALID, 0, 1, 0, 0, 0, 0,
                                -1, nSpellOption11, 0);
        }
        return 1;
    }
    return 0;
}
// PC 0x004FD1E0  Xbox 0x00181140
void CSWSCreature::Rest(int bFlag)
{
    CExoString sRest;
    int nRestState = 0;
    if (GetDead() == 0 && GetIsPCDying() == 0)
    {
        int nPlayerState = GetIsPC();
        if (nPlayerState != 0 && GetArea() != NULL && GetArea()->m_bNoRest != 0)
        {
            SendFeedbackMessage(FEEDBACK_REST_NOT_ALLOWED, NULL);
        }
        else if (GetNearestEnemy(30.0f, OBJECT_INVALID, OBJECT_INVALID) != OBJECT_INVALID)
        {
            SendFeedbackMessage(FEEDBACK_REST_ENEMIES_NEARBY, NULL);
        }
        else
        {
            ClearAllActions(1);
            if (nPlayerState != 0 && m_nExcitedStateTimer > 0)
            {
                SendFeedbackMessage(FEEDBACK_REST_IN_COMBAT, NULL);
            }
            else
            {
                m_nUnknown994 = 0;
                m_nUnknown998 = 0;
                m_nUnknown99C = 0;
                m_nUnknown9A0 = 0;
                ClearActivities(ACTIVITY_GROUP_BASIC);
                SetActivity(ACTIVITY_REST, TRUE, 0);
                ActionManager(ACTION_MANAGER_REST);
                if (nPlayerState != 0)
                {
                    SendFeedbackMessage(FEEDBACK_REST_STARTED, NULL);
                    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_REST;
                    pEvent->SetInteger(0, CScriptEvent::REST_EVENT_STARTED);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventAbsoluteTime(
                        0, 0, GetId(), pModule->GetId(),
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
                AddAction(ACTION_REST, ACTION_GROUP_NEW,
                    ACTION_PARAM_INTEGER, &nPlayerState, ACTION_PARAM_INTEGER, &nRestState,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            }
        }
    }
}
// PC 0x004FD490  Xbox 0x00170BB0
short CSWSCreature::GetMaxForcePoints()
{
    if (GetStats()->m_nRace == CSWRace::CRACE_DROID)
        return 0;

    if (GetStats()->m_bIsPC &&
        g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nControlledNPC == -1)
    {
        int nClass = GetStats()->GetClass(GetStats()->m_nClassCount - 1);
        if (!CSWClass::IsJedi(nClass))
            return 0;

        int nForcePoints = 0;
        for (int nLevel = 0; nLevel < GetStats()->GetLevel(0); ++nLevel)
        {
            int nForceDie = GetStats()->GetLevelStats(nLevel)->GetForceDie();
            if (nForceDie)
            {
                nForceDie += GetStats()->GetWISMod() + GetStats()->GetCHAMod();
                nForcePoints += max(1, nForceDie);
            }
        }
        if (GetStats()->HasFeat(CSWFeat::FEAT_FORCE_SENSITIVE))
            nForcePoints += CSWSCreatureStats::FORCE_SENSITIVE_FORCE_POINT_BONUS;
        for (unsigned char nClass = 0; nClass < GetStats()->m_nClassCount; ++nClass)
        {
            if (GetStats()->GetClass(nClass) == CSWClass::CCLASS_JEDICONSULAR &&
                HasEffect(CSWSEffectListHandler::EFFECT_PUREEVILPOWERS))
            {
                nForcePoints += CSWSEffectListHandler::PURE_EVIL_FORCE_POINT_BONUS;
                break;
            }
        }
        return nForcePoints;
    }
    else
    {
        short nForcePoints = GetStats()->m_nForcePoints;
        nForcePoints += GetStats()->GetWISMod() * GetStats()->GetLevel(0);
        nForcePoints += GetStats()->GetCHAMod() * GetStats()->GetLevel(0);
        if (GetStats()->HasFeat(CSWFeat::FEAT_FORCE_SENSITIVE))
            nForcePoints += CSWSCreatureStats::FORCE_SENSITIVE_FORCE_POINT_BONUS;
        for (unsigned char nClass = 0; nClass < GetStats()->m_nClassCount; ++nClass)
        {
            if (GetStats()->GetClass(nClass) == CSWClass::CCLASS_JEDICONSULAR &&
                HasEffect(CSWSEffectListHandler::EFFECT_PUREEVILPOWERS))
            {
                nForcePoints += CSWSEffectListHandler::PURE_EVIL_FORCE_POINT_BONUS;
                break;
            }
        }
        if (GetStats()->GetIsJedi())
        {
            return max(GetStats()->GetLevel(0), nForcePoints);
        }
        return 0;
    }
}
// PC 0x004FD6F0  Xbox 0x001813F0
int CSWSCreature::CalculateDamagePower(CSWSObject* pObject)
{
    int nDamage = 0;

    return GetTotalEffectBonus(1, pObject, nDamage, 0, 0, 0, 0, 0xFF, 0xFF, 0);
}
// PC 0x004FD730  Xbox 0x00181430
int CSWSCreature::TransferGold(unsigned long oidTarget,
                              const Vector& vPosition, unsigned long nAmount)
{
    if (nAmount <= 0)
    {
        return 0;
    }

    int nGold = nAmount;
    if ((unsigned long)nGold > (unsigned long)GetGold())
    {
        nGold = GetGold();
    }

    CSWSItem* pGold = new CSWSItem(OBJECT_INVALID);
    CResRef cGoldResRef("g_i_credits001");
    pGold->LoadFromTemplate(cGoldResRef);
    pGold->m_nStackSize = nGold;
    pGold->SetPossessor(GetId(), FALSE, FALSE, OBJECT_INVALID);

    if (pGold != NULL)
    {
        if (oidTarget != OBJECT_INVALID)
        {
            CGameObject* pTarget =
                g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);
            if (pTarget != NULL)
            {
                if (pTarget->AsSWSCreature() != NULL)
                {
                    GetItemRepository(TRUE)->AddItem(&pGold, TRUE, FALSE, TRUE);
                    RemoveGold(nGold, TRUE);
                    return 1;
                }
                else if (pTarget->AsSWSPlaceable() != NULL)
                {
                    CSWSPlaceable* pPlaceable = pTarget->AsSWSPlaceable();
                    if (!pPlaceable->m_bLocked)
                    {
                        AddGiveItemActions(pGold->GetId(), oidTarget, -1, FALSE);
                        return 1;
                    }
                }
                else if (pTarget->AsSWSItem() != NULL)
                {
                    if (g_pRules->m_pBaseItems->GetBaseItem(
                            pTarget->AsSWSItem()->GetBaseItemType())->m_bIsContainer)
                    {
                        AddGiveItemActions(pGold->GetId(), oidTarget, -1, FALSE);
                        return 1;
                    }
                }
            }
        }
        else
        {
            AddDropItemActions(pGold->GetId(), vPosition, TRUE);
            return 1;
        }
    }
    delete pGold;
    return 0;
}
// PC 0x004F3ED0  Xbox 0x00177E40
void CSWSCreature::RemoveGold(int nGold, int bFeedback)
{
    int nNewGold = GetGold();
    nNewGold -= nGold;
    if (nNewGold < 0)
    {
        nNewGold = 0;
    }

    int nGoldRemoved = GetGold() - nNewGold;
    SetGold(nNewGold);

    if (bFeedback == 1 && nGoldRemoved != 0)
    {
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        pMessageData->SetInteger(0, nGoldRemoved);
        SendFeedbackMessage(FEEDBACK_GOLD_REMOVED, pMessageData);
    }
}
// PC 0x004EDD90  Xbox 0x00171750
void CSWSCreature::SetGold(int nGold)
{
    if (nGold > 999999999)
    {
        nGold = 999999999;
    }

    if (m_bInParty != 0)
    {
        g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nGold = nGold;
        return;
    }

    m_nGold = nGold;
}
// PC 0x004EDDD0  Xbox 0x00171790
int CSWSCreature::DecrementSpellReadyCount(unsigned long nSpell,
                                           unsigned char nClass,
                                           unsigned char nSpellLevel,
                                           unsigned char nMetamagic,
                                           unsigned char nSpellLikeClass)
{
    if (m_bIgnoreSpellReadyCount != 0)
    {
        return 1;
    }

    if (nClass == 0xFE)
    {
        m_pStats->SetSpellLikeAbilityReady(nSpell, 0, nSpellLikeClass);
        return 1;
    }

    return m_pStats->UnReadySpell(nSpell);
}
// PC 0x004EDE10  Xbox 0x001717D0
void CSWSCreature::SendFeedbackMessage(unsigned short nMessage,
                                      CSWCCMessageData* pMessageData)
{
    CSWSPlayer* pPlayer =
        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL)
    {
        if (pMessageData == NULL)
        {
            pMessageData = new CSWCCMessageData;
            pMessageData->SetInteger(CSWSMessage::FEEDBACK_INTEGER_MESSAGE, nMessage);
        }
        else
        {
            pMessageData->SetInteger(CSWSMessage::FEEDBACK_INTEGER_MESSAGE, nMessage);
        }
        CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        pMessage->SendServerToPlayerCCMessage(pPlayer->m_nPlayerId,
            CSWSMessage::CC_MESSAGE_FEEDBACK, pMessageData, NULL);
    }
    if (pMessageData != NULL)
    {
        delete pMessageData;
    }
}
// PC 0x004EDED0  Xbox 0x00171890
void CSWSCreature::AutoCloseGUIPanels(int bForce)
{
    CSWSPlayer* pPlayer;
    CGameObject* pObject;
    CSWSItem* pItem;
    CSWSPlaceable* pPlaceable;
    CGameObjectArray* pObjectArray;
    unsigned long oidContainer;

    pPlayer =
        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL && pPlayer->m_pContainerGUI->m_oidContainer !=
                               CGameObjectArray::INVALIDOBJID)
    {
        pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
        oidContainer = pPlayer->m_pContainerGUI->m_oidContainer;
        if (pObjectArray->GetGameObject(oidContainer, &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
            {
                pItem = pObject->AsSWSItem();
                if (pItem != NULL)
                {
                    pItem->CloseInventory(m_nId, 1);
                }
            }
            else if (pObject->m_nObjectType ==
                     CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                pPlaceable = pObject->AsSWSPlaceable();
                if (pPlaceable != NULL)
                {
                    pPlaceable->CloseInventory(m_nId, 1);
                }
            }
        }
    }
}
// PC 0x004EDF80  Xbox 0x00171940
void CSWSCreature::CutWaypointPath()
{
    CPathfindInformation* pPathfind = m_pPathfindInformation;
    ++pPathfind->m_bField_100;
    pPathfind->m_nField_F8 = 1;
    m_pPathfindInformation->m_nWayPointCutState = 0;
    m_pPathfindInformation->m_nField_104 =
        (m_pPathfindInformation->m_nWayPointCount -
         m_pPathfindInformation->m_nField_9C) >> 1;
    m_pPathfindInformation->m_vWayPointOrigin = m_vPosition;
    m_pPathfindInformation->m_vWayPointDirection = m_vOrientation;
}
// PC 0x004EE010  Xbox 0x001719D0
int CSWSCreature::Disguise(int nAppearance, CGameEffect* pEffect,
                           int bKeepPosition)
{
    CExoString sModel;
    CSWSArea* pArea;

    if (m_bPMIsDisguised != 0)
    {
        return 0;
    }

    m_nPMAppearance = m_pStats->m_nAppearance;
    m_pStats->m_nAppearance = nAppearance;
    m_cAppearanceInfo.m_nAppearance = nAppearance;
    UpdatePersonalSpace();
    UpdateAppearanceDependantInfo(1);
    m_bPMIsDisguised = 1;
    if (bKeepPosition == 0)
    {
        pArea = ((CSWSObject*)this)->GetArea();
        if (pArea != NULL)
        {
            CPathfindInformation* pPathfind = m_pPathfindInformation;
            Vector vSafePosition;

            if (pArea->ComputeSafeLocation(m_vPosition, 20.0f, pPathfind, 1,
                                           &vSafePosition))
            {
                ((CSWSObject*)this)->SetPosition(vSafePosition, 1, 1, 0);
            }
            else
            {
                vSafePosition = m_vPosition;
            }
        }
    }
    return 1;
}
// PC 0x004EE150  Xbox 0x00171B10
int CSWSCreature::UnDisguise(CGameEffect* pEffect)
{
    CSWSArea* pArea;

    if (m_bPMIsDisguised != 0)
    {
        m_pStats->m_nAppearance = m_nPMAppearance;
        m_cAppearanceInfo.m_nAppearance = m_nPMAppearance;
        UpdatePersonalSpace();
        UpdateAppearanceDependantInfo(1);
        m_bPMIsDisguised = 0;
        pArea = ((CSWSObject*)this)->GetArea();
        if (pArea != NULL)
        {
            CPathfindInformation* pPathfind = m_pPathfindInformation;
            Vector vSafePosition;

            if (pArea->ComputeSafeLocation(m_vPosition, 20.0f, pPathfind, 1,
                                           &vSafePosition))
            {
                ((CSWSObject*)this)->SetPosition(vSafePosition, 1, 1, 0);
            }
        }
    }
    return 1;
}
// PC 0x004EE210  Xbox 0x00171BD0
int CSWSCreature::GetBlind()
{
    unsigned char nFlags = m_nVisionFlags;
    return (nFlags & VISION_BLIND) != 0 ||
           ((nFlags & VISION_DARKNESS) != 0 &&
            (nFlags & VISION_ULTRAVISION) == 0);
}
// PC 0x004EE230  Xbox 0x00171BF0
int CSWSSpellScriptData::SaveData(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldINT(pStruct, m_nSpellId, "SpellId");
    pRes->WriteFieldDWORD(pStruct, m_oidCaster, "CasterId");
    pRes->WriteFieldDWORD(pStruct, m_oidTarget, "TargetId");
    pRes->WriteFieldDWORD(pStruct, m_oidArea, "AreaId");
    pRes->WriteFieldDWORD(pStruct, m_oidItem, "ItemId");
    pRes->WriteFieldCExoString(pStruct, m_sScript, "Script");
    pRes->WriteFieldFLOAT(pStruct, m_vTargetPosition.m_fX, "TargetPosX");
    pRes->WriteFieldFLOAT(pStruct, m_vTargetPosition.m_fY, "TargetPosY");
    pRes->WriteFieldFLOAT(pStruct, m_vTargetPosition.m_fZ, "TargetPosZ");
    return 1;
}
// PC 0x004EE2E0  Xbox 0x00171CA0
int CSWSSpellScriptData::LoadData(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    m_nSpellId = pRes->ReadFieldINT(pStruct, "SpellId", bSuccess, 0);
    m_oidCaster = pRes->ReadFieldDWORD(pStruct, "CasterId", bSuccess, 0);
    m_oidTarget = pRes->ReadFieldDWORD(pStruct, "TargetId", bSuccess, 0);
    m_oidArea = pRes->ReadFieldDWORD(pStruct, "AreaId", bSuccess, 0);
    m_oidItem = pRes->ReadFieldDWORD(pStruct, "ItemId", bSuccess, 0);

    {
        CExoString sScript("");
        m_sScript =
            pRes->ReadFieldCExoString(pStruct, "Script", bSuccess, sScript);
    }

    m_vTargetPosition.m_fX =
        pRes->ReadFieldFLOAT(pStruct, "TargetPosX", bSuccess, 0.0f);
    m_vTargetPosition.m_fY =
        pRes->ReadFieldFLOAT(pStruct, "TargetPosY", bSuccess, 0.0f);
    m_vTargetPosition.m_fZ =
        pRes->ReadFieldFLOAT(pStruct, "TargetPosZ", bSuccess, 0.0f);

    return 1;
}
// PC 0x004F3FE0  Xbox 0x00177F50
int CSWSCreature::GetTotalEffectBonus(unsigned char nEffectType, CSWSObject* pObject,
    int& nCapped, int nA, int nB, unsigned char nSavingThrow, unsigned char nC,
    unsigned char nD, unsigned char nE, int nF)
{
    nCapped = 0;
    CExoArrayList<CGameEffect*>& lstEffects = m_lstEffects;
    int nAttack = m_pcCombatRound->m_nCurrentAttack;
    CSWSCombatAttackData* pAttack = GetCombatRound()->GetAttack(nAttack);
    int nWeaponType = pAttack->m_nWeaponAttackType;
    if (nWeaponType == CSWSCombatRound::WEAPON_ATTACK_AUTOMATIC || nB != 0)
        nWeaponType = (nF != 0) + 1;
    int nRace;
    int nAlignment;
    int nEffectRace;
    CSWSItem* pItem;
    int nEffect;
    int bFound;
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        nRace = pObject->AsSWSCreature()->GetStats()->m_nRace;
        nAlignment = pObject->AsSWSCreature()->GetStats()->GetSimpleAlignmentGoodEvil();
    }
    else
    {
        nRace = -1;
        nAlignment = -1;
    }
    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
    {
        m_nEffectBonus[nStack] = -1;
        m_nEffectPenalty[nStack] = -1;
        m_nEffectBonusSpell[nStack] = CGameEffect::SPELL_ID_INVALID;
        m_nEffectPenaltySpell[nStack] = CGameEffect::SPELL_ID_INVALID;
        m_oidEffectBonusCreator[nStack] = OBJECT_INVALID;
        m_oidEffectPenaltyCreator[nStack] = OBJECT_INVALID;
    }
    int nTotalBonus;
    int nTotalPenalty;
    switch (nEffectType)
    {
    case EFFECT_BONUS_ATTACK:
    {
        nTotalBonus = 0;
        nTotalPenalty = 0;
        int nPenalty = 0;
        int nBonus = 0;
        if (lstEffects.num > 0)
        {
            for (nEffect = GetStats()->m_nAttackEffectIndex;
                 nEffect < lstEffects.num; ++nEffect)
            {
                CGameEffect* pEffect = lstEffects[nEffect];
                if (pEffect->GetEffectType() > CSWSEffectListHandler::EFFECT_ATTACK_DECREASE)
                    break;
                bFound = 0;
                int nEffectWeapon = pEffect->GetInteger(1);
                nEffectRace = pEffect->GetInteger(2);
                int nEffectAlignment = pEffect->GetInteger(4);
                int bWeapon = 0;
                if (nEffectWeapon == CSWSEffectListHandler::ATTACK_BONUS_MISC || nEffectWeapon == nWeaponType)
                    bWeapon = 1;
                if (nWeaponType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND && (nEffectWeapon == CSWSEffectListHandler::ATTACK_BONUS_ONHAND || nEffectWeapon == CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1))
                    bWeapon = 1;
                if (nWeaponType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED && nEffectWeapon == CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
                    bWeapon = 1;
                if (nEffectRace != g_pRules->m_nNumRaces && nEffectRace != nRace)
                    continue;
                if (nEffectAlignment != 0 && nEffectAlignment != nAlignment)
                    continue;
                if (bWeapon != 1)
                    continue;

                if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE)
                {
                    if (nEffectWeapon != CSWSEffectListHandler::ATTACK_BONUS_MISC)
                    {
                        nBonus += pEffect->GetInteger(0);
                        continue;
                    }
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int i = 0; i < EFFECT_STACK_COUNT; ++i)
                        {
                            if (m_nEffectBonusSpell[i] == pEffect->GetSpellId())
                            {
                                bFound = 1;
                                if (m_nEffectBonus[i] < pEffect->GetInteger(0))
                                {
                                    m_nEffectBonus[i] = pEffect->GetInteger(0);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int i = 0; i < EFFECT_STACK_COUNT; ++i)
                        {
                            if (m_nEffectBonus[i] == -1)
                            {
                                m_nEffectBonusSpell[i] = pEffect->GetSpellId();
                                m_nEffectBonus[i] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                }
                else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ATTACK_DECREASE)
                {
                    if (nEffectWeapon != CSWSEffectListHandler::ATTACK_BONUS_MISC)
                    {
                        nPenalty += pEffect->GetInteger(0);
                        continue;
                    }
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int i = 0; i < EFFECT_STACK_COUNT; ++i)
                        {
                            if (m_nEffectPenaltySpell[i] == pEffect->GetSpellId())
                            {
                                bFound = 1;
                                if (m_nEffectPenalty[i] < pEffect->GetInteger(0))
                                {
                                    m_nEffectPenalty[i] = pEffect->GetInteger(0);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int i = 0; i < EFFECT_STACK_COUNT; ++i)
                        {
                            if (m_nEffectPenalty[i] == -1)
                            {
                                m_nEffectPenaltySpell[i] = pEffect->GetSpellId();
                                m_nEffectPenalty[i] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                }
            }
        }
        nTotalBonus += nBonus;
        nTotalPenalty += nPenalty;
        for (int i = 0; i < EFFECT_STACK_COUNT; ++i)
        {
            if (m_nEffectBonus[i] != -1)
                nTotalBonus += m_nEffectBonus[i];
            if (m_nEffectPenalty[i] != -1)
                nTotalPenalty += m_nEffectPenalty[i];
        }
        break;
    }

case EFFECT_BONUS_DAMAGE:
{
    int anTypedBonus[CSWSCombatAttackData::DAMAGE_COUNT] = {0};
    int anTypedPenalty[CSWSCombatAttackData::DAMAGE_COUNT] = {0};
    int anBonusType[EFFECT_STACK_COUNT] = {0};
    int anPenaltyType[EFFECT_STACK_COUNT] = {0};
    int nNumDice;
    int nDie;
    int nDamageIndex;
    C2DA* pDamage;
    int nRank = 0;
    int nOldRank = 0;
    nNumDice = 0;
    nDie = 0;
    nTotalBonus = 0;
    nTotalPenalty = 0;
    pDamage = g_pRules->Get2DArrays()->m_pIPRPDamage;
    if (lstEffects.num > 0)
    {
        for (nEffect = GetStats()->m_nDamageIncreaseEffectIndex;
             nEffect < lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = lstEffects[nEffect];
            if (pEffect->GetEffectType() > CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
                break;
            bFound = FALSE;
            int nDamageType = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE);
            nEffectRace = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP);
            int nEffectAlignment = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2);
            int nEffectWeapon = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON);
            int bWeaponMatch = FALSE;
            if (nEffectWeapon == CSWSEffectListHandler::DAMAGE_BONUS_MISC)
                bWeaponMatch = TRUE;
            if (nEffectWeapon == nWeaponType)
                bWeaponMatch = TRUE;
            if (pObject != NULL && pObject->AsSWSItem() != NULL &&
                pObject->GetId() != pEffect->m_oidCreator)
                bWeaponMatch = FALSE;
            if (nWeaponType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND &&
                (nEffectWeapon == CSWSEffectListHandler::DAMAGE_BONUS_ONHAND ||
                 nEffectWeapon == CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1))
                bWeaponMatch = TRUE;
            if (nWeaponType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED &&
                nEffectWeapon == CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
                bWeaponMatch = TRUE;

            if (!(((nEffectRace == g_pRules->m_nNumRaces || nEffectRace == nRace) &&
                   (nEffectAlignment == 0 || nEffectAlignment == nAlignment) && bWeaponMatch == TRUE &&
                   ((nA == 0 && (nDamageType & DAMAGE_TYPE_PHYSICAL_MASK)) || (nDamageType & ~DAMAGE_TYPE_PHYSICAL_MASK))) ||
                  (nA == 1 && !(nDamageType & DAMAGE_TYPE_PHYSICAL_MASK))))
                continue;
            nDamageIndex = Log_Base2(nDamageType);
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
            {
                if (nEffectWeapon != CSWSEffectListHandler::DAMAGE_BONUS_MISC)
                {
                    pDamage->GetINTEntry(pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS),
                                         "Rank", &nRank);
                    nOldRank = pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                    int nDamage = 0;
                    if (nOldRank > 5)
                    {
                        pDamage->GetINTEntry(nOldRank, "NumDice", &nNumDice);
                        pDamage->GetINTEntry(nOldRank, "Die", &nDie);
                        if (nB)
                        {
                            nDamage = nNumDice * nDie;
                            nCapped += nNumDice - nDamage;
                        }
                        else
                        {
                            int bCritical = pAttack->m_nAttackResult ==
                                CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT;
                            int nMultiplier = bCritical ? GetStats()->GetCriticalHitMultiplier(nF) : 1;
                            for (int nRoll = nMultiplier; nRoll > 0; --nRoll)
                                nDamage += g_pRules->RollDice(nNumDice, nDie);
                        }
                    }
                    else
                        nDamage = nOldRank;
                    anTypedBonus[nDamageIndex] += nDamage;
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonusSpell[nStack] == pEffect->GetSpellId() &&
                                anBonusType[nStack] == nDamageType)
                            {
                                bFound = TRUE;
                                pDamage->GetINTEntry(m_nEffectBonus[nStack], "Rank", &nOldRank);
                                if (nRank > nOldRank)
                                {
                                    m_nEffectBonus[nStack] = pEffect->GetInteger(
                                        CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_nEffectBonusSpell[nStack] = pEffect->GetSpellId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(
                                    CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                                anBonusType[nStack] = nDamageType;
                                break;
                            }
                        }
                    }
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
            {
                if (nEffectWeapon != CSWSEffectListHandler::ATTACK_BONUS_MISC)
                {
                    int nPenaltyRank;
                    pDamage->GetINTEntry(anTypedPenalty[nDamageIndex], "Rank", &nPenaltyRank);
                    if (nRank > nPenaltyRank)
                        anTypedPenalty[nDamageIndex] = pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenaltySpell[nStack] == pEffect->GetSpellId() &&
                                anPenaltyType[nStack] == nDamageType)
                            {
                                bFound = TRUE;
                                pDamage->GetINTEntry(m_nEffectPenalty[nStack], "Rank", &nOldRank);
                                if (nRank > nOldRank)
                                {
                                    m_nEffectPenalty[nStack] = pEffect->GetInteger(
                                        CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_nEffectPenaltySpell[nStack] = pEffect->GetSpellId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(
                                    CSWSEffectListHandler::DAMAGE_INTEGER_BONUS);
                                anPenaltyType[nStack] = nDamageType;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    int nLastDamageType;
    for (int nType = 0; nType < CSWSCombatAttackData::DAMAGE_COUNT; ++nType)
    {
        int nDamage = anTypedBonus[nType];
        if (nDamage > 0)
        {
            nLastDamageType = 1 << nType;
            if (pObject != NULL)
            {
                nDamage = pObject->DoDamageImmunity(this, nDamage, nLastDamageType, nB, 1);
                nDamage = pObject->DoDamageResistance(this, nDamage, nLastDamageType, nB, 1, 0);
            }
            if (!nB)
                pAttack->AddDamage(nLastDamageType, nDamage, 1);
            nTotalBonus += nDamage;
        }
        nDamage = 0;
        if (anTypedPenalty[nType] > 0)
        {
            nDamage = anTypedPenalty[nType];
            pAttack->AddDamage(nLastDamageType, -nDamage, 1);
        }
        nTotalPenalty += nDamage;
    }
    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
    {
        int nDamage = 0;
        if (m_nEffectBonus[nStack] != -1)
        {
            nDamageIndex = Log_Base2(anBonusType[nStack]);
            if (m_nEffectBonus[nStack] > 5)
            {
                pDamage->GetINTEntry(m_nEffectBonus[nStack], "NumDice", &nNumDice);
                pDamage->GetINTEntry(m_nEffectBonus[nStack], "Die", &nDie);
                if (nB == 1)
                    nDamage = nNumDice * nDie;
                else if (pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT)
                {
                    for (int nRoll = 0; nRoll < GetStats()->GetCriticalHitMultiplier(nF); ++nRoll)
                        nDamage += g_pRules->RollDice(nNumDice, nDie);
                }
                else
                    nDamage = g_pRules->RollDice(nNumDice, nDie);
                if (nA == 1)
                {
                    if (nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_BLUDGEONING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_PIERCING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_SLASHING)
                    {
                        if (pObject != NULL)
                        {
                            nDamage = pObject->DoDamageImmunity(this, nDamage, nLastDamageType, nB, 1);
                            nDamage = pObject->DoDamageResistance(this, nDamage, nLastDamageType, nB, 1, 0);
                        }
                        if (nB)
                            nTotalBonus += nDamage;
                        else
                            pAttack->AddDamage(anBonusType[nStack], nDamage, 1);
                    }
                }
                else
                    nTotalBonus += nDamage;
            }
            else
            {
                if (pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT)
                {
                    for (int nRoll = 0; nRoll < GetStats()->GetCriticalHitMultiplier(nF); ++nRoll)
                        nDamage += m_nEffectBonus[nStack];
                }
                else
                    nDamage = m_nEffectBonus[nStack];
                if (nA == 1)
                {
                    if (nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_BLUDGEONING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_PIERCING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_SLASHING)
                    {
                        if (pObject != NULL)
                        {
                            nDamage = pObject->DoDamageImmunity(this, nDamage, nLastDamageType, nB, 1);
                            nDamage = pObject->DoDamageResistance(this, nDamage, nLastDamageType, nB, 1, 0);
                        }
                        if (nB)
                            nDamage *= 2;
                        else
                            pAttack->AddDamage(anBonusType[nStack], nDamage, 1);
                    }
                }
                else
                    nTotalBonus += nDamage;
            }
        }
        if (m_nEffectPenalty[nStack] != -1)
        {
            nDamageIndex = Log_Base2(anPenaltyType[nStack]);
            if (m_nEffectPenalty[nStack] > 5)
            {
                pDamage->GetINTEntry(m_nEffectPenalty[nStack], "NumDice", &nNumDice);
                pDamage->GetINTEntry(m_nEffectPenalty[nStack], "Die", &nDie);
                if (nB == 1)
                    nDamage = nNumDice * nDie;
                else if (pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT)
                {
                    for (int nRoll = 0; nRoll < GetStats()->GetCriticalHitMultiplier(nF); ++nRoll)
                        nDamage += g_pRules->RollDice(nNumDice, nDie);
                }
                else
                    nDamage = g_pRules->RollDice(nNumDice, nDie);
                if (nA == 1)
                {
                    if (nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_BLUDGEONING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_PIERCING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_SLASHING)
                        pAttack->AddDamage(anPenaltyType[nStack], -nDamage, 1);
                }
                else
                    nTotalPenalty += nDamage;
            }
            else
            {
                if (pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT)
                {
                    for (int nRoll = 0; nRoll < GetStats()->GetCriticalHitMultiplier(nF); ++nRoll)
                        nDamage += m_nEffectPenalty[nStack];
                }
                else
                    nDamage = m_nEffectPenalty[nStack];
                if (nA == 1)
                {
                    if (nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_BLUDGEONING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_PIERCING &&
                        nDamageIndex != CSWSCombatAttackData::DAMAGE_INDEX_SLASHING)
                        pAttack->AddDamage(anPenaltyType[nStack], -nDamage, 1);
                }
                else
                    nTotalPenalty += nDamage;
            }
        }
    }
    if (nTotalBonus > EFFECT_BONUS_LIMIT)
        nTotalBonus = EFFECT_BONUS_LIMIT;
    if (nTotalPenalty > EFFECT_BONUS_LIMIT)
        nTotalPenalty = EFFECT_BONUS_LIMIT;
    return nTotalBonus - nTotalPenalty;
}

case EFFECT_BONUS_SAVING_THROW:
{
    int nStack;
    if (lstEffects.num > 0)
    {
        for (nEffect = GetStats()->m_nSavingThrowEffectIndex;
             nEffect < lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = lstEffects[nEffect];
            if (pEffect->GetEffectType() > CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE)
                break;
            bFound = FALSE;
            pEffect->GetInteger(0);
            int nSave = pEffect->GetInteger(1);
            int nVersus = pEffect->GetInteger(2);
            nEffectRace = pEffect->GetInteger(3);
            int nEffectAlignment = pEffect->GetInteger(5);
            if ((nSave != 0 && nSave != nSavingThrow) ||
                (nVersus != 0 && nVersus != nC) ||
                (nEffectRace != g_pRules->GetNumRaces() && nEffectRace != nRace) ||
                (nEffectAlignment != 0 && nEffectAlignment != nAlignment))
                continue;
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE)
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEffect->GetCreator());
                if (pItem != NULL)
                {
                    for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                    {
                        if (m_oidEffectBonusCreator[nStack] == pItem->GetId())
                        {
                            bFound = TRUE;
                            if (m_nEffectBonus[nStack] < pEffect->GetInteger(0))
                            {
                                m_nEffectBonus[nStack] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_oidEffectBonusCreator[nStack] = pItem->GetId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonusSpell[nStack] == pEffect->GetSpellId())
                            {
                                bFound = TRUE;
                                if (m_nEffectBonus[nStack] < pEffect->GetInteger(0))
                                {
                                    m_nEffectBonus[nStack] = pEffect->GetInteger(0);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_nEffectBonusSpell[nStack] = pEffect->GetSpellId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE)
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEffect->GetCreator());
                if (pItem != NULL)
                {
                    for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                    {
                        if (m_oidEffectPenaltyCreator[nStack] == pItem->GetId())
                        {
                            bFound = TRUE;
                            if (m_nEffectPenalty[nStack] < pEffect->GetInteger(0))
                            {
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_oidEffectPenaltyCreator[nStack] = pItem->GetId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenaltySpell[nStack] == pEffect->GetSpellId())
                            {
                                bFound = TRUE;
                                if (m_nEffectPenalty[nStack] < pEffect->GetInteger(0))
                                {
                                    m_nEffectPenalty[nStack] = pEffect->GetInteger(0);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_nEffectPenaltySpell[nStack] = pEffect->GetSpellId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(0);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    nTotalBonus = nTotalPenalty = 0;
    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
    {
        if (m_nEffectBonus[nStack] != -1)
            nTotalBonus += m_nEffectBonus[nStack];
        if (m_nEffectPenalty[nStack] != -1)
            nTotalPenalty += m_nEffectPenalty[nStack];
    }
    break;
}

case EFFECT_BONUS_ABILITY:
{
    int nBonus;
    int nPenalty;
    nBonus = nPenalty = 0;
    if (lstEffects.num > 0)
    {
        for (nEffect = GetStats()->m_nAbilityEffectIndex;
             nEffect < lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = lstEffects[nEffect];
            if (pEffect->GetEffectType() > CSWSEffectListHandler::EFFECT_ABILITY_DECREASE)
                break;
            bFound = FALSE;
            int nAbility = pEffect->GetInteger(0);
            pEffect->GetInteger(1);
            if (nAbility != nE)
                continue;
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ABILITY_INCREASE)
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEffect->GetCreator());
                if (pItem != NULL)
                {
                    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                    {
                        if (m_oidEffectBonusCreator[nStack] == pItem->GetId())
                        {
                            bFound = TRUE;
                            if (m_nEffectBonus[nStack] < pEffect->GetInteger(1))
                            {
                                m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_oidEffectBonusCreator[nStack] = pItem->GetId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonusSpell[nStack] == pEffect->GetSpellId())
                            {
                                bFound = TRUE;
                                if (m_nEffectBonus[nStack] < pEffect->GetInteger(1))
                                {
                                    m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_nEffectBonusSpell[nStack] = pEffect->GetSpellId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ABILITY_DECREASE)
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEffect->GetCreator());
                if (pItem != NULL)
                {
                    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                    {
                        if (m_oidEffectPenaltyCreator[nStack] == pItem->GetId())
                        {
                            bFound = TRUE;
                            m_nEffectPenalty[nStack] += pEffect->GetInteger(1);
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_oidEffectPenaltyCreator[nStack] = pItem->GetId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenaltySpell[nStack] == pEffect->GetSpellId())
                            {
                                bFound = TRUE;
                                m_nEffectPenalty[nStack] += pEffect->GetInteger(1);
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_nEffectPenaltySpell[nStack] = pEffect->GetSpellId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
    {
        if (m_nEffectBonus[nStack] != -1)
            nBonus += m_nEffectBonus[nStack];
        if (m_nEffectPenalty[nStack] != -1)
            nPenalty += m_nEffectPenalty[nStack];
    }
    if (nBonus > EFFECT_ABILITY_BONUS_LIMIT)
        nBonus = EFFECT_ABILITY_BONUS_LIMIT;
    if (nPenalty > EFFECT_ABILITY_PENALTY_LIMIT)
        nPenalty = EFFECT_ABILITY_PENALTY_LIMIT;
    return nBonus - nPenalty;
}

case EFFECT_BONUS_SKILL:
{
    int nBonus;
    int nPenalty;
    nBonus = nPenalty = 0;
    if (lstEffects.num > 0)
    {
        for (nEffect = GetStats()->m_nSkillDecreaseEffectIndex;
             nEffect < lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = lstEffects[nEffect];
            if (pEffect->GetEffectType() > CSWSEffectListHandler::EFFECT_SKILL_DECREASE)
                break;
            bFound = FALSE;
            int nSkill = pEffect->GetInteger(0);
            pEffect->GetInteger(1);
            nEffectRace = pEffect->GetInteger(2);
            int nEffectAlignment = pEffect->GetInteger(4);
            if ((nSkill != EFFECT_SKILL_ALL && nSkill != nD) ||
                (nEffectRace != g_pRules->GetNumRaces() && nEffectRace != nRace) ||
                (nEffectAlignment != 0 && nEffectAlignment != nAlignment))
                continue;
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SKILL_INCREASE)
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEffect->GetCreator());
                if (pItem != NULL)
                {
                    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                    {
                        if (m_oidEffectBonusCreator[nStack] == pItem->GetId())
                        {
                            bFound = TRUE;
                            if (m_nEffectBonus[nStack] < pEffect->GetInteger(1))
                            {
                                m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_oidEffectBonusCreator[nStack] = pItem->GetId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonusSpell[nStack] == pEffect->GetSpellId())
                            {
                                bFound = TRUE;
                                if (m_nEffectBonus[nStack] < pEffect->GetInteger(1))
                                {
                                    m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectBonus[nStack] == -1)
                            {
                                m_nEffectBonusSpell[nStack] = pEffect->GetSpellId();
                                m_nEffectBonus[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SKILL_DECREASE)
            {
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEffect->GetCreator());
                if (pItem != NULL)
                {
                    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                    {
                        if (m_oidEffectPenaltyCreator[nStack] == pItem->GetId())
                        {
                            bFound = TRUE;
                            if (m_nEffectPenalty[nStack] < pEffect->GetInteger(1))
                            {
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_oidEffectPenaltyCreator[nStack] = pItem->GetId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenaltySpell[nStack] == pEffect->GetSpellId())
                            {
                                bFound = TRUE;
                                if (m_nEffectPenalty[nStack] < pEffect->GetInteger(1))
                                {
                                    m_nEffectPenalty[nStack] = pEffect->GetInteger(1);
                                    break;
                                }
                            }
                        }
                    }
                    if (!bFound)
                    {
                        for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
                        {
                            if (m_nEffectPenalty[nStack] == -1)
                            {
                                m_nEffectPenaltySpell[nStack] = pEffect->GetSpellId();
                                m_nEffectPenalty[nStack] = pEffect->GetInteger(1);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    for (int nStack = 0; nStack < EFFECT_STACK_COUNT; ++nStack)
    {
        if (m_nEffectBonus[nStack] != -1)
            nBonus += m_nEffectBonus[nStack];
        if (m_nEffectPenalty[nStack] != -1)
            nPenalty += m_nEffectPenalty[nStack];
    }
    if (nBonus > EFFECT_SKILL_LIMIT)
        nBonus = EFFECT_SKILL_LIMIT;
    if (nPenalty > EFFECT_SKILL_LIMIT)
        nPenalty = EFFECT_SKILL_LIMIT;
    return nBonus - nPenalty;
}
    default:
        return 0;
    }
    if (nTotalBonus > EFFECT_ATTACK_LIMIT)
        nTotalBonus = EFFECT_ATTACK_LIMIT;
    if (nTotalPenalty > EFFECT_ATTACK_LIMIT)
        nTotalPenalty = EFFECT_ATTACK_LIMIT;
    return nTotalBonus - nTotalPenalty;
}
// PC 0x004F5F70  Xbox 0x00179CA0
void CSWSCreature::RemoveCombatInvisibilityEffects()
{
    int nEffect;
    CGameEffect* pEffect;

    nEffect = m_pStats->m_nInvisibilityEffectIndex;
    while (nEffect < m_lstEffects.num &&
           m_lstEffects[nEffect]->m_nType <=
               CSWSEffectListHandler::EFFECT_SANCTUARY)
    {
        pEffect = m_lstEffects[nEffect];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_INVISIBILITY &&
            pEffect->GetInteger(0) == 1)
        {
            ((CSWSObject*)this)->RemoveEffectById(pEffect->m_nID);
        }
        else if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_SANCTUARY)
        {
            ((CSWSObject*)this)->RemoveEffectById(pEffect->m_nID);
        }
        else
        {
            nEffect++;
        }
    }
}
// PC 0x004F6000  Xbox 0x00179D30
int CSWSCreature::GetIsInUseRange(unsigned long oidObject, float fRange,
                                 int bFlag)
{
    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    Vector vUsePosition;
    {
        float fUseRange = 0.0f;

        if (pObject == NULL || pObject->AsSWSObject() == NULL ||
            pObject->AsSWSObject()->GetArea() == NULL)
        {
            return 0;
        }

        unsigned long oidArea = m_oidArea;
        if (oidArea != pObject->AsSWSObject()->m_oidArea)
            return 0;

        fRange += 0.1f;
        if (pObject->AsSWSTrigger() != NULL &&
            pObject->AsSWSTrigger()->m_nTriggerType != 0)
        {
            return pObject->AsSWSTrigger()->InTrigger(m_vPosition);
        }

        GetUseRange(oidObject, vUsePosition, fUseRange, 0);
        Vector vStart = m_vPosition;
        Vector vEnd = vUsePosition;
        vStart.m_fZ += 1.5f;
        vEnd.m_fZ += 1.5f;
        Vector vHit;
        unsigned long oidHit;
        if (pObject->AsSWSObject()->GetArea()->ClearLineOfSight(
                vStart, vEnd, &vHit, &oidHit, GetId(), oidObject, bFlag, 0))
        {
            vStart.m_fZ = 0.0f;
            vEnd.m_fZ = 0.0f;
            Vector vDelta = vEnd - vStart;
            fRange += fUseRange;
            if (vDelta * vDelta <= fRange * fRange)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x004FD960  Xbox 0x001816C0
void CSWSCreature::PacifyCreature()
{
    CSWSArea* pArea = ((CSWSObject*)this)->GetArea();
    if (pArea != NULL)
    {
        for (int nObject = 0; nObject < pArea->m_lstObjectIds.num; nObject++)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pArea->m_lstObjectIds[nObject]);
            if (pCreature != NULL &&
                (pCreature->m_oidAttemptedAttackTarget == m_nId ||
                 pCreature->m_oidAttackTarget == m_nId ||
                 pCreature->m_oidAttemptedSpellTarget == m_nId ||
                 pCreature->m_oidSpellTarget == m_nId ||
                 ((CSWSObject*)pCreature)->m_oidLastHostileActor == m_nId))
            {
                pCreature->SetCombatState(FALSE, TRUE);
                if (((CSWSObject*)pCreature)->m_oidLastHostileActor == m_nId)
                {
                    ((CSWSObject*)pCreature)->SetLastHostileActor(OBJECT_INVALID, TRUE);
                }
                pCreature->ClearHostileActionsVersus(this);
            }
        }
    }
}
// PC 0x00500CA0  Xbox 0x00184710
int CSWSCreature::AddUseTalentOnObjectActions(
    int nTalentType, int nTalentId, unsigned long oidTarget,
    unsigned char nMultiClass, unsigned long oidItem, int nItemProperty,
    unsigned char nSpellOption11, unsigned char nSpellOption2)
{
    CSWSCreature* pTarget =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
    if (pTarget == NULL)
    {
        return 0;
    }

    if (m_bCommandable == 0)
    {
        return 0;
    }

    Vector vTarget = pTarget->m_vPosition;
    unsigned long oidArea = GetArea()->GetId();
    unsigned long oidPowerItem;
    int nPowerItemProperty;

    switch (nTalentType)
    {
        case CScriptTalent::TALENT_TYPE_SPELL:
            if (oidItem != OBJECT_INVALID &&
                nItemProperty != ITEM_PROPERTY_INVALID)
            {
                UseItem(oidItem, nItemProperty, 0, oidTarget, vTarget,
                        oidArea, 0);
                return 1;
            }
            if (GetStats()->GetItemDetailsForPower(
                    nTalentId, oidPowerItem, nPowerItemProperty) == 1)
            {
                UseItem(oidPowerItem, nPowerItemProperty, 0, oidTarget,
                        vTarget, oidArea, 0);
                return 1;
            }
            AddCastSpellActions(nTalentId, nMultiClass, 0, nSpellOption2, 0,
                                vTarget, oidTarget, 0, 0, 0, 0, 0, 0, -1,
                                nSpellOption11, 0);
            return 1;
        case CScriptTalent::TALENT_TYPE_FEAT:
            AddAttackActions(oidTarget, nTalentId, 0, 0, 1, 0, 0, 1,
                             10009, CSWSCombatRound::FEAT_ATTACK_TIME,
                             1, 0, 0, 4, 0);
            return 1;
        case CScriptTalent::TALENT_TYPE_SKILL:
            UseSkill(nTalentId, 0, oidTarget, vTarget, oidArea,
                     OBJECT_INVALID, 0);
            return 1;
    }
    return 1;
}
// PC 0x00500E50  Xbox 0x001848C0
int CSWSCreature::EquipMostDamagingMeleeWeapon(unsigned long oidTarget,
                                              int bOffHand, int bRecursive)
{
    CSWSObject* pTarget = NULL;
    CSWSCreature* pTargetCreature = NULL;

    if (GetItemRepository(TRUE) == NULL)
        return FALSE;

    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        pTarget = pObject->AsSWSObject();
        if (pTarget->AsSWSCreature() != NULL)
            pTargetCreature = pTarget->AsSWSCreature();
    }

    unsigned long nSlot;
    if (!bOffHand)
    {
        int nAttackType = m_pcCombatRound->GetWeaponAttackType();
        if (nAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE1)
            nSlot = INVENTORY_SLOT_CREATUREWEAPON1;
        else if (nAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE2)
            nSlot = INVENTORY_SLOT_CREATUREWEAPON2;
        else if (nAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE3)
            nSlot = INVENTORY_SLOT_CREATUREWEAPON3;
        else
            nSlot = INVENTORY_SLOT_RIGHTHAND;
    }
    else
        nSlot = INVENTORY_SLOT_LEFTHAND;

    CSWSItem* pOldWeapon = m_pInventory->GetItemInSlot(nSlot);
    if (pOldWeapon != NULL &&
        g_pRules->GetBaseItemArray()->GetBaseItem(pOldWeapon->GetBaseItemType())->m_nRangedWeapon > 0)
    {
        UnequipItem(pOldWeapon);
        GetItemRepository(TRUE)->AddItem(&pOldWeapon, FALSE, FALSE, TRUE);
        pOldWeapon = NULL;
        if (nSlot == INVENTORY_SLOT_RIGHTHAND)
        {
            CSWSItem* pOffHand = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            if (pOffHand != NULL)
            {
                UnequipItem(pOffHand);
                GetItemRepository(TRUE)->AddItem(&pOffHand, FALSE, FALSE, TRUE);
            }
        }
    }
    if (bOffHand == TRUE && pOldWeapon == NULL)
    {
        CSWSItem* pRightHand = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pRightHand != NULL &&
            g_pRules->m_pBaseItems->GetBaseItem(pRightHand->GetBaseItemType())->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
        {
            pOldWeapon = pRightHand;
            nSlot = INVENTORY_SLOT_RIGHTHAND;
        }
    }
    if (pOldWeapon != NULL)
    {
        UnequipItem(pOldWeapon);
        GetItemRepository(TRUE)->AddItem(&pOldWeapon, FALSE, FALSE, TRUE);
    }

    CSWSItem* pBestWeapon = NULL;
    GetStats()->GetDamageRoll(pTarget, FALSE, FALSE, FALSE, TRUE, TRUE);
    CalculateMaxElementalDamage(pTargetCreature, FALSE);
    int nBestDamage = 0;
    for (unsigned long nItem = 0; nItem < GetItemRepository(TRUE)->GetNumItems(); ++nItem)
    {
        unsigned long nEquipSlot;
        CSWSItem* pItem = GetItemRepository(TRUE)->ItemListGetItem(nItem);
        if (pItem != NULL)
        {
            CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem != NULL && pBaseItem->m_nWeaponType != 0)
            {
                nEquipSlot = nSlot;
                int nWeaponSize = GetRelativeWeaponSize(pItem);
                if ((!bOffHand && CanEquipItem(pItem, &nEquipSlot, FALSE, FALSE, TRUE)) ||
                    (bOffHand && CanEquipItem(pItem, &nEquipSlot, FALSE, FALSE, TRUE) == 1))
                {
                    if (pBaseItem->m_nRangedWeapon <= 0 && pBaseItem->m_nWeaponType != 0 &&
                        (!bOffHand || (nWeaponSize < 1 && nWeaponSize != -10)))
                    {
                        EquipItem(nEquipSlot, pItem, TRUE, FALSE, FALSE);
                        int nDamage = GetStats()->GetDamageRoll(pTarget, bOffHand, FALSE, FALSE, TRUE, TRUE);
                        nDamage += CalculateMaxElementalDamage(pTargetCreature, bOffHand);
                        if (nDamage >= nBestDamage)
                        {
                            nBestDamage = nDamage;
                            pBestWeapon = pItem;
                        }
                        UnequipItem(pItem);
                    }
                }
            }
        }
    }
    if (pOldWeapon != NULL)
    {
        GetItemRepository(TRUE)->RemoveItem(pOldWeapon);
        EquipItem(nSlot, pOldWeapon, TRUE, FALSE, FALSE);
    }
    if (pBestWeapon == NULL)
    {
        if (pOldWeapon != NULL && oidTarget != OBJECT_INVALID && !bRecursive)
        {
            EquipMostDamagingMeleeWeapon(OBJECT_INVALID, bOffHand, TRUE);
            return TRUE;
        }
    }
    else if (pOldWeapon == NULL || pOldWeapon->GetId() != pBestWeapon->GetId())
        AddEquipItemActions(pBestWeapon, nSlot, FALSE, FALSE, FALSE, FALSE, FALSE);
    return TRUE;
}
// PC 0x00501250  Xbox 0x00184D80
int CSWSCreature::EquipMostDamagingRangedWeapon(unsigned long oidTarget)
{
    CSWSCreature* pTargetCreature = NULL;
    CSWSItem* pOldWeapon = NULL;
    int nBestAttackModifier = -30;
    CSWSItem* pBestWeapon = NULL;

    if (GetItemRepository(TRUE) == NULL)
        return FALSE;

    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        CSWSObject* pTarget = pObject->AsSWSObject();
        if (pTarget->AsSWSCreature() != NULL)
            pTargetCreature = pTarget->AsSWSCreature();
    }

    pOldWeapon = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    if (pOldWeapon != NULL)
    {
        CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pOldWeapon->GetBaseItemType());
        if (pBaseItem != NULL && pBaseItem->m_nRangedWeapon > 0)
        {
            pBestWeapon = pOldWeapon;
            nBestAttackModifier = m_pStats->GetAttackModifierVersus(pTargetCreature);
        }
        if (pOldWeapon != NULL)
        {
            UnequipItem(pOldWeapon);
            GetItemRepository(TRUE)->AddItem(&pOldWeapon, FALSE, FALSE, TRUE);
            CSWSItem* pOffHand = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            if (pOffHand != NULL)
            {
                UnequipItem(pOffHand);
                GetItemRepository(TRUE)->AddItem(&pOffHand, FALSE, FALSE, TRUE);
            }
        }
    }

    unsigned long nEquipSlot;
    for (unsigned long nItem = 0; nItem < GetItemRepository(TRUE)->GetNumItems(); ++nItem)
    {
        CSWSItem* pItem = GetItemRepository(TRUE)->ItemListGetItem(nItem);
        if (pItem != NULL)
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem != NULL && pBaseItem->m_nWeaponType != 0)
            {
                nEquipSlot = INVENTORY_SLOT_RIGHTHAND;
                if (CanEquipItem(pItem, &nEquipSlot, FALSE, FALSE, TRUE) &&
                    pBaseItem->m_nRangedWeapon > 0)
                {
                    EquipItem(nEquipSlot, pItem, TRUE, FALSE, FALSE);
                    int nAttackModifier = m_pStats->GetAttackModifierVersus(pTargetCreature);
                    if (nAttackModifier > nBestAttackModifier)
                    {
                        nBestAttackModifier = nAttackModifier;
                        pBestWeapon = pItem;
                    }
                    UnequipItem(pItem);
                }
            }
        }
    }
    if (pOldWeapon != NULL)
    {
        GetItemRepository(TRUE)->RemoveItem(pOldWeapon);
        EquipItem(INVENTORY_SLOT_RIGHTHAND, pOldWeapon, TRUE, FALSE, FALSE);
    }
    if (pBestWeapon != NULL && pOldWeapon != pBestWeapon)
    {
        unsigned long nSlot = INVENTORY_SLOT_RIGHTHAND;
        AddEquipItemActions(pBestWeapon, nSlot, FALSE, FALSE, FALSE, FALSE, FALSE);
        return TRUE;
    }
    return FALSE;
}
// PC 0x005014A0  Xbox 0x00185070
CExoArrayList<CGameEffect*> CSWSCreature::GetFilteredEffectList(unsigned char nType)
{
    CExoArrayList<CGameEffect*> lstEffects;
    int bFound = FALSE;
    int nIndex;
    int nFiltered;
    CGameEffect* pEffect;
    CGameEffect* pPrevious;
    int nPreviousRank;
    CExoArrayList<CGameEffect*>* pEffects = &m_lstEffects;
    switch (nType)
    {
    case FILTERED_EFFECT_ATTACK:
    {
        if (pEffects->num > 0)
        {
            for (nIndex = m_pStats->m_nAttackEffectIndex;
                 nIndex < pEffects->num &&
                 (*pEffects)[nIndex]->GetEffectType() <=
                     CSWSEffectListHandler::EFFECT_ATTACK_DECREASE; nIndex++)
            {
                pEffect = (*pEffects)[nIndex];
                if (pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_ATTACK_INCREASE &&
                    pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_ATTACK_DECREASE)
                    continue;
                for (nFiltered = 0; nFiltered < lstEffects.num; nFiltered++)
                {
                    bFound = FALSE;
                    pPrevious = lstEffects[nFiltered];
                    if (pPrevious->GetEffectType() == pEffect->m_nType &&
                        pPrevious->GetInteger(1) == pEffect->GetInteger(1) &&
                        pPrevious->GetInteger(2) == pEffect->GetInteger(2) &&
                        pPrevious->GetInteger(3) == pEffect->GetInteger(3) &&
                        pPrevious->GetInteger(4) == pEffect->GetInteger(4) &&
                        (pPrevious->GetSpellId() == CGameEffect::SPELL_ID_INVALID ||
                         pEffect->GetSpellId() == CGameEffect::SPELL_ID_INVALID ||
                         pPrevious->GetSpellId() == pEffect->GetSpellId()))
                    {
                        bFound = TRUE;
                        break;
                    }
                }
                if (bFound == TRUE)
                {
                    if (pEffect->GetInteger(0) > pPrevious->GetInteger(0))
                        lstEffects[nFiltered] = pEffect;
                }
                else
                {
                    lstEffects.Add(pEffect);
                }
            }
        }
        return lstEffects;
    }
    case FILTERED_EFFECT_DAMAGE:
    {
        if (pEffects->num > 0)
        {
            for (nIndex = m_pStats->m_nDamageIncreaseEffectIndex;
                 nIndex < pEffects->num &&
                 (*pEffects)[nIndex]->GetEffectType() <=
                     CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE; nIndex++)
            {
                bFound = FALSE;
                pEffect = (*pEffects)[nIndex];
                if (pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE &&
                    pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
                    continue;
                for (nFiltered = 0; nFiltered < lstEffects.num; nFiltered++)
                {
                    bFound = FALSE;
                    pPrevious = lstEffects[nFiltered];
                    if (pPrevious->GetEffectType() == pEffect->m_nType &&
                        pPrevious->GetInteger(1) == pEffect->GetInteger(1) &&
                        pPrevious->GetInteger(2) == pEffect->GetInteger(2) &&
                        pPrevious->GetInteger(3) == pEffect->GetInteger(3) &&
                        pPrevious->GetInteger(4) == pEffect->GetInteger(4) &&
                        pPrevious->GetInteger(5) == pEffect->GetInteger(5) &&
                        (pPrevious->GetSpellId() == CGameEffect::SPELL_ID_INVALID ||
                         pEffect->GetSpellId() == CGameEffect::SPELL_ID_INVALID ||
                         pPrevious->GetSpellId() == pEffect->GetSpellId()))
                    {
                        bFound = TRUE;
                        break;
                    }
                }
                if (bFound == TRUE)
                {
                    Log_Base2(pEffect->GetInteger(1));
                    int nRank;
                    {
                        CExoString sColumn("Rank");
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPDamage;
                        pTable->GetINTEntry(pEffect->GetInteger(0), sColumn, &nRank);
                    }
                    {
                        CExoString sColumn("Rank");
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pIPRPDamage;
                        pTable->GetINTEntry(pPrevious->GetInteger(0), sColumn, &nPreviousRank);
                    }
                    if (nRank > nPreviousRank)
                        lstEffects[nFiltered] = pEffect;
                }
                else
                {
                    lstEffects.Add(pEffect);
                }
            }
        }
        return lstEffects;
    }
    default:
        return lstEffects;
    }
}
// PC 0x00501950  Xbox 0x00185530
int CSWSCreature::GetInvisible(CSWSObject* pObject)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        int nRace = pCreature->m_pStats->m_nRace;
        int nAlignment = pCreature->m_pStats->m_nGoodEvilAlignment;
        for (int nEffect = m_pStats->m_nInvisibilityEffectIndex;
             nEffect < m_lstEffects.num &&
             m_lstEffects[nEffect]->GetEffectType() <=
                 CSWSEffectListHandler::EFFECT_SANCTUARY; nEffect++)
        {
            CGameEffect* pEffect = m_lstEffects[nEffect];
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_INVISIBILITY)
            {
                if ((nRace == pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_RACIAL_GROUP) ||
                     g_pRules->GetNumRaces() == pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_RACIAL_GROUP)) &&
                    (nAlignment == pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_ALIGNMENT) ||
                     pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_ALIGNMENT) == CSWSEffectListHandler::ALIGNMENT_ALL))
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_TYPE) == CSWSEffectListHandler::INVISIBILITY_TYPE_NORMAL ||
                        pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_TYPE) == CSWSEffectListHandler::INVISIBILITY_TYPE_IMPROVED)
                    {
                        if ((pCreature->m_nVisionFlags & (VISION_SEE_INVISIBLE | VISION_TRUESEEING)) != 0)
                        {
                            return FALSE;
                        }
                    }
                    else if (pEffect->GetInteger(CSWSEffectListHandler::INVISIBILITY_INTEGER_TYPE) == CSWSEffectListHandler::INVISIBILITY_TYPE_DARKNESS &&
                             (pCreature->m_nVisionFlags & (VISION_ULTRAVISION | VISION_TRUESEEING)) != 0)
                    {
                        return FALSE;
                    }
                    return TRUE;
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SANCTUARY &&
                     pCreature->GetAIStateReputation(GetId()) == AI_STATE_REPUTATION_ENEMY &&
                     (nRace == pEffect->GetInteger(CSWSEffectListHandler::SANCTUARY_INTEGER_RACIAL_GROUP) ||
                      g_pRules->GetNumRaces() == pEffect->GetInteger(CSWSEffectListHandler::SANCTUARY_INTEGER_RACIAL_GROUP)) &&
                     (nAlignment == pEffect->GetInteger(CSWSEffectListHandler::SANCTUARY_INTEGER_ALIGNMENT) ||
                      pEffect->GetInteger(CSWSEffectListHandler::SANCTUARY_INTEGER_ALIGNMENT) == CSWSEffectListHandler::ALIGNMENT_ALL))
            {
                CSWVisibilityNode* pNode = pCreature->GetVisibleListElement(GetId());
                if (pNode != NULL)
                {
                    unsigned char nSanctuary =
                        pNode->m_nSanctuary;
                    if (nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED)
                    {
                        if ((pCreature->m_nVisionFlags & VISION_TRUESEEING) != 0)
                        {
                            return FALSE;
                        }
                        return TRUE;
                    }
                    else if (nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_SUCCEEDED)
                    {
                        return FALSE;
                    }
                    else if (nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_NOT_ROLLED)
                    {
                        if (pCreature->SavingThrowRoll(SAVING_THROW_WILL,
                                pEffect->GetInteger(CSWSEffectListHandler::SANCTUARY_INTEGER_SAVE_DC), 0, OBJECT_INVALID, 1, 0, 0) == SAVING_THROW_RESULT_FAILED)
                        {
                            PacifyCreature(pCreature->GetId());
                            pCreature->AddToVisibleList(GetId(), 0, 0,
                                CSWVisibilityNode::SANCTUARY_SAVE_FAILED, 0);
                            return TRUE;
                        }
                        else
                        {
                            pCreature->AddToVisibleList(GetId(), 0, 0,
                                CSWVisibilityNode::SANCTUARY_SAVE_SUCCEEDED, 0);
                        }
                    }
                }
                else
                {
                    if (pCreature->SavingThrowRoll(SAVING_THROW_WILL,
                            pEffect->GetInteger(CSWSEffectListHandler::SANCTUARY_INTEGER_SAVE_DC), 0, OBJECT_INVALID, 1, 0, 0) == SAVING_THROW_RESULT_FAILED)
                    {
                        PacifyCreature(pCreature->GetId());
                        pCreature->AddToVisibleList(GetId(), 0, 0,
                            CSWVisibilityNode::SANCTUARY_SAVE_FAILED, 0);
                        return TRUE;
                    }
                    else
                    {
                        pCreature->AddToVisibleList(GetId(), 0, 0,
                            CSWVisibilityNode::SANCTUARY_SAVE_SUCCEEDED, 0);
                    }
                }
            }
        }
    }
    return FALSE;
}
// PC 0x004FDA20  Xbox 0x00181780
void CSWSCreature::PacifyCreature(unsigned long oidCreature)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL &&
        (pCreature->m_oidAttemptedAttackTarget == m_nId ||
         pCreature->m_oidAttackTarget == m_nId ||
         pCreature->m_oidAttemptedSpellTarget == m_nId ||
         pCreature->m_oidSpellTarget == m_nId ||
         ((CSWSObject*)pCreature)->m_oidLastHostileActor == m_nId))
    {
        pCreature->SetCombatState(FALSE, TRUE);
        if (((CSWSObject*)pCreature)->m_oidLastHostileActor == m_nId)
        {
            ((CSWSObject*)pCreature)->SetLastHostileActor(OBJECT_INVALID, TRUE);
        }
        pCreature->ClearHostileActionsVersus(this);
    }
}
// PC 0x004FDAA0  Xbox 0x00181800
void CSWSCreature::CancelCombat(int nArg1)
{
    CSWCCreature* pClientCreature;

    ((CSWSObject*)this)->ClearAllActions(1);
    SetCombatState(0, 1);
    ((CSWSObject*)this)->SetLastHostileActor(OBJECT_INVALID,
                                             1);
    ClearHostileActionsVersus(NULL);
    m_pcCombatRound->EndCombatRound(nArg1);
    m_pcCombatRound->m_oidUnknown9CC = OBJECT_INVALID;

    pClientCreature = GetClientCreature();
    if (pClientCreature != NULL)
    {
        pClientCreature->SetCombatMode(0);
    }
}
// PC 0x004F61D0  Xbox 0x00179F20
void CSWSCreature::ClearHostileActionsVersus(CSWSCreature* pCreature)
{
    CSWSObjectActionNode* pAction;
    int nGroups;
    CExoLinkedList<CSWSObjectActionNode>* pActions = &m_lstActions;
    if (pActions != NULL)
    {
        nGroups = GetNumActionGroups();
        for (int nGroup = 0; nGroup < nGroups; ++nGroup)
        {
            CExoLinkedListNode* pPosition = GetPositionByGroupIndex(nGroup);
            if (pPosition != NULL)
            {
                CSWSObjectActionNode* pNode = pActions->GetAtPos(pPosition);
                if (pNode != NULL)
                {
                    unsigned short nGroupId = pNode->m_nGroupId;
                    if (GetActionByGroupId(nGroupId, &pAction) != ACTION_TYPE_INVALID &&
                        pAction != NULL)
                    {
                        if (pAction->m_nActionType == ACTION_PHYSICAL_ATTACKS)
                        {
                            if (pCreature == NULL ||
                                (unsigned long)pAction->m_apParam[CSWSObjectActionNode::PHYSICAL_ATTACK_TARGET_PARAMETER] == pCreature->GetId())
                            {
                                RemoveGroup(nGroupId);
                                nGroup = 0;
                            }
                        }
                        else if (pAction->m_nActionType == ACTION_CAST_SPELL)
                        {
                            if (pCreature == NULL ||
                                (unsigned long)pAction->m_apParam[CSWSObjectActionNode::SPELL_TARGET_PARAMETER] == pCreature->GetId())
                            {
                                RemoveGroup(nGroupId);
                                nGroup = 0;
                            }
                        }
                        else if (pAction->m_nActionType == ACTION_ITEM_CAST_SPELL)
                        {
                            if (pCreature == NULL ||
                                (unsigned long)pAction->m_apParam[CSWSObjectActionNode::ITEM_SPELL_TARGET_PARAMETER] == pCreature->GetId())
                            {
                                RemoveGroup(nGroupId);
                                nGroup = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}
// PC 0x004F62A0  Xbox 0x00179FF0
int CSWSCreature::GetIsCreatureBumpable(CSWSCreature* pCreature)
{
    if (bDisableBumping)
        return 0;

    int bBumpable = 0;
    if (GetReputation(pCreature->GetId(), 0) >= NWSCREATURE_BUMP_MIN_REPUTATION &&
        !pCreature->m_bInParty)
    {
        CExoLinkedList<CSWSObjectActionNode>* pActions = &pCreature->m_lstActions;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pPlayer = pServer->GetPlayerCreature();
        if (pPlayer != this && pPlayer == pCreature)
            bBumpable = 0;
        else if (pPlayer == this && pPlayer != pCreature)
        {
            bBumpable = pCreature->m_bInParty;
            CSWSObjectActionNode* pAction = pActions->GetHead();
            if (pAction != NULL &&
                (pAction->m_nActionType == ACTION_PHYSICAL_ATTACKS ||
                 pAction->m_nActionType == ACTION_COMBAT ||
                 pAction->m_nActionType == ACTION_CAST_SPELL ||
                 pAction->m_nActionType == ACTION_ITEM_CAST_SPELL))
                bBumpable = 0;
        }
        else if (pPlayer != this && pPlayer != pCreature)
        {
            bBumpable = 0;
            if (pActions != NULL &&
                pActions->IsEmpty() && !pCreature->m_bCombatState &&
                !pCreature->m_pcCombatRound->m_bRoundPaused)
                bBumpable = 1;
        }
    }
    return bBumpable;
}
// PC 0x004F6390  Xbox 0x0017A0E0
int CSWSCreature::BumpFriends(CSWSCreature* pCreature, const Vector& vStart,
                             const Vector& vEnd)
{
    if (GetIsCreatureBumpable(pCreature))
    {
        CExoString sDebug;
        sDebug.Format("Bump! %x (%s) bumps %x (%s)\n", GetId(), m_sTag.CStr(),
                      pCreature->GetId(), pCreature->m_sTag.CStr());
        EXOLOG(sDebug.CStr());

        Vector vCreaturePosition = pCreature->m_vPosition;
        float fDistance = pCreature->GetPathfindInformation()->GetCreaturePersonalSpace();
        fDistance += 0.1f;
        fDistance += GetPathfindInformation()->GetCreaturePersonalSpace();
        Vector vProjection = lineproject(vStart, vEnd, vCreaturePosition);
        Vector vNormal = normalize(vCreaturePosition - vProjection);
        Vector vBumpPosition = vNormal * fDistance + vProjection;
        Vector vOldPosition = m_vPosition;
        SetPosition(vProjection, 1, 0, 0);

        float fMinHeight;
        float fMaxHeight;
        if (vBumpPosition.m_fZ < vCreaturePosition.m_fZ)
        {
            fMinHeight = vBumpPosition.m_fZ - 0.1f;
            fMaxHeight = pCreature->GetPathfindInformation()->m_fHalfStep;
            fMaxHeight += vBumpPosition.m_fZ;
        }
        else
        {
            fMinHeight = vCreaturePosition.m_fZ - 0.1f;
            fMaxHeight = pCreature->GetPathfindInformation()->m_fHalfStep;
            fMaxHeight += vCreaturePosition.m_fZ;
        }

        int bSafe = GetArea()->ComputeSafeLocation(vBumpPosition, 5.0f,
            pCreature->GetPathfindInformation(), 1, &vBumpPosition);
        unsigned long oidHit;
        int bClear = GetArea()->NoNonWalkPolysInDoors(vCreaturePosition,
            vBumpPosition, fMinHeight, fMaxHeight,
            pCreature->GetPathfindInformation()->GetStepScale(), OBJECT_INVALID,
            oidHit, NULL, 1, 0) == 1;
        if (bSafe && bClear)
        {
            pCreature->SetPosition(vBumpPosition, 1, 1, 0);
            sDebug.Format("Bumped %x ends up at %f,%f\n", pCreature->GetId(),
                          vBumpPosition.m_fX, vBumpPosition.m_fY);
            EXOLOG(sDebug.CStr());
            SetPosition(vOldPosition, 1, 1, 0);
            return 1;
        }

        else
        {
            Vector vSafePosition;
            if (GetArea()->ComputeSafeLocationInDirection(vEnd, -m_vOrientation,
                4.0f, pCreature->GetPathfindInformation(), 1, &vSafePosition) == 1)
            {
                pCreature->SetPosition(vSafePosition, 1, 1, 0);
                if (pCreature->GetInParty())
                {
                    int nMember = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(g_pAppManager->GetClientExoApp()->ServerToClientObjectId(pCreature->GetId()));
#ifdef _XBOX
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetMemberData(nMember)->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_FIND_PATH;
#else
                    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty(); pParty->m_aPartyMemberData[nMember].m_nCharacterIndex = CSWPartyMemberData::FOLLOW_FIND_PATH;
#endif
                }
                sDebug.Format("Bumped %x ends up at %f,%f\n", pCreature->GetId(),
                              vSafePosition.m_fX, vSafePosition.m_fY);
                EXOLOG(sDebug.CStr());
                SetPosition(vOldPosition, 1, 1, 0);
                return 1;
            }
            SetPosition(vOldPosition, 1, 1, 0);
        }
    }
    return 0;
}
// PC 0x004F6790  Xbox 0x0017A500
int CSWSCreature::ApplyDiseasePayload(CGameEffect* pEffect,
                                      unsigned long nDay, unsigned long nTime)
{
    if (pEffect->GetEffectType() != CSWSEffectListHandler::EFFECT_DISEASE)
        return 0;

    int bDamageApplied = 1;
    CWorldTimer* pActiveTimer =
        g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId());
#ifdef _XBOX
    CTwoDimArrays* pArraysForDisease = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pArraysForDisease->m_pDisease);
    C2DA* pDisease = pArraysForDisease->m_pDisease;
#else
    C2DA* pDisease = g_pRules->Get2DArrays()->m_pDisease;
#endif
    int nDisease = pEffect->GetInteger(0);
    int nSubsequentSave;
    pDisease->GetINTEntry(nDisease, "Subs_Save", &nSubsequentSave);
    if (SavingThrowRoll(SAVING_THROW_FORT, (unsigned short)nSubsequentSave,
                         SAVING_THROW_TYPE_DISEASE, pEffect->GetCreator(), 1, 0, 0) != 0)
    {
        if (pEffect->GetInteger(1) == 1)
        {
            DisplayFloatyDataToSelf(0);
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), GetId(), CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
        }
        else
        {
            DisplayFloatyDataToSelf(0);
            pEffect->SetInteger(1, 1);
        }
        if (pEffect->GetInteger(2) == 0)
            pEffect->SetInteger(2, 1);
        bDamageApplied = 0;
    }
    else
    {
        CExoString sDice("Dice_");
        CExoString sDamage("Dam_");
        CExoString sType("Type_");
        int nDice, nDie, nAbility;
        for (int nPayload = 1; nPayload < 4; nPayload++)
        {
            CExoString sPayload(nPayload);
            if (pDisease->GetINTEntry(nDisease, sDice + sPayload, &nDice) &&
                pDisease->GetINTEntry(nDisease, sDamage + sPayload, &nDie) &&
                pDisease->GetINTEntry(nDisease, sType + sPayload, &nAbility))
            {
                int nDamage = (unsigned short)g_pRules->RollDice(
                    (unsigned char)nDie, (unsigned char)nDice);
                CGameEffect* pDamage = new CGameEffect(1);
                pDamage->SetCreator(pEffect->GetCreator());
                pDamage->m_fDuration = pEffect->m_fDuration;
                unsigned short nSubType = pEffect->m_nSubType & CGameEffect::SUBTYPE_MASK;
                pDamage->m_nSubType = (pDamage->m_nSubType & ~CGameEffect::SUBTYPE_MASK) | nSubType;
                unsigned short nDurationType = pEffect->m_nSubType & CGameEffect::DURATION_MASK;
                pDamage->m_nSubType = (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) | nDurationType;
                pDamage->m_nSpellId = pEffect->GetSpellId();
                pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
                pDamage->SetInteger(0, nAbility);
                pDamage->SetInteger(1, nDamage);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), GetId(), CServerAIMaster::EVENT_APPLY_EFFECT, pDamage);
            }
        }
        CGameEffect* pVisualEffect = new CGameEffect(pEffect);
        pVisualEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pVisualEffect->SetInteger(0, CSWSEffectListHandler::VFX_DISEASE_PAYLOAD);
        ApplyEffect(pVisualEffect, 0, 0);
        if (pEffect->GetInteger(2) == 0)
        {
            CExoString sScript;
            if (pDisease->GetCExoStringEntry(nDisease, "End_Incu_Script", &sScript))
                g_pVirtualMachine->RunScript(&sScript, GetId(), 1);
            pEffect->SetInteger(2, 1);
        }
        else
        {
            CExoString sScript;
            if (pDisease->GetCExoStringEntry(nDisease, "24_Hour_Script", &sScript))
                g_pVirtualMachine->RunScript(&sScript, GetId(), 1);
        }
        DisplayFloatyDataToSelf(0);
    }
    unsigned long nNextDay, nNextTime;
    unsigned long nHourLength =
        g_pAppManager->GetServerExoApp()->GetModule()->m_nMinutesPerHour *
        CWorldTimer::MILLISECONDS_IN_MINUTE;
    pActiveTimer->AddWorldTimes(nDay, nTime, 0, 24 * nHourLength, &nNextDay, &nNextTime);
    pEffect->SetInteger(3, nNextDay);
    pEffect->SetInteger(4, nNextTime);
    return bDamageApplied;
}
// PC 0x004F6D70  Xbox 0x0017AB40
void CSWSCreature::ComputeModifiedMovementRate()
{
    int bStealthMode = FALSE;
    if (m_nStealthMode == 1)
        bStealthMode = TRUE;
    int nEffect = GetStats()->m_nLimitMovementSpeedEffectIndex;
    int nNumEffects = m_lstEffects.num;
    while (nEffect < nNumEffects &&
           m_lstEffects[nEffect]->GetEffectType() <=
               CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED)
    {
        CGameEffect* pEffect = m_lstEffects[nEffect];
        if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_INNATE && pEffect->GetInteger(0) == 1)
        {
            unsigned __int64 nEffectId = pEffect->m_nID;
            RemoveEffectById(nEffectId);
            nEffect = GetStats()->m_nLimitMovementSpeedEffectIndex;
        }
        else
            nEffect++;
        nNumEffects = m_lstEffects.num;
    }
    nEffect = GetStats()->m_nMovementSpeedDecreaseEffectIndex;
    while (nEffect < m_lstEffects.num &&
           m_lstEffects[nEffect]->GetEffectType() <=
               CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE)
    {
        CGameEffect* pEffect = m_lstEffects[nEffect];
        if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_INNATE && pEffect->GetCreator() == GetId())
        {
            unsigned __int64 nEffectId = pEffect->m_nID;
            RemoveEffectById(nEffectId);
            nEffect = GetStats()->m_nMovementSpeedDecreaseEffectIndex;
        }
        else
            nEffect++;
    }
    if (bStealthMode > 0)
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED;
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INNATE;
        pEffect->m_bExposed = 0;
        pEffect->SetCreator(GetId());
        pEffect->SetInteger(0, 1);
        ApplyEffect(pEffect, 0, 0);
    }
}
// PC 0x004F6F30  Xbox 0x0017AD00
void CSWSCreature::UpdateActionQueue()
{
    CExoLinkedList<CSWSObjectActionNode>* pActions = &m_lstActions;
    if (pActions != NULL)
    {
        int nCount;
        for (nCount = 0; nCount < 10; nCount++)
        {
            m_aActionQueue[nCount].Clear();
        }

        int nSkipped = 0;
        CSWSObjectActionNode* pNode;
        float fX;
        float fY;
        float fZ;
        for (nCount = 0; nCount < 10; nCount++)
        {
            CExoLinkedListNode* pPosition;
            unsigned short nType;
            pPosition = GetPositionByGroupIndex(nSkipped + nCount);
            if (pPosition != NULL)
                pNode = pActions->GetAtPos(pPosition);
            while (TRUE)
            {
                if (pPosition == NULL)
                    goto ClearRemaining;
                if (pNode != NULL)
                {
                    nType = GetActionByGroupId(pNode->m_nGroupId, &pNode);
                    if (nType != ACTION_TYPE_INVALID)
                        break;
                    nSkipped++;
                }
                pPosition = GetPositionByGroupIndex(nSkipped + nCount);
                if (pPosition != NULL)
                    pNode = pActions->GetAtPos(pPosition);
            }
            if (pNode == NULL)
                break;

            switch (nType)
            {
            case ACTION_MOVE_TO_POINT:
                {
                    memcpy(&fX, &pNode->m_apParam[0], sizeof(float));
                    memcpy(&fY, &pNode->m_apParam[1], sizeof(float));
                    memcpy(&fZ, &pNode->m_apParam[2], sizeof(float));
                    unsigned long oidTarget = (unsigned long)pNode->m_apParam[3];
                    Vector vPosition(fX, fY, fZ);
                    if ((((unsigned char)((unsigned long)pNode->m_apParam[5] >> 6)) & 3) == 0)
                    {
                        m_aActionQueue[nCount].m_vLocation = vPosition;
                        m_aActionQueue[nCount].m_oidTarget = oidTarget;
                    }
                }
                break;
            case ACTION_CAST_SPELL:
                {
                    int nSpell = (int)pNode->m_apParam[0];
                    unsigned long oidTarget = (unsigned long)pNode->m_apParam[5];
                    memcpy(&fX, &pNode->m_apParam[6], sizeof(float));
                    memcpy(&fY, &pNode->m_apParam[7], sizeof(float));
                    memcpy(&fZ, &pNode->m_apParam[8], sizeof(float));
                    Vector vPosition(fX, fY, fZ);
                    m_aActionQueue[nCount].m_nParameter = nSpell;
                    m_aActionQueue[nCount].m_oidTarget = oidTarget;
                    m_aActionQueue[nCount].m_vLocation = vPosition;
                }
                break;
            case ACTION_ITEM_CAST_SPELL:
                {
                    unsigned long oidItem = (unsigned long)pNode->m_apParam[0];
                    int nProperty = (int)pNode->m_apParam[1];
                    memcpy(&fX, &pNode->m_apParam[4], sizeof(float));
                    unsigned long oidTarget = (unsigned long)pNode->m_apParam[3];
                    memcpy(&fY, &pNode->m_apParam[5], sizeof(float));
                    memcpy(&fZ, &pNode->m_apParam[6], sizeof(float));
                    CSWSItem* pItem = g_pAppManager->GetServerExoApp()
                        ->GetItemByGameObjectID(oidItem);
                    if (pItem == NULL)
                        continue;
                    CSWItemProperty* pProperty = pItem->GetActiveProperty(nProperty);
                    if (pProperty == NULL)
                        continue;
                    unsigned short nSubtype = pProperty->m_nSubtype;
                    Vector vPosition(fX, fY, fZ);
                    m_aActionQueue[nCount].m_oidTarget = oidTarget;
                    m_aActionQueue[nCount].m_vLocation = vPosition;
                    m_aActionQueue[nCount].m_nParameter = nSubtype;
                }
                break;
            case ACTION_DROP_ITEM:
                memcpy(&fX, &pNode->m_apParam[1], sizeof(float));
                memcpy(&fY, &pNode->m_apParam[2], sizeof(float));
                memcpy(&fZ, &pNode->m_apParam[3], sizeof(float));
                m_aActionQueue[nCount].m_vLocation = Vector(fX, fY, fZ);
                break;
            case ACTION_SET_MINE:
                {
                    memcpy(&fX, &pNode->m_apParam[2], sizeof(float));
                    memcpy(&fY, &pNode->m_apParam[3], sizeof(float));
                    memcpy(&fZ, &pNode->m_apParam[4], sizeof(float));
                    Vector vPosition(fX, fY, fZ);
                    m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[1];
                    m_aActionQueue[nCount].m_vLocation = vPosition;
                }
                break;
            case ACTION_PICK_UP_ITEM:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_PHYSICAL_ATTACKS:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_OPEN_DOOR:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_CLOSE_DOOR:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_DIALOG_OBJECT:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_DISARM_MINE:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_RECOVER_MINE:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_FLAG_MINE:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_EXAMINE_MINE:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_UNLOCK_OBJECT:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_LOCK_OBJECT:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_USE_OBJECT:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_ANIMAL_EMPATHY:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_TAUNT:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_COUNTER_SPELL:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_PICK_POCKET:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_FORCE_FOLLOW_OBJECT:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_HEAL:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            case ACTION_BARK_STRING:
                m_aActionQueue[nCount].m_oidTarget = (unsigned long)pNode->m_apParam[0];
                break;
            }
            m_aActionQueue[nCount].m_nActionId = pNode->m_nGroupId;
            m_aActionQueue[nCount].m_nGroupActionId = GetAQActionIDByID(nType, 0);
        }
ClearRemaining:
        for (; nCount < 10; nCount++)
        {
            m_aActionQueue[nCount].Clear();
        }
    }
}
// PC 0x004F7530  Xbox 0x0017B3F0
void CSWSCreature::ProcessPendingCombatActions()
{
    if (m_pcCombatRound->GetActionPending() == 0)
    {
        return;
    }
    CExoLinkedList<CSWSCombatRoundAction>* pActions =
        m_pcCombatRound->m_pScheduledActions;
    if (pActions == NULL)
    {
        return;
    }
    CExoLinkedListNode* pPosition = pActions->GetTailPos();
    if (pPosition == NULL)
    {
        return;
    }
    CSWSCombatRoundAction* pAction = pActions->GetAtPos(pPosition);
    while (pPosition != NULL)
    {
        CExoLinkedListNode* pRemove = NULL;
        CSWSCombatRoundAction* pDelete = NULL;
        if (pAction->m_nActionType == CSWSCombatRoundAction::ACTION_EQUIP_ITEM)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->
                GetItemByGameObjectID(pAction->m_oidTarget);
            if (pItem != NULL)
            {
                AddEquipItemActions(pItem, pAction->m_nInventorySlot, 1, 1,
                                    pAction->m_nEquipFlags,
                                    pAction->m_nAttackFlags, 0);
                pRemove = pPosition;
                pDelete = pAction;
            }
        }
        else if (pAction->m_nActionType ==
                 CSWSCombatRoundAction::ACTION_UNEQUIP_ITEM)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->
                GetItemByGameObjectID(pAction->m_oidTarget);
            if (pItem != NULL)
            {
                AddUnequipActions(pItem, pAction->m_oidTargetRepository, 1,
                                  1, pAction->m_nEquipFlags);
                pRemove = pPosition;
                pDelete = pAction;
            }
        }
        pAction = pActions->GetPrev(pPosition);
        if (pRemove != NULL)
        {
            pActions->Remove(pRemove);
            delete pDelete;
        }
    }
}
// PC 0x004F7650  Xbox 0x0017B510
int CSWSCreature::IsPartyVisible()
{
    g_pAppManager->m_pClientExoApp->GetSWParty();
    for (int nNode = 0; nNode < m_lstVisibilityList.num; nNode++)
    {
        CSWVisibilityNode* pNode = m_lstVisibilityList[nNode];
        if (pNode->m_bSeen != 0)
        {
            unsigned long oidObject = pNode->m_oidObject;
            CSWSCreature* pCreature =
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                    oidObject);
            if (pCreature != NULL && pCreature->m_bInParty != 0)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x004F76C0  Xbox 0x0017B580
int CSWSCreature::RemoveAction(unsigned long nAction)
{
    CExoLinkedListNode* pPosition = m_lstActions.GetHeadPos();
    int bRemoved = 0;
    while (pPosition != NULL)
    {
        CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPosition);
        if (pAction->m_nActionType == nAction && ClearAction(pAction, 1) == 1)
        {
            m_lstActions.Remove(pPosition);
            delete pAction;
            bRemoved = 1;
            break;
        }
        m_lstActions.GetNext(pPosition);
    }
    return bRemoved;
}
// PC 0x004F7750  Xbox 0x0017B620
int CSWSCreature::GetCurrentlyFollowingLeader()
{
    CSWSObjectActionNode* pAction;

    if (m_lstActions.GetHeadPos() != NULL)
    {
        pAction = m_lstActions.GetAtPos(m_lstActions.GetHeadPos());
        if (pAction != NULL && pAction->m_nActionType == 61)
        {
            return TRUE;
        }
    }
    return FALSE;
}
// PC 0x004F7780  Xbox 0x0017B650
int CSWSCreature::GetTotalACEffectBonus()
{
    NwscreatureTrace1();
    CExoArrayList<CGameEffect*>* pEffects = &m_lstEffects;
    if (pEffects == NULL)
    {
        return 0;
    }

    int nBonus = 0;
    if (pEffects->num > 0)
    {
        for (int nEffect = 0; nEffect < pEffects->num; nEffect++)
        {
            CGameEffect* pEffect = pEffects->element[nEffect];
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
            {
                nBonus += pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS);
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_DECREASE)
            {
                nBonus -= pEffect->GetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS);
            }
        }
    }
    return nBonus;
}
// PC 0x004FDB20  Xbox 0x00181880
void CSWSCreature::SetInParty(int bInParty, int bUpdateAI)
{
    m_bInParty = bInParty;
    if (bUpdateAI != 0)
    {
        if (bInParty != 0)
        {
            if (m_pFollowInfo == NULL)
            {
                m_pFollowInfo = new CSWSCreaturePartyFollowInfo;
            }
            if (m_nAILevel != CServerAIMaster::AI_LEVEL_PARTY_CREATURE)
            {
                g_pAppManager->m_pServerExoApp->GetServerAIMaster()->SetAILevel(
                    (CSWSObject*)this, CServerAIMaster::AI_LEVEL_PARTY_CREATURE);
                return;
            }
        }
        else
        {
            if (m_pFollowInfo != NULL)
            {
                delete m_pFollowInfo;
                m_pFollowInfo = NULL;
            }
            if (m_nAILevel != CServerAIMaster::AI_LEVEL_NONPARTY_CREATURE)
            {
                g_pAppManager->m_pServerExoApp->GetServerAIMaster()->SetAILevel(
                    (CSWSObject*)this, CServerAIMaster::AI_LEVEL_NONPARTY_CREATURE);
                return;
            }
        }
    }
}
// PC 0x004FDBD0
void CSWSCreature::AddPureGoodPowers()
{
    RemovePureGoodPowers();

    CGameEffect* pEffect = new CGameEffect(TRUE);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_PUREGOODPOWERS;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_INNATE |
        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(m_nId);
    pEffect->m_nSubType &= ~CGameEffect::SUBTYPE_MASK;
    pEffect->m_nSpellId = -1;
    CSWSObject::ApplyEffect(pEffect, FALSE, FALSE);
}
// PC 0x004F77E0
void CSWSCreature::RemovePureGoodPowers()
{
    int nEffect;

    for (nEffect = 0; nEffect < m_lstEffects.num; nEffect++)
    {
        if (m_lstEffects[nEffect]->m_nType ==
            CSWSEffectListHandler::EFFECT_PUREGOODPOWERS)
        {
            ((CSWSObject*)this)
                ->RemoveEffectById(m_lstEffects[nEffect]->m_nID);
            return;
        }
    }
}
// PC 0x00501C40  Xbox 0x00185AB0
void CSWSCreature::ForceEquipClothing()
{
    if (m_bForceEquipClothing)
    {
        CItemRepository* pRepository = GetItemRepository(TRUE);
        unsigned long oidClothing =
            pRepository->FindItemWithTag("g_a_clothes01");
        CSWSItem* pItem = g_pAppManager->GetServerExoApp()
            ->GetItemByGameObjectID(oidClothing);

        if (pItem == NULL)
        {
            pItem = new CSWSItem(OBJECT_INVALID);
            if (pItem == NULL)
                goto Done;

            CResRef cTemplate("g_a_clothes01");
            if (!pItem->LoadFromTemplate(cTemplate))
            {
                delete pItem;
                goto Done;
            }

            pItem->SetDropable(TRUE);
            pItem->SetPossessor(GetId(), FALSE, FALSE, OBJECT_INVALID);
        }

        if (pRepository->GetItemInRepository(pItem, FALSE))
        {
            pRepository->RemoveItem(pItem);
        }
        EquipItem(INVENTORY_SLOT_ARMOUR, pItem, TRUE, FALSE, FALSE);
        m_bForceEquipClothing = FALSE;
        return;

Done:
        m_bForceEquipClothing = FALSE;
    }
}
// PC 0x00501D90
void CSWSCreature::UpdatePureGoodEvilPowers()
{
    if (m_pStats != NULL)
    {
        if (m_pStats->m_nGoodEvilAlignment == 0)
        {
            RemovePureGoodPowers();
            AddPureEvilPowers();
        }
        else if (m_pStats->m_nGoodEvilAlignment == 100)
        {
            RemovePureEvilPowers();
            AddPureGoodPowers();
        }
        else
        {
            RemovePureGoodPowers();
            RemovePureEvilPowers();
        }
    }
}
// PC 0x004FDC80
void CSWSCreature::AddPureEvilPowers()
{
    RemovePureEvilPowers();

    CGameEffect* pEffect = new CGameEffect(TRUE);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_PUREEVILPOWERS;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_INNATE |
        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(m_nId);
    pEffect->m_nSubType &= ~CGameEffect::SUBTYPE_MASK;
    pEffect->m_nSpellId = -1;
    CSWSObject::ApplyEffect(pEffect, FALSE, FALSE);
}
// PC 0x004F7830
void CSWSCreature::RemovePureEvilPowers()
{
    int nEffect;

    for (nEffect = 0; nEffect < m_lstEffects.num; nEffect++)
    {
        if (m_lstEffects[nEffect]->m_nType ==
            CSWSEffectListHandler::EFFECT_PUREEVILPOWERS)
        {
            ((CSWSObject*)this)
                ->RemoveEffectById(m_lstEffects[nEffect]->m_nID);
            return;
        }
    }
}
// PC 0x004F7880
int CSWSCreature::HasEffect(int nEffectType)
{
    int nEffect;

    for (nEffect = 0; nEffect < m_lstEffects.num; nEffect++)
    {
        if (m_lstEffects[nEffect]->m_nType == nEffectType)
        {
            return TRUE;
        }
    }
    return FALSE;
}
// PC 0x00501DE0  Xbox 0x00185C20
int CSWSCreature::RunEquip(unsigned long oidItem, unsigned long nSlot, int bInstant)
{
    CSWSItem* pOldItem;
    int bUnequipped = FALSE;
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        int bOwned = FALSE;
        CItemRepository* pRepository;
        if (pItem->GetPossessor() == GetId())
        {
            pRepository = GetItemRepository(TRUE);
            bOwned = TRUE;
        }
        else
        {
            CSWSItem* pContainer = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pItem->GetPossessor());
            if (pContainer != NULL && pContainer->GetPossessor() == GetId())
            {
                pRepository = pContainer->m_pRepository;
                bOwned = TRUE;
            }
            else
            {
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                CSWPartyTable* pPartyTable = g_pAppManager->GetServerExoApp()->GetPartyTable();
                CSWParty* pParty = pClient->GetSWParty();
                if (pParty->IsPartyMember(pClient->ServerToClientObjectId(GetId())) ||
                    pPartyTable->GetIsAvailable(GetId()))
                {
                    if (pParty->IsPartyMember(pClient->ServerToClientObjectId(pItem->GetPossessor())) ||
                        pPartyTable->GetIsAvailable(pItem->GetPossessor()))
                    {
                        pRepository = GetItemRepository(TRUE);
                        bOwned = TRUE;
                    }
                    else
                    {
                        pContainer = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pItem->GetPossessor());
                        if (pContainer != NULL && pPartyTable->GetIsAvailable(pContainer->GetPossessor()))
                        {
                            pRepository = pContainer->m_pRepository;
                            bOwned = TRUE;
                        }
                    }
                }
            }
        }

        if (!bOwned)
        {
            if (pPlayer != NULL && pMessage != NULL)
                pMessage->SendServerToPlayerInventory_EquipCancel(pPlayer->m_nPlayerId, oidItem, nSlot);
            return FALSE;
        }

        unsigned char nResult = CanEquipItem(pItem, &nSlot, TRUE, FALSE, TRUE);
        if (nResult == EQUIP_ITEM_OK)
        {
            if (pItem->m_nStackSize > 1)
                pItem = pItem->SplitItem(1);
            if (pRepository->GetItemInRepository(pItem, FALSE) == TRUE)
                pRepository->RemoveItem(pItem);
            if (pItem->GetPossessor() != GetId())
                pItem->SetPossessor(GetId(), FALSE, FALSE, OBJECT_INVALID);
            EquipItem(nSlot, pItem, TRUE, FALSE, bInstant);
            SetLastInstantEquip(m_pInventory->GetSlotFromItem(pItem), bInstant);
        }
        else if (nResult == EQUIP_ITEM_REPLACE || nResult == EQUIP_ITEM_REPLACE_HANDS)
        {
            if (nResult == EQUIP_ITEM_REPLACE)
            {
                pOldItem = m_pInventory->GetItemInSlot(nSlot);
                if (pOldItem != NULL)
                {
                    if (g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nMaxStackSize > 1)
                    {
                        if (pOldItem->CompareItem(pItem) == 0)
                        {
                            UnequipItem(pOldItem);
                            if (!GetItemRepository(TRUE)->AddItem(&pOldItem, TRUE, FALSE, FALSE))
                            {
                                RemoveItem(pOldItem, FALSE, TRUE, NULL);
                                pOldItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                                    m_vPosition.m_fZ + 0.2f, TRUE);
                            }
                            bUnequipped = TRUE;
                        }
                    }
                    else
                    {
                        UnequipItem(pOldItem);
                        if (!GetItemRepository(TRUE)->AddItem(&pOldItem, TRUE, FALSE, FALSE))
                        {
                            RemoveItem(pOldItem, FALSE, TRUE, NULL);
                            pOldItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                                m_vPosition.m_fZ + 0.2f, TRUE);
                        }
                        bUnequipped = TRUE;
                    }
                }
            }
            else
            {
                pOldItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                if (pOldItem != NULL)
                {
                    UnequipItem(pOldItem);
                    if (!GetItemRepository(TRUE)->AddItem(&pOldItem, TRUE, FALSE, FALSE))
                    {
                        RemoveItem(pOldItem, FALSE, TRUE, NULL);
                        pOldItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                            m_vPosition.m_fZ + 0.2f, TRUE);
                    }
                    bUnequipped = TRUE;
                }
                pOldItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                if (pOldItem != NULL)
                {
                    UnequipItem(pOldItem);
                    if (!GetItemRepository(TRUE)->AddItem(&pOldItem, TRUE, FALSE, FALSE))
                    {
                        RemoveItem(pOldItem, FALSE, TRUE, NULL);
                        pOldItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                            m_vPosition.m_fZ + 0.2f, TRUE);
                    }
                    bUnequipped = TRUE;
                }
            }

            if (bUnequipped == TRUE)
            {
                if (pItem->m_nStackSize > 1)
                    pItem = pItem->SplitItem(1);
                if (pRepository->GetItemInRepository(pItem, FALSE) == TRUE)
                    pRepository->RemoveItem(pItem);
                if (pItem->GetPossessor() != GetId())
                    pItem->SetPossessor(GetId(), FALSE, FALSE, OBJECT_INVALID);
                EquipItem(nSlot, pItem, TRUE, FALSE, FALSE);
                SetLastInstantEquip(m_pInventory->GetSlotFromItem(pItem), bInstant);
            }
            else
            {
                if (pMessage != NULL && pPlayer != NULL)
                    pMessage->SendServerToPlayerInventory_EquipCancel(pPlayer->m_nPlayerId, oidItem, nSlot);
                return FALSE;
            }
        }
        else
        {
            if (pMessage != NULL && pPlayer != NULL)
                pMessage->SendServerToPlayerInventory_EquipCancel(pPlayer->m_nPlayerId, oidItem, nSlot);
            return FALSE;
        }

        CSWSItem* pRight = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        CSWSItem* pLeft = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
        {
            if (pRight != NULL || pLeft != NULL)
                SetCombatMode(0, TRUE);
        }
        else if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
        {
            if (pRight == NULL || g_pRules->m_pBaseItems->GetBaseItem(pRight->GetBaseItemType())->m_nRangedWeapon <= 0)
                SetCombatMode(0, TRUE);
        }
        else if (m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK ||
                 m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK ||
                 m_nActiveCombatMode == SWSOBJECT_COMBAT_MODE_PARRY)
        {
            if (pRight != NULL && g_pRules->m_pBaseItems->GetBaseItem(pRight->GetBaseItemType())->m_nRangedWeapon > 0)
                SetCombatMode(0, TRUE);
        }
        return TRUE;
    }

    if (pMessage != NULL && pPlayer != NULL)
        pMessage->SendServerToPlayerInventory_EquipCancel(pPlayer->m_nPlayerId, oidItem, nSlot);
    return FALSE;
}
// PC 0x005023A0  Xbox 0x001861E0
int CSWSCreature::RunUnequip(unsigned long oidItem, unsigned long oidTarget,
                            int bMerge, CSWSItem** ppItem, int bInstant)
{
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
        ->GetClientObjectByObjectId(GetId());
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    if (ppItem != NULL)
        *ppItem = NULL;

    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem != NULL && pItem->m_oidPossessor == GetId() &&
        m_pInventory->GetItemInInventory(pItem) == TRUE)
    {
        CItemRepository* pRepository = NULL;
        if (oidTarget == OBJECT_INVALID)
            pRepository = GetItemRepository(TRUE);
        else
        {
            CSWSItem* pContainer = g_pAppManager->GetServerExoApp()
                ->GetItemByGameObjectID(oidTarget);
            if (pContainer != NULL && pContainer->m_oidPossessor == GetId())
                pRepository = pContainer->m_pRepository;
        }
        if (pRepository == NULL)
        {
            if (pPlayer != NULL && pMessage != NULL)
                pMessage->SendServerToPlayerInventory_UnequipCancel(
                    pPlayer->m_nPlayerId, oidItem);
            return FALSE;
        }

        unsigned char nCanUnequip = CanUnEquipWeapon(pItem);
        if (nCanUnequip == 1)
        {
            SetLastInstantEquip(m_pInventory->GetSlotFromItem(pItem), bInstant);
            UnequipItem(pItem);
            if (!pRepository->AddItem(&pItem, bMerge, FALSE, FALSE))
                pItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                 m_vPosition.m_fZ + 0.2f, TRUE);
            if (ppItem != NULL)
                *ppItem = pItem;
            if (oidTarget == OBJECT_INVALID)
                pItem->SetPossessor(GetId(), FALSE, FALSE, OBJECT_INVALID);
            else
                pItem->SetPossessor(oidTarget, FALSE, FALSE, OBJECT_INVALID);
            if (pPlayer != NULL && pMessage != NULL)
            {
                pMessage->SendServerToPlayerInventory_Unequip(
                    pPlayer->m_nPlayerId, oidItem, bInstant);
                return TRUE;
            }
        }
        else if (nCanUnequip == 2)
        {
            SetLastInstantEquip(m_pInventory->GetSlotFromItem(pItem), bInstant);
            UnequipItem(pItem);
            if (!pRepository->AddItem(&pItem, bMerge, FALSE, FALSE))
                pItem->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY,
                                 m_vPosition.m_fZ + 0.2f, TRUE);
            CSWSItem* pLeftHand = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            if (pLeftHand != NULL)
            {
                UnequipItem(pLeftHand);
                EquipItem(INVENTORY_SLOT_RIGHTHAND, pLeftHand, TRUE, FALSE, FALSE);
            }
            if (ppItem != NULL)
                *ppItem = pItem;
            if (oidTarget == OBJECT_INVALID)
                pItem->SetPossessor(GetId(), FALSE, FALSE, OBJECT_INVALID);
            else
                pItem->SetPossessor(oidTarget, FALSE, FALSE, OBJECT_INVALID);
            if (pPlayer != NULL && pMessage != NULL)
            {
                pMessage->SendServerToPlayerInventory_Unequip(
                    pPlayer->m_nPlayerId, pLeftHand->GetId(), bInstant);
                pMessage->SendServerToPlayerInventory_Unequip(
                    pPlayer->m_nPlayerId, pItem->GetId(), bInstant);
                return TRUE;
            }
        }
    }
    if (pPlayer != NULL && pMessage != NULL)
        pMessage->SendServerToPlayerInventory_UnequipCancel(
            pPlayer->m_nPlayerId, oidItem);
    return FALSE;
}
// PC 0x005026D0  Xbox 0x00186540
int CSWSCreature::LoadFromTemplate(const CResRef& cResRef, int bLoadFromTemplate)
{
    CResGFF* pRes = new CResGFF(RESTYPE_UTC, "UTC ", cResRef);
    if (!pRes->m_bLoaded)
    {
        delete pRes;
        CExoString sError;
        sError.Format("Creature template '%s' doesn't exist.\n", cResRef.GetResRefStr());
        pRes = new CResGFF(RESTYPE_UTC, "UTC ", "NW_BADGER");
        if (!pRes->m_bLoaded)
        {
            delete pRes;
            return 0;
        }
    }

    CResStruct stCreature;
    pRes->GetTopLevelStruct(&stCreature);
    if (GetStats()->ReadStatsFromGff(pRes, &stCreature, &m_cAppearanceInfo) != 0)
    {
        delete pRes;
        return 0;
    }
    ReadScriptsFromGff(pRes, &stCreature);
    ReadItemsFromGff(pRes, &stCreature, 0, 1, bLoadFromTemplate);
    GetStats()->ReadSpellsFromGff(pRes, &stCreature);
    int bSuccess;
    m_bPMIsDisguised = pRes->ReadFieldBYTE(&stCreature, "PM_IsDisguised", bSuccess, 0);
    if (m_bPMIsDisguised != 0)
        m_nPMAppearance = pRes->ReadFieldWORD(&stCreature, "PM_Appearance", bSuccess, 0);
    int nStealthMode = pRes->ReadFieldBYTE(&stCreature, "StealthMode", bSuccess, 0);
    SetActivity(ACTIVITY_STEALTH, nStealthMode, 0);
    LoadObjectState(pRes, &stCreature);

    Vector vPosition;
    Vector vOrientation;
    vPosition.m_fX = pRes->ReadFieldFLOAT(&stCreature, "XPosition", bSuccess, 0.0f);
    vPosition.m_fY = pRes->ReadFieldFLOAT(&stCreature, "YPosition", bSuccess, 0.0f);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(&stCreature, "ZPosition", bSuccess, 0.0f);
    vOrientation.m_fX = pRes->ReadFieldFLOAT(&stCreature, "XOrientation", bSuccess, 0.0f);
    vOrientation.m_fY = pRes->ReadFieldFLOAT(&stCreature, "YOrientation", bSuccess, 0.0f);
    vOrientation.m_fZ = pRes->ReadFieldFLOAT(&stCreature, "ZOrientation", bSuccess, 0.0f);
    m_nUnknown22C = pRes->ReadFieldINT(&stCreature, "JoiningXP", bSuccess, 0);
    SetPosition(vPosition, 1, 1, 0);
    if (vOrientation.m_fX != 0.0f || vOrientation.m_fY != 0.0f || vOrientation.m_fZ != 0.0f)
        SetOrientation(vOrientation);
    PostProcess();
    delete pRes;
    return 1;
}
// PC 0x00502AC0  Xbox 0x00186920
void CSWSCreature::DoPerceptionUpdateOnCreature(CSWSCreature* pCreature)
{
    Vector vPosition;
    Vector vTarget;
    Vector vHit;
    int bAdded = 0;
    int bRemoved = 0;
    int bInvisible = 0;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    int bSeen;
    int bHeard;
    int bPotentiallySeen;
    CServerAIMaster* pAIMaster = pServer->GetServerAIMaster();
    unsigned long nDay, nTime, nElapsedDays, nElapsedTime;
    g_pAppManager->GetServerExoApp()->GetActiveTimer(OBJECT_INVALID)->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(nDay, nTime,
        m_nLastStealthRollDay, m_nLastStealthRollTime, &nElapsedDays, &nElapsedTime);
    if (nElapsedTime >= 20000)
    {
        if (GetStealthMode() == 1)
        {
            m_nHideCheckRoll = rand() % 10 + 11;
            m_nMoveSilentlyCheckRoll = rand() % 10 + 11;
        }
        if (GetDetectMode() == 1 || GetDetectMode() == 2)
        {
            m_nSpotCheckRoll = rand() % 20 + 1;
            m_nListenCheckRoll = rand() % 20 + 1;
        }
        else
        {
            m_nSpotCheckRoll = rand() % 10 + 1;
            m_nListenCheckRoll = rand() % 10 + 1;
        }
        m_nLastStealthRollDay = nDay;
        m_nLastStealthRollTime = nTime;
    }
    CSWSArea* pArea = GetArea();
    if (pArea == NULL || pCreature->GetId() == GetId())
        return;
    float fRange = GetSpotCheckDistance() >= GetListenCheckDistance() ?
        GetSpotCheckDistance() : GetListenCheckDistance();
    unsigned long oidTarget = pCreature->GetId();
    vPosition = m_vPosition;
    vTarget = pCreature->m_vPosition;
    vPosition.m_fZ += 1.5f;
    vTarget.m_fZ += 1.5f;
    if (pCreature->GetArea() != pArea)
    {
        bPotentiallySeen = 0;
    }
    else
    {
        if (GetId() == pServer->GetPlayerCreatureId())
            bPotentiallySeen = 1;
        else
        {
            unsigned long oidHit;
            Vector vDelta = vPosition - vTarget;
            if (vDelta * vDelta >= fRange * fRange)
                bPotentiallySeen = 0;
            else
                bPotentiallySeen = pArea->ClearLineOfSight(vPosition, vTarget, &vHit, &oidHit,
                                                           GetId(), oidTarget, 1, 0) == 1;
        }
        CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
        int nCreature;
        for (nCreature = 0; nCreature < pModule->m_lstInvisibleCreatures.num; ++nCreature)
        {
            CSWSCreature* pInvisible = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                pModule->m_lstInvisibleCreatures[nCreature]);
            if (pInvisible != NULL && GetArea() != NULL && pInvisible->GetArea() != NULL &&
                GetArea()->GetId() == pInvisible->GetArea()->GetId())
            {
                bInvisible = 1;
                break;
            }
        }
        if (GetBlind())
            bInvisible = 1;
        if (bInvisible == 1 && !GetBlind())
            bInvisible = pCreature->GetInvisible(AsSWSObject()) & 1;
    }
    CSWVisibilityNode* pVisible = GetVisibleListElement(oidTarget);
    if (pVisible != NULL)
    {
        if (bInvisible != pVisible->m_bInvisible)
        {
            if (bInvisible == 1)
                ClearHostileActionsVersus(pCreature);
            pVisible->m_bInvisible = bInvisible;
        }
        bHeard = pVisible->m_bHeard;
        bSeen = pVisible->m_bSeen;
        if (bPotentiallySeen)
        {
            if (!pVisible->m_bSeen)
            {
                DoStealthDetection(pCreature, bPotentiallySeen, &bSeen, &bHeard, bInvisible);
                if (!pVisible->m_bHeard && bHeard)
                    bAdded = 1;
                if (!pVisible->m_bSeen && bSeen)
                    bAdded = 1;
            }
            if (pVisible->m_bSeen && bInvisible)
            {
                bSeen = 0;
                bRemoved = 1;
            }
            if (pVisible->m_bHeard && pCreature->m_bSilenced)
            {
                bHeard = 0;
                bRemoved = 1;
            }
            if (bInvisible && (pVisible->m_bSeen || pVisible->m_bHeard))
                bRemoved = 1;
        }
        else
        {
            if (pVisible->m_bSeen)
            {
                bSeen = 0;
                bRemoved = 1;
            }
            DoStealthDetection(pCreature, 0, &bSeen, &bHeard, bInvisible);
            if (bHeard)
                bAdded = 1;
            else
                bRemoved = 1;
        }
    }
    else
    {
        if (bPotentiallySeen)
        {
            DoStealthDetection(pCreature, bPotentiallySeen, &bSeen, &bHeard, bInvisible);
            if (bSeen || bHeard)
                bAdded = 1;
        }
        else
        {
            DoStealthDetection(pCreature, 0, &bSeen, &bHeard, bInvisible);
            if (bHeard)
                bAdded = 1;
        }
    }
    unsigned long oidClientTarget = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidTarget);
    unsigned long oidClient = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId());
    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    if (pParty != NULL && pParty->IsPartyMember(oidClient) && pParty->IsPartyMember(oidClientTarget))
    {
        bInvisible = 0;
        bSeen = 1;
        bHeard = 1;
        bAdded = 1;
    }
    else if (bRemoved)
    {
        pVisible = GetVisibleListElement(oidTarget);
        if (pVisible != NULL)
        {
            if (pVisible->m_bHeard && !bHeard && !m_bVisibleListUpdatePending)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_LOST_HEARING);
                pEvent->SetObjectID(0, oidTarget);
                pAIMaster->AddEventDeltaTime(0, 0, oidTarget, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            if (pVisible->m_bSeen && !bSeen && !m_bVisibleListUpdatePending)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_LOST_SIGHT);
                pEvent->SetObjectID(0, oidTarget);
                pAIMaster->AddEventDeltaTime(0, 0, oidTarget, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            pVisible->m_bSeen = bSeen & 1;
            pVisible->m_bHeard = bHeard & 1;
            if (!pVisible->m_bSeen && !pVisible->m_bHeard && !bInvisible)
                RemoveFromVisibleList(oidTarget);
        }
    }
    int bNewlySeen = 0;
    if (bAdded)
    {
        pVisible = GetVisibleListElement(oidTarget);
        if (pVisible == NULL)
        {
            AddToVisibleList(oidTarget, bSeen, bHeard, 0, bInvisible);
            if (bHeard && !m_bVisibleListUpdatePending)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_GAINED_HEARING);
                pEvent->SetObjectID(0, oidTarget);
                pAIMaster->AddEventDeltaTime(0, 0, oidTarget, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            if (bSeen && !m_bVisibleListUpdatePending)
            {

                CScriptEvent* pEvent;
                unsigned long oidSelf;
                pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_GAINED_SIGHT);
                pEvent->SetObjectID(0, oidTarget);
                oidSelf = GetId();
                pAIMaster->AddEventDeltaTime(0, 0, oidTarget, oidSelf, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);

                if (pVisible != NULL && oidSelf != OBJECT_INVALID)
                    return;
            CheckLeaderState:
                if (pCreature->m_pStats->m_bIsPC ||
                    g_pAppManager->m_pServerExoApp->GetPartyTable()->GetIsLeader(pCreature->GetId()))
                    m_nAttackedPartyLeaderState = 6000;
            }
        }
        else
        {
            if (!pVisible->m_bSeen && bSeen)
            {
                pVisible->m_bSeen = 1;
                bNewlySeen = 1;
                if (!m_bVisibleListUpdatePending)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                    pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_GAINED_SIGHT);
                    pEvent->SetObjectID(0, oidTarget);
                    pAIMaster->AddEventDeltaTime(0, 0, oidTarget, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
            if (!pVisible->m_bHeard && bHeard)
            {
                pVisible->m_bHeard = 1;
                if (!m_bVisibleListUpdatePending)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION;
                    pEvent->SetInteger(0, CScriptEvent::PERCEPTION_EVENT_GAINED_HEARING);
                    pEvent->SetObjectID(0, oidTarget);
                    pAIMaster->AddEventDeltaTime(0, 0, oidTarget, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
            if (bNewlySeen)
                goto CheckLeaderState;
        }
    }
}
// PC 0x0060E760
void CSWSCreature::BroadcastAttackOfOpportunity(unsigned long oidTarget)
{
}
// PC 0x005B5E90
void CSWSCreature::BroadcastAttackDataToParty(CSWCCMessageData* pData,
                                            CSWSCombatAttackData* pAttack)
{
}
// PC 0x004EE440  Xbox 0x00171E00
int CSWSCreature::GetUseRange(unsigned long oidObject, Vector& vPosition,
                             float& fRange, int bIgnorePreciseUse)
{
    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    fRange = GetPathfindInformation()->m_fStepScale;
    if (pObject == NULL || pObject->AsSWSObject() == NULL ||
        pObject->AsSWSObject()->GetArea() == NULL)
    {
        return 0;
    }

    switch (pObject->m_nObjectType)
    {
    case SW_OBJECT_TYPE_CREATURE:
        if (oidObject == GetPathfindInformation()->m_oidUseTarget &&
            GetPathfindInformation()->m_oidUseTarget != OBJECT_INVALID)
            fRange = GetPathfindInformation()->m_fTargetUseRange;
        else
            fRange = GetPathfindInformation()->GetCreaturePersonalSpace();
        vPosition = pObject->AsSWSObject()->m_vPosition;
        fRange = pObject->AsSWSCreature()->GetPathfindInformation()
                     ->GetCreaturePersonalSpace() + fRange + 0.3f;
        return 1;

    case SW_OBJECT_TYPE_PLACEABLE:
        vPosition = pObject->AsSWSPlaceable()->GetNearestActionPoint(m_vPosition);
        if (pObject->AsSWSPlaceable()->m_bPreciseUse &&
            !bIgnorePreciseUse &&
            GetArea()->TestSafeLocationPoint(vPosition, GetPathfindInformation()) == 1)
            fRange = 0.1f;
        else
            fRange += 0.75f;
        if (pObject->AsSWSPlaceable()->m_bIsCorpse)
            fRange += 5.0f;
        return 1;

    case SW_OBJECT_TYPE_DOOR:
        vPosition = pObject->AsSWSDoor()->GetNearestActionPoint(m_vPosition, 0);
        vPosition.m_fZ = pObject->AsSWSDoor()->GetArea()
                            ->ComputeHeight(vPosition, 1, NULL);
        if (oidObject == GetPathfindInformation()->m_oidUseTarget &&
            GetPathfindInformation()->m_oidUseTarget != OBJECT_INVALID)
        {
            float fTargetRange = GetPathfindInformation()->m_fTargetUseRange;
            fRange += fTargetRange;
        }
        else
        {
            if (pObject->AsSWSDoor()->m_bPreciseUse &&
                pObject->AsSWSDoor()->m_bLocked && !bIgnorePreciseUse &&
                GetArea()->TestSafeLocationPoint(vPosition, GetPathfindInformation()) == 1)
                fRange = 0.1f;
            else
                fRange += 0.75f;
        }
        return 1;

    case SW_OBJECT_TYPE_TRIGGER:
        {
            CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
            if (pTrigger->m_nTriggerType != 0)
                vPosition = pTrigger->m_vPosition;
            else
            {
                pTrigger->CalculateNearestPoint(m_vPosition, &vPosition);
                fRange += 0.5f;
            }
        }
        return 1;

    default:
        vPosition = pObject->AsSWSObject()->m_vPosition;
        return 1;
    }
}
// PC 0x004EE770  Xbox 0x00172160
int CSWSCreature::ApplyPoisonDamage(int nPoison, CGameEffect* pEffect,
                                    int bLoadingModule, float fDamage)
{
    int nDamageMultiplier = (int)fDamage;
    unsigned long nSpellId = pEffect->GetSpellId();
#ifdef _XBOX
    CTwoDimArrays* pArraysForPoison = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pArraysForPoison->m_pPoison);
    C2DA* pPoison = pArraysForPoison->m_pPoison;
#else
    C2DA* pPoison = g_pRules->Get2DArrays()->m_pPoison;
#endif
    if (pPoison == NULL)
        return 0;

    int nHPDamage, nFPDamage;
    int nSTRDamage, nDEXDamage, nCONDamage;
    int nINTDamage, nWISDamage, nCHADamage;
    pPoison->GetINTEntry(nPoison, "DAM_HP", &nHPDamage);
    pPoison->GetINTEntry(nPoison, "DAM_FP", &nFPDamage);
    pPoison->GetINTEntry(nPoison, "DAM_STR", &nSTRDamage);
    pPoison->GetINTEntry(nPoison, "DAM_DEX", &nDEXDamage);
    pPoison->GetINTEntry(nPoison, "DAM_CON", &nCONDamage);
    pPoison->GetINTEntry(nPoison, "DAM_INT", &nINTDamage);
    pPoison->GetINTEntry(nPoison, "DAM_WIS", &nWISDamage);
    pPoison->GetINTEntry(nPoison, "DAM_CHR", &nCHADamage);

    int nRemainingDuration = 0;
    int nDuration = 0;
    int nPoisonPeriod = 0;
    if (nSTRDamage > 0 || nDEXDamage > 0 || nCONDamage > 0 ||
        nINTDamage > 0 || nWISDamage > 0 || nCHADamage > 0)
    {
        pPoison->GetINTEntry(nPoison, "DURATION", &nDuration);
        pPoison->GetINTEntry(nPoison, "PERIOD", &nPoisonPeriod);
        nRemainingDuration = nDuration - nPoisonPeriod * (nDamageMultiplier - 1);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Ability Damage duration = %d seconds\n",
                      nRemainingDuration);
        sTrace.CStr();
    }
    if (nHPDamage > 0)
    {
        int nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_POISON;
        int nDamagePower = CSWSEffectListHandler::DAMAGE_POWER_NORMAL;
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pDamage->SetNumIntegers(CSWSEffectListHandler::APPLIED_DAMAGE_INTEGER_COUNT);
        for (int nType = 0; nType < CSWSEffectListHandler::APPLIED_DAMAGE_TYPE_COUNT; nType++)
            pDamage->SetInteger(nType, -1);
        pDamage->SetInteger(Log_Base2(nDamageFlags), nHPDamage);
        pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_BASE, nHPDamage);
        pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_ANIMATION_DELAY, 1000);
        pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_FLAGS, nDamageFlags);
        pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_POWER, nDamagePower);
        pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_COMBAT, 1);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying HP damage: %d\n", nHPDamage);
        sTrace.CStr();
        CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
        pAIMaster->AddEventDeltaTime(0, 0, pDamage->GetCreator(), GetId(),
                                     CServerAIMaster::EVENT_APPLY_EFFECT, pDamage);
    }
    if (nFPDamage > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_DAMAGEFORCEPOINTS;
        pDamage->SetInteger(0, nFPDamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying FP damage: %d\n", nFPDamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    if (nSTRDamage > 0 && nRemainingDuration > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pDamage->SetInteger(0, ABILITY_STRENGTH);
        pDamage->SetInteger(1, nDamageMultiplier * nSTRDamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pDamage->m_fDuration = (float)nRemainingDuration;
        pDamage->m_bExposed = 0;
        RemoveEffectByType(nSpellId, CSWSEffectListHandler::EFFECT_ABILITY_DECREASE,
                            ABILITY_STRENGTH);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying STR damage: %d\n", nDamageMultiplier * nSTRDamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    if (nDEXDamage > 0 && nRemainingDuration > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pDamage->SetInteger(0, ABILITY_DEXTERITY);
        pDamage->SetInteger(1, nDamageMultiplier * nDEXDamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pDamage->m_fDuration = (float)nRemainingDuration;
        pDamage->m_bExposed = 0;
        RemoveEffectByType(nSpellId, CSWSEffectListHandler::EFFECT_ABILITY_DECREASE,
                            ABILITY_DEXTERITY);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying DEX damage: %d\n", nDamageMultiplier * nDEXDamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    if (nCONDamage > 0 && nRemainingDuration > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pDamage->SetInteger(0, ABILITY_CONSTITUTION);
        pDamage->SetInteger(1, nDamageMultiplier * nCONDamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pDamage->m_fDuration = (float)nRemainingDuration;
        pDamage->m_bExposed = 0;
        RemoveEffectByType(nSpellId, CSWSEffectListHandler::EFFECT_ABILITY_DECREASE,
                            ABILITY_CONSTITUTION);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying CON damage: %d\n", nDamageMultiplier * nCONDamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    if (nINTDamage > 0 && nRemainingDuration > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pDamage->SetInteger(0, ABILITY_INTELLIGENCE);
        pDamage->SetInteger(1, nDamageMultiplier * nINTDamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pDamage->m_fDuration = (float)nRemainingDuration;
        pDamage->m_bExposed = 0;
        RemoveEffectByType(nSpellId, CSWSEffectListHandler::EFFECT_ABILITY_DECREASE,
                            ABILITY_INTELLIGENCE);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying INT damage: %d\n", nDamageMultiplier * nINTDamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    if (nWISDamage > 0 && nRemainingDuration > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pDamage->SetInteger(0, ABILITY_WISDOM);
        pDamage->SetInteger(1, nDamageMultiplier * nWISDamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pDamage->m_fDuration = (float)nRemainingDuration;
        pDamage->m_bExposed = 0;
        RemoveEffectByType(nSpellId, CSWSEffectListHandler::EFFECT_ABILITY_DECREASE,
                            ABILITY_WISDOM);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying WIS damage: %d\n", nDamageMultiplier * nWISDamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    if (nCHADamage > 0 && nRemainingDuration > 0)
    {
        CGameEffect* pDamage = new CGameEffect(pEffect);
        pDamage->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pDamage->SetInteger(0, ABILITY_CHARISMA);
        pDamage->SetInteger(1, nDamageMultiplier * nCHADamage);
        pDamage->m_nSubType =
            (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pDamage->m_fDuration = (float)nRemainingDuration;
        pDamage->m_bExposed = 0;
        RemoveEffectByType(nSpellId, CSWSEffectListHandler::EFFECT_ABILITY_DECREASE,
                            ABILITY_CHARISMA);
        CExoString sTrace;
        sTrace.Format("POISONTRACE: Applying CHR damage: %d\n", nDamageMultiplier * nCHADamage);
        sTrace.CStr();
        ApplyEffect(pDamage, bLoadingModule, 0);
    }
    return 1;
}
// PC 0x004EF2E0  Xbox 0x00172D00
unsigned char CSWSCreature::GetBodyBagAppearance()
{
    int nAppearance;
    bool bFailed;
    {
        CExoString sColumn("Appearance");
#ifdef _XBOX
        CTwoDimArrays* pArraysForBodyBag = g_pRules->Get2DArrays();
        EXOXBOXASSERT(pArraysForBodyBag->m_pBodyBag);
        C2DA* pTable = pArraysForBodyBag->m_pBodyBag;
#else
        C2DA* pTable = g_pRules->Get2DArrays()->m_pBodyBag;
#endif
        bFailed = !pTable->GetINTEntry(m_nBodyBag, sColumn, &nAppearance);
    }
    if (bFailed)
    {
        int nBodyBag;
        {
            CExoString sColumn("Body_Bag");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pAppearance;
            bFailed = !pTable->GetINTEntry(m_cAppearanceInfo.m_nAppearance, sColumn, &nBodyBag);
        }
        if (bFailed)
            nBodyBag = CTwoDimArrays::BODY_BAG_DEFAULT_ROW;
        {
            CExoString sColumn("Appearance");
#ifdef _XBOX
            CTwoDimArrays* pArraysForBodyBag = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pArraysForBodyBag->m_pBodyBag);
            C2DA* pTable = pArraysForBodyBag->m_pBodyBag;
#else
            C2DA* pTable = g_pRules->Get2DArrays()->m_pBodyBag;
#endif
            bFailed = !pTable->GetINTEntry(nBodyBag, sColumn, &nAppearance);
        }
        if (bFailed)
            nAppearance = CTwoDimArrays::BODY_BAG_DEFAULT_APPEARANCE;
    }
    return nAppearance;
}
// PC 0x004EF450  Xbox 0x00172E80
void CSWSCreature::RemoveSpellActionFromRound()
{
    if (m_pcCombatRound->GetSpellActionPending() == 1)
    {
        m_pcCombatRound->RemoveSpellAction();
    }
}
// PC 0x004EF480  Xbox 0x00172EB0
void CSWSCreature::StartGuiTimingBar(unsigned long nTime, unsigned char nType)
{
    CSWSPlayer* pPlayer;
    CSWSMessage* pMessage;

    if (m_nAreaWaitState == 1)
    {
        pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(
            m_nId);
        pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        if (pPlayer != NULL && pMessage != NULL)
        {
            pMessage->SendServerToPlayerGuiTimingEvent(pPlayer, 1, nType,
                                                       nTime);
        }
    }
}
// PC 0x004EF4D0  Xbox 0x00172F00
void CSWSCreature::StopGuiTimingBar()
{
    CSWSPlayer* pPlayer;
    CSWSMessage* pMessage;

    if (m_nAreaWaitState == 1)
    {
        pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(
            m_nId);
        pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        if (pPlayer != NULL && pMessage != NULL)
        {
            pMessage->SendServerToPlayerGuiTimingEvent(pPlayer, 0, 0, 0);
        }
    }
}
// PC 0x004EF520  Xbox 0x00172F50
void CSWSCreature::SetInvitedToParty(int bInvited)
{
    CServerExoApp* pServer;

    m_bInvitedToParty = bInvited;
    pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetWorldTimer()->GetWorldTime(&m_nInvitedToPartyDay,
                                           &m_nInvitedToPartyTime);
}
// PC 0x004EF560  Xbox 0x00172F90
void CSWSCreature::DisplayFloatyDataToSelf(unsigned long nData)
{
    CSWSPlayer* pPlayer =
        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_nId);
    if (pPlayer != NULL)
    {
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        if (pMessageData != NULL)
        {
            pMessageData->SetObjectID(0, m_nId);
            pMessageData->SetInteger(CSWSMessage::FEEDBACK_INTEGER_MESSAGE,
                                     FEEDBACK_FLOATY_DATA);
            pMessageData->SetInteger(0, nData);
            int nPlayerId = pPlayer->m_nPlayerId;
            g_pAppManager->m_pServerExoApp->GetSWSMessage()
                ->SendServerToPlayerCCMessage(nPlayerId,
                    CSWSMessage::CC_MESSAGE_FEEDBACK, pMessageData, NULL);
            delete pMessageData;
        }
    }
}
// PC 0x004EF640  Xbox 0x00173070
void CSWSCreature::DumpToLog()
{

    EXOLOG("Dumping Creature : %x\n", m_nId);
    EXOLOG("Name: %s\n", m_pStats->GetFullName().CStr());
    EXOLOG("IsPC: %s\n", m_pStats->m_bIsPC ? "TRUE" : "FALSE");
    EXOLOG("STR: %d\n", m_pStats->GetSTRStat());
    EXOLOG("INT: %d\n", m_pStats->GetINTStat());
    EXOLOG("WIS: %d\n", m_pStats->GetWISStat());
    EXOLOG("DEX: %d\n", m_pStats->GetDEXStat());
    EXOLOG("CON: %d\n", m_pStats->GetCONStat());
    EXOLOG("CHR: %d\n", m_pStats->GetCHAStat());
}
// PC 0x004EF6E0  Xbox 0x00173100
int CSWSCreature::CalculateSpellSaveDC(int nSpellId)
{
    char nWisdomModifier;
    char nCharismaModifier;
    int nDC;

    nCharismaModifier = m_pStats->m_nCharismaModifier;
    nWisdomModifier = m_pStats->m_nWisdomModifier;
    nDC = nWisdomModifier + m_pStats->GetLevel(1) + nCharismaModifier + 5;
    if (m_pStats->HasFeat(CSWFeat::FEAT_FORCE_FOCUS_MASTERY))
    {
        nDC += 4;
    }
    else if (m_pStats->HasFeat(CSWFeat::FEAT_FORCE_FOCUS_ADVANCED))
    {
        nDC += 2;
    }
    else if (m_pStats->HasFeat(CSWFeat::FEAT_FORCE_FOCUS_SENSE))
    {
        nDC += 1;
    }
    return nDC;
}

int CSWSCreature::UpdateSpecialAttacks()
{
    return 0;
}
#ifndef _XBOX
CItemRepository* CSWSCreature::GetItemRepository(int bParty)  // PC 0x004EF770  Xbox 0x00173190
{
    if (bParty)
    {
        if (m_bInParty != 0)
        {
            return g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->GetPartyItems();
        }

        if (g_pAppManager->m_pServerExoApp->GetServerInfo()->m_bPartyPicking &&
            g_pAppManager->m_pServerExoApp->GetPartyTable()->GetIsAvailable(GetId()) &&
            g_pAppManager->m_pClientExoApp->GetInGameGui()
                ->GetPartyAccessPanelUp(0))
        {
            return g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->GetPartyItems();
        }
    }

    return m_pcItemRepository;
}
#endif
int CSWSCreature::GetDead()  // PC 0x004EF820  Xbox 0x001731D0
{
    int nCharacters;
    int nCharacter;
    CSWCCreature* pCharacter;
    unsigned long oidClient;
    CClientExoApp* pClient;

    nCharacters = g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters;
    for (nCharacter = 0; nCharacter < nCharacters; nCharacter++)
    {
        pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(
            nCharacter);
        pClient = g_pAppManager->m_pClientExoApp;
        oidClient = pClient->ServerToClientObjectId(m_nId);
        if (pCharacter != NULL && pCharacter->m_nId == oidClient)
        {
            return 0;
        }
    }
    return CSWSObject::GetDead();
}
// PC 0x004EF890  Xbox 0x00173240
int CSWSCreature::GetDeadTemp()
{
    int nCharacters;
    int nCharacter;
    CSWCCreature* pCharacter;
    unsigned long oidClient;
    CClientExoApp* pClient;

    nCharacters = g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters;
    for (nCharacter = 0; nCharacter < nCharacters; nCharacter++)
    {
        pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(
            nCharacter);
        pClient = g_pAppManager->m_pClientExoApp;
        int nId = m_nId;
        oidClient = pClient->ServerToClientObjectId(nId);
        if (pCharacter != NULL && pCharacter->m_nId == oidClient)
        {
            return GetCurrentHitPoints(0) <= 0;
        }
    }
    return 0;
}
// PC 0x004EF910  Xbox 0x001732C0
int CSWSCreature::OnEndDialog(CSWSObject* pObject)
{
    return g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_END_DIALOGUE], m_nId, 1);
}
#ifndef _XBOX
// Xbox 0x00185820
void CSWSCreature::GetSpikeDetails(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                                   unsigned char nSpikeType)
{
    CItemRepository* pRepository = GetItemRepository(1);
    CExoString sName;
    int nLanguage = g_pAppManager->m_pClientExoApp->GetClientLanguage();

    if (pRepository == NULL)
    {
        return;
    }

    int nAction = lstActions.num;
    for (int i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL &&
            pItem->GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_SPIKE)
        {
            continue;
        }

        CSWItemProperty* pProperty;
        if (nSpikeType == 0)
        {
            if (pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_COMPUTER_SPIKE, 0) &&
                pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_COMPUTER_SPIKE, 0) &&
                pProperty != NULL)
            {
                lstActions.SetSize(nAction + 1);
                pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
                lstActions[nAction].m_sName = sName;
                lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
                lstActions[nAction].m_resIcon = pItem->GetIcon();
                nAction++;
            }
        }
        else if (nSpikeType == 1)
        {
            if (pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE, 0) &&
                pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE, 0) &&
                pProperty != NULL)
            {
                lstActions.SetSize(nAction + 1);
                pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
                lstActions[nAction].m_sName = sName;
                lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
                lstActions[nAction].m_resIcon = pItem->GetIcon();
#ifndef _XBOX
                int nStackSize = pItem->m_nStackSize;
                if (nStackSize <= pItem->m_nCharges)
                {
                    nStackSize = pItem->m_nCharges;
                }
                lstActions[nAction].m_nStackSize = nStackSize;
#endif
                nAction++;
            }
        }
    }
}
#endif
void CSWSCreature::ReceiveExperience(int nExperience)  // PC 0x004EF930  Xbox 0x001732E0
{
    int nPercentXP = 100;
    int nPartyPercentXP = 100;
    float fPercent = 1.0f;
    CSWPartyTable* pParty = g_pAppManager->GetServerExoApp()->GetPartyTable();
#ifdef _XBOX
    CTwoDimArrays* pArraysForNPC = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pArraysForNPC->m_pNPC);
    C2DA* pNPCTable = pArraysForNPC->m_pNPC;
#else
    C2DA* pNPCTable = g_pRules->Get2DArrays()->m_pNPC;
#endif
    if (pParty != NULL)
    {
        int nNPC = pParty->GetNPCID(GetId());
        if (nNPC != -1 && pNPCTable != NULL)
        {
            pNPCTable->GetINTEntry(nNPC, "PercentXP", &nPercentXP);
            m_sTag.CStr();
            fPercent = nPercentXP * 0.01f;
        }
        {
            CExoString sColumn("PercentXP");
            pNPCTable->GetINTEntry(CSWPartyTable::PARTY_XP_ROW, sColumn,
                                  &nPartyPercentXP);
        }
        fPercent = (nPartyPercentXP / 100.0f) * fPercent;
    }
    m_sTag.CStr();
    GetStats()->AddExperience((unsigned long)ceil(nExperience * fPercent));
}
// PC 0x004EFA80  Xbox 0x00173460
int CSWSCreature::GetForceItemMask()
{
    int nMask;
    int nSlot;
    CSWSItem* pItem;

    nMask = 0;
    for (nSlot = 0; nSlot < 18; nSlot++)
    {
        pItem = m_pInventory->GetItemInSlot(nSlot);
        if (pItem != NULL)
        {
            switch (pItem->GetBaseItem()->m_nItemType)
            {
            case 0x1F:
                nMask |= 0x01;
                break;
            case 0x20:
                nMask |= 0x02;
                break;
            case 0x21:
                nMask |= 0x04;
                break;
            case 0x22:
                nMask |= 0x08;
                break;
            case 0x23:
                nMask |= 0x10;
                break;
            case 0x24:
                nMask |= 0x20;
                break;
            case 0x27:
            case 0x28:
            case 0x29:
                nMask |= 0x40;
                break;
            }
        }
    }
    return nMask;
}
// PC 0x004EFB20  Xbox 0x00173500
CSWCCreature* CSWSCreature::GetClientCreature()
{
    if (((CSWSObject*)this)->GetClientObject() != NULL)
    {
        return ((CSWSObject*)this)->GetClientObject()->AsSWCCreature();
    }
    return NULL;
}
// PC 0x004EFB40  Xbox 0x00173520
void CSWSCreature::AddPazaakCard(CSWSItem* pItem)
{
    int nCard = ((unsigned char)pItem->m_nModelVariation + 11) %
        CSWPartyTable::PAZAAK_CARD_COUNT;
    CSWPartyTable* pPartyTable =
        g_pAppManager->GetServerExoApp()->GetPartyTable();
    int nStackSize = pItem->m_nStackSize;
    pPartyTable->m_anPazaakCards[nCard] += nStackSize;

    CItemRepository* pRepository = GetItemRepository(TRUE);
    if (pRepository->FindItemWithBaseItemType(
            CSWBaseItem::ITEM_TYPE_PAZAAK_SIDE_DECK, 0) == OBJECT_INVALID)
    {
        CSWSItem* pSideDeck = new CSWSItem(OBJECT_INVALID);
        CResRef cTemplate("g_I_PazSidebd001");
        pSideDeck->LoadFromTemplate(cTemplate);
        pRepository->AddItem(&pSideDeck, TRUE, FALSE, TRUE);
    }
}
// PC 0x004EFC30  Xbox 0x00173630
int CSWSCreature::RemovePazaakCard(CSWSItem* pItem)
{
    int nCard = (unsigned char)pItem->m_nModelVariation - 1;
    CSWPartyTable* pPartyTable =
        g_pAppManager->GetServerExoApp()->GetPartyTable();
    int nAvailable = pPartyTable->m_anPazaakCards[nCard];
    int nStackSize = pItem->m_nStackSize;
    int nCards = nAvailable < nStackSize ? nAvailable : nStackSize;
    pPartyTable->m_anPazaakCards[nCard] = nAvailable - nCards;
    return nCards;
}
// PC 0x004EFC70  Xbox 0x00173670
float CSWSCreature::GetSpotCheckDistance()
{
    float fDistance;
    CServerExoApp* pServer;
    CTwoDimArrays* p2DArrays;
    C2DA* pRanges;

    fDistance = m_fSpotCheckDistance;
    if (m_bCombatState != 0)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetPlayerCreature() != this)
        {
            CExoString sColumn("PrimaryRange");

            p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(18, sColumn, &fDistance);
        }
    }
    return fDistance;
}
// PC 0x004EFD20  Xbox 0x00173730
float CSWSCreature::GetListenCheckDistance()
{
    float fDistance;
    CServerExoApp* pServer;
    CTwoDimArrays* p2DArrays;
    C2DA* pRanges;

    fDistance = m_fListenCheckDistance;
    if (m_bCombatState != 0)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetPlayerCreature() != this)
        {
            CExoString sColumn("SecondaryRange");

            p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(18, sColumn, &fDistance);
        }
    }
    return fDistance;
}
// PC 0x004EFDD0  Xbox 0x001737F0
int CSWSCreature::GetLastInstantEquip(unsigned long nSlot)
{
    switch (nSlot)
    {
    case 16:
        return m_nLastInstantEquipRight;
    case 32:
        return m_nLastInstantEquipLeft;
    }
    return 0;
}
// PC 0x004EFE00  Xbox 0x00173820
void CSWSCreature::SetLastInstantEquip(unsigned long nSlot, int nValue)
{
    switch (nSlot)
    {
    case 16:
        m_nLastInstantEquipRight = nValue;
        break;
    case 32:
        m_nLastInstantEquipLeft = nValue;
        break;
    }
}
// PC 0x004EFE30  Xbox 0x00173850
void CSWSCreature::InstantUseItem(unsigned long oidItem)
{
    CSWSItem* pItem;
    int nProperty;
    CSWItemProperty* pProperty;

    pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
    for (nProperty = 0; nProperty < pItem->m_nActiveProperties; nProperty++)
    {
        pProperty = pItem->GetActiveProperty(nProperty);
        if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
        {
            Vector vTarget;

            ((CSWSObject*)this)
                ->SpellCastAndImpact(pProperty->m_nSubtype, vTarget, m_nId,
                                     0, oidItem, 0, 0, 0, 0);
            pItem->DecrementUses(nProperty, m_nId);
            if (m_bCombatState != 0)
            {
                m_nUseItemTimer = 3000;
            }
            return;
        }
    }
}
// PC 0x004EFEF0  Xbox 0x00173910
void CSWSCreature::SetAmbientAnimationState(unsigned char nState)
{
    if ((nState == 4 || nState == 3) &&
        (m_nAnimation == 10139 || m_nAnimation == 10117 ||
         m_nAnimation == 10400))
    {
        nState = 14;
    }
    m_nAmbientAnimationState = nState;
}
// PC 0x004EFF30
int CSWSCreature::IsInjured()
{
    extern float g_fInjuredLevel;

    if (m_bDisableInjuredAnim == 0 &&
        (float)((CSWSObject*)this)->GetCurrentHitPoints(0) /
                (float)((CSWSObject*)this)->GetMaxHitPoints(1) <
            g_fInjuredLevel)
    {
        return 1;
    }

    return 0;
}

static inline void NwscreatureTrace1()
{
}

#ifdef _XBOX
// PC 0x004EC370  Xbox 0x0016F960
float CSWSCreature::GetMovementRateFactor()
{
    float fMovementRateFactor = m_fMovementRateFactor;
    if (fMovementRateFactor < 0.125f)
        fMovementRateFactor = 0.125f;
    else if (fMovementRateFactor > 1.5f)
        fMovementRateFactor = 1.5f;
    return fMovementRateFactor;
}
// PC 0x004EC3B0  Xbox 0x0016F9A0
void CSWSCreature::SetMovementRateFactor(float fMovementRateFactor)
{
    if (fMovementRateFactor < 0.125f)
        fMovementRateFactor = 0.125f;
    else if (fMovementRateFactor > 1.5f)
        fMovementRateFactor = 1.5f;
    m_fMovementRateFactor = fMovementRateFactor;
}
// PC 0x004EF770  Xbox 0x00173190
CItemRepository* CSWSCreature::GetItemRepository(int bParty)
{
    if (m_bInParty != 0 && bParty)
        return g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyItems();
    return m_pcItemRepository;
}
// Xbox 0x0016F9D0
void CSWSCreature::AddPrerequisiteSpells(unsigned long nSpell)
{
    const int nNoPrerequisiteSpell = -1;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    if (pSpell != NULL)
    {
        int* pNext = pSpell->m_aPrerequisites;
        if (*pNext != nNoPrerequisiteSpell)
        {
            int* pPrerequisite = pNext;
            do
            {
                unsigned long nPrerequisite = *pNext;
                if (m_pStats->HasSpell(0, nPrerequisite, 1) == 0)
                {
                    AddPrerequisiteSpells(nPrerequisite);
                }
                pNext = ++pPrerequisite;
            }
            while (*pNext != nNoPrerequisiteSpell);
        }
        m_pStats->AddKnownSpell(m_pStats->m_nClassCount - 1, nSpell);
    }
}
// Xbox 0x00185820
void CSWSCreature::GetSpikeDetails(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                                   unsigned char nSpikeType)
{
    CItemRepository* pRepository = GetItemRepository(1);
    CExoString sName;
    int nLanguage = g_pAppManager->m_pClientExoApp->GetClientLanguage();
    int nAction = lstActions.num;

    if (pRepository == NULL)
    {
        return;
    }

    for (unsigned long i = 0; i < pRepository->m_lstItems.num; i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL &&
            pItem->GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_SPIKE)
        {
            continue;
        }

        CSWItemProperty* pProperty;
        if (nSpikeType == 1)
        {
            if (pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE, 0) &&
                pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE, 0) &&
                pProperty != NULL)
            {
                lstActions.SetSize(nAction + 1);
                pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
                if (pItem->m_nStackSize > 1)
                    sName.Format("%s (%d)", sName.CStr(), pItem->m_nStackSize);
                else if (pItem->m_nCharges > 1)
                    sName.Format("%s (%d)", sName.CStr(), pItem->m_nCharges);
                lstActions[nAction].m_sName = sName;
                lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
                lstActions[nAction].m_resIcon = pItem->GetIcon();
                nAction++;
            }
        }
        else if (nSpikeType == 0)
        {
            if (pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_COMPUTER_SPIKE, 0) &&
                pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_COMPUTER_SPIKE, 0) &&
                pProperty != NULL)
            {
                lstActions.SetSize(nAction + 1);
                pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
                lstActions[nAction].m_sName = sName;
                lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
                lstActions[nAction].m_resIcon = pItem->GetIcon();
                nAction++;
            }
        }
    }
}
#endif
