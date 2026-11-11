#include <aurlist.h>
#include "nwccreature.h"
#include "pathfindinfo.h"
#include "aurcamera.h"

#include "nwccreatureappearance.h"
#include "gameobjectarray.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwcanimbase.h"
#include "exobase.h"
#include <stdlib.h>
#include <math.h>
#ifdef _XBOX
#include <float.h>
#endif
#include <string.h>
#include <stdio.h>
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "nwscreature.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwsitem.h"
#include "nwsobject.h"
#include "nwmessage.h"
#include "gameobjectarray.h"
#include "nwcarea.h"
#include "nwcitem.h"
#include "nwcprojectile.h"
#include "swparty.h"
#include "serverexoapp.h"
#include "soundset.h"
#include "nwcanimbase.h"
#include "nwcmodule.h"
#include "aurcamera.h"
#include "partytables.h"
#include "guiingame.h"
#include "nwcvisualeffectonobject.h"
#include "nwcvisualeffect.h"
#include "clientoptions.h"
#include "aurora.h"
#include "../../libsource/AURORA/aurpointer.h"
#include "nwcanimbase.h"
#include "nwitem.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "swsroom.h"
#include "nwrules.h"
#include "nwitem.h"
#include "nwfeat.h"
#include "aurscene.h"
#include "nwspell.h"
#include "nwrace.h"
#include "nwcspellvisual.h"
#include "nwcplaceable.h"
#include "nwsplaceable.h"
#include "nwscript.h"
#include "nwskill.h"
#include "nwcanimbaseheadwield.h"
#include "nwscreaturestats.h"
#include "worldtimer.h"
#include "clientaimaster.h"

static Vector* AllocateVectorArray(int nCount)
{
    return new Vector[nCount];
}
#include "swmain.h"
#include "resgff.h"
#include "nwsdoor.h"

static inline void NwccreatureTrace3();
static inline void NwccreatureTrace2();
static inline void NwccreatureTrace1();

#define NWC_CREATURE_SIGHT_HEIGHT 1.5f
#define SWCCREATURE_LIPSYNC_END_TIME_MIN 0.99999f
#define SWCCREATURE_LIPSYNC_END_TIME_MAX 1.00001f
#define LIP_VISEME_ANIMATION_SCALE 0.0625f

#include "../../libsource/AURORA/aurpointer.h"
#include "nwcdoor.h"
#include "nwscombatround.h"

const int CSWCCreature::MODE_DEFAULT = 0;
const int CSWCCreature::MODE_SWDRIVE = 1;
const int CSWCCreature::MODE_SWCOMBAT = 2;

float g_fEmoteIconHeight = 2.56f;

const char* CSWCCreature::CCREATURE_EMOTE_ICON[13] =
{
    "IR_CHEER", "IR_SALUTE", "IR_LAUGH", "IR_GOODBYE", "IR_GREET",
    "IR_BOW", "IR_THREATEN", "IR_BORED", "IR_WOOSEY", "IR_SCRATCH",
    "IR_BEG", "IR_TIRED", "IR_NOD"
};

const unsigned short CSWCCreature::CCREATURE_EMOTE_ANIMATION[13] =
{
    CREATURE_ANIMATION_FIREFORGET_VICTORY,
    CREATURE_ANIMATION_FIREFORGET_SALUTE,
    CREATURE_ANIMATION_LOOPING_TALK_LAUGHING,
    CREATURE_ANIMATION_FIREFORGET_GREETING,
    CREATURE_ANIMATION_FIREFORGET_GREETING,
    CREATURE_ANIMATION_FIREFORGET_BOW,
    CREATURE_ANIMATION_FIREFORGET_TAUNT,
    CREATURE_ANIMATION_FIREFORGET_PAUSE_BORED,
    CREATURE_ANIMATION_LOOPING_PAUSE_DRUNK,
    CREATURE_ANIMATION_FIREFORGET_PAUSE_SCRATCH_HEAD,
    CREATURE_ANIMATION_LOOPING_TALK_PLEADING,
    CREATURE_ANIMATION_LOOPING_PAUSE_TIRED,
    CREATURE_ANIMATION_LOOPING_LISTEN
};

const unsigned short CSWCCreature::CCREATURE_EMOTE_SOUNDID[13] =
{
    EMOTE_SOUND_INVALID, EMOTE_SOUND_INVALID, EMOTE_SOUND_INVALID,
    EMOTE_SOUND_INVALID, 28, EMOTE_SOUND_INVALID, EMOTE_SOUND_INVALID,
    0, 0, 0, 0, 0, 0
};

const unsigned long CSWCCreature::CCREATURE_EMOTE_STRREF[13] =
{
    1010, 703, 66856, 66857, 1069, 699, 66859,
    66861, 8047, 984, 985, 66863, 66864
};

float DriveWalkThreshold = 0.6f;

extern "C" int g_nCombatAnimation = 0;
int renderintersects = 0;
int renderwalktry = 0;
int RenderClosestPathPoint = 0;
int RenderTriangles = 0;
int g_nHackAlignment = -1;
static CExoString g_sHeadTurnBone = "";

inline int CSWCCreature::IsHostileUsableItem(CSWSItem* pItem)
{
    return pItem != NULL &&
        !m_pArea->GetServerArea()->GetPlayerRestrictMode() &&
        pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_HOSTILE_USABLE;
}
// PC 0x004015A0
CSWCCombatAttackData::~CSWCCombatAttackData()
{
}
// PC 0x00646650
void CSWCCreature::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x004015A0
void CSWCCreature::UpdateMonkVisuals()
{
}

int CSWCCreature::ResetAnimationState()
{
    if (GetLoopingAnimation() == CREATURE_ANIMATION_COMBAT_READY)
        SetAnimation(CREATURE_ANIMATION_COMBAT_READY, 0, 0);
    return 1;
}
// PC 0x00616FF0  Xbox 0x00047600
inline CResRef CSWCCreature::GetPortrait(int bAlternate)
{
    if (m_pStats != NULL)
    {
        return m_pStats->GetPortrait(bAlternate);
    }
    return CResRef("");
}
// PC 0x00617030  Xbox 0x00047640
inline CResRef CSWCCreature::GetPortrait(unsigned char nSize)
{
    if (m_pStats != NULL)
    {
        return m_pStats->m_cPortrait.GetPortrait(nSize);
    }
    return CResRef("");
}
// PC 0x00617070  Xbox 0x00047680
inline unsigned short CSWCCreature::GetPortraitId()
{
    if (m_pStats != NULL)
        return m_pStats->m_nPortraitId;
    return PORTRAIT_INVALID;
}
// PC 0x00617090  Xbox 0x000476A0
inline CExoString CSWCCreature::GetName()
{
    return m_pStats->GetFullName();
}
// PC 0x006170C0  Xbox 0x000476D0
inline int CSWCCreature::IsHostile()
{
    return this->m_nHostile == HOSTILE_STATE_HOSTILE;
}

void CSWCCreature::DoFootstepRumble(int nFootstepType)
{
    g_pAppManager->GetClientExoApp()->LookUpAndPerformRumbleWithCutOff(
        nFootstepType, 0, m_vPosition);
}
// PC 0x0060B380  Xbox 0x0003B190
void CSWCCreature::SetMoveOrientation(const Vector& vOrientation)
{
    m_vMoveOrientation = normalize(vOrientation);
}
// PC 0x0060B3C0  Xbox 0x0003B1D0
void CSWCCreature::SetDesiredOrientation(const Vector& vOrientation)
{
    m_vDesiredOrientation = normalize(vOrientation);
}
// PC 0x0060B400  Xbox 0x0003B210
void HitGroundEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CExoString sSound;
    CSWCCreature* pCreature = static_cast<CSWCCreature*>(pData);
    if (pCreature != NULL)
    {
        int nSoundAppearance;
        unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
        g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
            nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_SOUNDAPPTYPE,
            &nSoundAppearance);

        int nMaterial = CSWRoomSurfaceMesh::SURFACE_MATERIAL_UNDEFINED;
        if (pCreature->m_pArea != NULL)
            nMaterial = pCreature->m_pArea->GetSurfaceMaterial(
                pCreature->m_vPosition);

        switch (nMaterial)
        {
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_DIRT:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_GRASS:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_LEAVES:
            g_pRules->Get2DArrays()->GetAppearanceSounds()->GetCExoStringEntry(
                nSoundAppearance, "FallDirt", &sSound);
            break;
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_METAL:
            g_pRules->Get2DArrays()->GetAppearanceSounds()->GetCExoStringEntry(
                nSoundAppearance, "FallMetal", &sSound);
            break;
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_STONE:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_WOOD:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_CARPET:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_LAVA:
            g_pRules->Get2DArrays()->GetAppearanceSounds()->GetCExoStringEntry(
                nSoundAppearance, "FallHard", &sSound);
            break;
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_WATER:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_PUDDLES:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_SWAMP:
        case CSWRoomSurfaceMesh::SURFACE_MATERIAL_MUD:
            g_pRules->Get2DArrays()->GetAppearanceSounds()->GetCExoStringEntry(
                nSoundAppearance, "FallWater", &sSound);
            break;
        }

        g_pExoSound->Play3DOneShotSound(
            (char*)sSound.CStr(), pCreature->m_vPosition, 0.0f, 0, 0, 0, 0.0f, 0.0f);
    }
}
// PC 0x0060B630  Xbox 0x0003B470
CSWCCombatAttackData::CSWCCombatAttackData()
{
    m_oidCreature = OBJECT_INVALID;
    m_oidAttackObject = OBJECT_INVALID;
    m_bHit = 0;
    m_nAttackState = 0;
    m_nAttackRoll = 0;
    m_nDamage = 0;
    m_nAttackResult = 0;
    m_nAttackFlags = 0;
    m_nTargetObjectId = 0;
    m_nDamageType = 0;
    m_nDamageAmount = 0;
    m_nCriticalMultiplier = 0;
    m_vImpactPosition = Vector();
    m_nAttackIndex = -1;
    m_nWeaponIndex = 0;
    m_nAttackSequence = 0;
    m_nImpactVisual = 0;
    m_nImpactX = 0;
    m_nImpactY = 0;
    m_pNextAttack = NULL;
}
// PC 0x0060B6B0  Xbox 0x0003B500
void CSWCCombatAttackData::Copy(CSWCCombatAttackData* pAttackData)
{
    m_bHit = pAttackData->m_bHit;
    m_nAttackState = pAttackData->m_nAttackState;
    m_oidCreature = pAttackData->m_oidCreature;
    m_oidAttackObject = pAttackData->m_oidAttackObject;
    m_nAttackRoll = pAttackData->m_nAttackRoll;
    m_nDamage = pAttackData->m_nDamage;
    m_nAttackResult = pAttackData->m_nAttackResult;
    m_nTargetObjectId = pAttackData->m_nTargetObjectId;
    m_nAttackFlags = pAttackData->m_nAttackFlags;
    m_nDamageType = pAttackData->m_nDamageType;
    m_nDamageAmount = pAttackData->m_nDamageAmount;
    m_nCriticalMultiplier = pAttackData->m_nCriticalMultiplier;
    m_vImpactPosition = pAttackData->m_vImpactPosition;
    m_nAttackIndex = pAttackData->m_nAttackIndex;
    m_nWeaponIndex = pAttackData->m_nWeaponIndex;
    m_nAttackSequence = pAttackData->m_nAttackSequence;
    m_nImpactVisual = pAttackData->m_nImpactVisual;
    m_nImpactX = pAttackData->m_nImpactX;
    m_nImpactY = pAttackData->m_nImpactY;
    m_pNextAttack = pAttackData->m_pNextAttack;
}
// PC 0x0060B750  Xbox 0x0003B5A0
CSWCCreaturePartyStatus::CSWCCreaturePartyStatus()
{
    m_nUnknown04 = 1;
    m_nUnknown08 = 1;
    m_nUnknown0C = 0;
    m_nUnknown14 = OBJECT_INVALID;
    m_sUnknown18 = "";
}
// PC 0x00610C90  Xbox 0x00040ED0
void SwingShortEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CExoString sSound("SwingShort");
    CSWCCombatAttackData* pAttackData =
        static_cast<CSWCCombatAttackData*>(pData);

    if (pAttackData != NULL)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(pAttackData->m_oidCreature);

        if (pCreature != NULL)
        {
            pCreature->PlaySwingSound(sSound, pAttackData, 3);
        }
    }
}
// PC 0x00610D10  Xbox 0x00040F50
void SwingLongEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CExoString sSound("SwingLong");
    CSWCCombatAttackData* pAttackData =
        static_cast<CSWCCombatAttackData*>(pData);

    if (pAttackData != NULL)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(pAttackData->m_oidCreature);

        if (pCreature != NULL)
        {
            pCreature->PlaySwingSound(sSound, pAttackData, 3);
        }
    }
}
// PC 0x00610D90  Xbox 0x00040FD0
void SwingTwirlEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CExoString sSound("SwingTwirl");
    CSWCCombatAttackData* pAttackData =
        static_cast<CSWCCombatAttackData*>(pData);

    if (pAttackData != NULL)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(pAttackData->m_oidCreature);

        if (pCreature != NULL)
        {
            pCreature->PlaySwingSound(sSound, pAttackData, 3);
        }
    }
}
// PC 0x00610E10  Xbox 0x00041050
void HitClashEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CExoString sSound("Clash");
    CSWCCombatAttackData* pAttackData =
        static_cast<CSWCCombatAttackData*>(pData);

    if (pAttackData != NULL)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(pAttackData->m_oidCreature);

        if (pCreature != NULL)
        {
            pCreature->PlaySwingSound(sSound, pAttackData, 2);
        }
    }
}
// PC 0x00610E90  Xbox 0x000410D0
void HitContactEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CSWCCombatAttackData* pAttackData =
        static_cast<CSWCCombatAttackData*>(pData);

    if (pAttackData != NULL)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(pAttackData->m_oidCreature);

        if (pCreature != NULL)
        {
            pCreature->ShowLightSaberContactVisual(pAttackData);
        }
    }
}
// PC 0x00610EC0  Xbox 0x00041100
void HitParryEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CExoString sSound("Parry");
    CSWCCombatAttackData* pAttackData =
        static_cast<CSWCCombatAttackData*>(pData);

    if (pAttackData != NULL)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(pAttackData->m_oidCreature);

        if (pCreature != NULL)
        {
            pCreature->PlaySwingSound(sSound, pAttackData, 2);
        }
    }
}
// PC 0x00610F40  Xbox 0x000411A0
void SwitchWeaponEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CSWCCreature* pCreature = static_cast<CSWCCreature*>(pData);
    int bForceUpdate = 0;
    int bLoading = 0;
    int bPower = 1;
    if (sEvent != NULL)
    {
        if (strcmp(sEvent, "AppearanceForceUpdate") == 0)
        {
            bForceUpdate = 1;
            if (pCreature != NULL && pCreature->m_nCombatState == 0)
                bPower = 0;
        }
        if (strcmp(sEvent, "Loading") == 0)
        {
            bLoading = 1;
            bPower = 0;
        }
    }

    CSWCItem* pPreviousItem = NULL;
    if (pCreature != NULL)
    {
        unsigned long oidRight = pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
        CSWCItem* pRightItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidRight);
        unsigned long oidLeft = pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);
        CSWCItem* pLeftItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidLeft);
        float fWeaponScale;
        bool bScaleMissing;
        {
            CExoString sColumn("WEAPONSCALE");
            unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
            bScaleMissing = !g_pRules->Get2DArrays()->m_pAppearance->GetFLOATEntry(
                nAppearance, sColumn, &fWeaponScale);
        }
        if (bScaleMissing)
            fWeaponScale = 1.0f;

        if (pCreature->m_oidVisibleRightWeapon != oidRight || bForceUpdate)
        {
            if (pRightItem != NULL)
            {
                Gob* pModel = pRightItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                pRightItem->SetEquippedBy(pCreature);
                pRightItem->m_oidVisibleOwner = pCreature->GetId();
                if (pModel != NULL)
                {
                    if (pCreature == g_pAppManager->GetClientExoApp()->GetPlayerCreature())
                        pModel->SetLightPriority(Gob::LIGHT_PRIORITY_PLAYER_WEAPON);
                    else
                        pModel->SetLightPriority(Gob::LIGHT_PRIORITY_OTHER_WEAPON);
                    if (pCreature->HasVisualEffectApplied(CSWCVisualEffectOnObject::VFX_STEALTH_FRAMEBUFFER))
                    {
                        CSWCItem* pOldItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                            pCreature->m_oidVisibleRightWeapon);
                        if (pOldItem != NULL)
                            pOldItem->DisableDistortion();
                        pRightItem->EnableDistortion();
                    }
                    pCreature->GetAnimationBase()->Wield(pRightItem->GetId(), "rhand",
                        CSWCAnimBase::WIELD_HAND_RIGHT, fWeaponScale);
                    if (pCreature->AnimationLooping(pCreature->GetCurrentAnimation()))
                        pCreature->SetAnimation(pCreature->GetCurrentAnimation(), 0, 0);
                    pRightItem->PowerItem(bPower, 0,
                        !pCreature->GetLastInstant(INVENTORY_SLOT_RIGHTHAND), bForceUpdate | bLoading);
                }
            }
            else
            {
                Gob* pModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_RIGHTHAND, 1);
                if (pModel != NULL)
                {
                    pPreviousItem = (CSWCItem*)pModel->GetExternalData();
                    if (pPreviousItem != NULL)
                    {
                        pPreviousItem->m_oidVisibleOwner = OBJECT_INVALID;
                        pPreviousItem->PowerItem(0, 0,
                            !pCreature->GetLastInstant(INVENTORY_SLOT_RIGHTHAND), bForceUpdate | bLoading);
                    }
                }
                pCreature->GetAnimationBase()->Wield(OBJECT_INVALID, "",
                    CSWCAnimBase::WIELD_HAND_RIGHT, 1.0f);
            }
            pCreature->m_oidVisibleRightWeapon = oidRight;
        }

        if (pCreature->m_oidVisibleLeftWeapon != oidLeft || bForceUpdate)
        {
            if (pLeftItem != NULL)
            {
                Gob* pModel = pLeftItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                pLeftItem->SetEquippedBy(pCreature);
                pLeftItem->m_oidVisibleOwner = pCreature->GetId();
                if (pModel != NULL)
                {
                    if (pCreature == g_pAppManager->GetClientExoApp()->GetPlayerCreature())
                        pModel->SetLightPriority(Gob::LIGHT_PRIORITY_PLAYER_WEAPON);
                    else
                        pModel->SetLightPriority(Gob::LIGHT_PRIORITY_OTHER_WEAPON);
                    if (pCreature->HasVisualEffectApplied(CSWCVisualEffectOnObject::VFX_STEALTH_FRAMEBUFFER))
                    {
                        CSWCItem* pOldItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                            pCreature->m_oidVisibleLeftWeapon);
                        if (pOldItem != NULL)
                            pOldItem->DisableDistortion();
                        pLeftItem->EnableDistortion();
                    }
                    pCreature->GetAnimationBase()->Wield(pLeftItem->GetId(), "lhand",
                        CSWCAnimBase::WIELD_HAND_LEFT, fWeaponScale);
                    if (pCreature->AnimationLooping(pCreature->GetCurrentAnimation()))
                        pCreature->SetAnimation(pCreature->GetCurrentAnimation(), 0, 0);
                    pLeftItem->PowerItem(bPower, 0,
                        !pCreature->GetLastInstant(INVENTORY_SLOT_LEFTHAND), 0);
                }
            }
            else
            {
                Gob* pModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_LEFTHAND, 1);
                if (pModel != NULL)
                {
                    pPreviousItem = (CSWCItem*)pModel->GetExternalData();
                    if (pPreviousItem != NULL)
                        pPreviousItem->m_oidVisibleOwner = OBJECT_INVALID;
                }
                if (pPreviousItem != NULL)
                    pPreviousItem->PowerItem(0, 0,
                        !pCreature->GetLastInstant(INVENTORY_SLOT_RIGHTHAND), 0);
                pCreature->GetAnimationBase()->Wield(OBJECT_INVALID, "",
                    CSWCAnimBase::WIELD_HAND_LEFT, 1.0f);
            }
            pCreature->m_oidVisibleLeftWeapon = oidLeft;
        }
    }
}

static void DeleteStreamingSource(CExoStreamingSoundSource* pSource)
{
    delete pSource;
}
// PC 0x00611470  Xbox 0x000416D0
void CSWCCreature::PlaySoundSetSound(unsigned short nSound, int b3D)
{
    STR_RES strRes;
    if (!GetSoundSetStrres(nSound, strRes))
        return;

    if (GetServerCreature() != NULL &&
        (GetServerCreature()->GetDead() || GetServerCreature()->GetDeadTemp()) &&
        nSound != SOUNDSET_DEATH)
        return;

    unsigned char nPriorityGroup;
    if (g_pAppManager->GetClientExoApp()->GetPlayerCreature() == this)
        nPriorityGroup = CExoStreamingSoundSource::PRIORITY_GROUP_PLAYER_VOICE;
    else if (g_pAppManager->GetClientExoApp()->GetPlayerByGameObjectID(GetObjectId()) != NULL)
        nPriorityGroup = CExoStreamingSoundSource::PRIORITY_GROUP_PARTY_VOICE;
    else
    {
        int nSizeCategory;
        unsigned short nAppearance = GetAppearance()->m_nAppearance;
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = pArrays->m_pAppearance;
        pTable->GetINTEntry(nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY, &nSizeCategory);
        nPriorityGroup = nSizeCategory <= CREATURE_SIZE_MEDIUM ?
            CExoStreamingSoundSource::PRIORITY_GROUP_CREATURE_VOICE :
            CExoStreamingSoundSource::PRIORITY_GROUP_LARGE_CREATURE_VOICE;
    }

    m_nVocalizationFlag = 0;
    if (m_pStreamingSound != NULL)
    {
        delete m_pStreamingSound;
        m_pStreamingSound = NULL;
    }
    m_pStreamingSound = new CExoStreamingSoundSource;
    if (m_pStreamingSound != NULL)
    {
        m_pStreamingSound->Set3D(b3D);
        m_pStreamingSound->SetPriorityGroup(nPriorityGroup);
        if (b3D)
            m_pStreamingSound->SetPosition(m_vPosition);
        if (!m_pStreamingSound->Play(strRes.m_resSound, RESTYPE_WAV, 0, 0, 0))
        {
            delete m_pStreamingSound;
            m_pStreamingSound = NULL;
        }
    }
}
// PC 0x00616A00  Xbox 0x00046F50
void Blur(CAurObject* pObject, char* sEvent, void* pData)
{
    static_cast<CSWCCreature*>(pData)->BlurWeapon(1);
}
// PC 0x00616A10  Xbox 0x00046F60
void Unblur(CAurObject* pObject, char* sEvent, void* pData)
{
    static_cast<CSWCCreature*>(pData)->BlurWeapon(0);
}
// PC 0x006116A0  Xbox 0x00041930
void CSWCCreature::BlurWeapon(int bBlur)
{
    if (m_bBlurWeapon != bBlur)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(
            GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
        if (pItem == NULL)
        {
            m_bBlurWeapon = bBlur;
        }
        else
        {
            g_pRules->m_pBaseItems->GetBaseItem(pItem->m_nBaseItem);
            if (pItem->m_nUnknown198 != 1)
                m_bBlurWeapon = bBlur;
        }
    }
}
// PC 0x00611700  Xbox 0x00041990
int CSWCCreature::ComputeWeaponOverlays(int bCheckAnimations, int bUnused)
{
    CSWCAnimBase* pAnimationBase = GetAnimationBase();
    if (pAnimationBase == NULL)
    {
        return 0;
    }
    if (bCheckAnimations == 1)
    {
        CSWCAnimBase* pOverlayBase = GetAnimationBase();
        if (pOverlayBase->GetOverlayAnimationPlaying(10030) != 1)
        {
            CSWCAnimBase* pNextOverlayBase = GetAnimationBase();
            if (pNextOverlayBase->GetOverlayAnimationPlaying(10155) != 1)
            {
                CSWCAnimBase* pThirdOverlayBase = GetAnimationBase();
                if (pThirdOverlayBase->GetOverlayAnimationPlaying(10038) != 1)
                {
                    CSWCAnimBase* pFourthOverlayBase = GetAnimationBase();
                    if (pFourthOverlayBase->GetOverlayAnimationPlaying(10154) != 1)
                    {
                        CSWCAnimBase* pFifthOverlayBase = GetAnimationBase();
                        if (pFifthOverlayBase->GetOverlayAnimationPlaying(10039) != 1)
                        {
                            CSWCAnimBase* pSixthOverlayBase = GetAnimationBase();
                            if (pSixthOverlayBase->GetOverlayAnimationPlaying(10040) != 1)
                            {
                                CSWCAnimBase* pSeventhOverlayBase = GetAnimationBase();
                                if (pSeventhOverlayBase->GetOverlayAnimationPlaying(10041) != 1)
                                {
                                    CSWCAnimBase* pLastOverlayBase = GetAnimationBase();
                                    if (pLastOverlayBase->GetOverlayAnimationPlaying(10042) != 1 &&
                                        m_bAmbientPause == 0 && m_nWeaponOverlayState != 0)
                                    {
                                        unsigned long oidLeftItem = GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);
                                        g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidLeftItem);
                                        if (m_nCombatState != COMBAT_STATE_ACTIVE)
                                        {
                                            GetAnimationBase();
                                            unsigned long oidRightItem = GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
                                            g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidRightItem);
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
    return 1;
}
// PC 0x00611850  Xbox 0x00041AE0
void CSWCCreature::RotateCreatureToOrientation(const Vector& vOrientation)
{
    if (GetServerCreature() != NULL &&
        (GetServerCreature()->IsDebilitated() || GetServerCreature()->GetDead() ||
         GetServerCreature()->GetDeadTemp()))
        return;

    if (GetServerCreature() == NULL)
        return;

    if (GetServerCreature() != NULL && GetServerCreature()->m_bCombatState &&
        !GetServerCreature()->HasAction(CSWSObject::ACTION_MOVE_TO_POINT))
    {
        if ((GetServerCreature()->m_oidAttemptedAttackTarget != OBJECT_INVALID &&
             GetServerCreature()->GetReputation(
                 GetServerCreature()->GetAttemptedAttackTarget(), 0) <=
                 CSWSObject::MAX_HOSTILE_REPUTATION) ||
            (GetServerCreature()->m_oidAttemptedSpellTarget != OBJECT_INVALID &&
             GetServerCreature()->GetReputation(
                 GetServerCreature()->GetAttemptedSpellTarget(), 0) <=
                 CSWSObject::MAX_HOSTILE_REPUTATION))
        {
            CSWSCreature* pCreature = GetServerCreature();
            Vector vCurrent = pCreature->m_vOrientation;
            float fAngle = Yaw(vOrientation) - Yaw(vCurrent);
            if (fAngle > 180.0f)
                fAngle -= 360.0f;
            if (fAngle < -180.0f)
                fAngle += 360.0f;
            if (fAngle < 0.0f)
            {
                if (fAngle < -fCombatTurnSpeed * m_nTimeDelta)
                    fAngle = -fCombatTurnSpeed * m_nTimeDelta;
            }
            else
            {
                if (fAngle > fCombatTurnSpeed * m_nTimeDelta)
                    fAngle = fCombatTurnSpeed * m_nTimeDelta;
            }
            Vector vNew = YawPitchRoll(fAngle, 0.0f, 0.0f) * vCurrent;
            pCreature->SetOrientation(vNew);
            SetOrientation(vNew);
            return;
        }
    }

    {
        Vector vFacing = vOrientation;
        vFacing.m_fZ = 0.0f;
        vFacing = normalize(vFacing);
        Vector vNew;
        float fDirection = cross(m_vOrientation, vFacing).m_fZ >= 0.0f ? 1.0f : -1.0f;
        float fDot = vFacing * m_vOrientation;
        float fYaw = Yaw(m_vOrientation);
        float fDifference = (float)fabs(fYaw - Yaw(vFacing));
        if (fDot < 1.0f && fDifference > 15.0f &&
            (m_nCurrentAnimation == CREATURE_ANIMATION_COMBAT_IDLE ||
             m_nCurrentAnimation == CREATURE_ANIMATION_PAUSE_READY ||
             m_nCurrentAnimation == 10054 || m_nCurrentAnimation == 10053 ||
             m_nCurrentAnimation == 10038 || m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_PLEADING ||
             m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_FORCEFUL || m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_LAUGHING ||
             m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_SAD || m_nCurrentAnimation == 10154 ||
             m_nCurrentAnimation == 10155 || m_nCurrentAnimation == 10030))
        {
            if (m_nCurrentAnimation == 10054 || m_nCurrentAnimation == 10053)
                AbortFireForgetAnimation(0);
            if (fDirection < 0.0f)
            {
                if (m_nCurrentAnimation == CREATURE_ANIMATION_COMBAT_IDLE ||
                    m_nCurrentAnimation == 10038 || m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_PLEADING ||
                    m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_FORCEFUL || m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_LAUGHING ||
                    m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_SAD || m_nCurrentAnimation == 10154 ||
                    m_nCurrentAnimation == 10155 || m_nCurrentAnimation == 10030)
                {
                    if (GetAnimationBase() != NULL &&
                        GetAnimationBase()->m_nAnimSet == 1 &&
                        m_nCurrentAnimation != 367 && m_nCurrentAnimation != 368)
                        AddFireForgetAnimation(368, NULL);
                }
                else if (m_nCurrentAnimation == CREATURE_ANIMATION_PAUSE_READY)
                    AddFireForgetAnimation(368, NULL);
            }
            else
            {
                if (m_nCurrentAnimation == CREATURE_ANIMATION_COMBAT_IDLE ||
                    m_nCurrentAnimation == 10038 || m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_PLEADING ||
                    m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_FORCEFUL || m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_LAUGHING ||
                    m_nCurrentAnimation == CREATURE_ANIMATION_LOOPING_TALK_SAD || m_nCurrentAnimation == 10154 ||
                    m_nCurrentAnimation == 10155 || m_nCurrentAnimation == 10030)
                {
                    if (GetAnimationBase() != NULL &&
                        GetAnimationBase()->m_nAnimSet == 1 &&
                        m_nCurrentAnimation != 367 && m_nCurrentAnimation != 368)
                        AddFireForgetAnimation(367, NULL);
                }
                else if (m_nCurrentAnimation == CREATURE_ANIMATION_PAUSE_READY)
                    AddFireForgetAnimation(367, NULL);
            }
        }

        float fSpeed = (1.0f - fDot) * 3.0f;
        if (m_nCurrentAnimation == CREATURE_ANIMATION_RUN)
            fSpeed *= 2.0f;
        if (fSpeed <= 0.55f)
            fSpeed = 0.55f;
        else if (fSpeed >= 1.0f)
            fSpeed = 1.0f;
        float fTurn = fSpeed * m_nTimeDelta * 0.004f;
        if (cos(fTurn) <= fDot)
            vNew = vOrientation;
        else
        {
            fTurn *= fDirection;
            vNew.m_fX = (float)cos(fTurn) * m_vOrientation.m_fX -
                (float)sin(fTurn) * m_vOrientation.m_fY;
            vNew.m_fY = (float)cos(fTurn) * m_vOrientation.m_fY +
                (float)sin(fTurn) * m_vOrientation.m_fX;
        }
        vNew.m_fZ = 0.0f;
        vNew = normalize(vNew);
        SetOrientation(vNew);
    }
}
// PC 0x00611EC0  Xbox 0x00042190
void CSWCCreature::ClientServerSyncHandler_Orientation(const Vector& vOrientation)
{
    m_vDesiredOrientation = normalize(vOrientation);
    m_vMoveOrientation = normalize(vOrientation);
    if (AnimationStationary(GetCurrentAnimation()) != 0)
    {
        RotateCreatureToOrientation(m_vDesiredOrientation);
    }
    else
    {
        RotateCreatureToOrientation(m_vMoveOrientation);
    }
}
// PC 0x00611F50  Xbox 0x00042220
void CSWCCreature::ClientServerSyncHandler_Position(const Vector& vPosition,
                                                   int nAnimation, float fBlend)
{
    float fDistance;
    m_vDesiredPosition = vPosition;
    MoveCreatureToPosition(vPosition);
    SetGroundNormal(Vector(0.0f, 0.0f, vPosition.m_fZ));
    if (AnimationStationary(nAnimation) == 0)
    {
        float fSpeed;
        float fSpeedFactor;
        float fAnimationSpeed;
        {
            fDistance = 0.0f;
            if (AnimationWalking(nAnimation))
            {
                fSpeed = m_fWalkSpeed;
                g_pRules->Get2DArrays()->m_pAppearance->GetFLOATEntry(
                    m_pAppearance->m_nAppearance,
                    CTwoDimArrays::COL2DA_APPEARANCETABLE_WALKDIST, &fDistance);
            }
            else if (AnimationRunning(nAnimation))
            {
                fSpeed = m_fDriveSpeed;
                if (m_pAppearance->GetRunLooksAwfulRate() > fSpeed)
                    SetRunLooksAwful(1);
                else
                    SetRunLooksAwful(0);
                g_pRules->Get2DArrays()->m_pAppearance->GetFLOATEntry(
                    m_pAppearance->m_nAppearance,
                    CTwoDimArrays::COL2DA_APPEARANCETABLE_RUNDIST, &fDistance);
            }
            fDistance *= 1000.0f;
            fSpeedFactor = ComputeSpeedFactor(fSpeed);
            m_fSpeedFactor = fSpeedFactor;
            fAnimationSpeed = GetAnimationLength(GetCurrentAnimation());
            if (fDistance == 0.0f || fAnimationSpeed == 0.0f)
                fAnimationSpeed = 0.0f;
            else
                fAnimationSpeed = fSpeed * 0.001f / (fDistance / fAnimationSpeed);
        }

        if (fBlend < 0.5f && nAnimation == CREATURE_ANIMATION_RUN)
        {
            fBlend = m_pAnimBase->GetAnimationLength(m_nIdleAnimation);
            g_pRules->Get2DArrays()->m_pAppearance->GetFLOATEntry(
                m_pAppearance->m_nAppearance,
                CTwoDimArrays::COL2DA_APPEARANCETABLE_WALKDIST, &fDistance);
            fDistance *= 1000.0f;
            if (fBlend == 0.0f || fDistance == 0.0f)
                fAnimationSpeed = 0.0f;
            else
                fAnimationSpeed = fSpeed * 0.001f / (fDistance / fBlend);
            float fAnimationTime = m_pAnimBase->GetAnimationTime(GetCurrentAnimation());
            if (fAnimationTime > 0.0f)
            {
                CSWCAnimBase* pAnimationBase = m_pAnimBase;
                float fBlendTime = fAnimationTime /
                    pAnimationBase->GetAnimationLength(GetCurrentAnimation());
                m_pAnimBase->SetAnimation(m_nIdleAnimation,
                    fAnimationSpeed * fSpeedFactor, 0,
                    fBlendTime * fBlend * 0.001f);
            }
            else
                m_pAnimBase->SetAnimation(m_nIdleAnimation,
                    fAnimationSpeed * fSpeedFactor, 0, 0.0f);
        }
        else
        {
            if (m_pAnimBase->m_bRunLooksAwful == 1)
            {
                fBlend = m_pAnimBase->GetAnimationLength(m_nIdleAnimation);
                g_pRules->Get2DArrays()->m_pAppearance->GetFLOATEntry(
                    m_pAppearance->m_nAppearance,
                    CTwoDimArrays::COL2DA_APPEARANCETABLE_WALKDIST, &fDistance);
                fDistance *= 1000.0f;
                if (fBlend == 0.0f || fDistance == 0.0f)
                    fAnimationSpeed = 0.0f;
                else
                    fAnimationSpeed = fSpeed * 0.001f / (fDistance / fBlend);
            }
            float fAnimationTime = m_pAnimBase->GetAnimationTime(m_nIdleAnimation);
            if (fAnimationTime > 0.0f)
            {
                float fIdleLength = m_pAnimBase->GetAnimationLength(m_nIdleAnimation);
                CSWCAnimBase* pAnimationBase = m_pAnimBase;
                float fBlendTime = pAnimationBase->GetAnimationLength(GetCurrentAnimation()) *
                    (fAnimationTime / fIdleLength);
                m_pAnimBase->SetAnimation(GetCurrentAnimation(),
                    fAnimationSpeed * fSpeedFactor, 0, fBlendTime * 0.001f);
            }
            else
                m_pAnimBase->SetAnimation(GetCurrentAnimation(),
                    fAnimationSpeed * fSpeedFactor, 0, 0.0f);
        }
        if (m_pLoopingSound != NULL)
            m_pLoopingSound->SetPosition(vPosition, 1.5f);
        if (m_pStreamingSound != NULL)
            m_pStreamingSound->SetPosition(vPosition);
        if (AnimationStationary(nAnimation))
            RotateCreatureToOrientation(m_vDesiredOrientation);
        else
            RotateCreatureToOrientation(m_vMoveOrientation);
    }
}
// PC 0x006123E0  Xbox 0x00042750
void CSWCCreature::SaveCharGenCreature()
{
    NwccreatureTrace1();
    CExoLocString locString;
    CResGFF* pGFF = new CResGFF;
    CResStruct topStruct;
    pGFF->CreateGFFFile(&topStruct, "BIC ", "V2.0");

    locString.AddString(0, GetStats()->m_sFirstName, 0);
    pGFF->WriteFieldCExoLocString(&topStruct, locString, "FirstName");
    locString.AddString(0, GetStats()->m_sLastName, 0);
    pGFF->WriteFieldCExoLocString(&topStruct, locString, "LastName");
    locString.AddString(0, GetStats()->m_sDescription, 0);
    pGFF->WriteFieldCExoLocString(&topStruct, locString, "Description");
    pGFF->WriteFieldDWORD(&topStruct, GetGold(), "Gold");
    {
        CResRef resConversation = GetStats()->m_cConversation;
        pGFF->WriteFieldCResRef(&topStruct, resConversation, "Conversation");
    }
    pGFF->WriteFieldINT(&topStruct, GetStats()->GetAge(), "Age");
    {
        unsigned char nValue = GetStats()->m_nGender;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Gender");
    }
    {
        int nRace = GetStats()->m_nRace;
        pGFF->WriteFieldBYTE(&topStruct, nRace, "Race");
    }
    pGFF->WriteFieldCExoString(&topStruct, GetStats()->m_sSubRace, "Subrace");
    {
        unsigned char nValue = GetStats()->m_nSubraceIndex;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "SubraceIndex");
    }
    {
        unsigned char nValue = GetStats()->m_nStartingPackage;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "StartingPackage");
    }
    pGFF->WriteFieldCExoString(&topStruct, GetStats()->m_sDeity, "Deity");
    pGFF->WriteFieldSHORT(&topStruct, GetStats()->GetArmorClass(1), "ArmorClass");
    {
        char nValue = GetStats()->m_nReflexSave;
        pGFF->WriteFieldCHAR(&topStruct, nValue, "RefSaveThrow");
    }
    {
        char nValue = GetStats()->m_nWillSave;
        pGFF->WriteFieldCHAR(&topStruct, nValue, "WillSaveThrow");
    }
    {
        char nValue = GetStats()->m_nFortitudeSave;
        pGFF->WriteFieldCHAR(&topStruct, nValue, "FortSaveThrow");
    }
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetNumClasses() - 1, "MClassLevUpIn");
    pGFF->WriteFieldWORD(&topStruct, GetSoundSet(), "SoundSetFile");

    CResList classList;
    CResStruct classStruct;
    pGFF->AddList(&classList, &topStruct, "ClassList");
    pGFF->AddListElement(&classStruct, &classList, 0);
    pGFF->WriteFieldSHORT(&classStruct, GetStats()->GetClassLevel(0), "ClassLevel");
    pGFF->WriteFieldINT(&classStruct, GetStats()->GetClass(0), "Class");

    {
        CResList knownList;
        if (GetStats()->GetNumberKnownSpells(0, 0))
        {
            CExoString sKnownList;
            sKnownList.Format("%s%d", "KnownList", 0);
            pGFF->AddList(&knownList, &classStruct, sKnownList.CStr());
        }
        CResStruct spellStruct;
        unsigned char nSpell;
        for (nSpell = 0; nSpell < GetStats()->GetNumberKnownSpells(0, 0); ++nSpell)
        {
            pGFF->AddListElement(&spellStruct, &knownList, 0);
            pGFF->WriteFieldWORD(&spellStruct, (unsigned short)GetStats()->GetKnownSpell(0, 0, nSpell), "Spell");
        }
    }
    CResList skillList;
    CResStruct skillStruct;
    pGFF->WriteFieldWORD(&topStruct, GetStats()->GetSkillPoints(), "SkillPoints");
    pGFF->AddList(&skillList, &topStruct, "SkillList");
    unsigned short nSkill;
    for (nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
    {
        pGFF->AddListElement(&skillStruct, &skillList, 0);
        pGFF->WriteFieldBYTE(&skillStruct, GetStats()->GetSkillRank(nSkill), "Rank");
    }

    CResList featList;
    CResStruct featStruct;
    pGFF->AddList(&featList, &topStruct, "FeatList");
    unsigned short nFeat;
    for (nFeat = 0; nFeat < (unsigned short)GetStats()->GetNumFeats(); ++nFeat)
    {
        unsigned short nFeatId = GetStats()->GetFeat(nFeat);
        pGFF->AddListElement(&featStruct, &featList, 0);
        pGFF->WriteFieldWORD(&featStruct, nFeatId, "Feat");
    }

    CSWRace* pRace = &g_pRules->m_pRaces[GetStats()->m_nRace];
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetSTRStat() - pRace->GetStrengthAdjust(), "Str");
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetDEXStat() - pRace->GetDexterityAdjust(), "Dex");
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetCONStat() - pRace->GetConstitutionAdjust(), "Con");
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetINTStat() - pRace->GetIntelligenceAdjust(), "Int");
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetWISStat() - pRace->GetWisdomAdjust(), "Wis");
    pGFF->WriteFieldBYTE(&topStruct, GetStats()->GetCHAStat() - pRace->GetCharismaAdjust(), "Cha");

    short nCurrentHitPoints = GetStats()->GetBaseHitPoints() - (GetStats()->GetMaxHitPoints() - GetStats()->GetCurrentHitPoints());
    short nCurrentForce = GetStats()->GetBaseForcePoints() - (GetStats()->GetMaxForcePoints() - GetStats()->GetForcePoints());
    pGFF->WriteFieldSHORT(&topStruct, nCurrentHitPoints, "CurrentHitPoints");
    pGFF->WriteFieldSHORT(&topStruct, GetStats()->GetBaseHitPoints(), "HitPoints");
    pGFF->WriteFieldSHORT(&topStruct, GetStats()->GetMaxHitPoints(), "MaxHitPoints");
    pGFF->WriteFieldSHORT(&topStruct, GetStats()->GetCurrentHitPoints(), "PregameCurrent");
    pGFF->WriteFieldSHORT(&topStruct, nCurrentForce, "CurrentForce");
    pGFF->WriteFieldSHORT(&topStruct, GetStats()->GetBaseForcePoints(), "ForcePoints");
    pGFF->WriteFieldSHORT(&topStruct, GetStats()->GetMaxForcePoints(), "MaxForcePoints");

    CResList levelList;
    CResStruct levelStruct;
    pGFF->AddList(&levelList, &topStruct, "LvlStatList");
    pGFF->AddListElement(&levelStruct, &levelList, 0);
    {
        unsigned char nHitDie = g_pRules->m_pClasses[GetStats()->GetClass(0)].m_nHitDie;
        pGFF->WriteFieldBYTE(&levelStruct, nHitDie, "LvlStatHitDie");
    }
    pGFF->WriteFieldBYTE(&levelStruct, GetStats()->GetClass(0), "LvlStatClass");
    {
        CResList knownList;
        CResStruct spellStruct;
        unsigned char nSpell;
        if (GetStats()->GetNumberKnownSpells(0, 0))
        {
            CExoString sKnownList;
            sKnownList.Format("%s%d", "KnownList", 0);
            pGFF->AddList(&knownList, &levelStruct, sKnownList.CStr());
        }
        for (nSpell = 0; nSpell < GetStats()->GetNumberKnownSpells(0, 0); ++nSpell)
        {
            pGFF->AddListElement(&spellStruct, &knownList, 0);
            pGFF->WriteFieldWORD(&spellStruct, (unsigned short)GetStats()->GetKnownSpell(0, 0, nSpell), "Spell");
        }
    }
    pGFF->WriteFieldWORD(&levelStruct, GetStats()->GetSkillPoints(), "SkillPoints");
    pGFF->AddList(&skillList, &levelStruct, "SkillList");
    for (nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
    {
        pGFF->AddListElement(&skillStruct, &skillList, 0);
        pGFF->WriteFieldBYTE(&skillStruct, GetStats()->GetSkillRank(nSkill), "Rank");
    }
    pGFF->AddList(&featList, &levelStruct, "FeatList");
    for (nFeat = 0; nFeat < (unsigned short)GetStats()->GetNumFeats(); ++nFeat)
    {
        unsigned short nFeatId = GetStats()->GetFeat(nFeat);
        pGFF->AddListElement(&featStruct, &featList, 0);
        pGFF->WriteFieldWORD(&featStruct, nFeatId, "Feat");
    }

    pGFF->WriteFieldDWORD(&topStruct, GetStats()->GetExperience(), "Experience");
    {
        int nPortraitId = GetStats()->m_nPortraitId;
        pGFF->WriteFieldWORD(&topStruct, nPortraitId, "PortraitId");
    }
    pGFF->WriteFieldCResRef(&topStruct, GetStats()->GetPortrait(0), "Portrait");
    {
        int nGoodEvil = GetStats()->m_nGoodEvilAlignment;
        pGFF->WriteFieldBYTE(&topStruct, nGoodEvil, "GoodEvil");
    }
    {
        unsigned char nValue = GetStats()->m_nColorSkin;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Color_Skin");
    }
    {
        unsigned char nValue = GetStats()->m_nColorHair;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Color_Hair");
    }
    {
        unsigned char nValue = GetStats()->m_nColorTattoo1;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Color_Tattoo1");
    }
    {
        unsigned char nValue = GetStats()->m_nColorTattoo2;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Color_Tattoo2");
    }
    {
        unsigned char nPhenotype = GetStats()->m_nPhenotype;
        pGFF->WriteFieldINT(&topStruct, nPhenotype, "Phenotype");
    }
    {
        int nAppearance = GetStats()->m_nAppearance;
        pGFF->WriteFieldWORD(&topStruct, nAppearance, "Appearance_Type");
    }
    {
        unsigned char nValue = GetStats()->m_nAppearanceHead;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Appearance_Head");
    }
    {
        unsigned char nValue = GetStats()->m_nDuplicatingHead;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "DuplicatingHead");
    }
    {
        int bUseBackupHead = GetStats()->m_bUseBackupHead;
        pGFF->WriteFieldBYTE(&topStruct, bUseBackupHead, "UseBackupHead");
    }
    {
        unsigned char nValue = GetStats()->m_nTail;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Tail");
    }
    {
        unsigned char nValue = GetStats()->m_nWings;
        pGFF->WriteFieldBYTE(&topStruct, nValue, "Wings");
    }

    CResList* pItemList = new CResList;
    CResStruct itemStruct;
    pGFF->AddList(pItemList, &topStruct, "ItemList");
    for (unsigned char nItem = 0; nItem < (unsigned char)GetStats()->m_lstInventoryRes.num; ++nItem)
    {
        pGFF->AddListElement(&itemStruct, pItemList, 0);
        CResRef resItem = GetStats()->m_lstInventoryRes.element[nItem];
        pGFF->WriteFieldCResRef(&itemStruct, resItem, "InventoryRes");
    }
    delete pItemList;

    CExoString sScript;
    sScript = "k_hen_heartbt01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptHeartbeat");
    sScript = "k_hen_percept01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptOnNotice");
    sScript = "k_def_spellat01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptSpellAt");
    sScript = "k_hen_attacked01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptAttacked");
    sScript = "k_def_damage01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptDamaged");
    sScript = "";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptDisturbed");
    sScript = "k_hen_combend01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptEndRound");
    sScript = "k_hen_dialogue01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptDialogue");
    sScript = "k_hen_spawn01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptSpawn");
    sScript = "";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptRested");
    sScript = "";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptDeath");
    sScript = "k_def_userdef01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptUserDefine");
    sScript = "k_def_blocked01";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptOnBlocked");
    sScript = "";
    pGFF->WriteFieldCResRef(&topStruct, sScript, "ScriptEndDialogue");

    CExoString sFileName;
    CExoString sPath;
    sPath = "temp";
    sFileName = sPath;
    sPath = CExoString("TEMP:") + sPath;
    pGFF->WriteGFFFile(sPath.CStr(), RESTYPE_BIC);
    delete pGFF;
}
// PC 0x006134C0  Xbox 0x00043880
int CSWCCreature::SetAppearance(CCAppearanceInfo* pAppearance,
                              unsigned char nParts, int bUpdateVisualEffects)
{
    unsigned short nLoopingAnimation;
    unsigned short nCurrentAnimation;

    DetachVisualEffects();
    if ((nParts & APPEARANCE_PART_BODY) || (nParts & APPEARANCE_PART_HEAD))
    {
        nLoopingAnimation = GetLoopingAnimation();
        nCurrentAnimation = GetCurrentAnimation();
    }

    if (nParts & APPEARANCE_PART_BODY)
    {
        Vector vPosition = m_vPosition;
        Vector vDesiredPosition = m_vDesiredPosition;
        Vector vOrientation = m_vOrientation;
        Vector vMoveOrientation = m_vMoveOrientation;
        Vector vDesiredOrientation = m_vDesiredOrientation;

        if (!m_pAppearance->CreateBodyAppearance(pAppearance))
            return 0;

        SetPosition(vPosition);
        m_vDesiredPosition = vDesiredPosition;
        SetOrientation(vOrientation);
        m_vMoveOrientation = normalize(vMoveOrientation);
        m_vDesiredOrientation = normalize(vDesiredOrientation);
    }

    if (nParts & APPEARANCE_PART_HEAD)
        m_pAppearance->CreateHeadAppearance(pAppearance);
    if (nParts & APPEARANCE_PART_TAIL)
        m_pAppearance->CreateTailAppearance(pAppearance);
    if (nParts & APPEARANCE_PART_WING)
        m_pAppearance->CreateWingAppearance(pAppearance);

    if ((nParts & APPEARANCE_PART_BODY) || (nParts & APPEARANCE_PART_HEAD))
    {
        SetLoopingAnimation(nLoopingAnimation);
        AbortFireForgetAnimation(0);
        if (AnimationLooping(nCurrentAnimation) == 1 ||
            !AnimationStationary(nCurrentAnimation))
            SetAnimation(nCurrentAnimation, 0, 0);
        else
            SetAnimation(nLoopingAnimation, 0, 0);
    }

    UpdateShadowBlob();
    if (bUpdateVisualEffects)
        ReAttachVisualEffects();
    LoadRollingFootstepSound();
    m_pAppearance->SetAppearanceAlignment(m_pStats->m_nGoodEvilAlignment);
    m_pAppearance->ForceAnimate();
    return 1;
}
// PC 0x00613710  Xbox 0x00043AD0
void CSWCCreature::UpdateWeaponClass()
{
    NwccreatureTrace1();
    unsigned long oidRightHand = m_aoidEquippedItems[EQUIPPED_ITEM_RIGHTHAND];
    unsigned long oidLeftHand = m_aoidEquippedItems[EQUIPPED_ITEM_LEFTHAND];
    unsigned char nWieldClass = CSWCAnimBase::WIELD_CLASS_NONE;
    unsigned char nOffHandType = 0;
    if (oidRightHand == OBJECT_INVALID)
    {
        if (oidLeftHand == OBJECT_INVALID)
        {
            nWieldClass = CSWCAnimBase::WIELD_CLASS_UNARMED;
            GetAnimationBase()->m_nOffHandType = CSWCAnimBase::OFF_HAND_TYPE_UNARMED;
        }
        else
        {
            CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidLeftHand);
            if (pItem != NULL)
                nWieldClass = CSWCAnimBase::WIELD_CLASS_NONE;
        }
    }
    else
    {
        CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidRightHand);
        if (pItem != NULL)
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
            if (oidLeftHand == OBJECT_INVALID)
            {
                switch ((unsigned char)pBaseItem->m_nWeaponWield)
                {
                case CSWBaseItem::WEAPON_WIELD_ONE_HANDED_1:
                    nWieldClass = CSWCAnimBase::WIELD_CLASS_1;
                    break;
                case CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2:
                    nWieldClass = CSWCAnimBase::WIELD_CLASS_2;
                    break;
                case CSWBaseItem::WIELD_DOUBLE_WEAPON:
                    nWieldClass = CSWCAnimBase::WIELD_CLASS_3;
                    break;
                case CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4:
                    nWieldClass = CSWCAnimBase::WIELD_CLASS_4;
                    break;
                case CSWBaseItem::WEAPON_WIELD_5:
                    nWieldClass = CSWCAnimBase::WIELD_CLASS_5;
                    break;
                case CSWBaseItem::WEAPON_WIELD_6:
                    nWieldClass = CSWCAnimBase::WIELD_CLASS_6;
                    break;
                }
                nOffHandType = pBaseItem->m_nWeaponType;
                GetAnimationBase()->m_nOffHandType = nOffHandType;
            }
            else
            {
                pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidLeftHand);
                if (pItem != NULL)
                {
                    pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
                    switch ((unsigned char)pBaseItem->m_nWeaponWield)
                    {
                    case CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2:
                        nWieldClass = CSWCAnimBase::WIELD_CLASS_DUAL_2;
                        break;
                    case CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4:
                        nWieldClass = CSWCAnimBase::WIELD_CLASS_DUAL_4;
                        break;
                    }
                    nOffHandType = pBaseItem->m_nWeaponType;
                    GetAnimationBase()->m_nOffHandType = nOffHandType;
                }
            }
        }
    }
    GetAnimationBase()->m_nWieldClass = nWieldClass;
}
// PC 0x00613850  Xbox 0x00043C10
void CSWCCreature::UpdateVisibleWeapons(unsigned long nSlot, CSWCItem* pItem, int bInstant)
{
    if (nSlot == INVENTORY_SLOT_RIGHTHAND || nSlot == INVENTORY_SLOT_LEFTHAND)
    {
        int nModelSlot;
        if (nSlot == INVENTORY_SLOT_RIGHTHAND)
            nModelSlot = CSWCAnimBase::MODEL_SLOT_RIGHTHAND;
        else
            nModelSlot = CSWCAnimBase::MODEL_SLOT_LEFTHAND;
        if (bInstant == 0)
        {
            CSWCAnimBase* pAnimationBase = GetAnimationBase();
            if (pAnimationBase->AnimationExists(CREATURE_ANIMATION_DRAW_WEAPON) == 1)
            {
                int bDrawBlocked = m_bWeaponDrawBlocked;
                CSWCItem* pVisibleItem = NULL;
                Gob* pModel = GetModel(nModelSlot, 1);
                if (pModel != NULL && pModel->GetExternalDataType() == Gob::EXTERNAL_DATA_TYPE_CLIENT_ITEM)
                    pVisibleItem = (CSWCItem*)pModel->GetExternalData();

                if (pItem != pVisibleItem && bDrawBlocked == 0)
                {
                    ComputeWeaponOverlays(0, 0);
                    unsigned char nFadeState = m_nFadeState;
                    m_nAmbientPauseTimer = 0;
                    m_bAmbientPause = 0;
                    if (nFadeState != 0)
                        PlayDrawAnimation(CREATURE_ANIMATION_DRAW_WEAPON);
                    if (pVisibleItem != NULL)
                        pVisibleItem->PowerItem(0, 0, 1, 0);
                }
            }
        }
    }
    SwitchWeaponEvent(NULL, NULL, this);
}
// PC 0x00613930  Xbox 0x00043CF0
void CSWCCreature::SetStealthState(int nState, int bSuppressAnimation)
{
    if (nState == 1)
    {
        if (m_nStealthState == 0)
        {
            PlaySoundSetSound(SOUNDSET_ENTER_STEALTH, 1);
#ifdef _XBOX
            AddVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_FRAMEBUFFER,
                0, OBJECT_INVALID, OBJECT_INVALID, 0, 0,
                Vector(0.0f, 0.0f, 0.0f));
#else
            if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nFrameBuffer &&
                AurCheckFrameBufferModificationsAvailable())
            {
                AddVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_FRAMEBUFFER,
                    0, OBJECT_INVALID, OBJECT_INVALID, 0, 0,
                    Vector(0.0f, 0.0f, 0.0f));
            }
            else
            {
                AddVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_FALLBACK,
                    0, OBJECT_INVALID, OBJECT_INVALID, 0, 0,
                    Vector(0.0f, 0.0f, 0.0f));
            }
#endif
            if (!bSuppressAnimation)
                AddFireForgetAnimation(CREATURE_ANIMATION_STEALTH_TRANSITION, NULL);
        }
    }
    else if (nState == 0 && m_nStealthState == 1)
    {
#ifdef _XBOX
        DeleteVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_FRAMEBUFFER);
#else
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nFrameBuffer &&
            AurCheckFrameBufferModificationsAvailable())
        {
            DeleteVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_FRAMEBUFFER);
        }
        else
        {
            DeleteVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_FALLBACK);
        }
#endif
        AddVisualEffect(CSWCVisualEffectOnObject::VFX_STEALTH_END,
            1, OBJECT_INVALID, OBJECT_INVALID, 0, 0,
            Vector(0.0f, 0.0f, 0.0f));
        if (!bSuppressAnimation)
            AddFireForgetAnimation(CREATURE_ANIMATION_STEALTH_TRANSITION, NULL);
    }
    m_nStealthState = nState;
}
// PC 0x00613A70  Xbox 0x00043DD0
void CSWCCreature::UpdateRelativePerception(unsigned long oidObject)
{
    int i;
    CSWVisibilityNode* pNode;
    CSWCCreature* pCreature;
    CSWCArea* pArea;
    int bShowInvisibility = 0;
    int bHideInvisibility = 0;

    for (i = 0; i < m_lstVisibilityList.num; i++)
    {
        pNode = m_lstVisibilityList[i];
        if (pNode->m_oidObject == oidObject)
        {
            pCreature = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidObject);
            if (pCreature == NULL)
                return;
            if ((pNode->m_bSeen == 0 && pNode->m_bHeard == 1) ||
                pNode->m_bInvisible == 1)
            {
                bShowInvisibility = 1;
            }
            else if (pNode->m_bSeen == 1 && pNode->m_bInvisible == 0)
            {
                bHideInvisibility = 1;
            }
            pArea = m_pArea;
            if (pArea->GetObjectsAtPositionVisible(pCreature->m_vPosition) != 1)
                return;
            if (bShowInvisibility == 1 && pCreature->m_bInvisibilityVisualApplied == 0)
            {
                pCreature->AddVisualEffect(VFX_DUR_INVISIBILITY, 0, OBJECT_INVALID,
                    OBJECT_INVALID, 0, 0, Vector(0.0f, 0.0f, 0.0f));
                pCreature->m_bInvisibilityVisualApplied = 1;
            }
            else if (bHideInvisibility == 1 && pCreature->m_bInvisibilityVisualApplied == 1)
            {
                pCreature->DeleteVisualEffect(VFX_DUR_INVISIBILITY);
                pCreature->m_bInvisibilityVisualApplied = 0;
            }
            return;
        }
    }
}
// PC 0x00616A20  Xbox 0x00046F70
CSWCCreature::CSWCCreature()
    : CSWCObject(CGameObject::OBJECT_TYPE_CREATURE)
{
    m_bCombatMode = 0;
    m_bLevelUpSent = 1;
    m_bInParty = 0;
    m_pSoundSet = NULL;
    m_nSoundSet = SOUNDSET_INVALID;
    m_pStats = new CSWCLevelUpStats;
    m_pWayPoints = NULL;
    m_pCurrentWayPoint = NULL;
    m_nWayPointCount = 0;
    m_nCurrentAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
    m_bConjuringCreature = 0;
    m_bWeaponDrawBlocked = 0;
    m_nUnknown150 = 0;
    m_nUnknown1B8 = 0;
    m_bOverrideLightsaberState = 0;
    m_oidVisibleRightWeapon = OBJECT_INVALID;
    m_oidVisibleLeftWeapon = OBJECT_INVALID;
    m_nDesiredPositionTimer = 0;
    m_apAttackData[0] = new CSWCCombatAttackData;
    m_apAttackData[1] = new CSWCCombatAttackData;
    for (int i = 0; i < EQUIPPED_ITEM_COUNT; ++i)
    {
        m_aoidEquippedItems[i] = CGameObjectArray::INVALIDOBJID;
        m_anLastInstant[i] = 0;
    }

    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_pAppearance = new CSWCCreatureAppearance(this);
    if (m_pAppearance == NULL)
        return;

    m_nUnknown1EE = 0;
    m_nUnknown1EF = 0;
    m_nUnknown1F0 = 0;
    m_nSpeedRampMode = SPEED_RAMP_FAST;
    m_nUnknown214 = 0;
    m_vOrientation = Vector(1.0f, 0.0f, 0.0f);
    m_vMoveOrientation = Vector(1.0f, 0.0f, 0.0f);
    m_vDesiredOrientation = Vector(1.0f, 0.0f, 0.0f);
    m_nLoopingAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
    m_bBlurWeapon = 0;
    m_pLoopingSound = NULL;
    m_bLoopingSoundPlaying = 0;
    m_nLoopingSoundId = 0;
    m_bDeadSelectable = 0;
    m_bLoopingSoundEnabled = 1;
    m_pSpellCastVisual = new CSWCSpellCastVisual;
    m_oidLockTarget = CGameObjectArray::INVALIDOBJID;
    m_nWeaponOverlayState = 0xffff;
    m_nCombatState = 0;
    m_pOwnedObject15C = NULL;
    m_nUnknown190 = 0;
    m_nStealthState = 0;
    m_nUnknown198 = 0;
    m_nUnknown19C = 0;
    m_nUnknown1A0 = 0;
    m_nUnknown1A4 = 0;
    m_nUnknown1A8 = 0;
    m_nUnknown1AC = 0;
    m_bInvisibilityVisualApplied = 0;
    for (int j = 0; j < 12; ++j)
        m_apOwnedObjects[j] = NULL;
    m_pBloodEffect = NULL;
    m_pOwnedObject130 = NULL;
    m_pOwnedObject134 = NULL;
#ifdef _XBOX
    m_fUnknown148 = 0.0f;
#else
    m_nUnknown148 = 0;
#endif
    m_bWetFootstep = 0;
    m_fFootstepGroundHeight = 0.0f;
    for (int k = 0; k < 4; ++k)
        m_apAttachedModels[k] = NULL;
    m_pMonkVisual = NULL;
    m_nMonkVisualState = 0;
    m_nMonkVisualTimer = 0;
    m_nHostile = HOSTILE_STATE_HOSTILE;
    m_nUnknown1B4 = 0;
    m_nUnknown308 = 200000;
    m_pPartyStatus = NULL;
    m_nGold = 0;
    m_oidUnknown2C8 = CGameObjectArray::INVALIDOBJID;
    m_nWeaponOverlayTimer = SWCCREATURE_WEAPON_OVERLAY_INTERVAL;
    m_nWeaponCheckTimer = 6000;
    m_pPendingAnimBase = NULL;
    m_fPendingSpeedFactor = 0.0f;
    m_nCameraFollowDelay = 0;
    m_bCameraFollowPending = 0;
    m_bCameraFollowSnap = 0;
    m_nUnknown370 = 0;
    m_nUnknown374 = 0;
    m_nUnknown378 = 0;
    for (int nObject = 0; nObject < 4; ++nObject)
        m_aoidUnknown37C[nObject] = CGameObjectArray::INVALIDOBJID;
    m_nAmbientPauseTimer = 0;
    m_bAmbientPause = 0;
    m_nAmbientPauseDelay = 30000 * (rand() % 4 + 2);
    m_fUnknown1FC = 1.0f;
    m_oidArea = CGameObjectArray::INVALIDOBJID;
    m_pSpellLikeAbilities = NULL;
    m_nMode = 0;
    m_nUnknown3AC = 0;
    m_fSWAnimationSpeed = 0.0f;
    m_fSWTargetAnimationSpeed = 0.0f;
    m_fDriveTurnAngle = 0.0f;
    m_nFollowAnimation = CREATURE_ANIMATION_NO_IDLE_FALLBACK;
    m_nAttackAnimationOverride = 0;
    m_nEngagedState = 0;
    m_bCombatReady = 0;
    m_oidThrownLightsaber = OBJECT_INVALID;
    m_bEquippedItemsHidden = 0;
    m_nCurrentAttackIndex = -1;
    m_nPreviousAttackIndex = -1;
    m_bEquippedHeadItemHidden = 0;
    m_oidParentCorpse = OBJECT_INVALID;
    m_bDriveRunning = 0;
    m_oidLookAtTarget = OBJECT_INVALID;
    for (int nCallback = 0; nCallback < 17; ++nCallback)
        m_apAnimationCallbacks[nCallback] = NULL;
    m_pRollingFootstepSound = NULL;
    m_fRollingFootstepPitchOffset = 0.0f;
    m_bRollingFootstepPlaying = 0;
    m_bRollingFootstepRunning = 0;
    m_nDrawAnimation = CREATURE_ANIMATION_CURRENT;
#ifndef _XBOX
    m_fFreeLookTurn = 0.0f;
#endif
}
// PC 0x006170D0  Xbox 0x000476E0
CSWCCreature::~CSWCCreature()
{
    delete m_pRollingFootstepSound;
    delete m_pSpellLikeAbilities;
    if (m_pSoundSet != NULL)
    {
        delete m_pSoundSet;
        m_pSoundSet = NULL;
    }
    if (m_pStats != NULL)
    {
        delete m_pStats;
        m_pStats = NULL;
    }
    if (m_pOwnedObject15C != NULL)
    {
        delete m_pOwnedObject15C;
        m_pOwnedObject15C = NULL;
    }
    if (m_pLoopingSound != NULL)
    {
        delete m_pLoopingSound;
        m_pLoopingSound = NULL;
    }
    delete m_pSpellCastVisual;
    delete m_pAppearance;
    if (m_pWayPoints != NULL)
        delete[] m_pWayPoints;
    UnloadModel();
    if (m_apAttackData[0] != NULL)
        delete m_apAttackData[0];
    if (m_apAttackData[1] != NULL)
        delete m_apAttackData[1];
    for (int i = 0; i < 12; ++i)
    {
        delete m_apOwnedObjects[i];
        m_apOwnedObjects[i] = NULL;
    }
    delete m_pBloodEffect;
    for (int j = 0; j < 4; ++j)
    {
        if (m_apAttachedModels[j] != NULL)
        {
            m_apAttachedModels[j]->AttachToScene(NULL);
            delete m_apAttachedModels[j];
            m_apAttachedModels[j] = NULL;
        }
    }
    if (m_pMonkVisual != NULL)
    {
        m_pMonkVisual->AttachToScene(NULL);
        delete m_pMonkVisual;
    }
    delete m_pOwnedObject130;
    delete m_pOwnedObject134;
    if (m_pPartyStatus != NULL)
    {
        delete m_pPartyStatus;
        m_pPartyStatus = NULL;
    }
    m_oidThrownLightsaber = CGameObjectArray::INVALIDOBJID;
    ClearVisibilityList();
    g_pAppManager->GetClientExoApp()->ClearCachedCreatures(this);
}
// PC 0x0060B7C0  Xbox 0x0003B610
void CSWCCreature::SetSoundSet(unsigned short nSoundSet)
{
    if (nSoundSet != m_nSoundSet)
    {
        m_nSoundSet = nSoundSet;
        if (m_pSoundSet != NULL)
        {
            delete m_pSoundSet;
            m_pSoundSet = NULL;
        }
        if (m_nSoundSet != SOUNDSET_INVALID)
        {
            CSoundSetList* pSoundSetList =
                g_pAppManager->m_pClientExoApp->GetSoundSetList();
            if (pSoundSetList != NULL)
            {
                CResRef cSoundSet = pSoundSetList->GetSoundSet(m_nSoundSet);
                if (!(cSoundSet == ""))
                {
                    m_pSoundSet = new CSoundSet(cSoundSet);
                }
            }
        }
    }
}
// PC 0x0060B8A0  Xbox 0x0003B700
int CSWCCreature::GetSoundSetStrres(unsigned short nSound, STR_RES& strRes)
{
    if (m_nSoundSet == 0xFFFF)
    {
        return 0;
    }
    if (m_pSoundSet == NULL)
    {
        return 0;
    }
    if (m_pSoundSet->m_pRes->Demand() == NULL)
    {
        return 0;
    }
    m_pSoundSet->GetStrres(nSound, strRes);
    m_pSoundSet->m_pRes->Release();
    return 1;
}
// PC 0x0060B900  Xbox 0x0003B760
void CSWCCreature::ResetUpdateTimes(unsigned long nDay, unsigned long nTime)
{
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
}
// PC 0x0060B920  Xbox 0x0003B780
void CSWCCreature::ResetCameraToFollow()
{
    if (g_pAppManager->m_pClientExoApp->GetSWParty()->GetIndex(m_nId) == 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode ==
            CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            g_pAppManager->m_pClientExoApp->GetModule()->RestoreCamera();
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
        }
        CSWCameraOnAStick* pCamera = reinterpret_cast<CSWCameraOnAStick*>(
            g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->GetBehavior(
                AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pCamera != NULL)
            pCamera->ClearLookAtTarget();
    }
}
// PC 0x0060B9B0  Xbox 0x0003B810
void CSWCCreature::SetPosition(const Vector& vPosition)
{
    m_vPosition = vPosition;
    if (m_pArea != NULL)
    {
        m_pArea->GetObjectsAtPositionVisible(vPosition);
    }

    if (RenderPosition != 0)
    {
        AsSWCCreature();
    }

    if (m_pAnimBase != NULL && m_bOutOfPlace == 0)
    {
        m_pAnimBase->SetPosition(vPosition);
    }
    UpdateWalkMesh();
}
// PC 0x0060BA10  Xbox 0x0003B870
void CSWCCreature::SetDesiredOrientationForLock()
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    if (pObjectArray == NULL)
        return;

    unsigned long oidTarget = m_oidLockTarget;
    CGameObject* pObject;
    if (pObjectArray->GetGameObject(oidTarget, &pObject) ==
        CGameObjectArray::SUCCESS)
    {
        CSWCObject* pTarget = (CSWCObject*)pObject;
        if (pTarget != NULL)
        {
            Vector vDirection = pTarget->m_vPosition - m_vPosition;
            if (vDirection * vDirection > SWCCREATURE_LOCK_DIRECTION_EPSILON_SQUARED)
            {
                vDirection.m_fZ = 0.0f;
                SetDesiredOrientation(normalize(vDirection));
            }
        }
    }
    else
    {
        SetOrientation(m_vOrientation);
    }
}

void CSWCCreature::LoadMonkVisuals()
{
    unsigned short nAppearance = GetAppearance()->m_nAppearance;
    if (nAppearance != MONK_VISUAL_APPEARANCE_1 && nAppearance != MONK_VISUAL_APPEARANCE_2)
        return;

    unsigned char nAlignment = m_pStats->GetSimpleAlignmentGoodEvil();
    if ((nAlignment == ALIGNMENT_SIMPLE_EVIL && !m_nMonkVisualState) ||
        (nAlignment != ALIGNMENT_SIMPLE_EVIL && !m_nMonkVisualTimer))
    {
        if (m_pMonkVisual != NULL)
        {
            delete m_pMonkVisual;
            m_pMonkVisual = NULL;
        }

        int bEvil = m_pStats->GetSimpleAlignmentGoodEvil() == ALIGNMENT_SIMPLE_EVIL;
        m_pMonkVisual = (Gob*)NewCAurObject(bEvil ? "vfx_monkeyes" : "vfx_monkeyes01",
                                      "monkeyes", NULL, NULL);
        m_nMonkVisualState = bEvil;
        m_nMonkVisualTimer = !bEvil;
        if (m_pMonkVisual != NULL)
        {
            m_pMonkVisual->AttachToObject(
                GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN), "HEADHOOK", 0);
            m_pMonkVisual->PlayAnimation("impact", 1.0f, 0, 0.0f);
        }
    }
}
// PC 0x0060BAD0  Xbox 0x0003B950
void CSWCCreature::UnloadMonkVisuals()
{
    if (m_pMonkVisual != NULL)
    {
        m_pMonkVisual->AttachToScene(0);
        delete m_pMonkVisual;
        m_pMonkVisual = NULL;
        m_nMonkVisualState = 0;
        m_nMonkVisualTimer = 0;
    }
}

void CSWCCreature::LoadSpecialAttacks(unsigned char nType)
{
    switch (nType)
    {
    case SPECIAL_ATTACK_VISUAL_KI_1:
    case SPECIAL_ATTACK_VISUAL_KI_2:
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_1] == NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_1] =
                (Gob*)NewCAurObject("vcm_monkki", "kistrike", NULL, NULL);
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_2] == NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_2] =
                (Gob*)NewCAurObject("vcm_monkki", "kistrike", NULL, NULL);
        break;
    case SPECIAL_ATTACK_VISUAL_STUN:
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_STUN] == NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_STUN] =
                (Gob*)NewCAurObject("vcm_monkstun", "stunfist", NULL, NULL);
        break;
    case SPECIAL_ATTACK_VISUAL_QUIVER:
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_QUIVER] == NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_QUIVER] =
                (Gob*)NewCAurObject("vcm_monkquiv", "quivpalm", NULL, NULL);
        break;
    }

    char* sHook;
    if (GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_HAND_2 ||
        GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_HAND_1 ||
        GetAnimationBase()->GetOverlayAnimationPlaying(SPECIAL_ATTACK_ANIMATION_HAND_OVERLAY) == 1)
        sHook = "rhand";
    else if (GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_RIGHT_FOOT_1 ||
             GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_RIGHT_FOOT_2)
        sHook = "rfoot_g";
    else if (GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_LEFT_FOOT_2 ||
             GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_LEFT_FOOT_3 ||
             GetCurrentAnimation() == SPECIAL_ATTACK_ANIMATION_LEFT_FOOT_1)
        sHook = "lfoot_g";
    else
        return;

    m_apAttachedModels[nType]->AttachToObject(
        GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1), sHook, 0);
}

void CSWCCreature::StopSpecialAttacks(unsigned char nType)
{
    switch (nType)
    {
    case SPECIAL_ATTACK_VISUAL_KI_1:
    case SPECIAL_ATTACK_VISUAL_KI_2:
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_1] != NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_1]->PlayAnimation("cessation", 1.0f, 1, 0.0f);
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_2] != NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_KI_2]->PlayAnimation("cessation", 1.0f, 1, 0.0f);
        break;
    case SPECIAL_ATTACK_VISUAL_STUN:
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_STUN] != NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_STUN]->PlayAnimation("cessation", 1.0f, 1, 0.0f);
        break;
    case SPECIAL_ATTACK_VISUAL_QUIVER:
        if (m_apAttachedModels[SPECIAL_ATTACK_VISUAL_QUIVER] != NULL)
            m_apAttachedModels[SPECIAL_ATTACK_VISUAL_QUIVER]->PlayAnimation("cessation", 1.0f, 1, 0.0f);
        break;
    }
}
// PC 0x0060BB20  Xbox 0x0003B9A0
void CSWCCreature::HandleConjuringCreature()
{
    int bConjuring = 0;
    CSWSpell* pSpell;

    if (m_nCurrentAnimation == CREATURE_ANIMATION_CONJURE_1 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_CONJURE_2 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_CONJURE_3 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_6 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_7 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_8 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_9 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_10 ||
        m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_11)
    {
        int nSpell = m_nConjuringSpell;
        bConjuring = 1;
        pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    }

    if ((m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_6 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_7 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_8 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_9 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_10 ||
         m_nCurrentAnimation == CREATURE_ANIMATION_SPELLCAST_11) &&
        m_bConjuringCreature == 0)
    {
        CSWCSpellCastVisual* pVisual = m_pSpellCastVisual;
        pVisual->m_nConjureTime = SPELL_VISUAL_CONJURE_DURATION;
        pVisual->m_nCastTime = SPELL_VISUAL_CONJURE_DURATION;
        m_pSpellCastVisual->m_bTimerRunning = 0;
        m_bConjuringCreature = 1;
    }

    if (bConjuring == 1)
    {
        if (m_bConjuringCreature == 0 && pSpell != NULL)
        {
            CResRef cSound;
            cSound = "";
            if (g_pRules->m_pRaces[m_pStats->m_nRace].m_bPlayerRace != 0)
            {
                if (m_pAppearance->m_nGender == CCAppearanceInfo::GENDER_FEMALE)
                    cSound = pSpell->m_resConjureSoundFemale;
                else
                    cSound = pSpell->m_resConjureSoundMale;
            }
            CResRef cVocalization;
            cVocalization = pSpell->m_resConjureSoundVFX;
            m_pSpellCastVisual->UnloadConjureVisual();
            m_pSpellCastVisual->UnloadCastVisual();
            m_pSpellCastVisual->LoadConjureVisual(
                pSpell->m_resConjureHeadVisual, pSpell->m_resConjureHandVisual,
                pSpell->m_resConjureGroundVisual, cSound, cVocalization, this);
            CSWCSpellCastVisual* pVisual = m_pSpellCastVisual;
            pVisual->m_nConjureTime = SPELL_VISUAL_CONJURE_DURATION;
            pVisual->m_nCastTime = SPELL_VISUAL_CONJURE_DURATION;
            m_pSpellCastVisual->m_bTimerRunning = 0;
        }
    }
    else if (bConjuring == 0 && m_bConjuringCreature == 1)
    {
        if (m_pSpellCastVisual->m_bTimerRunning == 0)
        {
            m_pSpellCastVisual->UnloadConjureVisual();
            m_pSpellCastVisual->UnloadCastVisual();
        }
        m_nConjuringSpell = CSWSpell::SPELL_INVALID;
    }

    m_pSpellCastVisual->UpdateTimer(m_nTimeDelta);
    m_bConjuringCreature = bConjuring;
}
// PC 0x0060BDF0  Xbox 0x0003BC80
void CSWCCreature::MoveCreatureToPosition(const Vector& vPosition)
{
    if (m_nTimeDelta >= m_nDesiredPositionTimer)
    {
        m_nDesiredPositionTimer = 0;
        SetPosition(vPosition);
        return;
    }
    m_nDesiredPositionTimer -= m_nTimeDelta;
    Vector vDifference = m_vDesiredPosition - m_vPosition;
    if (vDifference * vDifference < SWCCREATURE_POSITION_EPSILON_SQUARED)
    {
        m_nDesiredPositionTimer = 0;
        SetPosition(vPosition);
        return;
    }
    if (vDifference.m_fX * vDifference.m_fX +
        vDifference.m_fY * vDifference.m_fY >=
            SWCCREATURE_POSITION_SNAP_DISTANCE_SQUARED)
    {
        m_nDesiredPositionTimer = 0;
        SetPosition(vPosition);
        return;
    }
    float fFraction = (float)m_nDesiredPositionTimer /
        (float)(m_nDesiredPositionTimer + m_nTimeDelta);
    SetPosition(vPosition + (m_vPosition - vPosition) * fFraction);
}
// PC 0x0060BF10  Xbox 0x0003BF30
void CSWCCreature::ResetDesiredPositionTimer()
{
    this->m_nDesiredPositionTimer = 512;
}
// PC 0x0060BF20  Xbox 0x0003BF40
void CSWCCreature::ComputeZValue(CSWCArea* pArea, Vector* pPosition,
                                 float fUnused, Vector* pUnused)
{
    if (g_pAppManager->GetServerExoApp() != NULL)
    {
        pPosition->m_fZ = 0.0f;
        pPosition->m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
            pArea->m_cAreaResRef, *pPosition, 1);
    }
}
// PC 0x0060BF60  Xbox 0x0003BF80
float CSWCCreature::ComputeSpeedFactor(float fSpeed)
{
    Vector* pWayPoint = m_pCurrentWayPoint;
    float fRampTime = SWCCREATURE_SPEED_RAMP_TIME;
    if (pWayPoint == NULL)
        return 0.0f;

    if (m_nSpeedRampMode == SPEED_RAMP_FAST)
        fRampTime = SWCCREATURE_FAST_SPEED_RAMP_TIME;

    float fDistance = 0.0f;
    Vector vPrevious = m_vDesiredPosition;
    do
    {
        Vector vWayPoint;
        vWayPoint = *pWayPoint;
        Vector vDelta(vWayPoint.m_fX - vPrevious.m_fX,
                      vWayPoint.m_fY - vPrevious.m_fY, 0.0f);
        fDistance = magnitude(vDelta) + fDistance;
        ++pWayPoint;
        vPrevious = vWayPoint;
    }
    while (pWayPoint < m_pWayPoints + m_nWayPointCount);

    fSpeed *= 1000.0f;
    fSpeed /= SWCCREATURE_SLOWDOWN_SPEED_DIVISOR;
    float fFactor = 1.0f;
    if (fDistance <= fSpeed)
    {
        fFactor = fDistance / fSpeed;
        if (fFactor >= SWCCREATURE_MIN_SLOWDOWN_RATIO)
            fFactor = sqrt(fFactor);
        else
            fFactor = SWCCREATURE_MIN_SPEED_FACTOR;
        fFactor -= m_nTimeDelta / 1000.0f;
    }
    if (m_fSpeedFactor <= 1.0f)
    {
        fSpeed = m_nTimeDelta / fRampTime + m_fSpeedFactor;
        if (fFactor >= fSpeed)
            fFactor = fSpeed;
    }
    if (fFactor <= SWCCREATURE_MIN_SPEED_FACTOR)
        return SWCCREATURE_MIN_SPEED_FACTOR;
    return fFactor;
}
// PC 0x0060C100  Xbox 0x0003C150
void CSWCCreature::ClientServerSyncHandler_Animation(int nAnimation,
                                                     float fSpeed)
{
    m_fAnimationSpeed = fSpeed;
    if (nAnimation != 10009 && nAnimation != 10109 && nAnimation != 10012 &&
        nAnimation != 10011 && nAnimation != 10014 && nAnimation != 10006 &&
        nAnimation != 10008 && nAnimation != 10156)
    {
        if (nAnimation != GetCurrentAnimation())
        {
            if (AnimationFireAndForget(nAnimation) != 0)
            {
                AddFireForgetAnimation(nAnimation, NULL);
            }
            else
            {
                SetAnimation(nAnimation, 0, 0);
            }
        }
    }
}
// PC 0x0060C190  Xbox 0x0003C1E0
Vector* CSWCCreature::GetCurrentWayPoint()
{
    return this->m_pCurrentWayPoint;
}
// PC 0x0060C1A0  Xbox 0x0003C1F0
Vector* CSWCCreature::GetEndWayPoint()
{
    unsigned short nWayPointCount = m_nWayPointCount;

    if (nWayPointCount == 0)
    {
        return NULL;
    }
    else
    {
        return m_pWayPoints + nWayPointCount - 1;
    }
}
// PC 0x0060C1C0  Xbox 0x0003C210
Vector* CSWCCreature::GetStartWayPoint()
{
    if ((this->m_nWayPointCount == 0))
    {
        return 0;
    }
    else
    {
        return this->m_pWayPoints;
    }
}
// PC 0x0060C1E0  Xbox 0x0003C230
int CSWCCreature::RequestModel(const CResRef& cModel, const CResRef& cTexture,
                              unsigned char nAnimType)
{
    if (m_pAnimBase == NULL || m_pAnimBase->m_nAnimationType != nAnimType)
    {
        switch (nAnimType)
        {
        case CSWCAnimBase::ANIMATION_TYPE_BASE:
            m_pPendingAnimBase = new CSWCAnimBase;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_TAIL_WINGS:
            m_pPendingAnimBase = new CSWCAnimBaseTW;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_HEAD:
            m_pPendingAnimBase = new CSWCAnimBaseHead;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_WIELD:
            m_pPendingAnimBase = new CSWCAnimBaseWield;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_BTYPE:
            m_pPendingAnimBase = new CSWCAnimBaseHeadWield;
            break;
        default:
            return 0;
        }
        if (m_pPendingAnimBase == NULL)
            return 0;

        m_pPendingAnimBase->Set(CSWAnimBase::VALUE_FIRST, 1850.0f);
        m_pPendingAnimBase->Set(CSWAnimBase::VALUE_SECOND, 6600.0f);
        m_pPendingAnimBase->Set(CSWAnimBase::VALUE_THIRD, 0.925f);
        m_pPendingAnimBase->Set(CSWAnimBase::VALUE_FOURTH, 3.3f);
        m_fPendingSpeedFactor = 0.0f;
        if (m_pAnimBase == NULL)
        {
            m_pAnimBase = m_pPendingAnimBase;
            m_fSpeedFactor = 0.0f;
            m_pPendingAnimBase = NULL;
        }
    }

    CExoString sName;
    cModel.CopyToString(sName);
    if (!CAuroraInterface::IsResident(sName.CStr()))
    {
        CAuroraInterface::RequestModel(sName.CStr());
        cTexture.CopyToString(sName);

        sName.GetLength();
        return 1;
    }
    return 1;
}
// PC 0x0060C4A0  Xbox 0x0003C4D0
int CSWCCreature::MoveToNextWayPoint()
{
    unsigned short nWayPointCount = m_nWayPointCount;
    Vector* pCurrentWayPoint;

    if (nWayPointCount == 0)
    {
        return 0;
    }

    pCurrentWayPoint = m_pCurrentWayPoint;
    if (pCurrentWayPoint == m_pWayPoints + nWayPointCount - 1)
    {
        return 0;
    }

    m_pCurrentWayPoint = pCurrentWayPoint + 1;
    return 1;
}
// PC 0x0060C4E0  Xbox 0x0003C510
int CSWCCreature::GetWayPoints(unsigned short* pnWayPointCount, Vector** ppWayPoints,
                               unsigned short* pnCurrentWayPoint)
{
    int nWayPoint;

    *pnWayPointCount = m_nWayPointCount;
    *ppWayPoints = m_pWayPoints;

    for (nWayPoint = 0; nWayPoint < m_nWayPointCount; ++nWayPoint)
    {
        if (m_pWayPoints + nWayPoint == m_pCurrentWayPoint)
        {
            *pnCurrentWayPoint = nWayPoint;
            return nWayPoint;
        }
    }

    *pnCurrentWayPoint = 0;
    return nWayPoint;
}
// PC 0x0060C540  Xbox 0x0003C570
CExoString CSWCCreature::GetSaveCreatureFilename(const CExoString& sFirstName,
                                               const CExoString& sLastName)
{
    CExoString sBaseName;
    CExoString sFilename;
    int nSuffix = 1;
    CExoString sSuffix;

    sBaseName = m_pStats->m_sFirstName + m_pStats->m_sLastName;
    sBaseName = sBaseName.SubString(0, 16);
    sBaseName.StripNonAlphaNumeric(1, 0, 0);
    if (sBaseName[0] == 0)
        sBaseName = "0";
    sFilename = sBaseName;

    while (1)
    {
        CResRef cFilename((char*)sFilename.CStr());
        if (!g_pExoResMan->Exists(cFilename, RESTYPE_BIC, (unsigned long*)NULL))
            break;
        sSuffix = CExoString(nSuffix);
        sBaseName = sBaseName.SubString(0, 16 - sSuffix.GetLength());
        sFilename = sBaseName + sSuffix;
        ++nSuffix;
    }
    return sFilename;
}
// PC 0x0060C790  Xbox 0x0003C7C0
void CSWCCreature::SetWayPoints(unsigned short nWayPointCount, Vector* pWayPoints)
{
    if (m_nWayPointCount == nWayPointCount)
    {
        memcpy(m_pWayPoints, pWayPoints, nWayPointCount * sizeof(Vector));
        m_pCurrentWayPoint = m_pWayPoints;
        return;
    }

    m_nWayPointCount = nWayPointCount;
    if (m_pWayPoints)
    {
        delete [] m_pWayPoints;
    }

    if (nWayPointCount == 0)
    {
        m_pWayPoints = NULL;
        m_pCurrentWayPoint = NULL;
        return;
    }

    m_pWayPoints = new Vector[nWayPointCount];
    m_pCurrentWayPoint = m_pWayPoints;
    if (m_pWayPoints)
    {
        memcpy(m_pWayPoints, pWayPoints, nWayPointCount * sizeof(Vector));
    }
}
// PC 0x0060C8C0  Xbox 0x0003C8F0
void CSWCCreature::SetCreatureType(unsigned char nType)
{
    m_nCreatureType = nType;
    m_pAnimBase->m_nAnimSet = nType;
}
// PC 0x0060C8E0  Xbox 0x0003C910
void CSWCCreature::UnloadModel()
{
    HidePopUpBubble();

    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x0060C910  Xbox 0x0003C940
int CSWCCreature::RequestAppearance(CCAppearanceInfo* pAppearance,
                                    unsigned char nParts)
{
    if (nParts & APPEARANCE_PART_BODY)
        m_pAppearance->RequestBodyAppearance(pAppearance);
    if (nParts & APPEARANCE_PART_HEAD)
        m_pAppearance->RequestHeadAppearance(pAppearance);
    if (nParts & APPEARANCE_PART_TAIL)
        m_pAppearance->RequestTailAppearance(pAppearance);
    if (nParts & APPEARANCE_PART_WING)
        m_pAppearance->RequestWingAppearance(pAppearance);
    memcpy(m_aUnmapped31C, pAppearance, sizeof(CCAppearanceInfo));
    return 1;
}
// PC 0x0060C980  Xbox 0x0003C9B0
void CSWCCreature::UpdateRelativePerception(CSWVisibilityNode* pNode)
{
    int bInvisible = 0;
    int bVisible = 0;
    unsigned long oidObject = pNode->m_oidObject;
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        if ((pNode->m_bSeen == 0 && pNode->m_bHeard == 1) ||
            pNode->m_bInvisible == 1)
        {
            bInvisible = 1;
        }
        else if (pNode->m_bSeen == 1 && pNode->m_bInvisible == 0)
        {
            bVisible = 1;
        }
        CSWCArea* pArea = m_pArea;
        if (pArea->GetObjectsAtPositionVisible(pCreature->m_vPosition) == 1)
        {
            if (bInvisible == 1 && pCreature->m_bInvisibilityVisualApplied == 0)
            {
                pCreature->AddVisualEffect(
                    VFX_DUR_INVISIBILITY,
                    0, OBJECT_INVALID, OBJECT_INVALID, 0, 0,
                    Vector(0.0f, 0.0f, 0.0f));
                pCreature->m_bInvisibilityVisualApplied = 1;
            }
            else if (bVisible == 1 && pCreature->m_bInvisibilityVisualApplied == 1)
            {
                pCreature->DeleteVisualEffect(
                    VFX_DUR_INVISIBILITY);
                pCreature->m_bInvisibilityVisualApplied = 0;
            }
        }
    }
}
// PC 0x0060CA80  Xbox 0x0003CAB0
unsigned long CSWCCreature::GetEquippedItemID(unsigned long nSlot)
{
    switch (nSlot)
    {
    case 1:
        return m_aoidEquippedItems[0];
    case 2:
        return m_aoidEquippedItems[1];
    case 4:
        return m_aoidEquippedItems[2];
    case 8:
        return m_aoidEquippedItems[3];
    case 16:
        return m_aoidEquippedItems[4];
    case 32:
        return m_aoidEquippedItems[5];
    case 64:
        return m_aoidEquippedItems[6];
    case 128:
        return m_aoidEquippedItems[7];
    case 256:
        return m_aoidEquippedItems[8];
    case 512:
        return m_aoidEquippedItems[9];
    case 1024:
        return m_aoidEquippedItems[10];
    case 16384:
        return m_aoidEquippedItems[11];
    case 32768:
        return m_aoidEquippedItems[12];
    case 65536:
        return m_aoidEquippedItems[13];
    case 131072:
        return m_aoidEquippedItems[14];
    default:
        return CGameObjectArray::INVALIDOBJID;
    }
}
// PC 0x0060CBD0  Xbox 0x0003CC00
void CSWCCreature::SetEquippedItemID(unsigned long nSlot, unsigned long oidItem)
{
    switch (nSlot)
    {
    case 1:
        m_aoidEquippedItems[0] = oidItem;
        break;
    case 2:
        m_aoidEquippedItems[1] = oidItem;
        break;
    case 4:
        m_aoidEquippedItems[2] = oidItem;
        break;
    case 8:
        m_aoidEquippedItems[3] = oidItem;
        break;
    case 16:
        m_aoidEquippedItems[4] = oidItem;
        break;
    case 32:
        m_aoidEquippedItems[5] = oidItem;
        break;
    case 64:
        m_aoidEquippedItems[6] = oidItem;
        break;
    case 128:
        m_aoidEquippedItems[7] = oidItem;
        break;
    case 256:
        m_aoidEquippedItems[8] = oidItem;
        break;
    case 512:
        m_aoidEquippedItems[9] = oidItem;
        break;
    case 1024:
        m_aoidEquippedItems[10] = oidItem;
        break;
    case 16384:
        m_aoidEquippedItems[11] = oidItem;
        break;
    case 32768:
        m_aoidEquippedItems[12] = oidItem;
        break;
    case 65536:
        m_aoidEquippedItems[13] = oidItem;
        break;
    case 131072:
        m_aoidEquippedItems[14] = oidItem;
        break;
    }
}
// PC 0x0060CD50  Xbox 0x0003CD80
void CSWCCreature::SetLastInstant(unsigned long nSlot, int nLastInstant)
{
    switch (nSlot)
    {
    case 1:
        m_anLastInstant[0] = nLastInstant;
        break;
    case 2:
        m_anLastInstant[1] = nLastInstant;
        break;
    case 4:
        m_anLastInstant[2] = nLastInstant;
        break;
    case 8:
        m_anLastInstant[3] = nLastInstant;
        break;
    case 16:
        m_anLastInstant[4] = nLastInstant;
        break;
    case 32:
        m_anLastInstant[5] = nLastInstant;
        break;
    case 64:
        m_anLastInstant[6] = nLastInstant;
        break;
    case 128:
        m_anLastInstant[7] = nLastInstant;
        break;
    case 256:
        m_anLastInstant[8] = nLastInstant;
        break;
    case 512:
        m_anLastInstant[9] = nLastInstant;
        break;
    case 1024:
        m_anLastInstant[10] = nLastInstant;
        break;
    case 16384:
        m_anLastInstant[11] = nLastInstant;
        break;
    case 32768:
        m_anLastInstant[12] = nLastInstant;
        break;
    case 65536:
        m_anLastInstant[13] = nLastInstant;
        break;
    case 131072:
        m_anLastInstant[14] = nLastInstant;
        break;
    }
}
// PC 0x0060CED0  Xbox 0x0003CF00
int CSWCCreature::GetLastInstant(unsigned long nSlot)
{
    switch (nSlot)
    {
    case 1:
        return m_anLastInstant[0];
    case 2:
        return m_anLastInstant[1];
    case 4:
        return m_anLastInstant[2];
    case 8:
        return m_anLastInstant[3];
    case 16:
        return m_anLastInstant[4];
    case 32:
        return m_anLastInstant[5];
    case 64:
        return m_anLastInstant[6];
    case 128:
        return m_anLastInstant[7];
    case 256:
        return m_anLastInstant[8];
    case 512:
        return m_anLastInstant[9];
    case 1024:
        return m_anLastInstant[10];
    case 16384:
        return m_anLastInstant[11];
    case 32768:
        return m_anLastInstant[12];
    case 65536:
        return m_anLastInstant[13];
    case 131072:
        return m_anLastInstant[14];
    default:
        return 0;
    }
}
// PC 0x0060D020  Xbox 0x0003D050
void CSWCCreature::SetItemUnEquippedByMe(unsigned long oidItem)
{
    if (oidItem != CGameObjectArray::INVALIDOBJID)
    {
        CGameObject* pObject;
        CGameObjectArray* pObjectArray =
            g_pAppManager->m_pClientExoApp->GetGameObjectArray();
        unsigned char nResult = pObjectArray->GetGameObject(oidItem, &pObject);
        if (nResult == CGameObjectArray::SUCCESS)
        {
            CSWCItem* pItem = pObject->AsSWCItem();
            pItem->SetEquippedBy(NULL);
        }
    }
}
// PC 0x0060D070  Xbox 0x0003D0A0
unsigned long CSWCCreature::GetAttackingWeapon(int nAttackType)
{
    switch (nAttackType)
    {
    case 1:
    case 6:
        return GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
    case 2:
        return GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);
    case 3:
        return m_aoidEquippedItems[11];
    case 4:
        return m_aoidEquippedItems[12];
    case 5:
        return m_aoidEquippedItems[13];
    default:
        return OBJECT_INVALID;
    }
}
// PC 0x0060D0E0  Xbox 0x0003D110
int CSWCCreature::ResolveAttackIndex(unsigned short nAnimation, int nMode,
                                     int nPreviousIndex)
{
    int nAttackIndex;

    if (nAnimation == 10109 && nMode == 0)
    {
        for (nAttackIndex = m_nPreviousAttackIndex;
             nAttackIndex == m_nPreviousAttackIndex ||
             nAttackIndex == nPreviousIndex;
             nAttackIndex = rand() % 5)
        {
        }
    }
    else
    {
        nAttackIndex = rand() % 2;
    }
    m_nCurrentAttackIndex = nAttackIndex;
    m_nPreviousAttackIndex = nAttackIndex;
    return nAttackIndex;
}
// PC 0x0060D140  Xbox 0x0003D170
unsigned short CSWCCreature::ResolveReactionAnimation(unsigned short nAnimation,
                                                      unsigned short nReaction,
                                                      int nWieldClass)
{
    CExoString sAnimation(nAnimation);
    CExoString sColumn(nWieldClass);
    int nResolvedAnimation = 0;
    int bResolveAnimation = 1;

    switch (nReaction)
    {
    case CREATURE_ANIMATION_COMBAT_READY:
        bResolveAnimation = 0;
        break;
    case CREATURE_ANIMATION_PARRY:
        sColumn = CExoString("Parry") + sColumn;
        break;
    case CREATURE_ANIMATION_DODGE:
        sColumn = CExoString("Dodge") + sColumn;
        break;
    case CREATURE_ANIMATION_DAMAGE:
        sColumn = CExoString("Damage") + sColumn;
        break;
    }

    if (bResolveAnimation)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pCombatAnimations = p2DArrays->m_pCombatAnimations;
        pCombatAnimations->GetINTEntry(sAnimation, sColumn,
                                       &nResolvedAnimation);
    }
    else
    {
        nResolvedAnimation = CREATURE_ANIMATION_COMBAT_READY;
    }
    return (unsigned short)nResolvedAnimation;
}
// PC 0x00613BA0  Xbox 0x00043F00
void CSWCCreature::EquipToInventorySlot(unsigned long nSlot, CSWCItem* pItem)
{
    Gob* pModel;
    unsigned long oidItem;
    if (pItem != NULL)
    {
        pModel = pItem->GetModel(MODEL_SLOT_DEFAULT, 1);
        oidItem = pItem->m_nId;
        pItem->SetEquippedBy(this);
    }
    else
    {
        pModel = NULL;
        oidItem = OBJECT_INVALID;
    }

    CSWCItem* pOldItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        GetEquippedItemID(nSlot));
    if (nSlot != INVENTORY_SLOT_RIGHTHAND && nSlot != INVENTORY_SLOT_LEFTHAND)
    {
        if (nSlot == INVENTORY_SLOT_ARMOUR)
        {
            if (pModel != NULL)
                pModel->AttachToScene(NULL);
            if (GetEquippedItemID(INVENTORY_SLOT_ARMOUR) != oidItem)
                SetItemUnEquippedByMe(GetEquippedItemID(INVENTORY_SLOT_ARMOUR));
            SetEquippedItemID(INVENTORY_SLOT_ARMOUR, oidItem);
        }
        else if (nSlot == INVENTORY_SLOT_HEAD)
        {
            if (GetEquippedItemID(INVENTORY_SLOT_HEAD) != oidItem)
                SetItemUnEquippedByMe(GetEquippedItemID(INVENTORY_SLOT_HEAD));
            SetEquippedItemID(INVENTORY_SLOT_HEAD, oidItem);
            if (pItem != NULL)
                pItem->m_oidVisibleOwner = m_nId;
        }
        else
        {
            if (GetEquippedItemID(nSlot) != oidItem)
                SetItemUnEquippedByMe(GetEquippedItemID(nSlot));
            SetEquippedItemID(nSlot, oidItem);
        }
    }
    else
    {
        if (GetEquippedItemID(nSlot) != oidItem)
        {
            if (nSlot == INVENTORY_SLOT_RIGHTHAND)
            {
                if (GetEquippedItemID(INVENTORY_SLOT_LEFTHAND) == CGameObjectArray::INVALIDOBJID ||
                    GetEquippedItemID(INVENTORY_SLOT_LEFTHAND) != oidItem)
                    SetItemUnEquippedByMe(GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
            }
            else
            {
                if (GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND) == CGameObjectArray::INVALIDOBJID ||
                    GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND) != oidItem)
                    SetItemUnEquippedByMe(GetEquippedItemID(nSlot));
            }
        }
        SetEquippedItemID(nSlot, oidItem);
        UpdateWeaponClass();
        UpdateVisibleWeapons(nSlot, pItem, GetLastInstant(nSlot));
        ResetAnimationState();
    }

    if (pOldItem != NULL && pOldItem != pItem)
    {
        if (pOldItem->GetModel(MODEL_SLOT_DEFAULT, 1) != NULL)
            pOldItem->UnloadModel();
    }
    if (pItem != NULL)
        pItem->SetArea(NULL);
}
// PC 0x00617330  Xbox 0x00047940
int CSWCCreature::UpdateAttackData(unsigned short* pAnimation,
                                  CSWCCombatAttackData* pAttackData)
{
    CSWCObject* pGameObject = g_pAppManager->GetClientExoApp()->GetGameObject(
        pAttackData->m_oidAttackObject);
    if (pGameObject == NULL)
    {
        *pAnimation = ATTACK_ANIMATION_INVALID;
        pAttackData->m_nDamageAmount = ATTACK_ANIMATION_INVALID;
        return 0;
    }

    unsigned char nWieldClass = GetAnimationBase()->m_nWieldClass;
    CSWCObject* pTarget = pGameObject->AsSWCObject();
    int nAnimation;
    if (m_nAttackAnimationOverride != 0)
    {
        nAnimation = m_nAttackAnimationOverride;
        m_nAttackAnimationOverride = 0;
    }
    else
    {
        nAnimation = g_nCombatAnimation;
        if (g_nCombatAnimation == 0 || GetObjectId() != COMBAT_ANIMATION_OVERRIDE_OBJECT_ID)
        {
            if (pAttackData->m_nImpactVisual != 0)
            {
                nAnimation = UpdateRangedAttackData(
                    (unsigned short)pAttackData->m_nAttackFlags, nWieldClass);
            }
            else
            {
                int bInCombat = InCreatureCombat(pTarget);
                nAnimation = UpdateMeleeAttackData(*pAnimation,
                    (unsigned short)pAttackData->m_nAttackFlags,
                    bInCombat, nWieldClass);
            }
        }
    }

    unsigned char nTargetWieldClass = pTarget->GetAnimationBase()->m_nWieldClass;
    if (pTarget != NULL && pTarget->AsSWCCreature() != NULL &&
        pTarget->AsSWCCreature()->IsCreature())
    {
        nTargetWieldClass = 0;
    }
    int nReaction = ResolveReactionAnimation(nAnimation,
        pAttackData->m_nDamageAmount, nTargetWieldClass);
    ResolveSpecialAttackVisuals((unsigned short)pAttackData->m_nAttackFlags,
        nWieldClass, pAttackData->m_nImpactVisual);
    *pAnimation = nAnimation;
    pAttackData->m_nDamageAmount = nReaction;
    return 1;
}
// PC 0x00613DA0  Xbox 0x000440E0
int CSWCCreature::UpdateMeleeAttackData(unsigned short nAnimation,
int nAttackType, int bInCombat,
int nWieldClass)
{
    int nAttackAnimation = MELEE_ANIMATION_NONE;
    if (nAttackType != 0 && !IsCreature())
    {
        int bCriticalStrike = nAttackType == CSWFeat::FEAT_MASTER_CRITICAL_STRIKE ||
            nAttackType == CSWFeat::FEAT_IMPROVED_CRITICAL_STRIKE ||
            nAttackType == CSWFeat::FEAT_CRITICAL_STRIKE;
        if (bCriticalStrike)
        {
            switch (nWieldClass)
            {
            case CSWCAnimBase::WIELD_CLASS_1:
                nAttackAnimation = MELEE_ANIMATION_CRITICAL_WIELD_1;
                break;
            case CSWCAnimBase::WIELD_CLASS_2:
                nAttackAnimation = MELEE_ANIMATION_CRITICAL_WIELD_2;
                break;
            case CSWCAnimBase::WIELD_CLASS_3:
                nAttackAnimation = MELEE_ANIMATION_CRITICAL_WIELD_3;
                break;
            case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                nAttackAnimation = MELEE_ANIMATION_CRITICAL_DUAL_2;
                break;
            }
        }
        else
        {
            int bFlurry = nAttackType == CSWFeat::FEAT_WHIRLWIND_ATTACK ||
                nAttackType == CSWFeat::FEAT_IMPROVED_FLURRY ||
                nAttackType == CSWFeat::FEAT_FLURRY;
            if (bFlurry)
            {
                switch (nWieldClass)
                {
                case CSWCAnimBase::WIELD_CLASS_1:
                    nAttackAnimation = MELEE_ANIMATION_FLURRY_WIELD_1;
                    break;
                case CSWCAnimBase::WIELD_CLASS_2:
                    nAttackAnimation = MELEE_ANIMATION_FLURRY_WIELD_2;
                    break;
                case CSWCAnimBase::WIELD_CLASS_3:
                    nAttackAnimation = MELEE_ANIMATION_FLURRY_WIELD_3;
                    break;
                case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                    nAttackAnimation = MELEE_ANIMATION_FLURRY_DUAL_2;
                    break;
                }
            }
            else
            {
                int bPowerAttack = nAttackType == CSWFeat::FEAT_MASTER_POWER_ATTACK ||
                    nAttackType == CSWFeat::FEAT_IMPROVED_POWER_ATTACK ||
                    nAttackType == CSWFeat::FEAT_POWER_ATTACK;
                if (bPowerAttack)
                {
                    switch (nWieldClass)
                    {
                    case CSWCAnimBase::WIELD_CLASS_1:
                        nAttackAnimation = MELEE_ANIMATION_POWER_WIELD_1;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_2:
                        nAttackAnimation = MELEE_ANIMATION_POWER_WIELD_2;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_3:
                        nAttackAnimation = MELEE_ANIMATION_POWER_WIELD_3;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                        nAttackAnimation = MELEE_ANIMATION_POWER_DUAL_2;
                        break;
                    }
                }
                else if (nAttackType == CSWFeat::FEAT_FORCE_JUMP_MASTERY ||
                    nAttackType == CSWFeat::FEAT_FORCE_JUMP_ADVANCED ||
                    nAttackType == CSWFeat::FEAT_FORCE_JUMP)
                {
                    switch (nWieldClass)
                    {
                    case CSWCAnimBase::WIELD_CLASS_2:
                        nAttackAnimation = MELEE_ANIMATION_JUMP_WIELD_2;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_3:
                        nAttackAnimation = MELEE_ANIMATION_JUMP_WIELD_3;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                        nAttackAnimation = MELEE_ANIMATION_JUMP_DUAL_2;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if (nAnimation == CREATURE_ANIMATION_MELEE_SEQUENCE)
        {
            int nAttackIndex = m_nCurrentAttackIndex;
            if (bInCombat)
            {
                if (IsCreature())
                {
                    if (GetAnimationBase()->AnimationExists(
                            MELEE_ANIMATION_CREATURE_COMBAT))
                        nAttackAnimation = MELEE_ANIMATION_CREATURE_COMBAT + nAttackIndex;
                    else
                        nAttackAnimation = MELEE_ANIMATION_CREATURE + nAttackIndex;
                }
                else
                {
                    switch (nWieldClass)
                    {
                    case CSWCAnimBase::WIELD_CLASS_1:
                        nAttackAnimation = MELEE_ANIMATION_COMBAT_WIELD_1 + nAttackIndex;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_2:
                        nAttackAnimation = MELEE_ANIMATION_COMBAT_WIELD_2 + nAttackIndex;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_3:
                        nAttackAnimation = MELEE_ANIMATION_COMBAT_WIELD_3 + nAttackIndex;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                        nAttackAnimation = MELEE_ANIMATION_COMBAT_DUAL_2 + nAttackIndex;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_UNARMED:
                        nAttackAnimation = MELEE_ANIMATION_COMBAT_UNARMED + nAttackIndex;
                        break;
                    }
                }
            }
            else
            {
                switch (nWieldClass)
                {
                case CSWCAnimBase::WIELD_CLASS_2:
                    nAttackAnimation = MELEE_ANIMATION_NONCOMBAT_WIELD_2 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_3:
                    nAttackAnimation = MELEE_ANIMATION_NONCOMBAT_WIELD_3 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                    nAttackAnimation = MELEE_ANIMATION_NONCOMBAT_DUAL_2 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_1:
                    nAttackAnimation = MELEE_ANIMATION_NONCOMBAT_WIELD_1 + nAttackIndex % 2;
                    break;
                case CSWCAnimBase::WIELD_CLASS_UNARMED:
                    nAttackAnimation = MELEE_ANIMATION_NONCOMBAT_UNARMED + nAttackIndex % 2;
                    break;
                }
            }
        }
        else
        {
            int nAttackIndex = m_nCurrentAttackIndex;
            if (IsCreature())
            {
                nAttackAnimation = MELEE_ANIMATION_CREATURE + nAttackIndex;
            }
            else
            {
                switch (nWieldClass)
                {
                case CSWCAnimBase::WIELD_CLASS_1:
                    nAttackAnimation = MELEE_ANIMATION_NORMAL_WIELD_1 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_2:
                    nAttackAnimation = MELEE_ANIMATION_NORMAL_WIELD_2 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_3:
                    nAttackAnimation = MELEE_ANIMATION_NORMAL_WIELD_3 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_DUAL_2:
                    nAttackAnimation = MELEE_ANIMATION_NORMAL_DUAL_2 + nAttackIndex;
                    break;
                case CSWCAnimBase::WIELD_CLASS_UNARMED:
                    nAttackAnimation = MELEE_ANIMATION_NORMAL_UNARMED + nAttackIndex;
                    break;
                }
            }
        }
    }
    return nAttackAnimation;
}
// PC 0x00614110  Xbox 0x00044450
int CSWCCreature::UpdateRangedAttackData(int nAttackType, int nWieldClass)
{
    int nAnimation = RANGED_ANIMATION_NONE;
    int bHumanoid = IsCreature() == 0;
    int bPowerBlast = nAttackType == CSWFeat::FEAT_MASTER_POWER_BLAST ||
        nAttackType == CSWFeat::FEAT_IMPROVED_POWER_BLAST ||
        nAttackType == CSWFeat::FEAT_POWER_BLAST;
    if (bPowerBlast && bHumanoid)
    {
        switch (nWieldClass)
        {
        case CSWCAnimBase::WIELD_CLASS_4:
            nAnimation = RANGED_ANIMATION_POWER_BLAST_WIELD_4;
            break;
        case CSWCAnimBase::WIELD_CLASS_DUAL_4:
            nAnimation = RANGED_ANIMATION_POWER_BLAST_DUAL_4;
            break;
        case CSWCAnimBase::WIELD_CLASS_5:
            nAnimation = RANGED_ANIMATION_POWER_BLAST_WIELD_5;
            break;
        case CSWCAnimBase::WIELD_CLASS_6:
            nAnimation = RANGED_ANIMATION_POWER_BLAST_WIELD_6;
            break;
        }
    }
    else if (bPowerBlast)
    {
        nAnimation = RANGED_ANIMATION_POWER_BLAST_CREATURE;
    }
    else
    {
        int bRapidShot = nAttackType == CSWFeat::FEAT_MULTI_SHOT ||
            nAttackType == CSWFeat::FEAT_IMPROVED_RAPID_SHOT ||
            nAttackType == CSWFeat::FEAT_RAPID_SHOT;
        if (bRapidShot && bHumanoid)
        {
            switch (nWieldClass)
            {
            case CSWCAnimBase::WIELD_CLASS_4:
                nAnimation = RANGED_ANIMATION_RAPID_SHOT_WIELD_4;
                break;
            case CSWCAnimBase::WIELD_CLASS_DUAL_4:
                nAnimation = RANGED_ANIMATION_RAPID_SHOT_DUAL_4;
                break;
            case CSWCAnimBase::WIELD_CLASS_5:
                nAnimation = RANGED_ANIMATION_RAPID_SHOT_WIELD_5;
                break;
            case CSWCAnimBase::WIELD_CLASS_6:
                nAnimation = RANGED_ANIMATION_RAPID_SHOT_WIELD_6;
                break;
            }
        }
        else if (bRapidShot)
        {
            nAnimation = RANGED_ANIMATION_RAPID_SHOT_CREATURE;
        }
        else
        {
            int bSniperShot = nAttackType == CSWFeat::FEAT_MASTER_SNIPER_SHOT ||
                nAttackType == CSWFeat::FEAT_IMPROVED_SNIPER_SHOT ||
                nAttackType == CSWFeat::FEAT_SNIPER_SHOT;
            if (bSniperShot && bHumanoid)
            {
                switch (nWieldClass)
                {
                case CSWCAnimBase::WIELD_CLASS_4:
                    nAnimation = RANGED_ANIMATION_SNIPER_SHOT_WIELD_4;
                    break;
                case CSWCAnimBase::WIELD_CLASS_DUAL_4:
                    nAnimation = RANGED_ANIMATION_SNIPER_SHOT_DUAL_4;
                    break;
                case CSWCAnimBase::WIELD_CLASS_5:
                    nAnimation = RANGED_ANIMATION_SNIPER_SHOT_WIELD_5;
                    break;
                case CSWCAnimBase::WIELD_CLASS_6:
                    nAnimation = RANGED_ANIMATION_SNIPER_SHOT_WIELD_6;
                    break;
                }
            }
            else if (bSniperShot)
            {
                nAnimation = RANGED_ANIMATION_SNIPER_SHOT_CREATURE;
            }
            else
            {
                if (bHumanoid)
                {
                    switch (nWieldClass)
                    {
                    case CSWCAnimBase::WIELD_CLASS_4:
                        nAnimation = RANGED_ANIMATION_NORMAL_WIELD_4;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_DUAL_4:
                        nAnimation = RANGED_ANIMATION_NORMAL_DUAL_4;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_5:
                        nAnimation = RANGED_ANIMATION_NORMAL_WIELD_5;
                        break;
                    case CSWCAnimBase::WIELD_CLASS_6:
                        nAnimation = RANGED_ANIMATION_NORMAL_WIELD_6;
                        break;
                    }
                }
                else
                {
                    nAnimation = RANGED_ANIMATION_NORMAL_CREATURE;
                }
            }
        }
    }
    return nAnimation;
}
// PC 0x0060D320  Xbox 0x0003D350
int CSWCCreature::IsCreature()
{
    int bCreature = 0;
    CExoString sModelType;
    CExoString sColumn("MODELTYPE");
    CSWCCreatureAppearance* pAppearance = m_pAppearance;
    unsigned short nAppearance = pAppearance->m_nAppearance;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = p2DArrays->m_pAppearance;
    pTable->GetCExoStringEntry(nAppearance, sColumn, &sModelType);

    char cModelType = sModelType[0];
    if (cModelType == CTwoDimArrays::APPEARANCE_MODELTYPE_S ||
        cModelType == CTwoDimArrays::APPEARANCE_MODELTYPE_L ||
        cModelType == CTwoDimArrays::APPEARANCE_MODELTYPE_LOWER_S ||
        cModelType == CTwoDimArrays::APPEARANCE_MODELTYPE_LOWER_L)
    {
        bCreature = 1;
    }
    return bCreature;
}
// PC 0x0060D3E0  Xbox 0x0003D410
int CSWCCreature::InCreatureCombat(CSWCObject* pTarget)
{
    int bInCombat = 0;
    if (pTarget != NULL)
    {
        if (IsCreature() ||
            (pTarget->AsSWCCreature() != NULL &&
             pTarget->AsSWCCreature()->IsCreature()))
            bInCombat = 1;
    }
    return bInCombat;
}
// PC 0x006142F0  Xbox 0x00044630
void CSWCCreature::ResolveSpecialAttackVisuals(int nAttackType, int nWieldClass,
                                             int bRanged)
{
    int nVisualEffect = 0;
    switch (nAttackType)
    {
    case CSWFeat::FEAT_IMPROVED_POWER_ATTACK:
        nVisualEffect = SelectSpecialAttackAnimation(nWieldClass,
            VFX_COM_POWER_ATTACK_IMPROVED_SABER,
            VFX_COM_POWER_ATTACK_IMPROVED_STAFF,
            VFX_COM_POWER_ATTACK_IMPROVED_SABER);
        break;
    case CSWFeat::FEAT_MASTER_POWER_ATTACK:
        nVisualEffect = SelectSpecialAttackAnimation(nWieldClass,
            VFX_COM_POWER_ATTACK_MASTERY_SABER,
            VFX_COM_POWER_ATTACK_MASTERY_STAFF,
            VFX_COM_POWER_ATTACK_MASTERY_SABER);
        break;
    case CSWFeat::FEAT_IMPROVED_CRITICAL_STRIKE:
        nVisualEffect = SelectSpecialAttackAnimation(nWieldClass,
            VFX_COM_CRITICAL_STRIKE_IMPROVED_SABER,
            VFX_COM_CRITICAL_STRIKE_IMPROVED_STAFF,
            VFX_COM_CRITICAL_STRIKE_IMPROVED_SABER);
        break;
    case CSWFeat::FEAT_MASTER_CRITICAL_STRIKE:
        nVisualEffect = SelectSpecialAttackAnimation(nWieldClass,
            VFX_COM_CRITICAL_STRIKE_MASTERY_SABER,
            VFX_COM_CRITICAL_STRIKE_MASTERY_STAFF,
            VFX_COM_CRITICAL_STRIKE_MASTERY_SABER);
        break;
    case CSWFeat::FEAT_IMPROVED_FLURRY:
        nVisualEffect = SelectSpecialAttackAnimation(nWieldClass,
            VFX_COM_FLURRY_IMPROVED_SABER,
            VFX_COM_FLURRY_IMPROVED_STAFF,
            VFX_COM_FLURRY_IMPROVED_SABER);
        break;
    case CSWFeat::FEAT_WHIRLWIND_ATTACK:
        nVisualEffect = SelectSpecialAttackAnimation(nWieldClass,
            VFX_COM_WHIRLWIND_STRIKE_SABER,
            VFX_COM_WHIRLWIND_STRIKE_STAFF,
            VFX_COM_WHIRLWIND_STRIKE_SABER);
        break;
    case CSWFeat::FEAT_SNIPER_SHOT:
        nVisualEffect = VFX_COM_SNIPER_SHOT;
        break;
    case CSWFeat::FEAT_IMPROVED_SNIPER_SHOT:
        nVisualEffect = VFX_COM_SNIPER_SHOT_IMPROVED;
        break;
    case CSWFeat::FEAT_MASTER_SNIPER_SHOT:
        nVisualEffect = VFX_COM_SNIPER_SHOT_MASTERY;
        break;
    case CSWFeat::FEAT_IMPROVED_RAPID_SHOT:
        nVisualEffect = VFX_COM_RAPID_SHOT_IMPROVED;
        break;
    case CSWFeat::FEAT_MULTI_SHOT:
        nVisualEffect = VFX_COM_MULTI_SHOT;
        break;
    case CSWFeat::FEAT_IMPROVED_POWER_BLAST:
        nVisualEffect = VFX_COM_POWER_BLAST_IMPROVED;
        break;
    case CSWFeat::FEAT_MASTER_POWER_BLAST:
        nVisualEffect = VFX_COM_POWER_BLAST_MASTERY;
        break;
    }

    if (nVisualEffect != 0)
    {
        CExoString sVisualEffect(nVisualEffect);
        CExoString sImpImpactNode;
        g_pRules->Get2DArrays()->GetVisualEffect()->GetCExoStringEntry(
            sVisualEffect, "Imp_Impact_Node", &sImpImpactNode);
        if (!sImpImpactNode.IsEmpty())
        {
            CExoString sHook;
            if (bRanged)
                sHook = "bullethook";
            else
                sHook = "root";

            CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
            if (pItem != NULL)
                pItem->SpawnVisualEffect(sImpImpactNode, sHook);

            if (nWieldClass == CSWCAnimBase::WIELD_CLASS_DUAL_2 ||
                nWieldClass == CSWCAnimBase::WIELD_CLASS_DUAL_4)
            {
                pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                    GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
                if (pItem != NULL)
                    pItem->SpawnVisualEffect(sImpImpactNode, sHook);
            }
        }
    }
}
// PC 0x0060D430  Xbox 0x0003D460
int CSWCCreature::SelectSpecialAttackAnimation(int nAttackType,
                                               int nType2Animation,
                                               int nType3Animation,
                                               int nType4Animation)
{
    int nAnimation = 0;

    switch (nAttackType)
    {
    case 2:
        nAnimation = nType2Animation;
        break;

    case 3:
        nAnimation = nType3Animation;
        break;

    case 4:
        nAnimation = nType4Animation;
        break;
    }
    return nAnimation;
}
// PC 0x0060D460  Xbox 0x0003D490
void CSWCCreature::SetNewLoopingCreatureSound(const CExoString& sSound)
{
    m_pLoopingSound = new CExoSoundSource(sSound);

    if (g_pAppManager->m_pClientExoApp->GetPlayerCreature() == this)
    {
        m_pLoopingSound->SetPriorityGroup(6);
    }
    else
    {
        m_pLoopingSound->SetPriorityGroup(7);
    }

    m_pLoopingSound->SetLooping(1);
}
// PC 0x0060D510  Xbox 0x0003D540
void CSWCCreature::DeleteLoopingCreatureSound()
{
    m_pLoopingSound->Stop();
    if (m_pLoopingSound != NULL)
    {
        delete m_pLoopingSound;
    }
    m_pLoopingSound = NULL;
}
// PC 0x0060D540  Xbox 0x0003D570
int CSWCCreature::GetArmourClass()
{
    if (m_pAppearance != NULL)
        return m_pAppearance->GetArmourClass();
    return 0;
}
// PC 0x0060D560  Xbox 0x0003D590
void CSWCCreature::SetPortrait(const CResRef& cPortrait)
{
    if (GetPortrait(0) != cPortrait)
    {
        m_pStats->m_cPortrait.CResRef::operator=(cPortrait);
        SetPopUpBubblePortrait(GetPortrait(1));
    }
}
// PC 0x0060D5C0  Xbox 0x0003D5F0
void CSWCCreature::SetPortraitId(unsigned short nPortraitId)
{
    if (m_pStats != NULL)
    {
        m_pStats->SetPortraitId(nPortraitId);
        SetPopUpBubblePortrait(GetPortrait(1));
    }
}
// PC 0x0060D600  Xbox 0x0003D630
void CSWCCreature::SetName(const CExoString& sFirstName,
                          const CExoString& sLastName)
{
    CExoString sBubbleText;
    m_pStats->SetName(sFirstName, sLastName);
    sBubbleText = "[";
    sBubbleText = sBubbleText + m_pStats->GetFullName() + "]";
    SetPopUpBubbleDefaultText(sBubbleText);
    SetPopUpBubbleDefaultCreatureID(GetObjectId());
}
// PC 0x0060D730  Xbox 0x0003D760
int CSWCCreature::SpawnHitVisuals(unsigned char nMode,
                                 CSWCCombatAttackData* pAttackData,
                                 const Vector& vDirection,
                                 const CExoString& sHook)
{
    CExoString sImpactHook(sHook);
    switch (nMode)
    {
    case HIT_VISUAL_IMPACT:
    {
        CExoString sVisualEffect;
        if (pAttackData->m_nImpactVisual != 0)
            sVisualEffect = VFX_COM_BLASTER_IMPACT;
        else if (pAttackData->m_nImpactY == CSWSCombatRound::WEAPON_ATTACK_UNARMED)
            sVisualEffect = VFX_COM_SPARKS_LARGE;
        else
            sVisualEffect = VFX_COM_SPARKS_LIGHTSABER;

        CExoString sImpImpactNode;
        g_pRules->Get2DArrays()->GetVisualEffect()->GetCExoStringEntry(
            sVisualEffect, "imp_impact_node", &sImpImpactNode);

        int nVisual;
        for (nVisual = 0; nVisual < 12; ++nVisual)
        {
            CAurObject* pVisual = m_apOwnedObjects[nVisual];
            if (pVisual != NULL)
            {
                float fTime = 0.0f;
                pVisual->GetAnimation("impact", &fTime, NULL);
                if (fTime != ANIMATION_TIME_UNAVAILABLE)
                    continue;
                delete pVisual;
                m_apOwnedObjects[nVisual] = NULL;
            }

            pVisual = NewCAurObject(
                (char*)sImpImpactNode.CStr(), "blood", NULL, NULL);
            if (pVisual == NULL)
                return 0;

            if (sImpactHook == "")
                sImpactHook = "impact";
            pVisual->AttachToObject(GetModel(MODEL_SLOT_DEFAULT, 1),
                (char*)sImpactHook.CStr(), Gob::ATTACH_NO_ORIENTATION);

            Vector vNormal;
            vNormal = normalize(vDirection);
            float fAngle = (vNormal * m_vOrientation > 1.0f)
                         ? 1.0f : vNormal * m_vOrientation;
            fAngle = (float)acos(fAngle);

            CClientExoApp* pApp = g_pAppManager->GetClientExoApp();
            CSWCItem* pWeapon = pApp->GetItemByGameObjectID(
                GetAttackingWeapon((unsigned char)pAttackData->m_nImpactY));
            if (pWeapon != NULL)
                g_pRules->GetBaseItemArray()->GetBaseItem(pWeapon->GetBaseItemType());

            if (cross(vDirection, normalize(m_vOrientation)).m_fZ >= 0.0f)
                pVisual->SetOrientation(Quaternion(Vector(0.0f, 0.0f, 1.0f), -fAngle) *
                    Quaternion(Vector(1.0f, 0.0f, 0.0f), 1.0f));
            else
                pVisual->SetOrientation(Quaternion(Vector(0.0f, 0.0f, 1.0f), fAngle) *
                    Quaternion(Vector(1.0f, 0.0f, 0.0f), 1.0f));

            pVisual->PlayAnimation("impact", 1.0f, Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
            m_apOwnedObjects[nVisual] = pVisual;
            return 1;
        }
        return 1;
    }
    default:
        return 0;
    }
}
// PC 0x0060DC50  Xbox 0x0003DCB0
int CSWCCreature::PlaySwingSound(const CExoString& sSound,
                               CSWCCombatAttackData* pAttackData,
                               int nSoundCount)
{
    int nMaterial = 0;
    CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
        GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
    if (pItem != NULL && pItem->GetBaseItem() != NULL)
        nMaterial = pItem->GetBaseItem()->m_nWeaponMatType;

    pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
        GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
    if (pItem != NULL && pItem->GetBaseItem() != NULL)
    {
        CSWSItem* pServerItem = pItem->GetServerItem();
        if (pServerItem != NULL && pServerItem->IsLightsaber() == 1)
            nMaterial = pItem->GetBaseItem()->m_nWeaponMatType;
    }

    int nSound = rand() % nSoundCount;
    CExoString sResRef;
    CExoString sColumn(nSound);
    sColumn = sSound + sColumn;
    {
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pArrays->m_pWeaponSounds);
        C2DA* pTable = pArrays->m_pWeaponSounds;
        pTable->GetCExoStringEntry(nMaterial, sColumn, &sResRef);
    }
    float fPitchOffset = 0.0f;
    {
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pArrays->m_pWeaponSounds);
        C2DA* pTable = pArrays->m_pWeaponSounds;
        pTable->GetFLOATEntry(nMaterial, CTwoDimArrays::COL2DA_WEAPONSOUNDS_PITCHOFFSET, &fPitchOffset);
    }
    return g_pExoSound->Play3DOneShotSound(
        sResRef, m_vPosition, 1.5f, CExoSound::PRIORITY_GROUP_WEAPON_SWING,
        0, 0, 0.0f, fPitchOffset);
}
// PC 0x0060DE20  Xbox 0x0003DEB0
int CSWCCreature::ShowLightSaberContactVisual(CSWCCombatAttackData* pAttackData)
{
    Vector vPosition;
    Quaternion qOrientation;
    if (!GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition("LightSaberHook", &vPosition,
                                          &qOrientation))
    {
        return FALSE;
    }
    m_pArea->ApplyFireAndForgetVisualEffect(VFX_COM_SPARKS_PARRY_METAL,
                                            vPosition);
    return TRUE;
}
// PC 0x0060DED0  Xbox 0x0003DF50
void CSWCCreature::DoFootstepAudio(unsigned char nFootstep)
{
    if (nFootstep == 0)
    {
        unsigned long oidCreature = m_nId;

        if (g_pAppManager->m_pClientExoApp->GetPlayerCreatureId() ==
            oidCreature)
        {
            int nRandom = rand();

            if (nRandom % 100 < 5)
            {
                CExoString sSound;

                sSound = "as_cv_florcreak";
                sSound = sSound + CExoString(rand() % 3 + 1);

                CResRef cResRef(sSound);
                g_pExoSound->Play3DOneShotSound(cResRef, m_vPosition, 0.0f, 19,
                                                0, 0, 0.0f, 0.0f);
            }
        }
    }
}
// PC 0x0060E010  Xbox 0x0003E090
void CSWCCreature::StripEquippedItems()
{
    for (unsigned char nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT; ++nSlot)
    {
        SetItemUnEquippedByMe(GetEquippedItemID(nSlot));
    }

    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->Wield(CGameObjectArray::INVALIDOBJID, "",
            CSWCAnimBase::WIELD_HAND_LEFT, 1.0f);
        m_pAnimBase->Wield(CGameObjectArray::INVALIDOBJID, "",
            CSWCAnimBase::WIELD_HAND_RIGHT, 1.0f);
    }
}
// PC 0x0060E120  Xbox 0x0003E1B0
void GetPersonalRadius(CAurObject* pObject, char* sProperty, void* pData)
{
    float fRadius = 0.0f;
    CSWCCreature* pCreature = static_cast<CSWCCreature*>(
        static_cast<Gob*>(pObject)->GetExternalData());

    if (pCreature != NULL)
    {
        g_pRules->m_p2DArrays->m_pAppearance->GetFLOATEntry(
            pCreature->m_pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_PERSPACE, &fRadius);
        *static_cast<float*>(pData) = fRadius;
    }
}
// PC 0x0060E170  Xbox 0x0003E260
void GetCreatureRadius(CAurObject* pObject, char* sProperty, void* pData)
{
    float fRadius = 0.0f;
    CSWCCreature* pCreature = static_cast<CSWCCreature*>(
        static_cast<Gob*>(pObject)->GetExternalData());
    if (pCreature != NULL)
    {
        g_pRules->m_p2DArrays->m_pAppearance->GetFLOATEntry(
            pCreature->m_pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_CREPERSPACE, &fRadius);
        *static_cast<float*>(pData) = fRadius;
    }

}
// PC 0x0060E1C0  Xbox 0x0003E310
void GetPath(CAurObject* pObject, char* sProperty, void* pData)
{
    Vector* pPath = static_cast<Vector*>(pData);
    pPath->m_fY = 0.0f;
    pPath->m_fZ = 0.0f;
}
// PC 0x0060E1D0  Xbox 0x0003E320
CSWCAnimBase* CSWCCreature::SetSpecialWalkAnimation(unsigned char a1)
{
    CSWCAnimBase* result;
    result = ((CSWCObject*)this)->GetAnimationBase();
    result->m_nSpecialWalkAnimation = a1;
    return result;
}
// PC 0x0060E1F0  Xbox 0x0003E340
CSWCAnimBase* CSWCCreature::SetRunLooksAwful(int a1)
{
    CSWCAnimBase* result;
    result = ((CSWCObject*)this)->GetAnimationBase();
    result->m_bRunLooksAwful = a1;
    return result;
}
// PC 0x0060E210  Xbox 0x0003E360
void CSWCCreature::HideEquippedHeadItem()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->HideHeadItem();
        m_bEquippedHeadItemHidden = 1;
    }
}
// PC 0x0060E230  Xbox 0x0003E380
void CSWCCreature::ShowEquippedHeadItem()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->ShowHeadItem();
        m_bEquippedHeadItemHidden = 0;
    }
}
// PC 0x0060E250  Xbox 0x0003E3A0
float CSWCCreature::GetHeadTurnHorizontal()
{
    float fHeadArc;
    unsigned short nAppearance = m_pAppearance->m_nAppearance;
    C2DA* pTable = g_pRules->m_p2DArrays->m_pAppearance;
    if (pTable->GetFLOATEntry(
            nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_HEAD_ARC_H, &fHeadArc) == 0)
    {
        return 40.0f;
    }
    return fHeadArc;
}
// PC 0x006145D0  Xbox 0x00044910
void CSWCCreature::ClearVisibilityList()
{
    for (int i = m_lstVisibilityList.num - 1; i >= 0; i--)
    {
        CSWVisibilityNode* pEntry = m_lstVisibilityList.element[i];

        m_lstVisibilityList.DelIndex(i);
        if (pEntry != NULL)
        {
            delete pEntry;
        }
    }
}
// PC 0x00614630  Xbox 0x00044970
void CSWCCreature::HideEquippedItems(const CResRef& cResRef,
                                     const CResRef& cResRef2)
{
    if (m_pAnimBase != NULL)
    {
        ComputeWeaponOverlays(0, 0);
        m_pAnimBase->HideWieldedItems(cResRef, cResRef2);
        m_bEquippedItemsHidden = 1;
    }
}
// PC 0x00614670  Xbox 0x000449B0
void CSWCCreature::ShowEquippedItems()
{
    if (m_pAnimBase != NULL)
    {
        ComputeWeaponOverlays(1, 1);
        m_pAnimBase->ShowWieldedItems();
        m_bEquippedItemsHidden = 0;

        if (m_oidThrownLightsaber != CGameObjectArray::INVALIDOBJID)
        {
            CSWCProjectile* pProjectile =
                g_pAppManager->m_pClientExoApp->GetProjectileByGameObjectID(
                    m_oidThrownLightsaber);
            if (pProjectile != NULL)
            {
                pProjectile->AttachToScene(NULL);
            }
            m_oidThrownLightsaber = CGameObjectArray::INVALIDOBJID;
        }
    }
}
// PC 0x006182A0  Xbox 0x00048AE0
void CSWCCreature::SetCombatState(int nState)
{
    NwccreatureTrace3();
    int nPreviousState = m_nCombatState;
    m_nCombatState = nState;
    if (nState != 0)
        GetAnimationBase();

    if (nPreviousState != nState)
    {
        CSWSCreature* pServerCreature = GetServerCreature();
        if (pServerCreature != NULL)
        {
            if (pServerCreature->GetDead() == 0)
            {
                if (pServerCreature->GetDeadTemp() == 0)
                {
                    unsigned long oidRightItem = GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
                    if (oidRightItem != OBJECT_INVALID)
                    {
                        CSWCObject* pRightItem = g_pAppManager->GetClientExoApp()->GetGameObject(oidRightItem);
                        if (pRightItem != NULL && pRightItem->AsSWCItem() != NULL)
                            pRightItem->AsSWCItem()->PowerItem(nState, 0, 1, 0);
                    }
                    unsigned long oidLeftItem = GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);
                    if (oidLeftItem != OBJECT_INVALID)
                    {
                        CSWCObject* pLeftItem = g_pAppManager->GetClientExoApp()->GetGameObject(oidLeftItem);
                        if (pLeftItem != NULL)
                        {
                            if (pLeftItem->AsSWCItem() != NULL)
                                pLeftItem->AsSWCItem()->PowerItem(nState, 0, 1, 0);
                        }
                    }
                }
            }
        }
    }
    if (nState != 0)
    {
        LookAt(OBJECT_INVALID, 0.0f);
        m_bCombatReady = 1;
    }
    else
        m_bCombatReady = 0;
}
// PC 0x006183D0  Xbox 0x00048C10
void CSWCCreature::AddFireForgetAnimation(unsigned short nAnimation, void* pData)
{
    float fAnimationLength;
    float fSpeed;
    unsigned long nLength = FIRE_FORGET_DEFAULT_LENGTH;
    if (nAnimation == CREATURE_ANIMATION_AMBIENT_PRESERVING_1 ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_PRESERVING_2 ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_PRESERVING_3 ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_PRESERVING_4 ||
        nAnimation == CREATURE_ANIMATION_AMBIENT_PRESERVING_5)
    {
        if (m_fAnimationSpeed != 0.0f)
            nLength = GetAnimationLength(nAnimation) / fabs(m_fAnimationSpeed);
        else
            nLength = GetAnimationLength(nAnimation);
        InsertFireForgetAnimation(nAnimation, nLength, m_fAnimationSpeed, pData, 0);
        if (pData == NULL && m_bAmbientPause)
        {
            m_nAmbientPauseTimer = 0;
            m_bAmbientPause = 0;
        }
    }
    else
    {
        if (m_bAmbientPause)
        {
            m_nAmbientPauseTimer = 0;
            m_bAmbientPause = 0;
        }
        if (pData != NULL)
        {
            CSWCCombatAttackData* pAttackData = (CSWCCombatAttackData*)pData;
            if (pAttackData->m_bHit == CSWCCombatAttackData::ANIMATION_EVENT_ATTACK)
            {
                ComputeWeaponOverlays(0, 1);
                m_nEngagedState = nAnimation == CREATURE_ANIMATION_MELEE_SEQUENCE;
                unsigned short nAttackAnimation = nAnimation;
                if (UpdateAttackData(&nAttackAnimation, pAttackData))
                {
                    fAnimationLength = GetAnimationLength(nAttackAnimation);
                    nLength = pAttackData->m_nAttackDuration;
                    fSpeed = (float)nLength != 0.0f ?
                        fAnimationLength / (float)nLength : 0.0f;
                    AbortFireForgetAnimation(0);
                    if (GetAnimationBase() != NULL &&
                        GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT) != NULL)
                        GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT)->PlayAnimation(
                            "NULL", 1.0f, 0, 0.0f);
                    InsertFireForgetAnimation(nAttackAnimation, nLength, fSpeed, pData, 0);
                    CSWCCreature* pTarget = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
                        pAttackData->m_oidAttackObject);
                    if (pTarget != NULL)
                    {
                        if (nAnimation == CREATURE_ANIMATION_MELEE_SEQUENCE)
                        {
                            unsigned long nTargetLength = pAttackData->m_nAttackDuration;
                            fSpeed = (float)nTargetLength != 0.0f ?
                                fAnimationLength / (float)nTargetLength : 0.0f;
                            if (pAttackData->m_nImpactVisual)
                            {
                                if (AnimationLooping(pTarget->GetCurrentAnimation()) &&
                                    pAttackData->m_nDamageAmount != CREATURE_ANIMATION_COMBAT_READY)
                                    pTarget->InsertFireForgetAnimation(pAttackData->m_nDamageAmount,
                                        nTargetLength, fSpeed, NULL, 0);
                            }
                            else
                            {
                                nTargetLength = pAttackData->m_nAttackDuration;
                                fSpeed = (float)nTargetLength != 0.0f ?
                                    fAnimationLength / (float)nTargetLength : 0.0f;
                                if (pAttackData->m_nDamageAmount != CREATURE_ANIMATION_COMBAT_READY)
                                {
                                    pTarget->AbortFireForgetAnimation(0);
                                    if (pTarget->GetAnimationBase() != NULL &&
                                        pTarget->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT) != NULL)
                                        pTarget->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT)->PlayAnimation(
                                            "NULL", 1.0f, 0, 0.0f);
                                    pTarget->InsertFireForgetAnimation(pAttackData->m_nDamageAmount,
                                        nTargetLength, fSpeed, NULL, 0);
                                }
                            }
                        }
                        else
                        {
                            if (AnimationLooping(pTarget->GetCurrentAnimation()) &&
                                pAttackData->m_nDamageAmount != CREATURE_ANIMATION_COMBAT_READY)
                            {
                                unsigned long nTargetLength = pAttackData->m_nAttackDuration;
                                fSpeed = (float)nTargetLength != 0.0f ?
                                    fAnimationLength / (float)nTargetLength : 0.0f;
                                pTarget->InsertFireForgetAnimation(pAttackData->m_nDamageAmount,
                                    nTargetLength, fSpeed, NULL, 0);
                            }
                            else
                            {
                                unsigned short nCurrentAnimation = pTarget->GetCurrentAnimation();
                            }
                        }
                    }
                    PlayHitSound(pAttackData, 1);
                    if (!pAttackData->m_nImpactVisual)
                    {
                        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                        if ((pPlayer != NULL && GetObjectId() == pPlayer->GetObjectId()) || rand() % 5 == 0)
                        {
                            int nVoice = rand() % 3;
                            unsigned short nSound = nVoice == 0 ? SOUNDSET_NASTY :
                                nVoice == 1 ? SOUNDSET_ATTACK_VOICE_2 : SOUNDSET_ATTACK_VOICE_3;
                            PlaySoundSetSound(nSound, 1);
                        }
                    }
                }
                else
                    delete pAttackData;
            }
            else if (pAttackData->m_bHit == CSWCCombatAttackData::ANIMATION_EVENT_SPELL)
            {
                if (nAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
                    nAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
                    nAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
                    nAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
                    nAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
                    nAnimation == CREATURE_ANIMATION_SPELLCAST_11)
                    nLength = FIRE_FORGET_SPELL_LENGTH;
                fAnimationLength = GetAnimationLength(nAnimation);
                fSpeed = (float)nLength != 0.0f ?
                    fAnimationLength / (float)nLength : 0.0f;
                InsertFireForgetAnimation(nAnimation, nLength, fSpeed, pData, 0);
            }
            else
            {
                if (pAttackData->m_bHit == CSWCCombatAttackData::ANIMATION_EVENT_SEQUENCE)
                {
                    nLength = GetAnimationLength(nAnimation);
                    if (nAnimation == CREATURE_ANIMATION_FIXED_SEQUENCE_1 ||
                        nAnimation == CREATURE_ANIMATION_FIXED_SEQUENCE_2 ||
                        nAnimation == CREATURE_ANIMATION_FIXED_SEQUENCE_3 ||
                        nAnimation == CREATURE_ANIMATION_FIXED_SEQUENCE_4)
                        nLength = FIRE_FORGET_SEQUENCE_LENGTH;
                    fAnimationLength = GetAnimationLength(nAnimation);
                    fSpeed = (float)nLength != 0.0f ?
                        fAnimationLength / (float)nLength : 0.0f;
                    InsertFireForgetAnimation(nAnimation, nLength, fSpeed, NULL, 0);
                    nLength = GetAnimationLength(nAnimation);
                    fAnimationLength = GetAnimationLength(pAttackData->m_nDamageAmount);
                    float fDuration = (float)pAttackData->m_nAnimationDelay + (float)nLength;
                    fSpeed = fDuration != 0.0f ? fAnimationLength / fDuration : 0.0f;
                    InsertFireForgetAnimation(pAttackData->m_nDamageAmount,
                        (unsigned long)fDuration, fSpeed, NULL, 0);
                }
                else if (pAttackData->m_bHit == CSWCCombatAttackData::ANIMATION_EVENT_TIMED)
                {
                    nLength = pAttackData->m_nAttackDuration;
                    fAnimationLength = GetAnimationLength(nAnimation);
                    fSpeed = (float)nLength != 0.0f ?
                        fAnimationLength / (float)nLength : 0.0f;
                    InsertFireForgetAnimation(nAnimation, nLength, fSpeed, NULL, 0);
                }
                else
                    return;
                delete pAttackData;
            }
        }
        else
        {
            if (m_fAnimationSpeed != 0.0f)
                nLength = GetAnimationLength(nAnimation) / fabs(m_fAnimationSpeed);
            else
                nLength = GetAnimationLength(nAnimation);
            if (nAnimation == CREATURE_ANIMATION_SHORT_IDLE)
                nLength = FIRE_FORGET_IDLE_LENGTH;
            InsertFireForgetAnimation(nAnimation, nLength, m_fAnimationSpeed, NULL, 0);
        }
    }
}
// PC 0x00617470  Xbox 0x00047A80
int CSWCCreature::PlayHitSound(CSWCCombatAttackData* pAttackData, int bQueued)
{
    if (pAttackData != NULL)
    {
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(
            pAttackData->m_oidAttackObject);
        if (pObject != NULL && pObject->AsSWCObject() != NULL)
        {
            CSWCObject* pTarget = pObject->AsSWCObject();
            int nSoundAppearance = 0;
            int nMaterial = 0;
            CExoString sSound;
            float fPitchOffset = 0.0f;
            if (!bQueued && pAttackData->m_nImpactVisual == 0)
            {
                unsigned short nAppearance = GetAppearance()->m_nAppearance;
                {
                    CTwoDimArrays* pArrays = g_pRules->Get2DArrays();
                    C2DA* pTable = pArrays->m_pAppearance;
                    pTable->GetINTEntry(
                        nAppearance,
                        CTwoDimArrays::COL2DA_APPEARANCETABLE_SOUNDAPPTYPE,
                        &nSoundAppearance);
                }
                {
                    CTwoDimArrays* pArrays = g_pRules->Get2DArrays();
                    C2DA* pTable = pArrays->GetAppearanceSounds();
                    pTable->GetINTEntry(
                        nSoundAppearance, CTwoDimArrays::COL2DA_APPEARANCESOUNDS_WEAPON,
                        &nMaterial);
                }
                if (nMaterial == 0)
                {
                    unsigned long nSlot = 0;
                    if (pAttackData->m_nImpactY == CSWSCombatRound::WEAPON_ATTACK_RIGHTHAND ||
                        pAttackData->m_nImpactY == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND ||
                        GetAnimationBase()->m_nWieldClass == CSWCAnimBase::WIELD_CLASS_3)
                        nSlot = INVENTORY_SLOT_RIGHTHAND;
                    else if (pAttackData->m_nImpactY == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND)
                        nSlot = INVENTORY_SLOT_LEFTHAND;
                    CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                        GetEquippedItemID(nSlot));
                    if (pItem != NULL)
                    {
                        int nBaseItem = pItem->GetBaseItemType();
                        CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()->GetBaseItem(nBaseItem);
                        nMaterial = pBaseItem->m_nWeaponMatType;
                    }
                }
                if ((pAttackData->m_nAttackResult == ATTACK_RESULT_PARRIED ||
                     pAttackData->m_nAttackResult == ATTACK_RESULT_MISS) &&
                    pTarget->AnimationParry(pTarget->GetCurrentAnimation()))
                {
                    CExoString sColumn = CExoString("Parry") + CExoString(rand() % 2);
                    {
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pWeaponSounds;
                        pTable->GetCExoStringEntry(
                            nMaterial, sColumn, &sSound);
                    }
                    {
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pWeaponSounds;
                        pTable->GetFLOATEntry(
                            nMaterial, CTwoDimArrays::COL2DA_WEAPONSOUNDS_PITCHOFFSET,
                            &fPitchOffset);
                    }
                }
                else
                {
                    CExoString sColumn;
                    if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                    {
                        CSWCCreature* pCreature = pTarget->AsSWCCreature();
                        if (pCreature->GetForceShield())
                            sColumn = "forcefield";
                        else if (pCreature->m_bStoneTexture || pCreature->m_bShadeTexture)
                            sColumn = "stone";
                        else if (pCreature->m_bBarkTexture)
                            sColumn = "wood";
                        else
                        {
                            unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
                            {
                                CTwoDimArrays* pArrays = g_pRules->Get2DArrays();
                                C2DA* pTable = pArrays->m_pAppearance;
                                pTable->GetINTEntry(
                                    nAppearance,
                                    CTwoDimArrays::COL2DA_APPEARANCETABLE_SOUNDAPPTYPE,
                                    &nSoundAppearance);
                            }
                            {
                                CTwoDimArrays* pArrays = g_pRules->Get2DArrays();
                                C2DA* pTable = pArrays->GetAppearanceSounds();
                                pTable->GetCExoStringEntry(
                                    nSoundAppearance, CTwoDimArrays::COL2DA_APPEARANCESOUNDS_ARMORTYPE,
                                    &sColumn);
                            }
                            if (sColumn.IsEmpty())
                            {
                                CSWCItem* pArmour = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                    pCreature->GetEquippedItemID(INVENTORY_SLOT_ARMOUR));
                                if (pArmour != NULL)
                                    sColumn = pArmour->GetBaseItem()->m_sArmorType;
                            }
                            if (sColumn.IsEmpty())
                                sColumn = "leather";
                        }
                    }
                    else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                    {
                        CSWCDoor* pDoor = pTarget->AsSWCDoor();
                        int nAppearanceType = pDoor->m_nAppearanceType;
                        if (nAppearanceType == 0)
                        {
                            unsigned char nGenericType = pDoor->m_nGenericType;
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pGenericDoors;
                            pTable->GetINTEntry(
                                nGenericType, CTwoDimArrays::COL2DA_GENERICDOORS_SOUNDAPPTYPE,
                                &nSoundAppearance);
                        }
                        else
                        {
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pDoorTypes;
                            pTable->GetINTEntry(
                                nAppearanceType, CTwoDimArrays::COL2DA_DOORTYPES_SOUNDAPPTYPE,
                                &nSoundAppearance);
                        }
                        {
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pPlaceableSounds;
                            pTable->GetCExoStringEntry(
                                nSoundAppearance, CTwoDimArrays::COL2DA_PLACEABLESOUNDS_ARMORTYPE,
                                &sColumn);
                        }
                    }
                    else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                    {
                        CSWCPlaceable* pPlaceable = pTarget->AsSWCPlaceable();
                        int nAppearance = pPlaceable->m_nAppearance;
                        {
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pPlaceables;
                            pTable->GetINTEntry(
                                nAppearance, CTwoDimArrays::COL2DA_PLACEABLES_SOUNDAPPTYPE,
                                &nSoundAppearance);
                        }
                        {
                            C2DA* pTable = g_pRules->Get2DArrays()->m_pPlaceableSounds;
                            pTable->GetCExoStringEntry(
                                nSoundAppearance, CTwoDimArrays::COL2DA_PLACEABLESOUNDS_ARMORTYPE,
                                &sColumn);
                        }
                    }
                    sColumn = sColumn + CExoString(rand() % 2);
                    {
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pWeaponSounds;
                        pTable->GetCExoStringEntry(
                            nMaterial, sColumn, &sSound);
                    }
                    {
                        C2DA* pTable = g_pRules->Get2DArrays()->m_pWeaponSounds;
                        pTable->GetFLOATEntry(
                            nMaterial, CTwoDimArrays::COL2DA_WEAPONSOUNDS_PITCHOFFSET,
                            &fPitchOffset);
                    }
                }
                sSound.CStr();
                g_pExoSound->Play3DOneShotSound(
                    (char*)sSound.CStr(), m_vPosition, 1.5f,
                    CExoSound::PRIORITY_GROUP_WEAPON_SWING, 0, 0, 0.0f, fPitchOffset);
            }
            return 1;
        }
    }
    return 0;
}
// PC 0x006179C0  Xbox 0x00048170
Gob* CSWCCreature::GetModel(unsigned char nModel, int bForceAppearanceUpdate)
{
    CSWCAnimBase* pAnimBase = m_pAnimBase;

    if (pAnimBase == NULL)
        return NULL;

    if (bForceAppearanceUpdate != 0 && m_nCameraFollowDelay > 0 &&
        nModel == 0xFF && pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT) == NULL)
    {
        ForceAppearanceUpdate();
        ReAttachCamera();
    }

    return (Gob*)m_pAnimBase->GetModel(nModel);
}
// PC 0x00617A20  Xbox 0x000481D0
void CSWCCreature::CreateNewSpellLikeAbilityList()
{
    m_pSpellLikeAbilities = new CExoArrayList<CSWCStats_SpellLikeAbility>(0);
}
// PC 0x00617A70  Xbox 0x00048220
void CSWCCreature::DeleteSpellLikeAbilityList()
{
    if (m_pSpellLikeAbilities != NULL)
    {
        m_pSpellLikeAbilities->SetSize(0);
        delete m_pSpellLikeAbilities;
        m_pSpellLikeAbilities = NULL;
    }
}
int SceneHitCheck(CAurScene* pScene, Vector vStart, Vector vEnd, Vector* pHit, CHitInfo* pHitInfo);
// PC 0x00617AD0  Xbox 0x00048280
int CSWCCreature::CanSee(CSWCObject* pObject)
{
    if (pObject == NULL)
        return 0;

    Vector vStart = m_vPosition;
    Vector vEnd;
    vStart.m_fZ += NWC_CREATURE_SIGHT_HEIGHT;
    vEnd = pObject->m_vPosition;
    vEnd.m_fZ += pObject->GetCanSeeHeight();

    DefaultHitInfo.ClearOutput();
    DefaultHitInfo.m_bResult0 = 1;
    DefaultHitInfo.m_lstHits.m_nCount = 0;
    DefaultHitInfo.m_lstHits.Add(GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
    Gob* pModel = GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
    if (pModel != NULL)
        DefaultHitInfo.m_lstHits.Add(pModel);
    DefaultHitInfo.m_lstHits.Add(pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
    pModel = pObject->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
    if (pModel != NULL)
        DefaultHitInfo.m_lstHits.Add(pModel);

    int nHit;
    do
    {
        DefaultHitInfo.ClearOutput();
        CAurScene* pScene = m_pArea->m_pScene;
        nHit = SceneHitCheck(pScene, vStart, vEnd,
                            NULL, &DefaultHitInfo);
        if (nHit == CHitInfo::HIT_GOB)
        {
            CSWCObject* pHitObject = (CSWCObject*)((Gob*)(CAurObject*)
                DefaultHitInfo.m_pOutputObject)->GetExternalData();
            if (pHitObject == NULL)
                nHit = CHitInfo::HIT_STATIC;
            else if (pHitObject->AsSWCDoor() != NULL)
            {
                CSWSDoor* pDoor = pHitObject->AsSWCDoor()->GetServerDoor();
                if (pDoor != NULL && pDoor->m_nOpenState == CSWSDoor::OPEN_STATE_CLOSED)
                    nHit = CHitInfo::HIT_STATIC;
                else
                    DefaultHitInfo.m_lstHits.Add(
                        pHitObject->AsSWCDoor()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
            }
            else if (pHitObject->AsSWCPlaceable() != NULL)
                DefaultHitInfo.m_lstHits.Add(
                    pHitObject->AsSWCPlaceable()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
            else
                break;
        }
    } while (nHit == CHitInfo::HIT_GOB);
    return nHit != CHitInfo::HIT_STATIC;
}
// PC 0x00617DD0  Xbox 0x00048580
void CSWCCreature::ActionMenuUseAttackFeat(unsigned long nFeat,
                                           CSWCCreature* pTarget)
{
    if (g_pRules->GetFeat((unsigned short)nFeat) != NULL)
    {
        unsigned long oidCreature = m_nId;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        if (pClientExoApp->ShowTutorialWindow(0, oidCreature, OBJECT_INVALID,
                                              (unsigned short)nFeat) == 0)
        {
            PerformCombatFeat((unsigned short)nFeat, pTarget);
        }
    }
}
// PC 0x00618A30  Xbox 0x00049300
void CSWCCreature::EnableFeatForMenu(
    int nFeat, CExoArrayList<CSWGuiInterfaceAction>& lstActions,
    PFNMENUACTION pfnAction, CSWCCreature* pTarget)
{
    if (nFeat != -1)
    {
        CSWFeat* pFeat = g_pRules->GetFeat((unsigned short)nFeat);
        int nAction = lstActions.num;
        lstActions.SetSize(nAction + 1);
        unsigned long nNameStrRef = pFeat->m_nNameStrRef;
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        CExoString sName = pClient->GetGUIString(nNameStrRef);
        if (pTarget == this)
        {
            sName = sName + " (" +
                g_pAppManager->GetClientExoApp()->GetGUIString(CSWGuiInterfaceAction::STRREF_SELF) + ")";
        }
        nFeat |= CSWGuiInterfaceAction::FEAT_ACTION_MASK;
        lstActions[nAction].m_sName = sName;
        lstActions[nAction].m_pfnAction = pfnAction;
#ifdef _XBOX
        lstActions[nAction].m_oidTarget = (unsigned long)pTarget;
#else
        lstActions[nAction].m_oidTarget = pTarget->GetObjectId();
#endif
        lstActions[nAction].m_nAction = nFeat;
        lstActions[nAction].m_resIcon = pFeat->m_cIcon;
    }
}
// PC 0x00618C20  Xbox 0x000494F0
void CSWCCreature::GetEquipmentForcePowers(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions,
    unsigned long nType, int nExclusion, unsigned long nItemMask)
{
    CExoArrayList<CSWCItem*> lstItems;
    CExoArrayList<short> lstPriorities;
    CExoArrayList<short> lstSlots;
    CSWCItem* pItem;
    CSWSpell* pSpell;

    for (int i = 0; i < EQUIPPED_MENU_ITEM_COUNT; ++i)
    {
        pItem = g_pAppManager->GetClientExoApp()->
            GetItemByGameObjectID(m_aoidEquippedItems[i]);
        if (pItem == NULL)
            continue;
        pSpell = g_pRules->m_pSpellArray->GetSpell(
            pItem->GetActivePropertySpellID(0));
        if (pSpell == NULL || (nExclusion & pSpell->m_nExclusion) != 0)
            continue;

        int nSubRadialSpellCount = pSpell->m_nSubRadialSpellCount;
        for (int j = 0; j < nSubRadialSpellCount; ++j)
        {
            int nPriority = (short)pSpell->GetSubRadialSpell((unsigned char)j);
            int nSlot = nPriority / 10;
            nPriority -= nSlot * 10;
            int nGroup = nSlot / 100;
            nSlot -= (short)nGroup * 100;
            if ((unsigned long)nGroup != nType)
                continue;

            int bInserted = 0;
            for (int k = 0; k < lstItems.num && !bInserted; ++k)
            {
                if (lstSlots[k] == nSlot)
                {
                    bInserted = 1;
                    if (lstPriorities[k] < nPriority)
                    {
                        lstPriorities[k] = nPriority;
                        lstItems[k] = pItem;
                    }
                }
                else if (lstSlots[k] > nSlot)
                {
                    bInserted = 1;
                    lstPriorities.Insert(nPriority, k);
                    lstSlots.Insert(nSlot, k);
                    lstItems.Insert(pItem, k);
                }
            }
            if (!bInserted)
            {
                lstItems.Add(pItem);
                lstPriorities.Add(nPriority);
                lstSlots.Add(nSlot);
            }
        }
    }

    int nAction = lstActions.num;
    for (int iItem = 0; iItem < lstItems.num; nAction++, iItem++)
    {
        pItem = lstItems[iItem];
        int nSpellID = pItem->GetActivePropertySpellID(0);
        pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellID);
        lstActions.SetSize(nAction + 1);
        lstActions[nAction].m_sName = pSpell->GetSpellNameText();
        lstActions[nAction].m_nAction =
            pItem->GetId() | CSWGuiInterfaceAction::SPELL_ACTION_MASK;
        if (pSpell->m_resIcon == "")
            lstActions[nAction].m_resIcon = pItem->m_cIconResRef;
        else
            lstActions[nAction].m_resIcon = pSpell->m_resIcon;
        lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_NONE;

        CSWSItem* pServerItem = pItem->GetServerItem();
        CSWItemProperty* pProperty = NULL;
        if (pServerItem != NULL)
            pProperty = pServerItem->GetActiveProperty(0);
        int bEnabled = 1;
        if (pServerItem != NULL && pProperty != NULL &&
            pServerItem->GetUsedActivePropertyUsesLeft(0) < 1 &&
            pProperty->m_nCostValue != CSWItemProperty::ACTIVATE_ITEM_UNLIMITED_ALT &&
            pProperty->m_nCostValue != CSWItemProperty::ACTIVATE_ITEM_UNLIMITED)
        {
            bEnabled = 0;
            lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_FORCE_POINTS;
        }
        if ((nItemMask & pSpell->m_nForbidItemMask) != 0)
        {
            bEnabled = 0;
            lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_FORBIDDEN_ITEM;
        }
        if ((nItemMask & pSpell->m_nRequireItemMask) != pSpell->m_nRequireItemMask)
        {
            bEnabled = 0;
            lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_REQUIRED_ITEM;
        }
        lstActions[nAction].m_bEnabled = bEnabled;
    }
}
// PC 0x006191F0  Xbox 0x00049AC0
void CSWCCreature::GetHostileForcePowers(
    CSWCObject* pTarget, CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    int nExclusion = 0;
    CSWSCreature* pServerCreature = GetServerCreature();
    if (pServerCreature != NULL &&
        pServerCreature->GetArea()->GetPlayerRestrictMode() == 0)
    {
        if (pTarget->AsSWCCreature() != NULL)
        {
            if (pTarget->AsSWCCreature()->GetStats()->m_nRace == CSWRace::CRACE_DROID)
                nExclusion = CSWRace::CRACE_DROID_MASK;
            else if (pTarget->AsSWCCreature()->GetStats()->m_nRace == CSWRace::CRACE_HUMAN)
                nExclusion = CSWRace::CRACE_HUMAN_MASK;
        }
        else
            nExclusion = CSWRace::CRACE_PLACEABLE_MASK;

        unsigned long nItemMask = pServerCreature->GetForceItemMask();
        unsigned long nForcePoints;
        if (pServerCreature->m_bBodyFuel)
            nForcePoints = pServerCreature->GetCurrentHitPoints(0);
        else
            nForcePoints = (short)(pServerCreature->m_pStats->m_nCurrentForcePoints +
                pServerCreature->m_pStats->m_nTemporaryForcePoints);

        int nFirstAction = lstActions.num;
        if (GetStats()->m_nRace == CSWRace::CRACE_DROID)
            GetEquipmentForcePowers(lstActions, CSWSpell::SUBRADIAL_TYPE_HOSTILE_FORCE_POWER,
                nExclusion, nItemMask);
        else
        {
            Vector vDistance = pTarget->m_vPosition - m_vPosition;
            GetStats()->GetMenuInfo(lstActions, CSWSpell::SUBRADIAL_TYPE_HOSTILE_FORCE_POWER,
                nForcePoints, nExclusion, nItemMask, vDistance * vDistance);
        }
        for (int i = nFirstAction; i < lstActions.num; ++i)
        {
            lstActions[i].m_pfnAction = (PFNMENUACTION)&CSWCCreature::ForceMenuHostileAction;
#ifdef _XBOX
            lstActions[i].m_oidTarget = (unsigned long)pTarget;
#else
            lstActions[i].m_oidTarget = pTarget->GetId();
#endif
        }
    }
}
// PC 0x006193A0  Xbox 0x00049F70
void CSWCCreature::CreateUsableItemEntry(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions, CSWSItem* pItem,
    int bEquipped, unsigned char nActionMask)
{
    int nEntry = lstActions.num;
    if (IsLeaderUsableItem(pItem, bEquipped, nActionMask))
    {
        CExoString sName;
        pItem->m_sLocalizedName.GetString(
            g_pAppManager->GetClientExoApp()->GetClientLanguage(), &sName, 0);
        sName.Format("%s (%s)", sName.CStr(),
            g_pAppManager->GetClientExoApp()->GetGUIString(
                CSWGuiInterfaceAction::STRREF_SELF).CStr());
        int nNum = lstActions.num;
        lstActions.SetSize(nNum + 1);
        lstActions[nEntry].m_sName = sName;
        lstActions[nEntry].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
        lstActions[nEntry].m_pfnAction =
            static_cast<PFNMENUACTION>(&CSWCCreature::PartyMenuUseItem);
#ifdef _XBOX
        lstActions[nEntry].m_oidTarget = (unsigned long)this;
#else
        lstActions[nEntry].m_oidTarget = GetObjectId();
#endif
        lstActions[nEntry].m_resIcon = pItem->GetIcon();
#ifndef _XBOX
        int nStackSize = pItem->m_nStackSize;
        lstActions[nEntry].m_nStackSize = nStackSize;
#endif

        int nItemType = pItem->GetBaseItem()->m_nItemType;
        if ((nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
             nItemType == CSWBaseItem::ITEM_TYPE_MEDPAC) &&
            GetStats()->GetCurrentHitPoints() >= GetStats()->GetMaxHitPoints())
        {
            CSWSCreature* pCreature = GetServerObject() != NULL ?
                GetServerObject()->AsSWSCreature() : NULL;
            if (pCreature == NULL || !pCreature->m_nPoisonState)
            {
                lstActions[nEntry].m_bEnabled = 0;
                lstActions[nEntry].m_nDisabledReason =
                    CSWGuiInterfaceAction::DISABLED_REASON_FULL_HEALTH;
            }
        }
        else if (nItemType == CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC)
        {
            int bInjured = 0;
            if (GetStats()->GetCurrentHitPoints() < GetStats()->GetMaxHitPoints())
                bInjured = 1;
            CSWCCreature* pMember =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(1);
            if (pMember != NULL && pMember->GetServerCreature() != NULL)
            {
                if (pMember->GetStats()->GetCurrentHitPoints() <
                    pMember->GetStats()->GetMaxHitPoints())
                    bInjured |= 1;
            }
            pMember = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(2);
            if (pMember != NULL && pMember->GetServerCreature() != NULL)
            {
                if (pMember->GetStats()->GetCurrentHitPoints() <
                    pMember->GetStats()->GetMaxHitPoints())
                    bInjured |= 1;
            }
            CSWSCreature* pCreature = GetServerObject() != NULL ?
                GetServerObject()->AsSWSCreature() : NULL;
            if (!bInjured || pCreature == NULL)
            {
                lstActions[nEntry].m_bEnabled = 0;
                lstActions[nEntry].m_nDisabledReason =
                    CSWGuiInterfaceAction::DISABLED_REASON_FULL_HEALTH;
            }
        }
        else if (bEquipped)
        {
            CSWItemProperty* pProperty;
            pItem->GetPropertyByType(&pProperty,
                CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0);
            lstActions[nEntry].m_bEnabled = (unsigned char)pProperty->m_bUseable;
        }
    }
}
// PC 0x006196A0  Xbox 0x0004A1D0
void CSWCCreature::CreateHostileItemEntry(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions, CSWSItem* pItem,
    CSWCCreature* pTarget)
{
    int nEntry = lstActions.num;
    if (IsHostileUsableItem(pItem))
    {
        CExoString sName;
        pItem->m_sLocalizedName.GetString(
            g_pAppManager->GetClientExoApp()->GetClientLanguage(), &sName, 0);
        int nNum = lstActions.num;
        lstActions.SetSize(nNum + 1);
        lstActions[nEntry].m_sName = sName;
        lstActions[nEntry].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
        lstActions[nEntry].m_pfnAction =
            static_cast<PFNMENUACTION>(&CSWCCreature::PartyMenuUseHostileItem);
#ifdef _XBOX
        lstActions[nEntry].m_oidTarget = (unsigned long)pTarget;
#else
        lstActions[nEntry].m_oidTarget = pTarget->GetObjectId();
#endif
        lstActions[nEntry].m_resIcon = pItem->GetIcon();
#ifndef _XBOX
        int nStackSize = pItem->m_nStackSize;
        lstActions[nEntry].m_nStackSize = nStackSize;
#endif
    }
}
// PC 0x00617E40  Xbox 0x000485F0
void CSWCCreature::PartyMenuUseHostileItem(unsigned long oidItem,
                                           CSWCCreature* pTarget)
{
    CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL && pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_HOSTILE_USABLE)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        if (pClientExoApp->ShowTutorialWindow(1, GetObjectId(),
                                               pTarget->GetObjectId(), oidItem) != 0)
        {
            return;
        }
    }
    PerformUseHostileItem(oidItem, pTarget);
}
// PC 0x00617EB0  Xbox 0x000486E0
void HitEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    CSWCCombatAttackData* pAttackData = (CSWCCombatAttackData*)pData;
    CSWCObject* pAttacker = NULL;
    CSWCObject* pTarget = NULL;
    if (pAttackData != NULL)
    {
        pTarget = g_pAppManager->GetClientExoApp()->GetGameObject(
            pAttackData->m_oidAttackObject);
        pAttacker = g_pAppManager->GetClientExoApp()->GetGameObject(
            pAttackData->m_oidCreature);
    }
    if ((pAttackData == NULL || (pAttacker != NULL && pTarget != NULL)) &&
        pAttacker->AsSWCCreature() != NULL)
    {
        CSWCCreature* pCreature = pAttacker->AsSWCCreature();
        if (pAttackData->m_nImpactVisual == 0)
        {
            if (pAttackData->m_nAttackResult == ATTACK_RESULT_HIT_SUCCESSFUL ||
                pAttackData->m_nAttackResult == ATTACK_RESULT_CRITICAL_HIT ||
                pAttackData->m_nAttackResult == ATTACK_RESULT_AUTOMATIC_HIT ||
                pAttackData->m_nAttackResult == ATTACK_RESULT_ATTACK_RESISTED)
            {
                pCreature->PlayHitSound(pAttackData, 0);
                if (pTarget->AsSWCCreature() != NULL)
                {
                    CSWCCreature* pTargetCreature = pTarget->AsSWCCreature();
                    if (pTargetCreature->IsCreature())
                    {
                        CSWSCreature* pServerCreature = pTargetCreature->GetServerCreature();
                        if (pServerCreature == NULL || !pServerCreature->IsDebilitated())
                        {
                            if (pAttackData->m_nAttackFlags != 0)
                            {
                                pTarget->GetAnimationBase()->SetOverlayAnimation(
                                    CREATURE_ANIMATION_NONMELEE_DAMAGE, 1);
                            }
                            else if (pTargetCreature->m_nCreatureType !=
                                     CSWCCreature::CREATURE_TYPE_UNLIMITED_WIELD)
                            {
                                pTarget->GetAnimationBase()->SetOverlayAnimation(
                                    CREATURE_ANIMATION_NONMELEE_DAMAGE, 1);
                            }
                        }
                    }
                }
                if (pTarget->AsSWCCreature() != NULL &&
                    pTarget->AsSWCCreature()->GetStats()->GetCurrentHitPoints() > 0 &&
                    pAttackData->m_nImpactX == 0)
                {
                    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                    if ((pPlayer != NULL && pTarget->GetId() == pPlayer->GetObjectId()) || rand() % 5 == 0)
                    {
                        unsigned short nSound = rand() % 2 ? SOUNDSET_PAIN_1 : SOUNDSET_PAIN_2;
                        pTarget->AsSWCCreature()->PlaySoundSetSound(nSound, 1);
                    }
                }
            }
            else if ((pAttackData->m_nAttackResult == ATTACK_RESULT_PARRIED &&
                      pTarget->AnimationParry(pTarget->GetCurrentAnimation())) ||
                     (pAttackData->m_nAttackResult == ATTACK_RESULT_MISS &&
                      pTarget->AnimationParry(pTarget->GetCurrentAnimation())))
            {
                pCreature->PlayHitSound(pAttackData, 0);
                int bAbortOnParry;
                {
                    CExoString sColumn("ABORTONPARRY");
                    unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
                    CTwoDimArrays* pArrays = g_pRules->Get2DArrays();
                    C2DA* pTable = pArrays->m_pAppearance;
                    pTable->GetINTEntry(nAppearance, sColumn, &bAbortOnParry);
                }
                if (bAbortOnParry)
                {
                    pCreature->BlurWeapon(0);
                    pCreature->AbortFireForgetAnimation(0);
                }
                if (pAttackData->m_pNextAttack)
                    pTarget->AbortFireForgetAnimation(0);
            }
            if (pAttackData->m_nAttackResult == ATTACK_RESULT_HIT_SUCCESSFUL ||
                pAttackData->m_nAttackResult == ATTACK_RESULT_CRITICAL_HIT ||
                pAttackData->m_nAttackResult == ATTACK_RESULT_AUTOMATIC_HIT ||
                pAttackData->m_nAttackResult == ATTACK_RESULT_ATTACK_RESISTED)
            {
                if (pTarget->AsSWCCreature() != NULL)
                {
                    CExoString sHook("impact");
                    pTarget->AsSWCCreature()->SpawnHitVisuals(
                        CSWCCreature::HIT_VISUAL_IMPACT, pAttackData,
                        pCreature->m_vPosition - pTarget->m_vPosition, sHook);
                }
                else if (pTarget->AsSWCDoor() != NULL && pTarget->m_pArea != NULL)
                {
                    Vector vPosition;
                    Quaternion qOrientation;
                    pTarget->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN)
                        ->GetPartPosition("lookathook", &vPosition, &qOrientation);
                    pTarget->m_pArea->ApplyFireAndForgetVisualEffect(VFX_COM_SPARKS_LARGE, vPosition);
                    pTarget->m_pArea->ApplyFireAndForgetVisualEffect(VFX_COM_SPARKS_PARRY_METAL, vPosition);
                }
            }
        }
    }
}
// PC 0x006146E0  Xbox 0x00044AB0
int CSWCCreature::LookAt(unsigned long oidTarget, float fTime)
{
    CExoString sTargetPart;
    CExoString sHeadBone;
    if (m_bCombatReady || m_oidLookAtTarget == oidTarget ||
        m_bPlayedOutOfPlace || GetAppearance() == NULL)
        return 0;

    Gob* pHead = GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 0);
    Gob* pBody = GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 0);
    if (pHead == NULL && pBody == NULL)
        return 0;

    int bHeadTrack;
    unsigned short nAppearance = m_pAppearance->m_nAppearance;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = p2DArrays->m_pAppearance;
    if (!pTable->GetINTEntry(nAppearance,
                             CTwoDimArrays::COL2DA_APPEARANCETABLE_HEADTRACK, &bHeadTrack))
        bHeadTrack = 0;
    if (bHeadTrack == 0 || oidTarget == OBJECT_INVALID)
    {
        m_oidLookAtTarget = OBJECT_INVALID;
        if (pHead != NULL)
            pHead->EndLookAtAnimate();
        if (pBody != NULL)
            pBody->EndLookAtAnimate();
        return 0;
    }

    CSWCObject* pTarget = g_pAppManager->GetClientExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        return 0;
    pTarget = pTarget->AsSWCObject();
    if (pTarget == NULL || pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 0) == NULL)
        return 0;
    Vector vDistance = pTarget->m_vPosition - m_vPosition;
    if (vDistance * vDistance > fTime * fTime)
        return 0;

    if (g_sHeadTurnBone != "")
        sHeadBone = g_sHeadTurnBone;
    else
    {
        nAppearance = m_pAppearance->m_nAppearance;
        p2DArrays = g_pRules->m_p2DArrays;
        pTable = p2DArrays->m_pAppearance;
        if (!pTable->GetCExoStringEntry(nAppearance,
                                        CTwoDimArrays::COL2DA_APPEARANCETABLE_HEADBONE, &sHeadBone))
            sHeadBone = "hturn_g";
    }

    float fHorizontal = GetHeadTurnHorizontal();
    float fVertical = GetHeadTurnVertical();
    if (pTarget->m_nObjectType != SW_OBJECT_TYPE_CREATURE)
        sTargetPart = pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetModelName();
    else
        sTargetPart = "CAMERAHOOK";

    m_oidLookAtTarget = oidTarget;
    if (pHead != NULL)
        pHead->BeginLookAtAnimate(pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
            sTargetPart.CStr(), sHeadBone.CStr(), fHorizontal, fVertical, 0.25f, fTime);
    if (pBody != NULL)
        pBody->BeginLookAtAnimate(pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
            sTargetPart.CStr(), sHeadBone.CStr(), fHorizontal, fVertical, 0.25f, fTime);
    return 1;
}
// PC 0x0060E2A0  Xbox 0x0003E450
float CSWCCreature::GetHeadTurnVertical()
{
    float fHeadArc;
    unsigned short nAppearance = m_pAppearance->m_nAppearance;
    C2DA* pTable = g_pRules->m_p2DArrays->m_pAppearance;
    if (pTable->GetFLOATEntry(
            nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_HEAD_ARC_V, &fHeadArc) == 0)
    {
        return 30.0f;
    }
    return fHeadArc;
}
// PC 0x0060E2F0  Xbox 0x0003E4F0
int CSWCCreature::ApplyShadowBlob()
{
    CExoString sShadowModel;
    if (m_pAnimBase == NULL)
    {
        return 0;
    }

    int nSizeCategory;
    unsigned short nAppearance = GetAppearance()->m_nAppearance;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = p2DArrays->m_pAppearance;
    if (!pTable->GetINTEntry(nAppearance,
                             CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY,
                             &nSizeCategory))
    {
        nSizeCategory = CREATURE_SIZE_MEDIUM;
    }

    switch (nSizeCategory)
    {
    case CREATURE_SIZE_TINY:
    case CREATURE_SIZE_SMALL:
        sShadowModel = "Fx_Shadow_S";
        break;
    case CREATURE_SIZE_LARGE:
    case CREATURE_SIZE_HUGE:
        sShadowModel = "Fx_Shadow_L";
        break;
    case CREATURE_SIZE_MEDIUM:
    default:
        sShadowModel = "Fx_Shadow_M";
        break;
    }

    int bResult = m_pAnimBase->ApplyShadowBlob(sShadowModel);
    m_pAnimBase->SetShadowBlobOrientation(
        RotationArc(Vector(0.0f, 0.0f, 1.0f), m_vGroundNormal));
    if (!m_bShadowsVisible)
    {
        HideShadowBlob();
    }
    return bResult;
}
// PC 0x0060E480  Xbox 0x0003E6B0
void CSWCCreature::RemoveShadowBlob()
{
    CSWCObject::RemoveShadowBlob();
}
// PC 0x0060E490  Xbox 0x0003E6C0
void CSWCCreature::TurnOnShadows()
{
    CSWCItem* pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        m_aoidEquippedItems[4]);
    if (pItem != NULL)
    {
        pItem->TurnOnShadows();
    }

    pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        m_aoidEquippedItems[5]);
    if (pItem != NULL)
    {
        pItem->TurnOnShadows();
    }

    pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        m_aoidEquippedItems[0]);
    if (pItem != NULL)
    {
        pItem->TurnOnShadows();
    }

    CSWCObject::TurnOnShadows();
}
// PC 0x0060E520  Xbox 0x0003E750
void CSWCCreature::TurnOffShadows()
{
    CSWCItem* pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        m_aoidEquippedItems[4]);
    if (pItem != NULL)
    {
        pItem->TurnOffShadows();
    }

    pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        m_aoidEquippedItems[5]);
    if (pItem != NULL)
    {
        pItem->TurnOffShadows();
    }

    pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
        m_aoidEquippedItems[0]);
    if (pItem != NULL)
    {
        pItem->TurnOffShadows();
    }

    CSWCObject::TurnOffShadows();
}
// PC 0x0060E5B0  Xbox 0x0003E7E0
void CSWCCreature::UpdateShadowBlob()
{
    unsigned char nShadowMode = g_pAppManager->m_pClientExoApp
        ->GetClientOptions()->m_nObjectShadowMode;
    if (nShadowMode == CClientOptions::OBJECT_SHADOWS_ALL)
    {
        RemoveShadowBlob();
    }
    else if (nShadowMode == CClientOptions::OBJECT_SHADOWS_PLAYER)
    {
        if (GetObjectId() == g_pAppManager->m_pClientExoApp->GetPlayerCreatureId())
        {
            RemoveShadowBlob();
            if (g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
                g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo != NULL)
            {
                g_pAppManager->m_pClientExoApp->GetModule();
            }
        }
        else
        {
            RemoveShadowBlob();
            ApplyShadowBlob();
        }
    }
    else
    {
        RemoveShadowBlob();
        ApplyShadowBlob();
    }

    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->SetShadowBlobOrientation(
            RotationArc(Vector(0.0f, 0.0f, 1.0f), m_vGroundNormal));
    }
}
// PC 0x0060E6C0  Xbox 0x0003E8F0
void CSWCCreature::ReAttachCamera()
{
    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
    if (m_nCameraFollowDelay == 0 &&
        this == g_pAppManager->m_pClientExoApp->GetPlayerCreature() &&
        m_bCameraFollowPending != 0 && pModule != NULL &&
        pModule->m_pModuleCamera != NULL &&
        pModule->m_pModuleCamera->GetBehavior(4205) == 0 &&
        pModule->m_pModuleCamera->GetBehavior(4206) == 0)
    {
        pModule->SetCameraFollow(m_nId, m_bCameraFollowSnap);
        m_bCameraFollowPending = 0;
        m_bCameraFollowSnap = 0;
    }
}
// PC 0x0060E760
void CSWCCreature::SetPopUpBubbleDecayTime(float fDecayTime)
{
}
// PC 0x0060E770  Xbox 0x0003E990
int CSWCCreature::Hilite(const Vector& vColor, unsigned char nModel,
                         float fSpeed, int bPulse)
{
    if (m_pAnimBase != NULL)
    {
        return m_pAnimBase->Hilite(vColor, nModel, bPulse);
    }
    return 0;
}
// PC 0x0060E7A0  Xbox 0x0003E9C0
int CSWCCreature::UnHilite(unsigned char nMode)
{
    CSWCAnimBase* pAnimationBase = m_pAnimBase;

    if (pAnimationBase != NULL)
    {
        return pAnimationBase->UnHilite(nMode);
    }
    else
    {
        return 0;
    }
}

int SceneHitCheck(CAurScene* pScene, Vector vStart, Vector vEnd,
                  Vector* pHit, CHitInfo* pHitInfo);
// PC 0x0060E7C0  Xbox 0x0003E9E0
int CSWCCreature::StopAmbientPauseAnim()
{
    m_nAmbientPauseTimer = 0;
    m_bAmbientPause = 0;
    return 0;
}

inline CSWCArea* CSWCObject::GetArea() { return m_pArea; }
// PC 0x0060E7D0  Xbox 0x0003E9F0
void CSWCCreature::EvaluateNewWayPoints(int nWayPointCount, Vector* pWayPoints,
                                       int nPathType, const Vector& vPosition)
{
    Vector vGroundNormal;
    int bPathFound = 0;
    int bDeleteWayPoints = 0;
    int nNewWayPointCount;
    Vector* pNewWayPoints;

    if (m_nUnknown214 == 0 || nPathType == 0 || m_nUnknown1EF == nPathType)
    {
        if (nWayPointCount > 0 && GetArea() != NULL)
        {
            float fDistance = SWCCREATURE_WAYPOINT_LINE_DISTANCE;
            Vector vStart = m_vPosition;
            Vector vEnd = pWayPoints[0];
            if (GetArea()->TestDirectLine(GetObjectId(), vStart, vEnd, fDistance,
                                      SWCCREATURE_WAYPOINT_LINE_WIDTH, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
            {
                pNewWayPoints = pWayPoints;
                bPathFound = 1;
                bDeleteWayPoints = 0;
                nNewWayPointCount = nWayPointCount;
            }
            else
            {
                vStart = m_vPosition;
                vEnd = vPosition;
                if (GetArea()->TestDirectLine(GetObjectId(), vStart, vEnd, fDistance,
                                          SWCCREATURE_WAYPOINT_LINE_WIDTH, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
                {
                    nNewWayPointCount = nWayPointCount + 1;
                    bPathFound = 1;
                    bDeleteWayPoints = 1;
                    pNewWayPoints = new Vector[nNewWayPointCount];
                    pNewWayPoints[0] = vPosition;
                    memcpy(pNewWayPoints + 1, pWayPoints, nWayPointCount * sizeof(Vector));
                }
            }

            int nWayPoint = 1;
            while (bPathFound == 0 && nWayPoint < nWayPointCount)
            {
                vStart = m_vPosition;
                vEnd = pWayPoints[nWayPoint];
                if (GetArea()->TestDirectLine(GetObjectId(), vStart, vEnd, fDistance,
                                          SWCCREATURE_WAYPOINT_LINE_WIDTH, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
                {
                    nNewWayPointCount = nWayPointCount - nWayPoint + 1;
                    bDeleteWayPoints = 1;
                    pNewWayPoints = new Vector[nNewWayPointCount];
                    memcpy(pNewWayPoints, pWayPoints + nWayPoint,
                           nNewWayPointCount * sizeof(Vector));
                    bPathFound = 1;
                }
                else
                    ++nWayPoint;
            }

            if (bPathFound == 0)
            {
                if (GetEndWayPoint() != NULL)
                {
                    Vector vOldEnd;
                    Vector vNewEnd;
                    Vector* pEndWayPoint = GetEndWayPoint();
                    if (pEndWayPoint != NULL)
                    {
                        vOldEnd = *pEndWayPoint;
                        vNewEnd = pWayPoints[nWayPointCount - 1];
                    }
                    vStart = vOldEnd;
                    vEnd = vNewEnd;
                    if (GetArea()->TestDirectLine(GetObjectId(), vStart, vEnd, fDistance,
                                              SWCCREATURE_WAYPOINT_LINE_WIDTH, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
                    {
                        bDeleteWayPoints = 1;
                        unsigned short nOldWayPointCount;
                        unsigned short nCurrentWayPoint;
                        Vector* pOldWayPoints;
                        GetWayPoints(&nOldWayPointCount, &pOldWayPoints, &nCurrentWayPoint);
                        nNewWayPointCount = nOldWayPointCount - nCurrentWayPoint + 1;
                        pNewWayPoints = new Vector[nNewWayPointCount];
                        memcpy(pNewWayPoints, pOldWayPoints + nCurrentWayPoint,
                               (nNewWayPointCount - 1) * sizeof(Vector));
                        pNewWayPoints[nNewWayPointCount - 1] = vEnd;
                        bPathFound = 1;
                    }
                }
                if (bPathFound == 0)
                {
                    Vector vWayPoint = pWayPoints[0];
                    bDeleteWayPoints = 0;
                    ComputeZValue(GetArea(), &vWayPoint, vWayPoint.m_fZ, &vGroundNormal);
                    pNewWayPoints = pWayPoints;
                    nNewWayPointCount = nWayPointCount;
                    SetPosition(vPosition);
                    m_vDesiredPosition = vPosition;
                    SetGroundNormal(vGroundNormal);
                }
            }
        }

        if (g_pAppManager->GetClientExoApp()->GetCaptureEvents() == 1)
        {
            CExoString sEvent;
            sEvent.Format("%d %c %x %f %f %f\n",
                (unsigned int)(g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetSnapshotTime() /
                               WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND),
                CPlayBackEventNode::POSITION, GetObjectId(), m_vPosition.m_fX, m_vPosition.m_fY, m_vPosition.m_fZ);
            g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sEvent);
        }
        SetWayPoints(nNewWayPointCount, pNewWayPoints);
        m_nSpeedRampMode = SPEED_RAMP_FAST;
        if (bDeleteWayPoints == 1 && pNewWayPoints != NULL)
            delete [] pNewWayPoints;
    }
}
// PC 0x0060ED80  Xbox 0x0003EFA0
void CSWCCreature::SetThrownLightsaber(unsigned long a1)
{
    this->m_oidThrownLightsaber = a1;
}
// PC 0x00614A30  Xbox 0x00044E60
void CSWCCreature::ForceAppearanceUpdate()
{
    CSWCModule* pModule;
    CSWCArea* pArea;

    m_nCameraFollowDelay = 0;
    pModule = g_pAppManager->m_pClientExoApp->GetModule();
    m_pAppearance->m_nAppearance = m_nAppearance;
    SetAppearance((CCAppearanceInfo*)m_aUnmapped31C, 0xEF, 1);
    if (pModule != NULL)
    {
        pArea = pModule->m_pInfo;
        if (pArea != NULL)
            AttachToScene(pArea->m_pScene);
    }
}
// PC 0x00614AA0  Xbox 0x00044ED0
int CSWCCreature::GetCreatureLight(int bRequireEnabled)
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();

    while (pPosition != NULL)
    {
        if (pEffect != NULL &&
            pEffect->GetHardCodedEffectClass() ==
                CSWCVisualEffectOnObject::HARDCODED_EFFECT_LIGHT &&
            pEffect->m_bVisualEffectDone == 0 &&
            pEffect->m_bLightSourceApplied != 0 &&
            (bRequireEnabled == 0 || pEffect->m_bLightSourceDynamic != 0))
        {
            return reinterpret_cast<int>(pEffect->m_pEffectModel);
        }
        pEffect = m_lstVisualEffects.GetNext(pPosition);
    }
    return 0;
}
// PC 0x00614B20  Xbox 0x00044F50
void CSWCCreature::SetArea(CSWCArea* pArea)
{
    CSWCObject::SetArea(pArea);
    UpdateAreaId();
}
// PC 0x0060ED90  Xbox 0x0003EFB0
void CSWCCreature::UpdateAreaId()
{
    if (m_pArea != NULL)
    {
        m_oidArea = m_pArea->m_nId;
    }
    else
    {
        m_oidArea = CGameObjectArray::INVALIDOBJID;
    }
}
// PC 0x0060EDC0  Xbox 0x0003EFE0
void CSWCCreature::UpdatePopUpBubble()
{
    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
    if (pModule != NULL && pModule->m_pInfo != NULL)
    {
        if (m_oidArea == pModule->m_pInfo->m_nId)
        {
            CSWCObject::UpdatePopUpBubble();
        }
        else
        {
            HidePopUpBubble();
        }
    }
}
// PC 0x0060EE00  Xbox 0x0003F020
void CSWCCreature::PerformSkill(unsigned char nSkill, unsigned char nUseType,
                                unsigned long oidTarget)
{
    if (oidTarget != CGameObjectArray::INVALIDOBJID)
    {
        CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();

        if (pMessage != NULL)
        {
            Vector vTarget;

            pMessage->SendPlayerToServerInput_UseSkill(nSkill, nUseType,
                                                       oidTarget, vTarget);
        }
    }
}
// PC 0x0060EE60  Xbox 0x0003F080
void CSWCCreature::PerformUseItem(unsigned long oidItem, int nPropertyIndex,
                                  unsigned long oidTarget,
                                  const Vector& vTarget)
{
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();

    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_UseItem(oidItem, nPropertyIndex,
                                                  nPropertyIndex >> 16,
                                                  oidTarget, vTarget);
    }
}
// PC 0x0060EEA0  Xbox 0x0003F0C0
void CSWCCreature::PerformGiveItem(unsigned long oidItem,
                                   unsigned long oidTarget, int nStack)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();

    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_GiveItem(m_nId, oidItem, oidTarget,
                                                   nStack);
    }
}
// PC 0x0060EEE0  Xbox 0x0003F0F0
void CSWCCreature::PerformAttack(unsigned long oidTarget)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();

    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_Attack(oidTarget, 0);
    }
}
// PC 0x0060EF10  Xbox 0x0003F120
void CSWCCreature::PerformCastSpell(unsigned long nSpell,
                                    unsigned long oidTarget,
                                    const Vector& vTarget,
                                    unsigned char nMultiClass,
                                    unsigned char nDomainLevel,
                                    unsigned char nMetaMagic, int bCheat)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_CastSpell(nSpell, nMultiClass,
                                                    nDomainLevel, nMetaMagic,
                                                    bCheat, vTarget, oidTarget);
    }
}

#include "nwcplaceable.h"

#include "nwsplaceable.h"

#include "clientoptions.h"

#include "swbehaviorcamera.h"

#include "nwitem.h"
// PC 0x0060EF50  Xbox 0x0003F160
int CSWCCreature::SetInParty(int bInParty)
{
    if (m_bInParty != bInParty)
    {
        m_bInParty = bInParty;
        if (bInParty != 0)
        {
            unsigned long oidCreature = m_nId;
            return g_pAppManager->m_pClientExoApp->GetSWParty()->AddCharacter(
                oidCreature);
        }
        if (g_pAppManager->m_pClientExoApp->IsPlayerCreature(this) != 0)
        {
            g_pAppManager->m_pClientExoApp->GetSWParty()->SetLeader(-3);
        }
        unsigned long oidCreature = m_nId;
        return g_pAppManager->m_pClientExoApp->GetSWParty()->RemoveCharacter(
            oidCreature);
    }
    return 1;
}
// PC 0x0060EFE0  Xbox 0x0003F1F0
CAurObject* CSWCCreature::GetCameraFollowPart()
{
    return m_pAppearance->GetCameraFollowPart();
}
// PC 0x0060EFF0  Xbox 0x0003F210
int CSWCCreature::GetFollowingBody()
{
    return m_pAppearance->GetFollowingBody();
}
// PC 0x0060F000
float CSWCCreature::GetHeadHeightOffset()
{
    return m_pAppearance->GetHeadHeightOffset();
}
// PC 0x0060F010  Xbox 0x0003F220
float CSWCCreature::GetCameraOffset()
{
    return m_pAppearance->GetCameraOffset();
}
// PC 0x0060F020  Xbox 0x0003F230
int CSWCCreature::GetPauseReadyAnimation()
{
    CServerExoApp* pServer;

    if (m_nCombatState != 0)
        return CREATURE_ANIMATION_COMBAT_READY;

    pServer = g_pAppManager->m_pServerExoApp;
    if (pServer == NULL)
        return CREATURE_ANIMATION_COMBAT_IDLE;

    unsigned long oidObject = m_nId;
    if (pServer->IsInjured(oidObject))
    {
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode !=
                CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
            this != g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0))
        {
            return CREATURE_ANIMATION_INJURED_IDLE;
        }
    }

    return CREATURE_ANIMATION_COMBAT_IDLE;
}
// PC 0x0060F090  Xbox 0x0003F2C0
void CSWCCreature::SwitchMode(int nMode)
{
    switch (nMode)
    {
    case 0:
        m_nMode = 0;
        break;
    case 1:
        m_nMode = 1;
        break;
    case 2:
        m_nMode = 2;
        break;
    }
}
// PC 0x0060F0D0  Xbox 0x0003F300
void CSWCCreature::SetSWAnimationSpeed(float fSpeed, float fTargetSpeed)
{
    EXOXBOXASSERT(_finite(fTargetSpeed));
    EXOXBOXASSERT(_finite(fSpeed));
    m_fSWTargetAnimationSpeed = fTargetSpeed;
    m_fSWAnimationSpeed = fSpeed;
}
// PC 0x0060F0F0  Xbox 0x0003F360
int CSWCCreature::IsDriveAnimationBlocked()
{
    int nEngagedState = m_nEngagedState;
    unsigned long oidServer =
        g_pAppManager->m_pServerExoApp->ClientToServerObjectId(m_nId);

    if (g_pAppManager->m_pServerExoApp->IsActionsPending(oidServer))
    {
        nEngagedState = 1;
    }
    if (AnimationDialog(m_nCurrentAnimation))
    {
        return 1;
    }
    return nEngagedState;
}

void Swing(const CExoString& sSound, CSWCCombatAttackData* pAttackData,
           int nSoundCount)
{
    if (pAttackData != NULL)
    {
        CSWCCreature* pCreature =
            g_pAppManager->m_pClientExoApp->GetCreatureByGameObjectID(
                pAttackData->m_oidCreature);
        if (pCreature != NULL)
        {
            pCreature->PlaySwingSound(sSound, pAttackData, nSoundCount);
        }
    }
}
// PC 0x0060F140  Xbox 0x0003F3B0
int CSWCCreature::SetEngagedState(int a1)
{
    this->m_nEngagedState = a1;
    return a1;
}
// PC 0x0060F150  Xbox 0x0003F3C0
void CSWCCreature::SetQuededAnimationCallback()
{
    if (m_pQueuedAnimationCallback == NULL)
    {
        Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        m_pQueuedAnimationCallback = pModel->RegisterCallBack(
            QueuedCallbackFunction, "*", AsSWCObject(), 9999.0f, false);
    }
}
// PC 0x0060F1A0  Xbox 0x0003F410
int CSWCCreature::ClearQuededAnimationCallback()
{
    Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    if (m_pQueuedAnimationCallback == NULL || pModel == NULL)
    {
        return 0;
    }

    pModel->DeregisterCallback(m_pQueuedAnimationCallback);
    m_pQueuedAnimationCallback = NULL;
    return 1;
}
// PC 0x0060F1E0  Xbox 0x0003F450
int SceneHitCheck(CAurScene* pScene, Vector vStart, Vector vEnd,
                  Vector* pHit, CHitInfo* pHitInfo)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWRoomSurfaceMeshHitInfo hitInfo(
        SWROOMSURFACEMESH_ALL_MATERIALS_MASK);
    int bHit = 0;

    if (pServer != NULL)
    {
        CSWSModule* pModule = pServer->GetModule();
        if (pModule != NULL)
        {
            CSWSArea* pArea = pModule->GetArea();
            if (pArea != NULL)
            {
                for (int nRoom = 0; pArea->GetRoom(nRoom) != NULL; ++nRoom)
                {
                    hitInfo.ClearOutput();
                    if (pArea->GetRoom(nRoom)->CheckAABBAll(vStart, vEnd, hitInfo))
                    {
                        vEnd = hitInfo.m_vHitPosition;
                        bHit = 1;
                    }
                }
            }
        }
    }

    int bStaticBSPParts = hitcheck_static_bsp_parts;
    hitcheck_static_bsp_parts = 0;
    int nSceneHit = pScene->HitCheck(vStart, vEnd, NULL, pHitInfo);
    hitcheck_static_bsp_parts = bStaticBSPParts;
    if (nSceneHit)
        return nSceneHit;
    return bHit;
}
// PC 0x0060F330  Xbox 0x0003F5A0
void CSWCCreature::ForceMenuAction(unsigned long nAction,
                                  CSWCCreature* pTarget)
{
    nAction &= ~CSWGuiInterfaceAction::SPELL_ACTION_MASK;
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();
    if (pMessage != NULL)
    {
        CSWCCreatureStats* pStats = pTarget->GetStats();
        unsigned char nClass;
        for (nClass = 0; nClass < 2; ++nClass)
        {
            if (pStats->GetClass(nClass) == CLASS_TYPE_JEDIGUARDIAN ||
                pStats->GetClass(nClass) == CLASS_TYPE_JEDICONSULAR ||
                pStats->GetClass(nClass) == CLASS_TYPE_JEDISENTINEL)
                break;
        }
        if (nClass < 2)
        {
            int nFORCEHOSTILE;
            int bFound;
            {
                CExoString sColumn("FORCEHOSTILE");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                EXOXBOXASSERT(p2DArrays->m_pSpells);
                C2DA* pSpells = p2DArrays->m_pSpells;
                bFound = pSpells->GetINTEntry(
                    nAction, sColumn, &nFORCEHOSTILE);
            }

            int nTutorialResult;
            if (bFound)
                nTutorialResult = g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
                    CClientExoApp::TUTORIAL_HOSTILE_FORCE_POWER, GetObjectId(), nClass, nAction);
            else
                nTutorialResult = g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
                    CClientExoApp::TUTORIAL_FORCE_POWER, GetObjectId(), nClass, nAction);
            if (!nTutorialResult)
            {
                Vector vTarget;
                PerformCastSpell(nAction, GetObjectId(), vTarget, nClass, 0, 0, 0);
            }
        }
        else if (pStats->m_nRace == CSWRace::CRACE_DROID)
        {
            pMessage->SendPlayerToServerInput_UseItem(nAction, 0, 0,
                                                     GetObjectId(), m_vPosition);
        }
    }
}
// PC 0x0060F4B0  Xbox 0x0003F750
void CSWCCreature::PartyMenuStealth(unsigned long oidTarget,
                                    CSWCCreature* pTarget)
{
    if (g_pAppManager->m_pServerExoApp->GetPartyTable()->m_bSoloMode != 0 ||
        g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters == 1)
    {
        PerformSkill(2, 0, m_nId);
    }
    else
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowSoloModeQuery(1);
    }
}
// PC 0x0060F510  Xbox 0x0003F7F0
int CSWCCreature::BarkString(unsigned long nStrRef, const CExoString& sText,
                             const CResRef& cSound, int bFlag)
{
    LipSync(cSound);
    return BarkBubble(nStrRef, sText, cSound, bFlag);
}
// PC 0x00614B60  Xbox 0x0003F2A0
int CSWCCreature::IsInjured()
{
    return GetServerCreature()->IsInjured();
}

inline CSWRoomSurfaceMesh* CSWSRoom::GetWalkMesh() { return m_pWalkMesh; }
// PC 0x00614B90  Xbox 0x00044F90
int CSWCCreature::SetPrecalculatedPosition(const Vector& vPosition)
{
    if (RenderClosestPathPoint)
    {
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->m_pPathfindInformation =
            GetServerCreature()->GetPathfindInformation();
        unsigned long nPathPoint;
        if (g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->ClosestPathPoint(
                m_vPosition, nPathPoint))
        {
            unsigned long nPoint = nPathPoint;
            CSWSArea* pArea = g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
            Vector vPoint(pArea->m_pPathPoints[nPoint].m_fX,
                          pArea->m_pPathPoints[nPoint].m_fY, 0.0f);
            g_pAppManager->GetServerExoApp()->ComputeHeight(
                g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                vPoint, 0);
        }
    }

    if (RenderTriangles)
    {
        int nRoom = GetServerCreature()->GetArea()->GetRoomIndex(vPosition, 0);
        CSWSRoom* pRoom = GetServerCreature()->GetArea()->GetRoom(nRoom);
        Vector vFirst, vSecond, vThird;
        int nFirst, nSecond, nThird;
        if (pRoom != NULL)
        {
            CSWRoomSurfaceMesh* pMesh = pRoom->GetWalkMesh();
            int nTriangle = pMesh->FindTriangle(
                vPosition, 1, Vector(1.0f, 1.0f, 0.0f));
            pRoom->GetWalkMesh()->GetTriangleVertices(0, nTriangle,
                                                   &nFirst, &nSecond, &nThird);
            pRoom->GetWalkMesh()->GetVertex(nFirst, &vFirst);
            pRoom->GetWalkMesh()->GetVertex(nSecond, &vSecond);
            pRoom->GetWalkMesh()->GetVertex(nThird, &vThird);
        }
        Vector vStart = GetServerCreature()->m_vPosition;
        vStart.m_fZ += SWCCREATURE_TRIANGLE_RAY_HEIGHT;
        Vector vEnd = vStart + GetServerCreature()->m_vOrientation * 2.0f;
        Vector vHit;
        GetServerCreature()->GetArea()->ClearLineOfSight(vStart, vEnd, &vHit,
            NULL, OBJECT_INVALID, OBJECT_INVALID, 0, 0);
        if (pRoom != NULL)
        {
            CSWRoomSurfaceMeshHitInfo cMeshHit(SWROOMSURFACEMESH_ALL_MATERIALS_MASK);
            pRoom->GetWalkMesh()->CheckAABBAll(vStart, vEnd, cMeshHit);
            if (cMeshHit.m_nField0C != -1)
            {
                pRoom->GetWalkMesh()->GetTriangleVertices(0, cMeshHit.m_nField0C,
                                                       &nFirst, &nSecond, &nThird);
                pRoom->GetWalkMesh()->GetVertex(nFirst, &vFirst);
                pRoom->GetWalkMesh()->GetVertex(nSecond, &vSecond);
                pRoom->GetWalkMesh()->GetVertex(nThird, &vThird);
            }
        }
    }

    CSWSCreature* pServerCreature = GetServerCreature();
    if (pServerCreature != NULL)
    {
        if (pServerCreature->IsDebilitated())
            return 1;
        if (!(pServerCreature->m_nAIStateFlags & CSWSCreature::AI_STATE_CAN_MOVE))
            return 1;
    }
    if (IsDriveAnimationBlocked())
        return 1;

    float fDistance = GetServerCreature()->GetPathfindInformation()->m_fStepScale;
    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    Vector vDirection = normalize(vPosition - m_vPosition);
    int nResult = CSWSArea::DIRECT_LINE_BLOCKED_ROOM;
    if (m_nMode != MODE_SWDRIVE)
        return 1;
    if (pModule != NULL && pModule->m_pInfo != NULL)
    {
        int nTries = 0;
        CWalkHitInfo cHit;
        static unsigned int WT[WALK_TRY_SQUARE_COUNT] =
            {-1, -1, -1, -1, -1, -1, -1, -1};
        if (renderwalktry && pModule != NULL)
        {
            for (int i = 0; i < WALK_TRY_SQUARE_COUNT; i++)
            {
                if (WT[i] != (unsigned int)-1)
                {
                    AurRemoveDrawableSquare(WT[i]);
                    WT[i] = -1;
                }
            }
        }

        Vector vPreviousNormal;
        Vector vReverse = normalize(m_vPosition - vPosition);
        CPathfindInformation* pPathfind =
            g_pAppManager->GetServerExoApp()->GetCreaturePathfindInformation(GetObjectId());
        if (pPathfind != NULL)
            pPathfind->m_oidPathfindTargetCreature = OBJECT_INVALID;
        Vector vNewPosition = vPosition;
        do
        {
            ++nTries;
            if (nResult != CSWSArea::DIRECT_LINE_BLOCKED_ROOM &&
                nResult != CSWSArea::DIRECT_LINE_BLOCKED_OBJECT &&
                nResult != CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
                break;
            if (renderwalktry)
            {
                Vector vSquare[4] =
                    {m_vPosition, m_vPosition, vNewPosition, vNewPosition};
                WT[nTries] = AurAddDrawableSquare((float*)vSquare,
                    nTries * SWCCREATURE_WALK_TRY_COLOR_STEP | SWCCREATURE_WALK_TRY_COLOR_MASK);
            }
            nResult = pModule->GetArea()->TestDirectLine(GetObjectId(), m_vPosition,
                vNewPosition, fDistance, pPathfind->GetHalfStep(), 1, &cHit);
            if (nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
            {
                CSWSCreature* pBlocker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                    cHit.m_oidBlocker);
                if (pBlocker != NULL)
                {
                    pServerCreature->BumpFriends(pBlocker, m_vPosition, vNewPosition);
                    nResult = pModule->GetArea()->TestDirectLine(GetObjectId(), m_vPosition,
                        vNewPosition, fDistance, pPathfind->GetHalfStep(), 1, &cHit);
                    if (nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE &&
                        !GetServerCreature()->GetArea()->TestSafeLocationPoint(m_vPosition, pPathfind))
                    {
                        GetServerCreature()->GetArea()->ComputeSafeLocation(
                            m_vPosition, 1.0f, pPathfind, 1, &m_vPosition);
                        nResult = pModule->GetArea()->TestDirectLine(GetObjectId(), m_vPosition,
                            vNewPosition, fDistance, pPathfind->GetHalfStep(), 1, &cHit);
                    }
                }
            }
            if ((nResult == CSWSArea::DIRECT_LINE_BLOCKED_ROOM ||
                 nResult == CSWSArea::DIRECT_LINE_BLOCKED_OBJECT ||
                 nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE) &&
                cHit.m_nUnknown00 == CWalkHitInfo::FAILURE_BLOCKED)
            {
                float fSide = vReverse * cross(zaxis, cHit.m_vUnknown10);
                float fPreviousSide = vReverse * cross(zaxis, vPreviousNormal);
                if (nTries > 1 && ((fSide > 0.0f && fPreviousSide < 0.0f) ||
                                  (fSide < 0.0f && fPreviousSide > 0.0f)))
                {
                    if (cHit.m_nUnknown44 != CWalkHitInfo::INTERSECTION_CORNER)
                        break;
                    Vector vOffset = cross(zaxis, vDirection) * SWCCREATURE_DRIVE_REVERSE_CORNER_OFFSET;
                    if (vDirection * Vector(-cHit.m_vUnknown34.m_fY,
                                              cHit.m_vUnknown34.m_fX, 0.0f) > 0.0f)
                        vNewPosition = m_vPosition + vOffset;
                    else
                        vNewPosition = m_vPosition - vOffset;
                }
                else
                {
                    Vector vAdjustedPosition;
                    if (cHit.m_nUnknown44 == CWalkHitInfo::INTERSECTION_CORNER)
                    {
                        Vector vOffset = cross(zaxis, vDirection) * SWCCREATURE_DRIVE_CORNER_OFFSET;
                        if (vDirection * Vector(-cHit.m_vUnknown10.m_fY,
                                                  cHit.m_vUnknown10.m_fX, 0.0f) > 0.0f)
                            vAdjustedPosition = m_vPosition - vOffset;
                        else
                            vAdjustedPosition = m_vPosition + vOffset;
                    }
                    else
                    {
                        Vector vToHit = cHit.m_vHitPosition - m_vPosition;
                        Vector vToDestination = vNewPosition - m_vPosition;
                        float fHitDistance = vToHit * vToHit;
                        float fDestinationDistance = vToDestination * vToDestination;
                        if (fHitDistance > fDestinationDistance)
                            vAdjustedPosition = vNewPosition - cHit.m_vUnknown10 *
                                ((cHit.m_vHitPosition - vNewPosition) * cHit.m_vUnknown10);
                        else
                            vAdjustedPosition = vNewPosition - cHit.m_vUnknown10 *
                                ((vNewPosition - cHit.m_vHitPosition) * cHit.m_vUnknown10);
                    }
                    vNewPosition = vAdjustedPosition;
                }
                vPreviousNormal = cHit.m_vUnknown10;
                if (renderintersects == 1)
                {
                    char sDebug[256];
                    sprintf(sDebug, "V1:%3.2f,%3.2f  V2:%3.2f,%3.2f IT:%d FR:%d PF:%d WT:%d",
                        cHit.m_vUnknown1C.m_fX, cHit.m_vUnknown1C.m_fY,
                        cHit.m_vUnknown28.m_fX, cHit.m_vUnknown28.m_fY,
                        cHit.m_nUnknown44, cHit.m_nUnknown00, nResult, nTries);
                }
            }
        } while (nTries <= DRIVE_WALK_LAST_TRY);

        if (nResult == CSWSArea::DIRECT_LINE_CLEAR)
        {
            ComputeZValue(pModule->m_pInfo, &vNewPosition, vNewPosition.m_fZ, NULL);
            g_pAppManager->GetServerExoApp()->UpdateSubareasOnMoveTo(m_nId,
                m_vPosition, vNewPosition);
            SetPosition(vNewPosition);
            m_vDesiredPosition = vNewPosition;
            g_pAppManager->GetServerExoApp()->UpDatePlayerPosition(m_nId,
                vNewPosition, m_vOrientation);
            CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
            pParty->m_cFollowPaths.ProcessPoint(vNewPosition, Yaw(m_vOrientation), fDistance);
            return 1;
        }
        return 0;
    }
    return 1;
}
// PC 0x00615960  Xbox 0x00045E40
void CSWCCreature::DriveAnimationUpdate()
{
    if (AnimationDialog(m_nCurrentAnimation))
        return;

    if (g_pAppManager->GetServerExoApp() != NULL)
    {
        CSWSCreature* pCreature = GetServerCreature();
        if (pCreature != NULL &&
            (pCreature->GetDead() || pCreature->GetIsPCDying() ||
             pCreature->GetDeadTemp() || pCreature->IsDebilitated() ||
             !(pCreature->m_nAIStateFlags & CSWSCreature::AI_STATE_CAN_MOVE)))
            return;
    }

    if (!AnimationStationary(m_nCurrentAnimation) && IsDriveAnimationBlocked())
    {
        m_pAnimBase->SetLean(0.0f);
        return;
    }

    float fDriveSpeed = m_pAppearance->m_fDriveMaxSpeed;
    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    if (m_bRunning && m_fSWAnimationSpeed > m_pAppearance->m_fDriveAnimWalk)
        m_fSWAnimationSpeed = m_pAppearance->m_fDriveAnimWalk;

    if (m_fSWAnimationSpeed > 0.0f && m_fSWTargetAnimationSpeed > 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_DRIVE_DIAGONAL;
        m_fAnimationSpeed = m_fSWTargetAnimationSpeed / fDriveSpeed;
        SetAnimation(CREATURE_ANIMATION_DRIVE_DIAGONAL, 0, 0);
    }
    else if (m_fSWAnimationSpeed > 0.0f && m_fSWTargetAnimationSpeed < 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_DRIVE_DIAGONAL_REVERSE;
        float fReverseSpeed = -1.0f / fDriveSpeed;
        m_fAnimationSpeed = fReverseSpeed * m_fSWTargetAnimationSpeed;
        SetAnimation(CREATURE_ANIMATION_DRIVE_DIAGONAL_REVERSE, 0, 0);
    }
    else if (m_fSWAnimationSpeed > 0.0f && m_fSWTargetAnimationSpeed == 0.0f)
    {
        int nMovement = 0;
        int nAnimation = GetPauseReadyAnimation();
        float fAnimationSpeed;
        float fWalkThreshold = DriveWalkThreshold * fDriveSpeed;
        int bTransition = 0;
        if (m_fSWAnimationSpeed > fWalkThreshold)
            nMovement = 1;

        switch (nMovement)
        {
        case 1:
            if (m_bInParty && IsInjured() &&
                (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
                     CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
                 this != g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)))
                nAnimation = CREATURE_ANIMATION_INJURED_RUN;
            else
                nAnimation = m_bRunning ? CREATURE_ANIMATION_SPECIAL_WALK : CREATURE_ANIMATION_RUN;
            m_bDriveRunning = 1;
            fDriveSpeed = m_pAppearance->m_fDriveAnimRun;
            fAnimationSpeed = m_fSWAnimationSpeed /
                (fDriveSpeed / (GetAnimationLength(nAnimation) * 0.001f));
            bTransition = 1;
            break;
        case 0:
            if (m_bInParty && IsInjured() &&
                (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
                     CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
                 this != g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)))
                nAnimation = CREATURE_ANIMATION_INJURED_WALK;
            else
                nAnimation = m_bRunning ? CREATURE_ANIMATION_SPECIAL_WALK : CREATURE_ANIMATION_WALK;
            m_bDriveRunning = 0;
            fDriveSpeed = m_pAppearance->m_fDriveAnimWalk;
            fAnimationSpeed = m_fSWAnimationSpeed /
                (fDriveSpeed / (GetAnimationLength(nAnimation) * 0.001f));
            bTransition = 1;
            break;
        }

        if (m_nFollowAnimation == CREATURE_ANIMATION_DRIVE_NO_TRANSITION)
            bTransition = 0;
        m_fAnimationSpeed = fAnimationSpeed;
        m_nFollowAnimation = nAnimation;
        SetAnimation(nAnimation, bTransition, 0);
    }
    else if (m_fSWAnimationSpeed == 0.0f && m_fSWTargetAnimationSpeed < 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_DRIVE_SIDEWAYS_REVERSE;
        float fReverseSpeed = -1.0f / fDriveSpeed;
        m_fAnimationSpeed = fReverseSpeed * m_fSWTargetAnimationSpeed;
        SetAnimation(CREATURE_ANIMATION_DRIVE_SIDEWAYS_REVERSE, 0, 0);
    }
    else if (m_fSWAnimationSpeed == 0.0f && m_fSWTargetAnimationSpeed > 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_DRIVE_SIDEWAYS;
        m_fAnimationSpeed = m_fSWTargetAnimationSpeed / fDriveSpeed;
        SetAnimation(CREATURE_ANIMATION_DRIVE_SIDEWAYS, 0, 0);
    }
    else if (m_fSWAnimationSpeed < 0.0f && m_fSWTargetAnimationSpeed > 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_DRIVE_DIAGONAL_REVERSE;
        float fReverseSpeed = -1.0f / fDriveSpeed;
        m_fAnimationSpeed = fReverseSpeed * m_fSWTargetAnimationSpeed;
        SetAnimation(CREATURE_ANIMATION_DRIVE_DIAGONAL_REVERSE, 0, 0);
    }
    else if (m_fSWAnimationSpeed < 0.0f && m_fSWTargetAnimationSpeed < 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_DRIVE_DIAGONAL;
        m_fAnimationSpeed = m_fSWTargetAnimationSpeed / fDriveSpeed;
        SetAnimation(CREATURE_ANIMATION_DRIVE_DIAGONAL, 0, 0);
    }
    else if (m_fSWAnimationSpeed < 0.0f && m_fSWTargetAnimationSpeed == 0.0f)
    {
        m_nFollowAnimation = CREATURE_ANIMATION_RUN;
        m_fAnimationSpeed = m_fSWAnimationSpeed / fDriveSpeed;
        SetAnimation(CREATURE_ANIMATION_RUN, 0, 0);
    }
    else
    {
        m_fAnimationSpeed = 1.0f;
        unsigned short nAnimation = GetPauseReadyAnimation();
        if (nAnimation == CREATURE_ANIMATION_COMBAT_READY ||
            nAnimation == CREATURE_ANIMATION_INJURED_IDLE ||
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode ==
                CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
            (nAnimation == CREATURE_ANIMATION_COMBAT_IDLE &&
             !AnimationPause(m_nCurrentAnimation)))
        {
            m_nFollowAnimation = nAnimation;
            SetAnimation(m_nFollowAnimation, 0, 0);
        }
    }
}
// PC 0x00615E80  Xbox 0x00046390
void CSWCCreature::FollowAnimationUpdate()
{
    if (GetServerCreature() != NULL &&
        (GetServerCreature()->GetDeadTemp() ||
         GetServerCreature()->GetDead() ||
         GetServerCreature()->IsDebilitated()))
        return;

    unsigned long oidCreature = m_nId;
    int bSetAnimation = 1;
    int nIndex = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(oidCreature);
    if (nIndex == 1 || nIndex == 2)
    {
#ifdef _XBOX
        CSWParty* pXboxParty = g_pAppManager->GetClientExoApp()->GetSWParty();
        EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
        CSWPartyMemberData* pFollowData = &pXboxParty->m_aPartyMemberData[nIndex];
#else
        CSWPartyMemberData* pFollowData =
            &g_pAppManager->GetClientExoApp()->GetSWParty()->m_aPartyMemberData[nIndex];
#endif
        float fSpeed = pFollowData->m_fSpeed;
        float fWalkThreshold = m_fDriveSpeed;
        fWalkThreshold *= DriveWalkThreshold;
        float fAnimationSpeed = 0.0f;
        float fRunDistance;
        float fWalkDistance;
        fWalkDistance = m_pAppearance->m_fDriveAnimWalk * 1000.0f;
        fRunDistance = m_pAppearance->m_fDriveAnimRun * 1000.0f;
        int nAnimation;

        if (fSpeed <= SWCCREATURE_FOLLOW_SPEED_EPSILON)
        {
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState &&
                AnimationDialog(m_nCurrentAnimation))
            {

                bSetAnimation = 0;
            }
            else
            {
                nAnimation = GetPauseReadyAnimation();
                fAnimationSpeed = 1.0f;
            }
        }
        else
        {
            float fAnimationMoveSpeed;
            if (fSpeed >= fWalkThreshold)
            {
                if (m_bInParty && IsInjured() &&
                    (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
                         CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
                     this != g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)))
                    nAnimation = CREATURE_ANIMATION_INJURED_RUN;
                else
                    nAnimation = m_bRunning ? CREATURE_ANIMATION_SPECIAL_WALK : CREATURE_ANIMATION_RUN;
                fAnimationMoveSpeed = fRunDistance / (GetAnimationLength(nAnimation) * 0.001f);
            }
            else
            {
                if (m_bInParty && IsInjured() &&
                    (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
                         CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
                     this != g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)))
                    nAnimation = CREATURE_ANIMATION_INJURED_WALK;
                else
                    nAnimation = m_bRunning ? CREATURE_ANIMATION_SPECIAL_WALK : CREATURE_ANIMATION_WALK;
                fAnimationMoveSpeed = fWalkDistance / (GetAnimationLength(nAnimation) * 0.001f);
            }
            fAnimationSpeed = fSpeed / fAnimationMoveSpeed;
        }

        m_fAnimationSpeed = fAnimationSpeed;
        m_nFollowAnimation = nAnimation;
        if (bSetAnimation)
            SetAnimation(nAnimation, 0, 0);
        RotateCreatureToOrientation(pFollowData->m_vFacing);
    }
}
// PC 0x00616140  Xbox 0x00046690
int CSWCCreature::CheckFeat(int nFeat1, int nFeat2, int nFeat3)
{
    int nFeat = -1;
    CSWSCreature* pServerCreature = GetServerCreature();
    if (pServerCreature != NULL && pServerCreature->m_pStats != NULL)
    {
        if (pServerCreature->m_pStats->HasFeat(nFeat1))
            nFeat = nFeat1;
        else if (pServerCreature->m_pStats->HasFeat(nFeat2))
            nFeat = nFeat2;
        else
        {
            CSWSCreatureStats* pStats = pServerCreature->m_pStats;
            if (pStats->HasFeat(nFeat3))
                nFeat = nFeat3;
        }
    }
    else
    {
        if (m_pStats->HasFeat(nFeat1))
            nFeat = nFeat1;
        else if (m_pStats->HasFeat(nFeat2))
            nFeat = nFeat2;
        else if (m_pStats->HasFeat(nFeat3))
            nFeat = nFeat3;
    }
    return nFeat;
}
// PC 0x00616200  Xbox 0x00046750
void CSWCCreature::ForceMenuHostileAction(unsigned long nAction,
                                         CSWCCreature* pTarget)
{
    if (pTarget != NULL)
    {
        pTarget->SetCombatMode(1);
    }
    ForceMenuAction(nAction, pTarget);
}
// PC 0x00616230  Xbox 0x00046780
void CSWCCreature::GetSelfForcePowers(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    unsigned long nForcePoints = 0;
    unsigned long nItemMask = 0;
    CSWSCreature* pServerCreature = GetServerCreature();
    if (pServerCreature != NULL)
    {
        nItemMask = pServerCreature->GetForceItemMask();
        if (pServerCreature->m_bBodyFuel)
            nForcePoints = pServerCreature->GetCurrentHitPoints(0);
        else
            nForcePoints = (short)(pServerCreature->m_pStats->m_nCurrentForcePoints +
                pServerCreature->m_pStats->m_nTemporaryForcePoints);
    }

    int nFirstAction = lstActions.num;
    m_pStats->GetMenuInfo(lstActions, CSWSpell::SUBRADIAL_TYPE_SELF_FORCE_POWER,
        nForcePoints, 0, nItemMask, SWCCREATURE_SELF_FORCE_POWER_DISTANCE_SQUARED);
    for (int i = nFirstAction; i < lstActions.num; ++i)
    {
        lstActions.element[i].m_pfnAction = (PFNMENUACTION)&CSWCCreature::ForceMenuAction;
#ifdef _XBOX
        lstActions.element[i].m_oidTarget = (unsigned long)this;
#else
        lstActions.element[i].m_oidTarget = m_nId;
#endif
    }
}

#ifdef _XBOX
#include "exoinput.h"
#endif
// PC 0x00616310  Xbox 0x00046860
int CSWCCreature::LipSync(const CResRef& cLip)
{

    CExoString sLip(cLip.GetResRefStr());
    CLIP* pLip = new CLIP;
    if (pLip != NULL)
    {
        if (pLip->LoadLip(sLip))
        {
            int nCount = pLip->GetNumVisemes();
            float fLength = pLip->GetAnimLength();
            float* pfTimes = new float[nCount];
            float* pfVisemes = new float[nCount];
            int nResult = 0;

            for (int i = 0; i < nCount; ++i)
            {
                pfTimes[i] = pLip->GetTimeIndex(i) / fLength;

                pfVisemes[i] = ((unsigned long)pLip->GetVisemeIndex(i) + 1.0f) *
                               LIP_VISEME_ANIMATION_SCALE;
            }

#ifdef _XBOX

            int nDebugOption = g_pExoInput->GetDebugOption();
            if (nDebugOption == 3)
            {
                fLength *= 2.0f / 3.0f;
            }
            else if (nDebugOption == 1)
            {
                fLength *= 4.0f / 3.0f;
            }
#endif
            if (pfTimes[0] == 0.0f &&
                !(pfTimes[nCount - 1] >= SWCCREATURE_LIPSYNC_END_TIME_MAX) &&
                !(pfTimes[nCount - 1] <= SWCCREATURE_LIPSYNC_END_TIME_MIN))
            {
                pfTimes[nCount - 1] = 1.0f;
                StopLipSync();
                nResult = m_pAnimBase->SetAnimationVisemes(pfTimes, pfVisemes,
                                                          nCount, fLength);
            }

            pLip->UnloadLip();
            delete pLip;
            delete[] pfTimes;
            if (pfVisemes != NULL)
            {
                delete[] pfVisemes;
            }
            return nResult;
        }
        delete pLip;
    }
    return 0;
}

#include "nwcdoor.h"
#include "nwctrigger.h"
// PC 0x0060F540  Xbox 0x0003F820
int CSWCCreature::StopLipSync()
{
    return m_pAnimBase->SetAnimationVisemes(NULL, NULL, 0, 0.0f);
}
// PC 0x0060F560  Xbox 0x00097810
int CSWCCreature::BarkBubble(unsigned long nStrRef, const CExoString& sText,
                             const CResRef& cSound, int nFlags)
{
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()->ShowBarkBubble(nStrRef, sText, cSound,
                                                  nFlags);
    return 1;
}
// PC 0x00616520  Xbox 0x00046AC0
int CSWCCreature::IsLeaderUsableItem(CSWSItem* pItem, int nSlot,
                                    unsigned char nMask)
{
    if (pItem != NULL)
    {
        CSWSCreature* pCreature = GetServerCreature();
        int nRestrictMode = m_pArea->GetServerArea()->GetPlayerRestrictMode();
        if (pCreature == NULL)
            return 0;

        CSWItemProperty* pProperty;
        CSWBaseItem* pBaseItem;
        int bUsable;
        if (pItem->m_bPlotObject)
        {
            if ((nMask & USABLE_ITEM_ACTION_MASK_2) &&
                pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_MINE)
            {
                bUsable = pItem->GetPropertyByType(&pProperty,
                         CSWItemProperty::ITEM_PROPERTY_TRAP, 0)
                       ? ((unsigned char)pProperty->m_bUseable
                          ? pCreature->m_pStats->GetCanUseSkill(CSWSkill::SKILL_DEMOLITIONS) : 0)
                       : 1;
            }
            else
            {
                if (!pItem->GetPropertyByType(&pProperty,
                                              CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
                    return 0;
                pBaseItem = pItem->GetBaseItem();
                if (pBaseItem != NULL)
                {
                    switch (pBaseItem->m_nItemType)
                    {
                    case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_12:
                    case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_13:
                    case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_20:
                        if (!pCreature->m_pInventory->GetItemInInventory(pItem) ||
                            !(nMask & USABLE_ITEM_ACTION_MASK_4))
                            return 0;
                        break;
                    default:
                        if (!(nMask & USABLE_ITEM_ACTION_MASK_4))
                            return 0;
                        break;
                    }
                }
                CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(pProperty->m_nSubtype);
                if (pSpell != NULL && pSpell->m_nItemTargeting == CSWSpell::ITEM_TARGETING_PERSONAL)
                    return pCreature->CanUseItem(pItem, 1);
                return 0;
            }
        }
        else
        {
            if (nRestrictMode)
                return 0;
            if ((nMask & USABLE_ITEM_ACTION_MASK_1) &&
                (pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
                 pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC ||
                 pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_MEDPAC))
            {
                bUsable = !pItem->GetPropertyByType(&pProperty,
                         CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0) ||
                       (unsigned char)pProperty->m_bUseable;
            }
            else
            {
                unsigned char nOtherItems = nMask & USABLE_ITEM_ACTION_MASK_4;
                if (nOtherItems &&
                    pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_STIMULANT)
                {
                    bUsable = !pItem->GetPropertyByType(&pProperty,
                             CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0) ||
                           (unsigned char)pProperty->m_bUseable;
                }
                else if ((nMask & USABLE_ITEM_ACTION_MASK_2) &&
                    pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_MINE)
                {
                    bUsable = pItem->GetPropertyByType(&pProperty,
                             CSWItemProperty::ITEM_PROPERTY_TRAP, 0)
                           ? ((unsigned char)pProperty->m_bUseable
                              ? pCreature->m_pStats->GetCanUseSkill(CSWSkill::SKILL_DEMOLITIONS) : 0)
                           : 1;
                }
                else
                {
                    if (!pItem->GetPropertyByType(&pProperty,
                                                  CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0) ||
                        !(unsigned char)pProperty->m_bUseable)
                        return 0;
                    pBaseItem = pItem->GetBaseItem();
                    if (pBaseItem != NULL)
                    {
                        switch (pBaseItem->m_nItemType)
                        {
                        case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_12:
                        case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_13:
                        case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_20:
                            if (!pCreature->m_pInventory->GetItemInInventory(pItem))
                                return 0;
                            if (!nOtherItems)
                                return 0;
                            break;
                        default:
                            if (!nOtherItems)
                                return 0;
                            break;
                        }
                    }
                    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(pProperty->m_nSubtype);
                    if (pSpell != NULL && pSpell->m_nItemTargeting == CSWSpell::ITEM_TARGETING_PERSONAL)
                        return pCreature->CanUseItem(pItem, 1);
                    return 0;
                }
            }
        }
        return bUsable ? pCreature->CanUseItem(pItem, 1) : 0;
    }
    return 0;
}
// PC 0x006167D0  Xbox 0x00046C70
void CSWCCreature::PerformUseHostileItem(unsigned long nItem,
                                        CSWCCreature* pTarget)
{
    if (pTarget != NULL)
    {
        pTarget->SetCombatMode(1);
    }
    PartyMenuUseItem(nItem, pTarget);
}
// PC 0x0060F590  Xbox 0x0003F840
void CSWCCreature::PartyMenuUseItem(unsigned long oidItem,
                                    CSWCCreature* pTarget)
{
    unsigned long oidCreature = m_nId;
    oidItem &= ~OBJECT_LISTTYPE_MASK;

    if (pTarget->m_nId != oidCreature)
    {
        CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
        if (pMessage != NULL)
        {
            pMessage->SendPlayerToServerInput_UseItem(oidItem, 0, 0,
                                                     OBJECT_INVALID,
                                                     m_vPosition);
        }
    }
    else
    {
        Vector vTarget;
        CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
        if (pMessage != NULL)
        {
            pMessage->SendPlayerToServerInput_UseItem(oidItem, 0, 0,
                                                     oidCreature, vTarget);
        }
    }
}
// PC 0x0060F620  Xbox 0x0003F8C0
void CSWCCreature::ActionInitiateDialog(unsigned long nAction,
                                       CSWCCreature* pInstigator)
{
    if (!g_pAppManager->GetClientExoApp()->GetInGameGui()->GetCanClick() ||
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState ||
        m_nCombatState || GetServerObject() == NULL)
        return;

    GetServerObject()->ClearAllActions(1);
    CSWCCreature* pLeader =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    if (pLeader != NULL && GetServerObject()->m_bDialogFaceSpeaker &&
        GetServerObject()->m_bCommandable)
    {
        Vector vLeaderPosition = pLeader->m_vPosition;
        Vector vPosition = m_vPosition;
        Vector vOrientation = vLeaderPosition - vPosition;
        if (GetServerObject() != NULL)
            GetServerObject()->SetOrientation(vOrientation);
    }
    g_pAppManager->GetClientExoApp()->GetSWCMessage()->
        SendPlayerToServerInput_Dialog(GetObjectId());
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(1);
}
// PC 0x0060F780  Xbox 0x0003FA30
void CSWCCreature::SetAlignmentGoodEvil(short nAlignment)
{
    if (m_pStats != NULL)
    {
        int nValue;
        if (nAlignment >= 0)
        {
            nValue = nAlignment;
            if (nValue > 100)
                nValue = 100;
        }
        else
        {
            nValue = 0;
        }
        m_pStats->m_nGoodEvilAlignment = nValue;
    }
    if (m_pAppearance != NULL)
        m_pAppearance->SetAppearanceAlignment(nAlignment);
}
// PC 0x0060F7C0  Xbox 0x0003FA70
void CSWCCreature::CGPauseCycle(float fTimeStep, int bInitialize)
{
    if (m_pAnimBase != NULL)
    {
        unsigned char nCreatureType = m_pAnimBase->m_nAnimSet;
        if (nCreatureType == CREATURE_TYPE_UNLIMITED_WIELD)
        {
            CExoString sPause;
            CExoString sGesture;
            float fAnimationTime = 0.0f;
            float fAnimationLength = 0.0f;

            if (bInitialize)
            {
                switch (rand() % CG_PAUSE_COUNT)
                {
                case CG_PAUSE_1: sPause = "pause1"; break;
                case CG_PAUSE_2: sPause = "pause2"; break;
                }
                m_nAmbientPauseDelay = (rand() % 201 + 100) * 100;
                int nStartPercent = rand() % 100;
                Gob* pModel = m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                if (pModel != NULL)
                {
                    pModel->GetAnimation((char*)sPause.CStr(), &fAnimationTime,
                                        &fAnimationLength);
                    pModel->PlayAnimation(sPause.CStr(), 1.0f, Gob::ANIMATION_FLAG_OUT_OF_PLACE,
                                         nStartPercent * fAnimationLength * 0.01f);
                }
                pModel = m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
                if (pModel != NULL)
                {
                    pModel->PlayAnimation(sPause.CStr(), 1.0f, Gob::ANIMATION_FLAG_OUT_OF_PLACE,
                                         nStartPercent * fAnimationLength * 0.01f);
                }
            }
            else
            {
                m_nAmbientPauseTimer += (unsigned long)(fTimeStep * 1000.0f);
                if (m_nAmbientPauseTimer >= m_nAmbientPauseDelay)
                {
                    switch (rand() % CG_PAUSE_COUNT)
                    {
                    case CG_PAUSE_1: sPause = "pause1"; break;
                    case CG_PAUSE_2: sPause = "pause2"; break;
                    }
                    switch (rand() % CG_GESTURE_COUNT)
                    {
                    case CG_GESTURE_GREETING: sGesture = "greeting"; break;
                    case CG_GESTURE_HEAD_TURN_LEFT: sGesture = "hturnl"; break;
                    case CG_GESTURE_HEAD_TURN_RIGHT: sGesture = "hturnr"; break;
                    case CG_GESTURE_PAUSE_BORED: sGesture = "pausebrd"; break;
                    case CG_GESTURE_PAUSE_SHORT: sGesture = "pausesh"; break;
                    case CG_GESTURE_PAUSE_3: sGesture = "pause3"; break;
                    }
                    Gob* pModel = m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                    if (pModel != NULL)
                    {
                        pModel->GetAnimation((char*)sGesture.CStr(), &fAnimationTime,
                                            &fAnimationLength);
                        pModel->AddAnimationFlag(Gob::ANIMATION_FLAG_NO_LOOP);
                        pModel->PlayAnimation(sGesture.CStr(), 1.0f,
                                             Gob::ANIMATION_FLAG_QUEUE | Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
                        m_nAmbientPauseDelay += (unsigned long)(fAnimationLength * 1000.0f);
                        pModel->PlayAnimation(sPause.CStr(), 1.0f,
                                             Gob::ANIMATION_FLAG_QUEUE | Gob::ANIMATION_FLAG_RESTART, 0.0f);
                    }
                    pModel = m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
                    if (pModel != NULL)
                    {
                        pModel->AddAnimationFlag(Gob::ANIMATION_FLAG_NO_LOOP);
                        pModel->PlayAnimation(sGesture.CStr(), 1.0f,
                                             Gob::ANIMATION_FLAG_QUEUE | Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
                        pModel->PlayAnimation(sPause.CStr(), 1.0f,
                                             Gob::ANIMATION_FLAG_QUEUE | Gob::ANIMATION_FLAG_RESTART, 0.0f);
                    }
                    int nDelay = rand() % 201;
                    m_nAmbientPauseTimer = 0;
                    m_nAmbientPauseDelay = (nDelay + 100) * 100;
                }
            }
        }
    }
}
// PC 0x00616800  Xbox 0x00046CA0
void CSWCCreature::ActionMenuAttack(unsigned long nAction,
                                    CSWCCreature* pInstigator)
{
    int nResult;
    if (g_pAppManager->GetClientExoApp()->GetAttackMashTutorialTimer() > 0)
    {
        nResult = g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
            CClientExoApp::TUTORIAL_ATTACK_MASH, pInstigator->GetObjectId(), GetObjectId(), 0);
        if (nResult)
        {
            return;
        }
    }
    nResult = g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
        CClientExoApp::TUTORIAL_ATTACK, pInstigator->GetObjectId(), GetObjectId(), 0);
    if (nResult)
    {
        return;
    }
    pInstigator->SetCombatMode(1);
    g_pAppManager->GetClientExoApp()->SetAttackMashTutorialTimer(
        CClientExoApp::ATTACK_MASH_TUTORIAL_DELAY);
    g_pAppManager->GetClientExoApp()->GetSWCMessage()->
        SendPlayerToServerInput_Attack(GetObjectId(), 0);
}
// PC 0x00616890  Xbox 0x00046D30
int CSWCCreature::GetForceShield()
{
    CSWSCreature* pCreature = GetServerCreature();
    int nResult = 0;
    if (pCreature != NULL)
    {
        for (int i = 0; i < pCreature->m_lstEffects.num; i++)
        {
            CGameEffect* pEffect = pCreature->m_lstEffects.element[i];
            if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_FORCESHIELD)
            {
                nResult = pCreature->m_lstEffects.element[i]->GetInteger(0);
                break;
            }
            if (pEffect->m_nType > CSWSEffectListHandler::EFFECT_FORCESHIELD)
                break;
        }
    }
    return nResult;
}
// PC 0x0060FB20  Xbox 0x0003FDE0
CSWSCreature* CSWCCreature::GetServerCreature()
{
    if (GetServerObject() != NULL)
    {
        CSWSCreature* pCreature = GetServerObject()->AsSWSCreature();
        return pCreature;
    }
    return NULL;
}
// PC 0x0060FB40  Xbox 0x0003FE00
void CSWCCreature::AttachToScene(CAurScene* pScene)
{
    if (m_bInParty != 0 &&
        g_pAppManager->m_pClientExoApp->GetMiniGame() != NULL)
    {
        CSWCObject::AttachToScene(NULL);
    }
    else
    {
        CSWCObject::AttachToScene(pScene);
    }
}
// PC 0x0060FB80  Xbox 0x0003FF30
void CSWCCreature::SetOverrideLightsaberState(int bOverride, int bPowered,
                                               int bShowTransition)
{
    if (bOverride != 0)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(
            GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
        if (pItem != NULL)
            pItem->PowerItem(bPowered, 1, bShowTransition, 0);

        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pItem = pClientExoApp->GetItemByGameObjectID(
            GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
        if (pItem != NULL)
            pItem->PowerItem(bPowered, 1, bShowTransition, 0);
    }
    m_bOverrideLightsaberState = bOverride;
}
// PC 0x0060FC00
char CSWCCreature::GetTargetInterfaceTargetType(CSWCObject* pTarget)
{
    int nTargetType = TARGET_INTERFACE_INVALID;
    if (pTarget != NULL)
    {
        if (pTarget->AsSWCCreature() != NULL)
        {
            if (pTarget->AsSWCCreature()->IsHostile() ||
                g_pAppManager->m_pClientExoApp->GetTargetChanging())
                nTargetType = TARGET_INTERFACE_HOSTILE;
            else
                nTargetType = TARGET_INTERFACE_FRIENDLY;
        }
        else if (pTarget->AsSWCDoor() != NULL)
            nTargetType = TARGET_INTERFACE_DOOR_OR_CONTAINER;
        else if (pTarget->AsSWCPlaceable() != NULL)
        {
            CSWCPlaceable* pPlaceable = pTarget->AsSWCPlaceable();
            CSWSPlaceable* pServerPlaceable = NULL;
            if (pPlaceable != NULL)
                pServerPlaceable = pPlaceable->GetServerPlaceable();
            if (pServerPlaceable != NULL && pServerPlaceable->m_bHasInventory)
                nTargetType = TARGET_INTERFACE_DOOR_OR_CONTAINER;
            else
                nTargetType = TARGET_INTERFACE_FRIENDLY;
        }
        else if (pTarget->AsSWCTrigger() != NULL)
            nTargetType = TARGET_INTERFACE_TRIGGER;
    }
    else
        nTargetType = TARGET_INTERFACE_INVALID;
    return nTargetType;
}
// PC 0x0060FCC0  Xbox 0x0003FFB0
int CSWCCreature::GetInterfaceTargetType(CSWCObject* pTarget)
{
    int nTargetType = INTERFACE_TARGET_NONE;
    if (pTarget != NULL)
    {
        if (pTarget->AsSWCCreature() != NULL)
        {
            if (pTarget->AsSWCCreature()->IsHostile() ||
                g_pAppManager->m_pClientExoApp->GetTargetChanging())
                nTargetType = INTERFACE_TARGET_HOSTILE;
            else
                nTargetType = INTERFACE_TARGET_FRIENDLY;
        }
        else if (pTarget->AsSWCDoor() != NULL)
            nTargetType = INTERFACE_TARGET_DOOR_OR_CONTAINER;
        else if (pTarget->AsSWCPlaceable() != NULL)
        {
            CSWCPlaceable* pPlaceable = pTarget->AsSWCPlaceable();
            CSWSPlaceable* pServerPlaceable = NULL;
            if (pPlaceable != NULL)
                pServerPlaceable = pPlaceable->GetServerPlaceable();
            if (pServerPlaceable != NULL && pServerPlaceable->m_bHasInventory)
                nTargetType = INTERFACE_TARGET_DOOR_OR_CONTAINER;
            else
                nTargetType = INTERFACE_TARGET_FRIENDLY;
        }
        else if (pTarget->AsSWCTrigger() != NULL)
            nTargetType = INTERFACE_TARGET_TRIGGER;
    }
    else
        nTargetType = INTERFACE_TARGET_NONE;
    return nTargetType;
}
// PC 0x0060FD80  Xbox 0x000400F0
unsigned char CSWCCreature::GetReticlePoint(Vector& vPoint)
{
    unsigned char nResult = RETICLE_POINT_UNAVAILABLE;
    Gob* pModel = m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
    if (pModel != NULL)
    {
        Part* pPart = pModel->GetPart("head_g");
        if (pPart == NULL)
        {
            Gob* pHead = m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
            if (pHead != NULL)
            {
                pPart = pHead->GetPart("head_g");
            }
            if (pPart == NULL)
            {
                pPart = pModel->GetPart("rootdummy");
            }
        }
        if (pPart != NULL)
        {
            Quaternion qOrientation;
            pPart->GetWorld(vPoint, qOrientation);
            CAurCamera* pCamera =
                g_pAppManager->GetClientExoApp()->GetModuleCamera();
            Vector vView = -pCamera->GetOrientation().zdir();
            if (vView * vPoint > vView * pCamera->GetPosition())
            {
                nResult = RETICLE_POINT_IN_FRONT;
            }
            else
            {
                nResult = RETICLE_POINT_BEHIND;
            }
            vPoint = pCamera->MapToScreen(vPoint, true);
            vPoint.m_fX *=
                g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth;
            vPoint.m_fY *=
                g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight;
        }
    }
    return nResult;
}
// PC 0x0060FF30
unsigned char CSWCCreature::GetMenuPoint(Vector& vPoint)
{
    CAurCamera* pCamera = g_pAppManager->GetClientExoApp()->GetModuleCamera();
    Vector vDirection = -pCamera->GetOrientation().zdir();
    vPoint = m_vPosition;

    CSWSCreature* pCreature = GetServerCreature();
    if (pCreature != NULL &&
        (pCreature->m_sTag == "zaalbar" || pCreature->m_sTag == "hk47"))
    {
        vPoint.m_fZ += 2.0f;
    }
    else
    {
        vPoint.m_fZ += 1.75f;
    }

    unsigned char nResult = (vDirection * vPoint > vDirection * pCamera->GetPosition())
        ? MENU_POINT_IN_FRONT : MENU_POINT_BEHIND;
    vPoint = pCamera->MapToScreen(vPoint, true);
    vPoint.m_fX *= g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth;
    vPoint.m_fY *= g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight;
    return nResult;
}
// PC 0x006100F0  Xbox 0x0004035C
void CSWCCreature::Setup3DScene(CSWGui3DSceneView& cView,
                               CSWCCreature* pCreature,
                               CSWSCreature* pServerCreature, int bAnimate)
{
    int nAlignment = pCreature != NULL ?
        pCreature->GetStats()->m_nGoodEvilAlignment : GetStats()->m_nGoodEvilAlignment;
    if (g_nHackAlignment != HACK_ALIGNMENT_DISABLED)
        nAlignment = g_nHackAlignment;

    CExoString sAnimation;
    CExoString sAlignmentAnimation;
    if (nAlignment == CSWCCreatureStats::GOOD_EVIL_DARK_SIDE)
        sAlignmentAnimation = "evil";
    else if (nAlignment == CSWCCreatureStats::GOOD_EVIL_LIGHT_SIDE)
        sAlignmentAnimation = "good";
    else
        sAlignmentAnimation.Format("align%d", nAlignment / 5 + 1);

    if (pServerCreature == NULL && g_pAppManager->GetServerExoApp() != NULL)
    {
        pServerCreature = pCreature != NULL ?
            pCreature->GetServerCreature() : GetServerCreature();
    }
    if (pServerCreature != NULL &&
        (pServerCreature->m_sTag == "t3m4" || pServerCreature->m_sTag == "hk47"))
        sAnimation = "pause1";
    else if (nAlignment < CSWCCreatureStats::GOOD_EVIL_NEUTRAL_MIN)
        sAnimation = "evil";
    else if (nAlignment >= CSWCCreatureStats::GOOD_EVIL_LIGHT_SIDE_MIN)
        sAnimation = "good";
    else
        sAnimation = "neutral";

    Gob* pModel = GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
    if (pModel != NULL && bAnimate)
        pModel->PlayAnimation(sAnimation.CStr(), 1.0f,
                              Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);
    pModel = GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
    if (pModel != NULL && bAnimate)
        pModel->PlayAnimation(sAnimation.CStr(), 1.0f,
                              Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);

    pModel = (Gob*)cView.m_cScene.GetModel(0);
    if (pModel != NULL)
    {
        float fLength;
        pModel->GetAnimation((char*)sAlignmentAnimation.CStr(), NULL, &fLength);
        if (fLength != Gob::ANIMATION_LENGTH_NOT_FOUND)
            pModel->PlayAnimation(sAlignmentAnimation.CStr(), 1.0f,
                                  Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);
        else
            pModel->PlayAnimation(sAnimation.CStr(), 1.0f,
                                  Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);
        CExoString sCameraHook;
        if (pServerCreature != NULL)
        {
            if (pServerCreature->m_sTag == "t3m4")
                sCameraHook = "camerahookt";
            else if (pServerCreature->m_sTag == "hk47")
                sCameraHook = "camerahookh";
            else if (pServerCreature->m_sTag == "zaalbar")
                sCameraHook = "camerahookz";
            else
                sCameraHook = "camerahook";
        }
        else
            sCameraHook = "camerahook";
        if (pModel->GetPart((char*)sCameraHook.CStr()) == NULL)
            sCameraHook = "camerahook";
        CAurCamera* pCamera = cView.m_cScene.m_pCamera;
        pCamera->AttachToObject(pModel, sCameraHook.CStr(), Gob::ATTACH_TRANSLATE);
        cView.m_cScene.m_pCamera->SetViewAngle(22.726f);
        CAurBehaviorAttachTranslate* pNode =
            (CAurBehaviorAttachTranslate*)cView.m_cScene.m_pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
        if (pNode != NULL)
        {
            pNode->m_nWorldSpace = 1;
            pNode->m_vTranslation = Vector(0.0f, 0.0f, GetHeadHeightOffset());
        }
    }
}
// PC 0x00610490  Xbox 0x000406D0
int CSWCCreature::GetFreeLookVideoEffect()
{
    int nVideoEffect = -1;
    if (GetAppearance() != NULL)
    {
        bool bFailed;
        {
            CExoString sColumn("FreeLookEffect");
            unsigned short nAppearance = GetAppearance()->m_nAppearance;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pAppearance;
            bFailed = !pTable->GetINTEntry(nAppearance, sColumn, &nVideoEffect);
        }
        if (bFailed)
            nVideoEffect = -1;
    }
    return nVideoEffect;
}
// PC 0x00610550  Xbox 0x00040790
void CSWCCreature::UnRegisterCallbacks()
{
    Gob* pModel = static_cast<Gob*>(m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT));
    if (pModel != NULL)
    {
        for (int nIndex = 0; nIndex < 17; ++nIndex)
        {
            if (m_apAnimationCallbacks[nIndex] != NULL)
            {
                pModel->DeregisterCallback(m_apAnimationCallbacks[nIndex]);
            }
        }
    }
}
// PC 0x00610590  Xbox 0x000407D0
float CSWCCreature::GetDriveAcceleration()
{
    if (m_bRunning)
        return SWCCREATURE_RUNNING_DRIVE_ACCELERATION;
    return m_pAppearance->m_fDriveAcceleration;
}
// PC 0x006105B0  Xbox 0x000407F0
int CSWCCreature::LoadRollingFootstepSound()
{
    CExoString sSound;

    if (m_pRollingFootstepSound != NULL)
    {
        delete m_pRollingFootstepSound;
        m_pRollingFootstepSound = NULL;
    }

    int nFootstepType;
    unsigned short nAppearance = GetAppearance()->m_nAppearance;
    if (!g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
            nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_FOOTSTEPTYPE, &nFootstepType))
        return 0;

#ifdef _XBOX
    CTwoDimArrays* pXbox2DArrays = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pXbox2DArrays->m_pFootstepSounds);
    C2DA* pFootstepSounds = pXbox2DArrays->m_pFootstepSounds;
#else
    C2DA* pFootstepSounds = g_pRules->Get2DArrays()->m_pFootstepSounds;
#endif
    if (!pFootstepSounds->GetCExoStringEntry(
            nFootstepType, CTwoDimArrays::COL2DA_FOOTSTEPSOUNDS_ROLLING,
            &sSound))
        return 0;

    m_pRollingFootstepSound = new CExoSoundSource(sSound);
    m_pRollingFootstepSound->Set3D(1);

    unsigned char nPriorityGroup;
    if (GetObjectId() ==
        g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
    {
        nPriorityGroup = CExoSound::PRIORITY_GROUP_PLAYER_FOOTSTEP;
    }
    else
    {
        int nSizeCategory;
        unsigned short nAppearance = GetAppearance()->m_nAppearance;
        g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
            nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY, &nSizeCategory);
        if (nSizeCategory == CREATURE_SIZE_TINY ||
            nSizeCategory == CREATURE_SIZE_SMALL ||
            nSizeCategory == CREATURE_SIZE_MEDIUM)
        {
            nPriorityGroup = CExoSound::PRIORITY_GROUP_CREATURE_FOOTSTEP;
        }
        else
        {
            nPriorityGroup = CExoSound::PRIORITY_GROUP_LARGE_CREATURE_FOOTSTEP;
        }
    }

    m_pRollingFootstepSound->SetPriorityGroup(nPriorityGroup);
    m_pRollingFootstepSound->SetLooping(1);
    m_pRollingFootstepSound->SetPosition(m_vPosition, 1.5f);
    {
        CTwoDimArrays* pArrays = g_pRules->Get2DArrays();
        EXOXBOXASSERT(pArrays->m_pFootstepSounds);
        C2DA* pTable = pArrays->m_pFootstepSounds;
        pTable->GetFLOATEntry(
            nFootstepType, CTwoDimArrays::COL2DA_FOOTSTEPSOUNDS_PITCHOFFSET,
            &m_fRollingFootstepPitchOffset);
    }
    m_bRollingFootstepPlaying = 0;
    m_bRollingFootstepRunning = 0;
    return m_pRollingFootstepSound != NULL;
}
// PC 0x006107C0  Xbox 0x00040AB0
void CSWCCreature::PlayRollingFootstepSound(int bRunning)
{
    if (m_pRollingFootstepSound)
    {
        m_pRollingFootstepSound->SetPosition(m_vPosition, 0.0f);

        if (!m_bRollingFootstepPlaying ||
            m_bRollingFootstepRunning != bRunning)
        {
            if (bRunning)
                m_pRollingFootstepSound->SetFixedVariance(
                    m_fRollingFootstepPitchOffset + 1.0f);
            else
                m_pRollingFootstepSound->SetFixedVariance(1.0f);

            m_pRollingFootstepSound->Play();
            m_bRollingFootstepPlaying = 1;
            m_bRollingFootstepRunning = bRunning;
        }
    }
}
// PC 0x00610840  Xbox 0x00040B30
void CSWCCreature::UpdateRollingFootstepSound()
{
    if (m_pRollingFootstepSound != NULL)
    {
        switch (GetCurrentAnimation())
        {
            case CREATURE_ANIMATION_WALK:
            case CREATURE_ANIMATION_WALK_BACKWARD:
            case CREATURE_ANIMATION_INJURED_WALK:
            case CREATURE_ANIMATION_SPECIAL_WALK:
                PlayRollingFootstepSound(0);
                break;

            case CREATURE_ANIMATION_RUN:
            case CREATURE_ANIMATION_INJURED_RUN:
                PlayRollingFootstepSound(1);
                break;

            default:
                if (m_pRollingFootstepSound != NULL && m_bRollingFootstepPlaying != 0)
                {
                    m_pRollingFootstepSound->Stop();
                    m_bRollingFootstepPlaying = 0;
                    m_bRollingFootstepRunning = 0;
                }
                break;
        }
    }
}
// PC 0x00610950  Xbox 0x00040C40
void CSWCCreature::SetLockOrientationToObject(unsigned long oidTarget, int bUnknown)
{
    if (g_pAppManager->m_pServerExoApp != NULL)
    {
        CSWSCreature* pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            g_pAppManager->m_pServerExoApp->ClientToServerObjectId(GetObjectId()));
        if (pCreature == NULL)
        {
            return;
        }
        if (bUnknown == 0)
        {
            if (pCreature->GetDead() != 0)
            {
                return;
            }
            if (pCreature->GetDeadTemp() != 0)
            {
                return;
            }
        }
        m_oidLockTarget = oidTarget;
    }
}
// PC 0x006109B0  Xbox 0x00040CA0
float CSWCCreature::GetLightSaberThrowRate()
{
    if (GetServerCreature() != NULL)
    {
        return GetServerCreature()->m_fLightsaberThrowRate;
    }
    return 37.0f;
}
// PC 0x00616900  Xbox 0x00046DA0
void CSWCCreature::PerformCombatFeat(unsigned short nFeat, CSWCCreature* pTarget)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();

    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_Attack(m_nId, nFeat);
    }
    if (pTarget != NULL)
    {
        pTarget->SetCombatMode(1);
    }
}
// PC 0x00610A10  Xbox 0x00040D00
void CSWCCreature::SetCombatMode(int nMode)
{
    m_bCombatMode = nMode;
    unsigned long oidCreature = m_nId;
    if (oidCreature == g_pAppManager->m_pClientExoApp->GetPlayerCreatureId())
    {
        g_pAppManager->m_pClientExoApp->SetCombatMode(m_bCombatMode);
    }
}
// PC 0x00610A70  Xbox 0x00040D60
void CSWCCreature::PlayDrawAnimation(unsigned short nValue)
{
    if (g_pAppManager->m_pClientExoApp->GetPausedByCombat() != 0)
    {
        m_nDrawAnimation = nValue;
    }
    else
    {
        InsertFireForgetAnimation(nValue, 1500, 1.0f, NULL, 0);
        m_nDrawAnimation = -1;
    }
}
// PC 0x00610AC0
int CSWCCreature::IsStealthCapable()
{
    if (GetServerCreature() != NULL &&
        GetServerCreature()->GetArea()->GetPlayerRestrictMode() == 0 &&
        GetStats()->GetCanUseSkill(CSWSkill::SKILL_STEALTH) != 0)
    {
        int bHasStealthDevice = 0;
        CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
            GetEquippedItemID(INVENTORY_SLOT_ARMS));
        if (pItem != NULL && pItem->GetBaseItem() != NULL)
        {
            bHasStealthDevice =
                pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_STEALTH_DEVICE;
        }
        pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
            GetEquippedItemID(INVENTORY_SLOT_CREATUREARMOUR));
        if (pItem != NULL && pItem->GetBaseItem() != NULL)
        {
            bHasStealthDevice |=
                pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_STEALTH_DEVICE;
        }
        if (bHasStealthDevice)
            return 1;
    }
    return 0;
}
// PC 0x006197D0  Xbox 0x0004A320
void CSWCCreature::GetUsableItems(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions, unsigned char nActionMask)
{
    CSWSCreature* pServerCreature = GetServerCreature();
    if (pServerCreature != NULL)
    {
        if (pServerCreature->GetArea()->GetPlayerRestrictMode() == 0)
        {
            unsigned long oidItem = m_aoidEquippedItems[EQUIPPED_ITEM_RIGHTRING];
            CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
            {
                CSWSItem* pServerItem = pItem->GetServerItem();
                CreateUsableItemEntry(lstActions, pServerItem, 1, nActionMask);
            }

            oidItem = m_aoidEquippedItems[EQUIPPED_ITEM_NECK];
            pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
            {
                CSWSItem* pServerItem = pItem->GetServerItem();
                CreateUsableItemEntry(lstActions, pServerItem, 1, nActionMask);
            }

            oidItem = m_aoidEquippedItems[EQUIPPED_ITEM_ARMS];
            pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
            {
                CSWSItem* pServerItem = pItem->GetServerItem();
                CreateUsableItemEntry(lstActions, pServerItem, 1, nActionMask);
            }
        }

        CItemRepository* pRepository = pServerCreature->GetItemRepository(1);
        for (unsigned long i = 0; i < pRepository->m_lstItems.num; ++i)
        {
            CSWSItem* pItem = pRepository->ItemListGetItem(i);
            CreateUsableItemEntry(lstActions, pItem, 0, nActionMask);
        }
    }
}
// PC 0x006198E0  Xbox 0x0004A430
void CSWCCreature::GetHostileItems(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions, CSWCCreature* pTarget)
{
    CItemRepository* pRepository = GetServerCreature()->GetItemRepository(1);

    for (unsigned long i = 0; i < pRepository->m_lstItems.num; ++i)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        CreateHostileItemEntry(lstActions, pItem, pTarget);
    }
}
// PC 0x00619950  Xbox 0x0004A770
void CSWCCreature::GetCombatFeatsSkills(
    CExoArrayList<CSWGuiInterfaceAction>& lstActions, CSWCCreature* pTarget)
{
    CSWSCreature* pServerCreature = GetServerCreature();
    if (pServerCreature != NULL &&
        pServerCreature->GetArea()->m_nPlayerRestrictMode == 0)
    {
        unsigned long oidWeapon = GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
        CSWCObject* pWeapon;
        CSWBaseItem* pBaseItem;
        int nFeat;
        if (oidWeapon != OBJECT_INVALID &&
            (pWeapon = g_pAppManager->GetClientExoApp()->GetGameObject(oidWeapon)) != NULL &&
            pWeapon->AsSWCItem() != NULL &&
            (pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pWeapon->AsSWCItem()->GetBaseItemType())) != NULL &&
            pBaseItem->m_nRangedWeapon > 0)
        {
            nFeat = CheckFeat(CSWFeat::FEAT_MASTER_POWER_BLAST,
                CSWFeat::FEAT_IMPROVED_POWER_BLAST, CSWFeat::FEAT_POWER_BLAST);
            EnableFeatForMenu(nFeat, lstActions, (PFNMENUACTION)&CSWCCreature::ActionMenuUseAttackFeat, pTarget);
            nFeat = CheckFeat(CSWFeat::FEAT_MULTI_SHOT,
                CSWFeat::FEAT_IMPROVED_RAPID_SHOT, CSWFeat::FEAT_RAPID_SHOT);
            EnableFeatForMenu(nFeat, lstActions, (PFNMENUACTION)&CSWCCreature::ActionMenuUseAttackFeat, pTarget);
            nFeat = CheckFeat(CSWFeat::FEAT_MASTER_SNIPER_SHOT,
                CSWFeat::FEAT_IMPROVED_SNIPER_SHOT, CSWFeat::FEAT_SNIPER_SHOT);
            EnableFeatForMenu(nFeat, lstActions, (PFNMENUACTION)&CSWCCreature::ActionMenuUseAttackFeat, pTarget);
        }
        else
        {
            nFeat = CheckFeat(CSWFeat::FEAT_MASTER_CRITICAL_STRIKE,
                CSWFeat::FEAT_IMPROVED_CRITICAL_STRIKE, CSWFeat::FEAT_CRITICAL_STRIKE);
            EnableFeatForMenu(nFeat, lstActions, (PFNMENUACTION)&CSWCCreature::ActionMenuUseAttackFeat, pTarget);
            nFeat = CheckFeat(CSWFeat::FEAT_WHIRLWIND_ATTACK,
                CSWFeat::FEAT_IMPROVED_FLURRY, CSWFeat::FEAT_FLURRY);
            EnableFeatForMenu(nFeat, lstActions, (PFNMENUACTION)&CSWCCreature::ActionMenuUseAttackFeat, pTarget);
            nFeat = CheckFeat(CSWFeat::FEAT_MASTER_POWER_ATTACK,
                CSWFeat::FEAT_IMPROVED_POWER_ATTACK, CSWFeat::FEAT_POWER_ATTACK);
            EnableFeatForMenu(nFeat, lstActions, (PFNMENUACTION)&CSWCCreature::ActionMenuUseAttackFeat, pTarget);
        }
    }
}

#include "swguicontrol.h"

#include "swmain.h"
// Xbox 0x0004A930
void CSWCCreature::GetFriendlyInterfaceActions(
    CSWCObject* pTarget, int nType,
    CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    int nAction = lstActions.num;
    lstActions.SetSize(nAction + 1);
    lstActions[nAction].m_sName =
        g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_DIALOG);
    lstActions[nAction].m_pfnAction =
        (PFNMENUACTION)&CSWCCreature::ActionInitiateDialog;
#ifdef _XBOX
    lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
    lstActions[nAction].m_oidTarget = m_nId;
#endif
    lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_DIALOG;
    lstActions[nAction].m_resIcon = "i_dialog";
}
// PC 0x00619B10  Xbox 0x0004AB00
void CSWCCreature::GetHostileInterfaceActions(
    CSWCObject* pTarget, int nType,
    CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    CSWSCreature* pCreature = GetServerCreature();
    if (pCreature != NULL && pCreature->GetArea()->m_nPlayerRestrictMode == 0)
    {
        int nAction = lstActions.num;
        lstActions.SetSize(nAction + 1);
        lstActions[nAction].m_sName =
            g_pAppManager->GetClientExoApp()->GetGUIString(CSWGuiInterfaceAction::STRREF_ATTACK);
        lstActions[nAction].m_pfnAction =
            (PFNMENUACTION)&CSWCCreature::ActionMenuAttack;
#ifdef _XBOX
        lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
        lstActions[nAction].m_oidTarget = m_nId;
#endif
        lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_ATTACK;
        lstActions[nAction].m_resIcon = "i_attack";
    }
}
// PC 0x00619C20  Xbox 0x0004AC00
void CSWCCreature::GetTargetActions(CSWCObject* pTarget, int nMenu,
                                    CExoArrayList<CSWGuiInterfaceAction>& lActions)
{
    switch (GetInterfaceTargetType(pTarget))
    {
    case INTERFACE_TARGET_DOOR_OR_CONTAINER:
        if (pTarget->AsSWCPlaceable() != NULL)
            pTarget->AsSWCPlaceable()->GetTargetActions(this, nMenu, lActions);
        else if (pTarget->AsSWCDoor() != NULL)
            pTarget->AsSWCDoor()->GetTargetActions(this, nMenu, lActions);
        break;
    case INTERFACE_TARGET_TRIGGER:
        if (pTarget->AsSWCTrigger() != NULL)
            pTarget->AsSWCTrigger()->GetTargetActions(this, nMenu, lActions);
        break;
    case INTERFACE_TARGET_FRIENDLY:
        if (pTarget->AsSWCPlaceable() != NULL)
        {
            if (nMenu == 1 &&
                pTarget->AsSWCPlaceable()->IsHostile() &&
                pTarget->AsSWCPlaceable()->GetServerPlaceable()->GetReputation(
                    GetServerObject()->GetId(), 0) <=
                    CSWSObject::MAX_HOSTILE_REPUTATION)
                GetHostileForcePowers(pTarget, lActions);
            else
                pTarget->AsSWCPlaceable()->GetTargetActions(this, nMenu, lActions);
        }
        break;
    case INTERFACE_TARGET_HOSTILE:

        switch (nMenu)
        {
        case 0:
            GetCombatFeatsSkills(lActions, pTarget->AsSWCCreature());
            pTarget->AsSWCCreature()->GetHostileInterfaceActions(this, 0, lActions);
            break;
        case 1:
            GetHostileForcePowers(pTarget, lActions);
            break;
        case 2:
            GetHostileItems(lActions, pTarget->AsSWCCreature());
            break;
        }
        break;
    }
}
// PC 0x00619DB0
void CSWCCreature::GetPersonalActions(
    int nMenu, CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    switch (nMenu)
    {
    case PERSONAL_ACTION_FORCE_POWERS:
        GetSelfForcePowers(lstActions);
        return;
    case PERSONAL_ACTION_USABLE_ITEMS_1:
        GetUsableItems(lstActions, USABLE_ITEM_ACTION_MASK_1);
        return;
    case PERSONAL_ACTION_USABLE_ITEMS_3:
        GetUsableItems(lstActions, USABLE_ITEM_ACTION_MASK_2);
        return;
    case PERSONAL_ACTION_USABLE_ITEMS_2:
        GetUsableItems(lstActions, USABLE_ITEM_ACTION_MASK_4);
        return;
    }
}
// PC 0x00619E10  Xbox 0x0004B260
void CSWCCreature::DoFootstepVisual(unsigned char nFootstep,
                                   int bUpdateGroundHeight)
{
    CSWCVisualEffect* pEffect = new CSWCVisualEffect(1);
    if (pEffect != NULL)
    {
        CExoString sModel;
        switch (nFootstep)
        {
        case FOOTSTEP_VISUAL_SPLASH: sModel = "fx_step_splash"; break;
        case FOOTSTEP_VISUAL_DUST: sModel = "fx_step_dust"; break;
        case FOOTSTEP_VISUAL_LEAF: sModel = "fx_step_leaf"; break;
        default: return;
        }
        if (pEffect->LoadModel(sModel, MODEL_SLOT_DEFAULT,
                               LOAD_MODEL_FLAG_VISUAL_EFFECT) && m_pArea != NULL)
        {
            Vector vPosition;
            if (m_nCurrentAnimation == CREATURE_ANIMATION_WALK ||
                m_nCurrentAnimation == CREATURE_ANIMATION_WALK_BACKWARD ||
                m_nCurrentAnimation == CREATURE_ANIMATION_RUN ||
                m_nCurrentAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10078 ||
                m_nCurrentAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10079 ||
                m_nCurrentAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10084 ||
                m_nCurrentAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10085 ||
                m_nCurrentAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10086 ||
                m_nCurrentAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10087)
            {
                if (GetModel(MODEL_SLOT_DEFAULT, 1))
                {
                    if (nFootstep == FOOTSTEP_VISUAL_SPLASH)
                    {
                        if (m_nCurrentAnimation == CREATURE_ANIMATION_WALK_BACKWARD)
                            vPosition = m_vPosition - m_vOrientation * 0.5f;
                        else
                            vPosition = m_vPosition + m_vOrientation * 0.5f;
                    }
                    else
                    {
                        if (m_nCurrentAnimation == CREATURE_ANIMATION_WALK_BACKWARD)
                            vPosition = m_vPosition - m_vOrientation * 0.1f;
                        else
                            vPosition = m_vPosition + m_vOrientation * 0.1f;
                    }
                    Vector vStart;
                    vStart.m_fX = m_vPosition.m_fX;
                    vStart.m_fY = m_vPosition.m_fY;
                    vStart.m_fZ = m_vPosition.m_fZ + 1.0f;
                    if ((bUpdateGroundHeight ||
                         fabs(m_vPosition.m_fZ - m_fFootstepGroundHeight) > 1.0) &&
                        m_pArea->m_pScene != NULL)
                    {
                        CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                        Vector vHit;
                        cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                        g_pAppManager->GetClientExoApp()->AddObjectModelDirectlyToIgnoreList(
                            GetObjectId(), &cHitInfo.m_lstHits);
                        int bStaticTriMeshParts = hitcheck_static_trimesh_parts;
                        hitcheck_static_trimesh_parts = 1;
                        if (m_pArea->m_pScene->HitCheck(vStart, m_vPosition, &vHit,
                                                       &cHitInfo))
                            m_fFootstepGroundHeight = vHit.m_fZ + 0.01f;
                        else
                            m_fFootstepGroundHeight = m_vPosition.m_fZ + 0.35f;
                        hitcheck_static_trimesh_parts = bStaticTriMeshParts;
                    }
                    if (nFootstep == FOOTSTEP_VISUAL_SPLASH)
                    {
                        Vector vSide = cross(Vector(0.0f, 0.0f, 1.0f), m_vOrientation);
                        if (vSide * vSide > 0.00001f)
                        {
                            vSide = normalize(vSide);
                            if (rand() % 2)
                                vPosition = vPosition + vSide * (float(rand() % 20) * 0.01f);
                            else
                                vPosition = vPosition - vSide * (float(rand() % 20) * 0.01f);
                        }
                        vPosition.m_fZ = m_fFootstepGroundHeight;
                    }
                }
            }
            else
                vPosition = m_vPosition;
            CAurScene* pScene = m_pArea->m_pScene;
            pEffect->AttachToScene(pScene);
            pEffect->m_bDeleteWhenFinished = 1;
            pEffect->m_bOneShot = 1;
            pEffect->SetPosition(vPosition);
            pEffect->SetOrientation(m_vOrientation);
            pEffect->SetAnimation(VFX_ANIMATION_START);
        }
    }
}
// PC 0x0061A2D0  Xbox 0x0004B830
void CSWCCreature::PlayFootstep()
{
    CExoString sMaterial;
    CExoString sSound;
    float fPitchOffset = 0.0f;
    CSWCArea* pArea = m_pArea;
    if (pArea != NULL)
    {
        int nFootstepType;
        unsigned short nAppearance = GetAppearance()->m_nAppearance;
        g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
            nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_FOOTSTEPTYPE,
            &nFootstepType);
        if (!g_pRules->Get2DArrays()->GetFootstepSounds()->GetCExoStringEntry(
                nFootstepType, CTwoDimArrays::COL2DA_FOOTSTEPSOUNDS_ROLLING,
                &sSound))
        {
            if (m_nStealthState == 1)
                nFootstepType = CTwoDimArrays::FOOTSTEP_TYPE_STEALTH;
            int nMaterial = pArea->GetSurfaceMaterial(m_vPosition);
            g_pRules->Get2DArrays()->GetSurfaceMaterial()->GetCExoStringEntry(
                nMaterial, CTwoDimArrays::COL2DA_SURFACEMATERIAL_NAME,
                &sMaterial);
            if (sMaterial != "")
                sMaterial = sMaterial + CExoString(rand() % 3);
            g_pRules->Get2DArrays()->GetFootstepSounds()->GetCExoStringEntry(
                nFootstepType, sMaterial, &sSound);
            g_pRules->Get2DArrays()->GetFootstepSounds()->GetFLOATEntry(
                nFootstepType, CTwoDimArrays::COL2DA_FOOTSTEPSOUNDS_PITCHOFFSET,
                &fPitchOffset);
            unsigned char nPriorityGroup;
            if (GetObjectId() ==
                g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
                nPriorityGroup = CExoSound::PRIORITY_GROUP_PLAYER_FOOTSTEP;
            else
            {
                int nSizeCategory;
                unsigned short nAppearance = GetAppearance()->m_nAppearance;
                g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
                    nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY,
                    &nSizeCategory);
                if (nSizeCategory == CREATURE_SIZE_TINY ||
                    nSizeCategory == CREATURE_SIZE_SMALL ||
                    nSizeCategory == CREATURE_SIZE_MEDIUM)
                    nPriorityGroup = CExoSound::PRIORITY_GROUP_CREATURE_FOOTSTEP;
                else
                    nPriorityGroup = CExoSound::PRIORITY_GROUP_LARGE_CREATURE_FOOTSTEP;
            }
            float fMaxDistance;
            float fMinDistance;
            Vector vListener;
            g_pExoSound->GetPriorityGroupDistance(nPriorityGroup,
                                                &fMaxDistance, &fMinDistance);
            g_pExoSound->GetListenerPosition(&vListener);
            float fDistance = magnitude(m_vPosition - vListener);
            if (fDistance > fMinDistance + 2.0f)
                return;
            {
                CResRef cSound((char*)sSound.CStr());
                g_pExoSound->Play3DOneShotSound(cSound, m_vPosition, 0.0f,
                    nPriorityGroup, 0, 0, 0.0f, fPitchOffset);
                if (nFootstepType != CTwoDimArrays::FOOTSTEP_TYPE_NO_VISUAL_6 &&
                    nFootstepType != CTwoDimArrays::FOOTSTEP_TYPE_NO_VISUAL_7)
                {
                    if (nMaterial == CSWRoomSurfaceMesh::SURFACE_MATERIAL_DIRT)
                    {
                        DoFootstepVisual(FOOTSTEP_VISUAL_DUST, 0);
                        m_bWetFootstep = 0;
                    }
                    else if (nMaterial == CSWRoomSurfaceMesh::SURFACE_MATERIAL_WATER ||
                             nMaterial == CSWRoomSurfaceMesh::SURFACE_MATERIAL_PUDDLES ||
                             nMaterial == CSWRoomSurfaceMesh::SURFACE_MATERIAL_SWAMP ||
                             nMaterial == CSWRoomSurfaceMesh::SURFACE_MATERIAL_MUD)
                    {
                        DoFootstepVisual(FOOTSTEP_VISUAL_SPLASH, !m_bWetFootstep);
                        m_bWetFootstep = 1;
                    }
                    else if (nMaterial == CSWRoomSurfaceMesh::SURFACE_MATERIAL_WOOD)
                        DoFootstepAudio(0);
                    else
                        m_bWetFootstep = 0;
                }
                DoFootstepRumble(nFootstepType);
            }
        }
    }
}
// PC 0x0061A670  Xbox 0x0004BCA0
void CSWCCreature::AnimateFireAndForget()
{
    if (AnimationFireAndForget(m_nCurrentAnimation) == 1)
    {
        m_nAnimationTimer -= m_nTimeDelta;
        if (m_nAnimationTimer < 0)
        {
            if (AnimationOverlay(m_nCurrentAnimation))
                m_pAnimBase->SetOverlayAnimation(m_nCurrentAnimation, 0);
            m_nAnimationTimer = 0;
            SetAnimation(GetLoopingAnimation(), 0, 0);
        }
    }

    if (AnimationLooping(m_nCurrentAnimation) == 1)
    {
        unsigned short nAnimation;
        float fSpeed;
        unsigned long nLength;
        void* pData;
        if (GetTopFireForgetAnimation(&nAnimation, &fSpeed, &nLength, &pData) == 1)
        {
            CSWCCombatAttackData* pAttackData = NULL;
            CSWCCombatAttackData* pSpellData = NULL;
            if (AnimationAttack(nAnimation) ||
                nAnimation == CREATURE_ANIMATION_COMBAT_READY)
            {
                pAttackData = (CSWCCombatAttackData*)pData;
                if (pAttackData != NULL)
                    m_apAttackData[0]->Copy(pAttackData);
            }
            if (nAnimation == CREATURE_ANIMATION_FOOTSTEP_1 ||
                nAnimation == CREATURE_ANIMATION_FOOTSTEP_3 ||
                nAnimation == CREATURE_ANIMATION_FOOTSTEP_4 ||
                nAnimation == CREATURE_ANIMATION_FOOTSTEP_2)
            {
                PlayFootstep();
            }
            if (nAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
                nAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
                nAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
                nAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
                nAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
                nAnimation == CREATURE_ANIMATION_SPELLCAST_11)
            {
                pSpellData = (CSWCCombatAttackData*)pData;
                if (pSpellData != NULL)
                    m_apAttackData[1]->Copy(pSpellData);
            }
            TestAndReEquipweapons(m_nCurrentAnimation, nAnimation);
            m_nCurrentAnimation = nAnimation;
            if (AnimationHideEquippedItems(nAnimation))
                HideEquippedItems(CResRef(), CResRef());

            int bOutOfPlace = 0;
            if (m_bOutOfPlace)
            {
                m_bOutOfPlace = 0;
                Vector vPosition = m_vPosition;
                Vector vOrientation = m_vOrientation;
                m_pAnimBase->SetPosition(vPosition);
                m_pAnimBase->SetOrientation(vOrientation);
                m_pAnimBase->SetAlwaysRender(0);
                m_pAnimBase->SetInCutscene(0);
                AbortFireForgetAnimation(0);
            }
            if (AnimationPlayOutOfPlace(nAnimation))
            {
                bOutOfPlace = 1;
                m_bOutOfPlace = 1;
                Vector vPosition;
                Quaternion qOrientation;
                m_pAnimBase->SetPosition(vPosition);
                m_pAnimBase->SetOrientation(qOrientation);
                m_pAnimBase->SetAlwaysRender(1);
                m_pAnimBase->SetInCutscene(1);
                m_bPlayedOutOfPlace = 1;
            }
            if (AnimationOverlay(nAnimation))
                m_pAnimBase->SetOverlayAnimation(nAnimation, 1);
            else if (bOutOfPlace == 0)
                m_pAnimBase->SetAnimation(nAnimation, fSpeed,
                    Gob::ANIMATION_FLAG_NO_LOOP, 0);
            else
                m_pAnimBase->SetAnimation(nAnimation, fSpeed,
                    Gob::ANIMATION_FLAG_NO_LOOP | Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0);

            m_nAnimationTimer = nLength;
            if (pData != NULL)
                PlayHitSound((CSWCCombatAttackData*)pData, 1);
            RemoveTopFireForgetAnimation();
            if (pAttackData != NULL)
                delete pAttackData;
            if (pSpellData != NULL)
                delete pSpellData;
        }
    }
}
// PC 0x0061A970  Xbox 0x0004BFA0
void FootstepEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    static_cast<CSWCCreature*>(pData)->PlayFootstep();
}
// PC 0x0061A980  Xbox 0x0004BFB0
void CSWCCreature::AnimationUpdate()
{
    if (AnimationStationary(m_nCurrentAnimation) ||
        GetServerCreature() == NULL ||
        GetServerCreature()->m_nAnimationState != ANIMATION_STATE_MOVING)
    {
        if (m_oidLockTarget != CGameObjectArray::INVALIDOBJID)
            SetDesiredOrientationForLock();

        if (AnimationStationary(m_nCurrentAnimation))
            RotateCreatureToOrientation(m_vDesiredOrientation);
        else
            RotateCreatureToOrientation(m_vMoveOrientation);

        MoveCreatureToPosition(m_vDesiredPosition);
        if (!AnimationStationary(m_nCurrentAnimation))
        {
            int nAnimation = GetLoopingAnimation();
            SetAnimation(nAnimation, 0, 0);
        }

        AnimateFireAndForget();

        if (m_nCurrentAnimation == CREATURE_ANIMATION_COMBAT_READY &&
            m_nReadyAnimationState != READY_ANIMATION_STATE_2 &&
            m_nReadyAnimationState != READY_ANIMATION_STATE_3 &&
            m_nReadyAnimationState != READY_ANIMATION_STATE_14 &&
            m_nCombatState != COMBAT_STATE_ACTIVE)
        {
            SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 0);
        }

        if (AnimationPause(m_nCurrentAnimation) &&
            (m_nReadyAnimationState == READY_ANIMATION_STATE_2 ||
             m_nReadyAnimationState == READY_ANIMATION_STATE_3 ||
             m_nReadyAnimationState == READY_ANIMATION_STATE_14))
        {
            SetAnimation(CREATURE_ANIMATION_COMBAT_READY, 0, 0);
        }

        if (AnimationPause(m_nCurrentAnimation) &&
            m_nCurrentAnimation != CREATURE_ANIMATION_INJURED_IDLE_EXCLUDED)
        {
            unsigned long oidObject = m_nId;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            if (!pServer->IsInjured(oidObject))
                return;
            if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode !=
                    CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED ||
                this != g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0))
            {
                SetAnimation(CREATURE_ANIMATION_INJURED_IDLE, 0, 0);
            }
        }
    }
}
// PC 0x0061AB40  Xbox 0x0004C170
void CSWCCreature::RegisterCallbacks()
{
    Gob* pModel = static_cast<Gob*>(m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT));
    if (pModel != NULL)
    {
        m_apAnimationCallbacks[CALLBACK_FOOTSTEP] = pModel->RegisterCallBack(
            FootstepEvent, "snd_Footstep", this, SWCCREATURE_FOOTSTEP_CALLBACK_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_HIT] = pModel->RegisterCallBack(
            HitEvent, "hit", m_apAttackData[0], AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_HIT_GROUND] = pModel->RegisterCallBack(
            HitGroundEvent, "snd_hitground", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_SWING_SHORT] = pModel->RegisterCallBack(
            SwingShortEvent, "SwingShort", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_SWING_LONG] = pModel->RegisterCallBack(
            SwingLongEvent, "SwingLong", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_SWING_TWIRL] = pModel->RegisterCallBack(
            SwingTwirlEvent, "SwingTwirl", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_HIT_CLASH] = pModel->RegisterCallBack(
            HitClashEvent, "Clash", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_HIT_CONTACT] = pModel->RegisterCallBack(
            HitContactEvent, "Contact", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_HIT_PARRY] = pModel->RegisterCallBack(
            HitParryEvent, "HitParry", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_BLUR_START] = pModel->RegisterCallBack(
            Blur, "blur_start", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_BLUR_END] = pModel->RegisterCallBack(
            Unblur, "blur_end", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_DONE_ATTACK_01] = pModel->RegisterCallBack(
            Unblur, "doneattack01", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_DONE_ATTACK_02] = pModel->RegisterCallBack(
            Unblur, "doneattack02", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_GET_PERSONAL_RADIUS] = pModel->RegisterCallBack(
            ::GetPersonalRadius, "GetPersonalRadius", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_GET_CREATURE_RADIUS] = pModel->RegisterCallBack(
            ::GetCreatureRadius, "GetCreatureRadius", this, AUR_CALLBACK_LONG_DELAY, false);
        m_apAnimationCallbacks[CALLBACK_GET_PATH] = pModel->RegisterCallBack(
            ::GetPath, "GetPath", this, AUR_CALLBACK_LONG_DELAY, false);
    }
}
// PC 0x0061AD60  Xbox 0x0004C390
void CSWCCreature::AIUpdate()
{
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    unsigned long nTime, nDay, nDeltaTime, nDeltaDay;
    pClient->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    pClient->GetWorldTimer()->SubtractWorldTimes(nDay, nTime,
        m_nLastUpdateDay, m_nLastUpdateTime, &nDeltaDay, &nDeltaTime);
    m_nTimeDelta = nDeltaTime;

    if (m_nFeedbackTime != 0)
    {
        if (m_nFeedbackTime > m_nTimeDelta)
            m_nFeedbackTime -= m_nTimeDelta;
        else
            m_nFeedbackTime = 0;
    }

    if (m_nDrawAnimation != CREATURE_ANIMATION_CURRENT &&
        !g_pAppManager->GetClientExoApp()->GetPausedByCombat())
        PlayDrawAnimation(m_nDrawAnimation);

    if (m_pAnimBase != NULL)
    {
        if (m_nCurrentAnimation == CREATURE_ANIMATION_COMBAT_IDLE)
        {
            m_nAmbientPauseTimer += m_nTimeDelta;
            if (m_nAmbientPauseTimer >= m_nAmbientPauseDelay &&
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState == 0)
            {
                int nAnimation;
                if (m_pAnimBase->m_nAnimSet == 1)
                {
                    switch (rand() % 7)
                    {
                    case 0: nAnimation = 10053; break;
                    case 1: nAnimation = 10054; break;
                    case 2: nAnimation = 10055; break;
                    case 3: nAnimation = 10056; break;
                    default: nAnimation = 10147; break;
                    }
                }
                else
                {
                    switch (rand() % 5)
                    {
                    case 0: nAnimation = 10053; break;
                    case 1: nAnimation = 10054; break;
                    default: nAnimation = 10147; break;
                    }
                }
                AddFireForgetAnimation(nAnimation, &nAnimation);
                m_nAmbientPauseTimer = 0;

                m_nAmbientPauseDelay = 5000 * (rand() % 4 + 3);
            }
        }
        else
            m_nAmbientPauseTimer = 0;
    }

    UpdatePopUpBubble();

    if (m_nMode == 1 && !IsDriveAnimationBlocked())
    {
        CClientExoApp* pDriveClient = g_pAppManager->GetClientExoApp();
        if (!pDriveClient->GetInGameGui()->IsInDialog(pDriveClient->GetPlayerCreatureId()))
        {
            AnimateFireAndForget();
            DriveAnimationUpdate();
        }
        else
            AnimationUpdate();
    }
    else if (m_bInParty)
    {
        int nIndex = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(GetId());
#ifdef _XBOX
        if (nIndex == 1 || nIndex == 2)
        {
            g_pAppManager->GetClientExoApp()->GetSWParty();
            EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
        }
#else
        if (nIndex == 1 || nIndex == 2)
            g_pAppManager->GetClientExoApp()->GetSWParty();
#endif
        CSWSCreature* pFollowingCreature;
        if (m_bInParty && (pFollowingCreature = GetServerCreature()) != NULL &&
            pFollowingCreature->GetCurrentlyFollowingLeader())
        {
            AnimateFireAndForget();
            FollowAnimationUpdate();
            MoveCreatureToPosition(m_vDesiredPosition);
        }
        else
            AnimationUpdate();
    }
    else
        AnimationUpdate();

    if (GetId() != OBJECT_INVALID && m_pAnimBase != NULL &&
        GetId() == g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
        m_pAnimBase->Lean();

    if (m_bOutOfPlace == 0)
    {
        if ((m_pArea != NULL && m_pArea->GetIsObjectCorpse(GetId()) == 1) ||
            (GetServerCreature() != NULL &&
             (GetServerCreature()->GetDead() == 1 || GetServerCreature()->GetDeadTemp() == 1)))
        {
            Vector vNormal = g_pAppManager->GetServerExoApp()->GetNormalOfPoint(m_vPosition);
            float fPitch = Pitch(vNormal);
            if (fPitch <= SWCCREATURE_CORPSE_GROUND_PITCH_MAX &&
                fPitch >= SWCCREATURE_CORPSE_GROUND_PITCH_MIN)
                GetAnimationBase()->SetGroundTilt(vNormal, m_vOrientation);
            else
                GetAnimationBase()->SetGroundTilt(zaxis, m_vOrientation);
        }
        else if (m_pAppearance->m_bGroundTilt)
        {
            Vector vNormal = g_pAppManager->GetServerExoApp()->GetNormalOfPoint(m_vPosition);
            if (Pitch(vNormal) > SWCCREATURE_GROUND_PITCH_MIN)
                GetAnimationBase()->SetGroundTilt(vNormal, m_vOrientation);
            else
                GetAnimationBase()->SetGroundTilt(zaxis, m_vOrientation);
        }
        else
            GetAnimationBase()->SetGroundTilt(zaxis, m_vOrientation);
    }

    HandleConjuringCreature();
    UpdateVisualEffectsList();
    g_pAppManager->GetClientExoApp()->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;

    if (m_pLoopingSound != NULL && !m_bLoopingSoundPlaying)
    {
        m_pLoopingSound->SetPosition(m_vPosition, 1.5f);
        m_pLoopingSound->Set3D(1);
        m_pLoopingSound->Play();
        m_bLoopingSoundPlaying = 1;
    }

    m_nWeaponCheckTimer -= m_nTimeDelta;
    if (m_nWeaponCheckTimer <= 0)

        m_nWeaponCheckTimer = 6000;
    m_nWeaponOverlayTimer -= m_nTimeDelta;
    if (m_nWeaponOverlayTimer <= 0)
    {
        m_nWeaponOverlayTimer = SWCCREATURE_WEAPON_OVERLAY_INTERVAL;
        ComputeWeaponOverlays(1, 1);
    }

    if (enablemodelrequest)
    {
        g_pAppManager->GetClientExoApp()->GetModule();
        if (m_nCameraFollowDelay > 0)
        {
            if (m_nCameraFollowDelay == 1)
                m_nCameraFollowDelay = 0;
            if (m_nCameraFollowDelay <= m_nTimeDelta)
                ForceAppearanceUpdate();
            else
                m_nCameraFollowDelay -= m_nTimeDelta;
        }
        ReAttachCamera();
    }

    if (FadeObject())
    {
        CClientAIMaster* pAIMaster = g_pAppManager->GetClientExoApp()->GetClientAIMaster();
        pAIMaster->m_aiList.RemoveObject(this);
        CGameObject* pObject;
        if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->Delete(GetId(), &pObject) ==
            CGameObjectArray::SUCCESS && pObject != NULL)
        {
            delete this;
            return;
        }
    }

    UpdateFloatyText();
    if ((m_nFadeState == FADE_STATE_VISIBLE &&
         g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nWindMode ==
             CClientOptions::WIND_ALL) ||
        (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nWindMode ==
             CClientOptions::WIND_PLAYER &&
         g_pAppManager->GetClientExoApp()->GetPlayerCreatureId() == GetId()))
        SetWind(1, creaturewindintensity);
    else
        SetWind(0, 0.0f);

    if (m_pAnimBase != NULL)
        m_pAnimBase->UpdateSound(GetId() == g_pAppManager->GetClientExoApp()->GetPlayerCreatureId());
    UpdateRollingFootstepSound();
}
// PC 0x0061B380  Xbox 0x0004C9D0
int CSWCCreature::LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nAnimType)
{
    if (m_pPendingAnimBase != NULL)
    {
        if (m_pAnimBase != NULL)
        {
            delete m_pAnimBase;
            m_pAnimBase = NULL;
        }
        m_pAnimBase = m_pPendingAnimBase;
        m_pPendingAnimBase = NULL;
        m_fSpeedFactor = m_fPendingSpeedFactor;
        m_fPendingSpeedFactor = 0.0f;
    }

    if (m_pAnimBase == NULL || m_pAnimBase->m_nAnimationType != nAnimType)
    {
        if (m_pAnimBase != NULL)
        {
            delete m_pAnimBase;
            m_pAnimBase = NULL;
        }
        switch (nAnimType)
        {
        case CSWCAnimBase::ANIMATION_TYPE_BASE:
            m_pAnimBase = new CSWCAnimBase;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_TAIL_WINGS:
            m_pAnimBase = new CSWCAnimBaseTW;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_HEAD:
            m_pAnimBase = new CSWCAnimBaseHead;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_WIELD:
            m_pAnimBase = new CSWCAnimBaseWield;
            break;
        case CSWCAnimBase::ANIMATION_TYPE_BTYPE:
            m_pAnimBase = new CSWCAnimBaseHeadWield;
            break;
        default:
            return 0;
        }
        if (m_pAnimBase == NULL)
            return 0;

        m_pAnimBase->Set(CSWAnimBase::VALUE_FIRST, 1850.0f);
        m_pAnimBase->Set(CSWAnimBase::VALUE_SECOND, 6600.0f);
        m_pAnimBase->Set(CSWAnimBase::VALUE_THIRD, 0.925f);
        m_pAnimBase->Set(CSWAnimBase::VALUE_FOURTH, 3.3f);
        m_fSpeedFactor = 0.0f;
    }

    if (!m_pAnimBase->LoadModel(cModel, nModel))
    {
        char sError[256];
        sprintf(sError, "CSWCCreature::LoadModel(): Failed to load creature model '%s'.",
                cModel.GetResRefStr());
        return 0;
    }

    if (nModel == CSWCAnimBase::MODEL_SLOT_MAIN ||
        nModel == CSWCAnimBase::MODEL_SLOT_HEAD ||
        nModel == CSWCAnimBase::MODEL_TAIL ||
        nModel == CSWCAnimBase::MODEL_WINGS)
    {
        Gob* pModel = m_pAnimBase->GetModel(nModel);
        pModel->SetExternalData(this);
        pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_CREATURE);
        if (nModel == CSWCAnimBase::MODEL_SLOT_MAIN)
        {
            Quaternion qOrientation;
            Vector vPosition;
            RegisterCallbacks();
            if (pModel->GetPartLocalPosition("headconjure", &vPosition, &qOrientation))
                m_fHeadHitHeight = vPosition.m_fZ - vPosition.m_fZ * 0.125f;
            else
                m_fHeadHitHeight = 2.1f;
        }
    }
    return 1;
}

static inline void NwccreatureTrace3()
{
    NwccreatureTrace2();
}

static inline void NwccreatureTrace2()
{
    NwccreatureTrace1();
}

static inline void NwccreatureTrace1()
{
}
