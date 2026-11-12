#include "exobase.h"

#include "exoresman.h"
#include "resgff.h"
#include "appmanager.h"
#include "nwsitem.h"
#include "nwsarea.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwsplaceable.h"
#include "worldtimer.h"
#include "nwcplaceable.h"
#include "nwsplayer.h"
#include "nwscript.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "gameobjectarray.h"
#include "serverexoapp.h"
#include "gameobjectarray.h"
#include "nwitem.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "nwmessage.h"
#include "serveraimaster.h"
#include "nwsscriptevent.h"
#include "nwsmodule.h"
#include "nwccreature.h"
#include "nwstrigger.h"
#include "nwctrigger.h"
#include "nwspell.h"
#include "nwitem.h"
#include "nwcanimbase.h"
#include "nwscombatround.h"
#include "clientoptions.h"

static inline void NwsplaceableTrace1();

static inline CServerExoApp* GetServerExoApp();

int RenderPlaceableBBox = 0;
// PC 0x0058A400
CSWSPlaceable::CSWSPlaceable(unsigned long oidSelf)
    : CSWSObject(CGameObject::OBJECT_TYPE_PLACEABLE, oidSelf, 0)
{
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->AddObject(this, 0);
    m_n32C = 0;
    m_bOpen = 0;
    m_nHeartbeatDay = 0;
    m_nHeartbeatTime = 0;
    m_oidLastDisarmedBy = OBJECT_INVALID;
    m_oidLastLockedBy = OBJECT_INVALID;
    m_oidLastUnlockedBy = OBJECT_INVALID;
    m_oidLastTriggeredBy = OBJECT_INVALID;
    m_oidLastOpenedBy = OBJECT_INVALID;
    m_oidLastClosedBy = OBJECT_INVALID;
    m_oidLastUsedBy = OBJECT_INVALID;
    m_pcItemRepository = new CItemRepository(m_nId, 0);
    m_bTrapFlagged = 0;
    m_bTrapDetectable = 1;
    m_bTrapDisarmable = 1;
    m_bTrapOneShot = 1;
    m_nOpenLockDC = 0;
    m_nTrapType = -1;
    m_bGroundPile = 0;
    m_bDieWhenEmpty = 0;
    m_nInventoryIndex = 0;
    m_nInventoryContainerIndex = 0;
    m_oidInventoryContainer = CGameObjectArray::INVALIDOBJID;
    m_nStaticBoundingBox = -1;
    m_cTemplateResRef = "";
    m_EffectSpellId = -1;
    m_fBearing = 0.0f;
    m_bLightIsOn = 1;
    m_n390 = 0;
    m_bDynamic = 0;
    if (oidSelf == OBJECT_INVALID)
        m_bDynamic = 1;
    m_nBodyBag = 0;
    m_n438 = 0;
    m_n43C = 0;
    m_bPreciseUse = 0;
    m_bIsBodyBag = 0;
    m_bIsBodyBagVisible = 1;
    m_oidBodyBagCreature = OBJECT_INVALID;
    m_bIsCorpse = 0;
    m_bInventoryOpening = 0;
}
// PC 0x0058A6B0
int CSWSPlaceable::LoadFromTemplate(const CResRef& cResRef, int bLoadState)
{
    CResStruct stTop;
    CResGFF* pRes = new CResGFF(RESTYPE_UTP, "UTP ", cResRef);

    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        CExoString sMessage;
        CExoString sResRef;
        cResRef.CopyToString(sResRef);
        sMessage.Format("Item template %s doesn't exist.\n", sResRef.CStr());
        return 0;
    }
    else
    {
        pRes->GetTopLevelStruct(&stTop);
        LoadPlaceable(pRes, &stTop, bLoadState);
        delete pRes;
        return 1;
    }
}
// PC 0x0057F480
int CSWSPlaceable::GetDead()
{
    if (m_bPlotObject != 0)
        return 0;
    return CSWSObject::GetDead();
}
// PC 0x00587DD0
CSWSPlaceable::~CSWSPlaceable()
{
    if (m_pcItemRepository != NULL)
    {
        delete m_pcItemRepository;
        m_pcItemRepository = NULL;
    }

    if (GetArea() != NULL)
        RemoveFromArea();

    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->RemoveObject(this);

    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        unsigned long oidObject = m_nId;
        pModule->RemoveObjectFromLookupTable(m_sTag, oidObject);
    }

    if (m_bIsBodyBag)
    {
        unsigned long oidCreature = g_pAppManager->m_pClientExoApp
            ->ServerToClientObjectId(m_oidBodyBagCreature);
        CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp
            ->GetCreatureByGameObjectID(oidCreature);
        if (pCreature != NULL)
            pCreature->m_oidParentCorpse = OBJECT_INVALID;
    }
}
// PC 0x00587F90
int CSWSPlaceable::LoadPlaceable(CResGFF* pRes, CResStruct* pStruct, int bLoadState)
{
    CExoString sScript;
    int bSuccess;
    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, ""));
    m_cTemplateResRef = pRes->ReadFieldCResRef(pStruct, "TemplateResRef", bSuccess, "");
    m_lsLocName = pRes->ReadFieldCExoLocString(pStruct, "LocName", bSuccess, CExoLocString());
    int bAutoRemoveKey = pRes->ReadFieldBYTE(pStruct, "AutoRemoveKey", bSuccess, 0);
    m_bAutoRemoveKey = bAutoRemoveKey;
    m_nFaction = pRes->ReadFieldDWORD(pStruct, "Faction", bSuccess, 0);
    int bInvulnerable = pRes->ReadFieldBYTE(pStruct, "Invulnerable", bSuccess, GetPlotObject());
    if (!bSuccess)
        bInvulnerable = pRes->ReadFieldBYTE(pStruct, "Plot", bSuccess, GetPlotObject());
    m_bNotBlastable = pRes->ReadFieldBYTE(pStruct, "NotBlastable", bSuccess, GetNotBlastable());
    m_bMin1HP = pRes->ReadFieldBYTE(pStruct, "Min1HP", bSuccess, GetMin1HP());
    int bPartyInteract = pRes->ReadFieldBYTE(pStruct, "PartyInteract", bSuccess, 0);
    m_bPartyInteract = bPartyInteract;
    m_nOpenLockDC = pRes->ReadFieldBYTE(pStruct, "OpenLockDC", bSuccess, 0);
    m_nOpenLockDiff = pRes->ReadFieldBYTE(pStruct, "OpenLockDiff", bSuccess, 0);
    m_nOpenLockDiffMod = pRes->ReadFieldCHAR(pStruct, "OpenLockDiffMod", bSuccess, 0);
    m_sKeyName = pRes->ReadFieldCExoString(pStruct, "KeyName", bSuccess, "");
    int bTrapDisarmable = pRes->ReadFieldBYTE(pStruct, "TrapDisarmable", bSuccess, 0);
    m_bTrapDisarmable = bTrapDisarmable;
    int bTrapDetectable = pRes->ReadFieldBYTE(pStruct, "TrapDetectable", bSuccess, 0);
    m_bTrapDetectable = bTrapDetectable;
    m_nDisarmDC = pRes->ReadFieldBYTE(pStruct, "DisarmDC", bSuccess, 0);
    m_nTrapDetectDC = pRes->ReadFieldBYTE(pStruct, "TrapDetectDC", bSuccess, 0);
    m_nOwnerDemolitionsSkill = pRes->ReadFieldINT(pStruct, "OwnerDemolitionsSkill", bSuccess, 0);
    int nTrapFlag = pRes->ReadFieldBYTE(pStruct, "TrapFlag", bSuccess, 0);
    m_nTrapFlag = nTrapFlag;
    int bTrapOneShot = pRes->ReadFieldBYTE(pStruct, "TrapOneShot", bSuccess, 0);
    m_bTrapOneShot = bTrapOneShot;
    m_nTrapType = pRes->ReadFieldBYTE(pStruct, "TrapType", bSuccess, GetTrapType());
    int bUseableValue = pRes->ReadFieldBYTE(pStruct, "Useable", bSuccess, 0);
    m_bUseable = bUseableValue;
    int bStatic = pRes->ReadFieldBYTE(pStruct, "Static", bSuccess, 0);
    if (!bSuccess)
    {
        int bUseable = m_bUseable;
        bStatic = bUseable != 1;
    }
    if (m_bDynamic == 1)
        bStatic = 0;
    m_bStatic = bStatic;
    m_bPlotObject = bStatic == 1 ? 1 : bInvulnerable;
    m_nAppearance = pRes->ReadFieldDWORD(pStruct, "Appearance", bSuccess, 0);
    m_bUseTweakColor = pRes->ReadFieldBYTE(pStruct, "UseTweakColor", bSuccess, 0);
    m_nTweakColor = pRes->ReadFieldDWORD(pStruct, "TweakColor", bSuccess, 0);
    int nHitPoints = pRes->ReadFieldSHORT(pStruct, "HP", bSuccess, 0);
    m_nMaxHitPoints = nHitPoints;
    SetCurrentHitPoints(pRes->ReadFieldSHORT(pStruct, "CurrentHP", bSuccess, 0));
    m_nHardness = pRes->ReadFieldBYTE(pStruct, "Hardness", bSuccess, 0);
    m_nFortSave = pRes->ReadFieldBYTE(pStruct, "Fort", bSuccess, 0);
    m_nWillSave = pRes->ReadFieldBYTE(pStruct, "Will", bSuccess, 0);
    m_nReflexSave = pRes->ReadFieldBYTE(pStruct, "Ref", bSuccess, 0);
    int bLockable = pRes->ReadFieldBYTE(pStruct, "Lockable", bSuccess, 0);
    m_bLockable = bLockable;
    SetLocked(pRes->ReadFieldBYTE(pStruct, "Locked", bSuccess, 0));
    int bHasInventory = pRes->ReadFieldBYTE(pStruct, "HasInventory", bSuccess, 0);
    m_bHasInventory = bHasInventory;
    int bKeyRequired = pRes->ReadFieldBYTE(pStruct, "KeyRequired", bSuccess, 0);
    m_bKeyRequired = bKeyRequired;
    m_nCloseLockDC = pRes->ReadFieldBYTE(pStruct, "CloseLockDC", bSuccess, 0);
    unsigned short nPortraitId = pRes->ReadFieldWORD(pStruct, "PortraitId", bSuccess, PORTRAIT_ID_INVALID);
    if (nPortraitId >= PORTRAIT_ID_RESREF_FIRST)
        SetPortrait(pRes->ReadFieldCResRef(pStruct, "Portrait", bSuccess, ""));
    else
        SetPortraitId(nPortraitId);
    m_cConversation = pRes->ReadFieldCResRef(pStruct, "Conversation", bSuccess, "");
    m_nBodyBag = pRes->ReadFieldBYTE(pStruct, "BodyBag", bSuccess, GetBodyBag());
    int bDieWhenEmpty = pRes->ReadFieldBYTE(pStruct, "DieWhenEmpty", bSuccess, 0);
    m_bDieWhenEmpty = bDieWhenEmpty;
    pRes->ReadFieldBYTE(pStruct, "GroundPile", bSuccess, GetGroundPile());
    m_bGroundPile = 1;
    SetLightIsOn(pRes->ReadFieldBYTE(pStruct, "LightState", bSuccess, GetLightIsOn()));
    m_lsDescription = pRes->ReadFieldCExoLocString(pStruct, "Description", bSuccess, CExoLocString());
    pRes->ReadFieldCResRef(pStruct, "OnClosed", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONCLOSED] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnDamaged", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONDAMAGED] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnDeath", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONDEATH] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnDisarm", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONDISARM] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnHeartbeat", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONHEARTBEAT] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnInvDisturbed", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONINVDISTURBED] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnLock", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONLOCK] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnMeleeAttacked", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONMELEEATTACKED] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnOpen", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONOPEN] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnSpellCastAt", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONSPELLCASTAT] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnUnlock", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONUNLOCK] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnUsed", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONUSED] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnUserDefined", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONUSERDEFINED] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnDialog", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONDIALOG] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnEndDialogue", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONENDDIALOGUE] = sScript;
    pRes->ReadFieldCResRef(pStruct, "OnTrapTriggered", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONTRAPTRIGGERED] = sScript;
    CExoString* pTrapScript = &m_sScripts[SCRIPT_ONTRAPTRIGGERED];
    if (pTrapScript == NULL || pTrapScript->IsEmpty() || *pTrapScript == "default")
    {
        CExoString sMineScript;
        g_pRules->Get2DArrays()->m_pTraps->GetCExoStringEntry(GetTrapType(), "MineScript", &sMineScript);
        m_sScripts[SCRIPT_ONTRAPTRIGGERED] = sMineScript;
    }
    pRes->ReadFieldCResRef(pStruct, "OnFailToOpen", bSuccess, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ONFAILTOOPEN] = sScript;
    int bIsBodyBag = pRes->ReadFieldBYTE(pStruct, "IsBodyBag", bSuccess, GetIsBodyBag());
    m_bIsBodyBag = bIsBodyBag;
    m_bIsCorpse = pRes->ReadFieldBYTE(pStruct, "IsCorpse", bSuccess, GetIsCorpse());
    m_c251 = pRes->ReadFieldBYTE(pStruct, "PCLevelAtSpawn", bSuccess, m_c251);
    m_bOpen = pRes->ReadFieldBYTE(pStruct, "Open", bSuccess, 0);
    if (m_bOpen)
        SetAnimation(PLACEABLE_ANIMATION_OPEN);
    else
    {
        int nAnimation = pRes->ReadFieldINT(pStruct, "Animation", bSuccess, 0);
        if (bSuccess)
            SetAnimation(nAnimation);
        else
        {
            int nState = pRes->ReadFieldBYTE(pStruct, "AnimationState", bSuccess, 0);
            if (bSuccess)
            {
                switch (nState)
                {
                case ANIMATION_STATE_BASE: nAnimation = PLACEABLE_ANIMATION_BASE; break;
                case ANIMATION_STATE_OPEN: nAnimation = PLACEABLE_ANIMATION_OPEN; break;
                case ANIMATION_STATE_CLOSED: nAnimation = PLACEABLE_ANIMATION_CLOSED; break;
                case ANIMATION_STATE_DESTROYED: nAnimation = PLACEABLE_ANIMATION_DESTROYED; break;
                case ANIMATION_STATE_ACTIVATE: nAnimation = PLACEABLE_ANIMATION_ACTIVATE; break;
                case ANIMATION_STATE_DEACTIVATE: nAnimation = PLACEABLE_ANIMATION_DEACTIVATE; break;
                default: nAnimation = PLACEABLE_ANIMATION_BASE; break;
                }
                SetAnimation(nAnimation);
            }
        }

    }
    CResList lstItems;
    CResStruct itemStruct;
    if (pRes->GetList(&lstItems, pStruct, "ItemList") == 1)
    {
        for (unsigned long nItem = 0; nItem < pRes->GetListCount(&lstItems); ++nItem)
        {
            CSWSItem* pItem = NULL;
            pRes->GetListElement(&itemStruct, &lstItems, nItem);
            unsigned long oidItem = pRes->ReadFieldDWORD(&itemStruct, "ObjectId", bSuccess, OBJECT_INVALID);
            pItem = new CSWSItem(oidItem);
            if (pItem != NULL)
            {
                if (bLoadState)
                {
                    CResRef cInventoryRes = pRes->ReadFieldCResRef(&itemStruct, "InventoryRes", bSuccess, "");
                    if (!bSuccess || !pItem->LoadFromTemplate(cInventoryRes))
                    {
                        delete pItem;
                        pItem = NULL;
                    }
                }
                else if (!pItem->LoadItem(pRes, &itemStruct))
                {
                    delete pItem;
                    pItem = NULL;
                }
                if (pItem != NULL)
                    AcquireItem(&pItem, OBJECT_INVALID, 1);
            }
        }
    }
    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(m_sTag, GetId());
    PostProcess();
    return 1;
}
// PC 0x00588EE0
int CSWSPlaceable::LoadBodyBag(unsigned char nAppearance)
{
    CExoLocString lsName;
    CExoString sScript;
    int nStrRef;
    bool bFailed;
    {
        CExoString sColumn("StrRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetINTEntry(nAppearance, sColumn, &nStrRef);
    }
    if (bFailed)
        nStrRef = 0;
    lsName.m_dwStringRef = nStrRef;
    {
        CResRef cEmpty("");
        m_cTemplateResRef = cEmpty;
    }
    SetTag("Body Bag");
    m_lsLocName = lsName;
    m_bAutoRemoveKey = 0;
    m_nFaction = 1;
    m_bTrapDisarmable = 1;
    m_bTrapDetectable = 1;
    m_nDisarmDC = BODY_BAG_DISARM_DC;
    m_nTrapDetectDC = 0;
    m_nTrapFlag = 0;
    m_bTrapOneShot = 1;
    m_nTrapType = 0;
    m_bPlotObject = 0;
    m_nOpenLockDC = BODY_BAG_OPEN_LOCK_DC;
    m_nOpenLockDiff = 1;
    m_nOpenLockDiffMod = 0;
    {
        CExoString sEmpty("");
        m_sKeyName = sEmpty;
    }
    m_nAppearance = nAppearance;
    m_nMaxHitPoints = BODY_BAG_HIT_POINTS;
    SetCurrentHitPoints(BODY_BAG_HIT_POINTS);
    m_nFortSave = BODY_BAG_FORTITUDE_SAVE;
    m_nWillSave = 0;
    m_nReflexSave = 0;
    m_bLockable = 0;
    m_nHardness = nAppearance == BODY_BAG_LOW_HARDNESS_APPEARANCE ? 1 : 5;
    m_bLocked = 0;
    m_bUseable = 1;
    m_bHasInventory = 1;
    m_bKeyRequired = 0;
    m_nCloseLockDC = 0;
    SetPortrait("PO_PLC_B04_");
    {
        CResRef cEmpty("");
        m_cConversation = cEmpty;
    }
    lsName.ClearLocString();
    lsName.m_dwStringRef = BODY_BAG_DESCRIPTION_STRREF;
    m_lsDescription = lsName;
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONTRAPTRIGGERED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONCLOSED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONDAMAGED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONDEATH] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONDISARM] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONHEARTBEAT] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONINVDISTURBED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONLOCK] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONMELEEATTACKED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONOPEN] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONSPELLCASTAT] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONUNLOCK] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONUSED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONUSERDEFINED] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONDIALOG] = sEmpty;
    }
    {
        CExoString sEmpty("");
        m_sScripts[SCRIPT_ONENDDIALOGUE] = sEmpty;
    }
    m_bStatic = 0;
    SetAnimation(PLACEABLE_ANIMATION_BASE);
    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(m_sTag, GetId());
    PostProcess();
    return 1;
}
// PC 0x00586EA0
void CSWSPlaceable::PostProcess()
{
    if (m_nHardness > 0)
    {
        m_n1EC = 1;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE;
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                              CGameEffect::DURATION_TYPE_PERMANENT;
        pEffect->SetInteger(0, CSWSEffectListHandler::DAMAGE_FLAG_ALL);
        pEffect->SetInteger(1, m_nHardness);
        pEffect->m_bExposed = 0;
        ApplyEffect(pEffect, 0, 0);
        m_n1EC = 0;
    }
}
// PC 0x005894A0
int CSWSPlaceable::SavePlaceable(CResGFF* pRes, CResStruct* pStruct)
{
    CResList lstItems;
    CResStruct itemStruct;
    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldCExoLocString(pStruct, *GetFirstName(), "LocName");
    pRes->WriteFieldBYTE(pStruct, m_bAutoRemoveKey, "AutoRemoveKey");
    pRes->WriteFieldDWORD(pStruct, m_nFaction, "Faction");
    pRes->WriteFieldBYTE(pStruct, m_bPlotObject, "Plot");
    pRes->WriteFieldBYTE(pStruct, m_bNotBlastable, "NotBlastable");
    pRes->WriteFieldBYTE(pStruct, m_bMin1HP, "Min1HP");
    pRes->WriteFieldBYTE(pStruct, m_nOpenLockDC, "OpenLockDC");
    pRes->WriteFieldBYTE(pStruct, m_nOpenLockDiff, "OpenLockDiff");
    pRes->WriteFieldCHAR(pStruct, m_nOpenLockDiffMod, "OpenLockDiffMod");
    pRes->WriteFieldCExoString(pStruct, m_sKeyName, "KeyName");
    pRes->WriteFieldBYTE(pStruct, m_bTrapDisarmable, "TrapDisarmable");
    pRes->WriteFieldBYTE(pStruct, m_bTrapDetectable, "TrapDetectable");
    pRes->WriteFieldBYTE(pStruct, GetDisarmDC(), "DisarmDC");
    pRes->WriteFieldBYTE(pStruct, GetDetectDC(), "TrapDetectDC");
    pRes->WriteFieldBYTE(pStruct, m_nTrapFlag, "TrapFlag");
    pRes->WriteFieldBYTE(pStruct, m_bTrapOneShot, "TrapOneShot");
    pRes->WriteFieldBYTE(pStruct, m_nTrapType, "TrapType");
    pRes->WriteFieldINT(pStruct, m_nOwnerDemolitionsSkill, "OwnerDemolitionsSkill");
    pRes->WriteFieldBYTE(pStruct, m_bUseable, "Useable");
    pRes->WriteFieldBYTE(pStruct, m_bStatic, "Static");
    pRes->WriteFieldBYTE(pStruct, m_bGroundPile, "GroundPile");
    pRes->WriteFieldDWORD(pStruct, m_nAppearance, "Appearance");
    pRes->WriteFieldBYTE(pStruct, m_bUseTweakColor, "UseTweakColor");
    pRes->WriteFieldDWORD(pStruct, m_nTweakColor, "TweakColor");
    pRes->WriteFieldSHORT(pStruct, m_nMaxHitPoints, "HP");
    pRes->WriteFieldSHORT(pStruct, GetCurrentHitPoints(1), "CurrentHP");
    pRes->WriteFieldBYTE(pStruct, m_nHardness, "Hardness");
    pRes->WriteFieldBYTE(pStruct, m_nFortSave, "Fort");
    pRes->WriteFieldBYTE(pStruct, m_nWillSave, "Will");
    pRes->WriteFieldBYTE(pStruct, m_nReflexSave, "Ref");
    pRes->WriteFieldBYTE(pStruct, m_bLockable, "Lockable");
    pRes->WriteFieldBYTE(pStruct, m_bLocked, "Locked");
    pRes->WriteFieldBYTE(pStruct, m_bHasInventory, "HasInventory");
    pRes->WriteFieldBYTE(pStruct, m_bKeyRequired, "KeyRequired");
    pRes->WriteFieldBYTE(pStruct, m_nCloseLockDC, "CloseLockDC");
    pRes->WriteFieldBYTE(pStruct, m_bOpen, "Open");
    pRes->WriteFieldBYTE(pStruct, m_bPartyInteract, "PartyInteract");
    if (GetPortraitId() == (unsigned short)-1)
        pRes->WriteFieldCResRef(pStruct, GetPortrait(), "Portrait");
    else
        pRes->WriteFieldWORD(pStruct, GetPortraitId(), "PortraitId");
    pRes->WriteFieldCResRef(pStruct, GetDialogResref(), "Conversation");
    pRes->WriteFieldBYTE(pStruct, m_nBodyBag, "BodyBag");
    pRes->WriteFieldBYTE(pStruct, m_bDieWhenEmpty, "DieWhenEmpty");
    pRes->WriteFieldBYTE(pStruct, GetLightIsOn(), "LightState");
    pRes->WriteFieldCExoLocString(pStruct, m_lsDescription, "Description");
    {
        CResRef cScript(m_sScripts[SCRIPT_ONCLOSED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnClosed");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONDAMAGED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDamaged");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONDEATH]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDeath");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONDISARM]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDisarm");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONHEARTBEAT]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnHeartbeat");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONINVDISTURBED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnInvDisturbed");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONLOCK]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnLock");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONMELEEATTACKED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnMeleeAttacked");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONOPEN]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnOpen");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONSPELLCASTAT]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnSpellCastAt");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONUNLOCK]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnUnlock");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONUSED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnUsed");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONUSERDEFINED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnUserDefined");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONDIALOG]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDialog");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONENDDIALOGUE]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnEndDialogue");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONTRAPTRIGGERED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnTrapTriggered");
    }
    {
        CResRef cScript(m_sScripts[SCRIPT_ONFAILTOOPEN]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnFailToOpen");
    }
    pRes->WriteFieldINT(pStruct, m_nAnimation, "Animation");
    if ((unsigned int)m_pcItemRepository->GetNumItems() > 0)
    {
        pRes->AddList(&lstItems, pStruct, "ItemList");
        for (unsigned int nItem = 0; nItem < m_pcItemRepository->GetNumItems(); ++nItem)
        {
            pRes->AddListElement(&itemStruct, &lstItems, 0);
            CSWSItem* pItem = m_pcItemRepository->ItemListGetItem(nItem);
            pRes->WriteFieldDWORD(&itemStruct, pItem->GetId(), "ObjectId");
            pItem->SaveItem(pRes, &itemStruct);
        }
    }

    float fBearing = Yaw(m_vOrientation) * DEG_TO_RAD;
    m_fBearing = fBearing;
    pRes->WriteFieldFLOAT(pStruct, fBearing, "Bearing");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "X");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "Y");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "Z");
    pRes->WriteFieldBYTE(pStruct, m_bIsBodyBag, "IsBodyBag");
    pRes->WriteFieldBYTE(pStruct, m_bIsBodyBagVisible, "IsBodyBagVisible");
    pRes->WriteFieldBYTE(pStruct, m_bIsCorpse, "IsCorpse");
    pRes->WriteFieldBYTE(pStruct, m_c251, "PCLevelAtSpawn");
    return 1;
}
// PC 0x00589C60
void CSWSPlaceable::AddToArea(CSWSArea* pArea, const Vector& vPosition,
                            int bLoadingGame)
{
    RemoveFromArea();
    if (pArea != NULL)
    {
        SetArea(pArea);
        SetPosition(vPosition, 0, 1, 0);

        CExoString sModelName;
        int nPreciseUse;
        int nHostile;
        {
            CExoString sColumn("PreciseUse");
            unsigned char nAppearance = m_nAppearance;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            pPlaceables->GetINTEntry(nAppearance, sColumn, &nPreciseUse);
        }
        int bPreciseUse = nPreciseUse == 1;
        m_bPreciseUse = bPreciseUse;
        {
            CExoString sColumn("Hostile");
            unsigned char nAppearance = m_nAppearance;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            pPlaceables->GetINTEntry(nAppearance, sColumn, &nHostile);
        }
        int bHostile = nHostile == 1;
        m_bHostile = bHostile;
        {
            CExoString sColumn("ModelName");
            unsigned char nAppearance = m_nAppearance;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            pPlaceables->GetCExoStringEntry(nAppearance, sColumn, &sModelName);
        }
        CResRef cModelResRef(sModelName);
        m_cSurfaceMesh.m_cResRef = cModelResRef;
        if (m_cSurfaceMesh.LoadMesh(0))
        {
            Vector vMinimum;
            Vector vMaximum;
            m_cSurfaceMesh.m_vWorldPosition = m_vPosition;
            Vector vAxis(0.0f, 0.0f, 1.0f);
            m_cSurfaceMesh.m_qOrientation =
                Quaternion(vAxis,
                           Yaw(m_vOrientation) * DEG_TO_RAD);
            m_cSurfaceMesh.GetMeshBoundingBox(vMinimum, vMaximum);
            if (RenderPlaceableBBox)
                m_cSurfaceMesh.RenderMesh(0, CSWCollisionMesh::RENDER_COLOR_RED,
                    CSWCollisionMesh::RENDER_COLOR_GREEN);
            unsigned long oidObject = m_nId;
            m_nStaticBoundingBox = pArea->AddStaticBoundingBox(
                oidObject, vMinimum, vMaximum);
        }
        GetArea()->AddObjectToArea(m_nId, 1);
    }
}
// PC 0x00586F80
void CSWSPlaceable::RemoveFromArea()
{
    if (GetArea() != NULL)
    {
        if (m_nStaticBoundingBox >= 0)
        {
            GetArea()->RemoveStaticBoundingBox(m_nId);
            m_nStaticBoundingBox = -1;
        }
        unsigned long oidObject = m_nId;
        GetArea()->RemoveObjectFromArea(oidObject);
    }

    SetArea(NULL);
    Vector vPosition;
    SetPosition(vPosition, 1, 1, 0);
}
// PC 0x00587000
void CSWSPlaceable::AIUpdate()
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    unsigned long nDifferenceDay;
    unsigned long nDifferenceTime;
    unsigned __int64 nTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();

    {
        CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
        pServerApp->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    }
    CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->SubtractWorldTimes(
        nCalendarDay, nTimeOfDay, m_nHeartbeatDay, m_nHeartbeatTime,
        &nDifferenceDay, &nDifferenceTime);
    if (nDifferenceTime >= HEARTBEAT_INTERVAL)
    {
        if (m_nHeartbeatTime != 0 && GetDead() == 0)
        {
            if (m_c251 == 0)
            {
                char nPCLevel = 0;
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(
                    "G_PC_LEVEL", nPCLevel);
                m_c251 = nPCLevel;
            }
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONHEARTBEAT], m_nId, 1);
        }
        m_nHeartbeatDay = nCalendarDay;
        m_nHeartbeatTime = nTimeOfDay;
    }
    else if (m_nHeartbeatTime == 0 && GetDead() == 0)
    {
        g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONHEARTBEAT], m_nId, 1);
        m_nHeartbeatTime = 1;
    }
    UpdateEffectList(nCalendarDay, nTimeOfDay);
    RunActions(nCalendarDay, nTimeOfDay, nTimer);
}
// PC 0x005871B0
int CSWSPlaceable::RemoveItem(CSWSItem* pItem)
{
    CItemRepository* pRepository = m_pcItemRepository;
    if (pRepository->GetItemInRepository(pItem, 0) == 1)
    {
        m_pcItemRepository->RemoveItem(pItem);
        pItem->SetPossessor(OBJECT_INVALID, 1, 1, OBJECT_INVALID);
        return 1;
    }

    return 0;
}
// PC 0x00587200
int CSWSPlaceable::AcquireItem(CSWSItem** ppItem, unsigned long oidPossessor,
                               int bDisplayFeedback)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CGameObject* pPossessor = NULL;
    CItemRepository* pRepository = m_pcItemRepository;
    unsigned long oidPreviousPossessor = OBJECT_INVALID;

    if ((*ppItem)->m_oidPossessor != oidPossessor)
        return 0;

    if (pObjectArray->GetGameObject(oidPossessor, &pPossessor) ==
            CGameObjectArray::SUCCESS && pPossessor != NULL)
    {
        oidPreviousPossessor = pPossessor->m_nId;
        if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
            pPossessor->AsSWSCreature()->RemoveItem(
                *ppItem, 1, bDisplayFeedback, ppItem);
        else if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
            pPossessor->AsSWSPlaceable()->RemoveItem(*ppItem);
        else if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_ITEM)
        {
            int nBaseItem = pPossessor->AsSWSItem()->m_nBaseItem;
            if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_bIsContainer == 1)
            {
                CSWSItem* pContainer = pPossessor->AsSWSItem();
                pContainer->m_pRepository->RemoveItem(*ppItem);
            }
        }
    }
    else
        (*ppItem)->RemoveFromArea();

    if (pRepository != NULL)
    {
        pRepository->AddItem(ppItem, 1, 0, 1);
        if (*ppItem != NULL)
            (*ppItem)->SetPossessor(m_nId, 1, 1, oidPreviousPossessor);
        return 1;
    }
    return 0;
}
// PC 0x005873C0
unsigned char CSWSPlaceable::GetDetectDC()
{
    unsigned char nDC = m_nTrapDetectDC;
    int nModifier = 0;
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
    {
        case 0:
            nModifier = -5;
            break;
        case 2:
            nModifier = 5;
            break;
    }
    nDC += nModifier;
    if (nDC < 1)
    {
        return 1;
    }
    return nDC;
}
// PC 0x00587410
unsigned char CSWSPlaceable::GetDisarmDC()
{
    unsigned char nDC = m_nDisarmDC;
    int nModifier = 0;
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
    {
        case 0:
            nModifier = -5;
            break;
        case 2:
            nModifier = 5;
            break;
    }
    nDC += nModifier;
    if (nDC < 1)
    {
        return 1;
    }
    return nDC;
}
// PC 0x00587460
unsigned char CSWSPlaceable::GetOpenLockDC()
{
    unsigned char nDC;
    unsigned char nPercent;
    char nModifier = 0;
    if (m_nOpenLockDiff == 4)
    {
        char nFixedModifier;
        switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
        {
            case 0:
                nFixedModifier = -5;
                break;
            case 1:
                nFixedModifier = 0;
                break;
            case 2:
                nFixedModifier = 5;
                break;
            default:
                nFixedModifier = 0;
                break;
        }
        nDC = m_nOpenLockDC;
        if (nDC + nFixedModifier >= 0)
        {
            return nDC + nFixedModifier;
        }
        return 0;
    }
    switch (m_nOpenLockDiff)
    {
        case 0:
            nPercent = 0;
            break;
        case 1:
            nPercent = 30;
            break;
        case 2:
            nPercent = 70;
            break;
        case 3:
            nPercent = 100;
            break;
        default:
            nPercent = 30;
            break;
    }
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
    {
        case 0:
            nModifier = -5;
            break;
        case 1:
            nModifier = 0;
            break;
        case 2:
            nModifier = 5;
            break;
    }
    nDC = (m_c251 + 7) * (nPercent / 100) + m_nOpenLockDiffMod + nModifier + 21;
    return nDC;
}
// PC 0x00587550
Vector CSWSPlaceable::GetNearestActionPoint(const Vector& vPosition)
{
    Vector vActionPoint;
    if (!m_cSurfaceMesh.GetUsePoint(0).IsZero())
    {
        Vector vFirstDistance = vPosition - m_cSurfaceMesh.GetUsePoint(0);
        Vector vSecondDistance = vPosition - m_cSurfaceMesh.GetUsePoint(1);
        if (m_cSurfaceMesh.GetUsePoint(1).IsZero() ||
            vFirstDistance * vFirstDistance < vSecondDistance * vSecondDistance)
            vActionPoint = m_cSurfaceMesh.GetUsePoint(0);
        else
            vActionPoint = m_cSurfaceMesh.GetUsePoint(1);
    }
    else
        vActionPoint = m_vPosition;
    return vActionPoint;
}
// PC 0x005876E0
int CSWSPlaceable::AddCastSpellActions(unsigned long nSpellId, int nMultiClass,
                                       const Vector& vTarget,
                                       unsigned long oidTarget, int bFake,
                                       unsigned char nProjectilePathType)
{
    Vector vTargetPosition = vTarget;
    if (oidTarget != OBJECT_INVALID)
    {
        CGameObject* pGameObject =
            g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);
        if (pGameObject == NULL)
            return 0;
        CSWSObject* pTarget = pGameObject->AsSWSObject();
        if (pTarget != NULL)
        {
            CSWSArea* pArea = pTarget->GetArea();
            if (pArea == GetArea())
                vTargetPosition = pTarget->m_vPosition;
            else
                return 0;
        }
    }

    unsigned long nPathType = nProjectilePathType;
    if (bFake)
        nPathType |= SWSOBJECT_CAST_SPELL_FAKE_FLAG;
    AddAction(0x0F, ACTION_GROUP_NEW, ACTION_PARAM_INTEGER,
              &nSpellId, ACTION_PARAM_INTEGER, &nMultiClass,
              ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_FLOAT,
              &vTargetPosition.m_fX, ACTION_PARAM_FLOAT, &vTargetPosition.m_fY,
              ACTION_PARAM_FLOAT, &vTargetPosition.m_fZ, ACTION_PARAM_INTEGER,
              &nPathType, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x005877F0
int CSWSPlaceable::AIActionCastSpell(CSWSObjectActionNode* pNode)
{
    Vector vTarget;
    if (GetDead() == 1)
        return ACTION_FAILED;

    int nSpellId = (int)pNode->m_apParam[0];
    unsigned long oidTarget = (unsigned long)pNode->m_apParam[2];
    int nX = (int)pNode->m_apParam[3];
    int nY = (int)pNode->m_apParam[4];
    int nZ = (int)pNode->m_apParam[5];
    int nProjectilePathType = (int)pNode->m_apParam[6];
    vTarget = Vector(*(float*)&nX, *(float*)&nY, *(float*)&nZ);
    int bFake =
        ((unsigned long)pNode->m_apParam[6] & SWSOBJECT_CAST_SPELL_FAKE_FLAG) != 0;
    unsigned char nSpellType = (unsigned char)nProjectilePathType == 0 ? 6 : 7;
    if (g_pRules->m_pSpellArray->GetSpell(nSpellId) == NULL)
        return ACTION_FAILED;

    m_vSpellTargetPosition = vTarget;
    m_nLastSpellId = nSpellId;
    m_oidSpellTargetObject = oidTarget;
    m_bSafeProjectileBroadcast = 0;
    CalculateLastSpellProjectileTime(nProjectilePathType);
    if (bFake)
    {
        CGameObject* pObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);
        CSWSObject* pTarget;
        if (pObject != NULL && (pTarget = pObject->AsSWSObject()) != NULL)
            BroadcastSafeProjectile(m_nId, oidTarget, m_vPosition, pTarget->m_vPosition,
                m_nLastSpellProjectileTime, nSpellType, nSpellId, 0,
                nProjectilePathType, 0, 0);
        else
            BroadcastSafeProjectile(m_nId, oidTarget, m_vPosition, m_vSpellTargetPosition,
                m_nLastSpellProjectileTime, nSpellType, nSpellId, 0,
                nProjectilePathType, 0, 0);
        m_bSafeProjectileBroadcast = 1;
    }
    else
        SpellCastAndImpact(nSpellId, vTarget, oidTarget, CASTING_CLASS_INNATE_FF,
            CGameObjectArray::INVALIDOBJID, 0, 0, nProjectilePathType, 0);
    return ACTION_COMPLETE;
}
#ifndef _XBOX
void CSWSPlaceable::SetLightIsOn(int bLightIsOn)  // PC 0x005879B0
{
    if (m_bLightIsOn != bLightIsOn)
    {
        int nLightColor;
        bool bFailed;
        {
            CExoString sColumn("LightColor");
            unsigned char nAppearance = m_nAppearance;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            bFailed = !pPlaceables->GetINTEntry(nAppearance, sColumn, &nLightColor);
        }
        if (!bFailed)
        {
            m_n390 = 1;
            m_bLightIsOn = bLightIsOn;
        }
    }
}
#endif
int CSWSPlaceable::GetLightIsOn()  // PC 0x00587A60
{
    int nLightColor;
    bool bFailed;
    {
        CExoString sColumn("LightColor");
        unsigned char nAppearance = m_nAppearance;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetINTEntry(nAppearance, sColumn, &nLightColor);
    }
    if (bFailed)
        return 0;

    return m_bLightIsOn;
}
// PC 0x00587B00
unsigned char CSWSPlaceable::GetBodyBagAppearance()
{
    int nAppearance;
    bool bFailed;
    {
        CExoString sColumn("Appearance");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pBodyBag);
        C2DA* pBodyBag = p2DArrays->m_pBodyBag;
        bFailed = !pBodyBag->GetINTEntry(m_nBodyBag, sColumn, &nAppearance);
    }
    if (bFailed)
    {
        int nBodyBag;
        {
            CExoString sColumn("BodyBag");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance = p2DArrays->m_pAppearance;
            bFailed = !pAppearance->GetINTEntry(m_nAppearance, sColumn, &nBodyBag);
        }
        if (bFailed)
            nBodyBag = 0;

        {
            CExoString sColumn("Appearance");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pBodyBag);
            C2DA* pBodyBag = p2DArrays->m_pBodyBag;
            bFailed = !pBodyBag->GetINTEntry(nBodyBag, sColumn, &nAppearance);
        }
        if (bFailed)
            nAppearance = 3;
    }

    return nAppearance;
}
// PC 0x00587C70
int CSWSPlaceable::NoNonWalkPolys(const Vector& vStart, const Vector& vEnd,
                                  const float& fDistance, const float& fWidth,
                                  const float& fHeight,
                                  CWalkHitInfo* pWalkHitInfo)
{
    m_cSurfaceMesh.m_vWorldPosition = m_vPosition;
    m_cSurfaceMesh.m_qOrientation = m_qOrientation;
    return m_cSurfaceMesh.NoNonWalkPolys(vStart, vEnd, fDistance, fWidth,
                                         fHeight, pWalkHitInfo);
}
// PC 0x00587CE0
void CSWSPlaceable::SetOrientation(const Quaternion& qOrientation)
{
    m_qOrientation = qOrientation;

    Vector vOrientation(
        2.0f * (qOrientation.m_fX * qOrientation.m_fY -
                qOrientation.m_fW * qOrientation.m_fZ),
        1.0f - 2.0f * (qOrientation.m_fX * qOrientation.m_fX +
                       qOrientation.m_fZ * qOrientation.m_fZ),
        0.0f);
    vOrientation = normalize(vOrientation);
    CSWSObject::SetOrientation(vOrientation);
}
// PC 0x00587D80
void CSWSPlaceable::OnEndDialog(CSWSObject* pObject)
{
    g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONENDDIALOGUE], m_nId,
                                 1);
    m_n43C = 0;
}
// PC 0x00587DB0
CSWCPlaceable* CSWSPlaceable::GetClientPlaceable()
{
    if (GetClientObject() != NULL)
        return GetClientObject()->AsSWCPlaceable();

    return NULL;
}
// PC 0x00589F00
void CSWSPlaceable::OpenInventory(unsigned long oidOpener, int bAnimate)
{
    if (m_bOpen == 0 && m_bHasInventory != 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState != 0)
        {
            SetAnimation(PLACEABLE_ANIMATION_CLOSED);
            return;
        }

        CSWSPlayer* pPlayer =
            GetServerExoApp()->GetClientObjectByObjectId(oidOpener);
        if (pPlayer != NULL)
        {
            unsigned int nItemCount = m_pcItemRepository->m_lstItems.num;
            int bHasItems = nItemCount > 0;
            unsigned long oidObject = m_nId;
            GetServerExoApp()->GetSWSMessage()
                ->SendServerToPlayerGuiContainerObject_Open(
                    pPlayer, oidObject, bHasItems);
        }

        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN;
        GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidOpener, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);

        if (bAnimate != 0)
            SetAnimation(PLACEABLE_ANIMATION_OPEN);
        m_bOpen = 1;
    }
}
// PC 0x0058A060
void CSWSPlaceable::CloseInventory(unsigned long oidCloser, int bUpdatePlayer)
{
    if (m_bOpen)
    {
        if (bUpdatePlayer)
        {
            CSWSPlayer* pPlayer = GetServerExoApp()->GetClientObjectByObjectId(oidCloser);
            if (pPlayer != NULL)
            {
                CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
                while (m_pcItemRepository->m_lstItems.num > 0u)
                {
                    unsigned int nItemCount = m_pcItemRepository->m_lstItems.num;
                    CSWSItem* pItem = m_pcItemRepository->ItemListGetItem(nItemCount - 1);
                    m_pcItemRepository->RemoveItem(pItem);
                    if (pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_DATAPAD)
                    {
                        if (m_sTag.CompareNoCase("lockertsfinv"))
                        {
                            pItem->m_lVarTable.SetLocalBoolean(94, 1);
                        }
                        else if (pItem->m_lVarTable.GetLocalBoolean(94) == 0)
                        {
                            pItem->m_lVarTable.SetLocalBoolean(94, 1);
                            CExoString sText;
                            pItem->m_sDescIdentified.GetString(0, &sText, 0);
                            CSWGuiMessageBox* pDatapad =
                                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pA4;
                            pDatapad->SetAllowCancel(0);
                            pDatapad->m_lblMessage.m_cText.m_cParams.SetAlignment(9);
                            CResRef cFont("fnt_d16x16");
                            pDatapad->m_lblMessage.m_cText.m_cParams.SetFont(cFont);
                            pDatapad->SetMessage(sText);
                            pDatapad->SetCallback(NULL, NULL, NULL);
                            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(pDatapad, 1, 1);
                            g_pAppManager->GetClientExoApp()->SetPausedByCombat(1, 12, 0);
                        }
                    }
                    pCreature->GetItemRepository(1)->AddItem(&pItem, 1, 1, 1);
                    if (pItem != NULL)
                        pItem->SetPossessor(oidCloser, 1, 1, OBJECT_INVALID);
                }
            }
        }

        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLOSE;
        GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidCloser, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        SetAnimation(PLACEABLE_ANIMATION_CLOSED);
        m_bOpen = 0;
        if (m_bHasInventory && m_bDieWhenEmpty && m_pcItemRepository->m_lstItems.num == 0)
        {
            m_bUseable = 0;
            GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, m_nId, m_nId, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
        }
    }
}
// PC 0x0058A330
int CSWSPlaceable::GetItemCount(int bDropableOnly)
{
    NwsplaceableTrace1();
    if (m_bHasInventory == 0)
        return 0;

    CItemRepository* pRepository = m_pcItemRepository;
    int nCount = 0;
    if (pRepository != NULL)
    {
        for (unsigned int i = 0; i < pRepository->GetNumItems(); i++)
        {
            CSWSItem* pItem = pRepository->ItemListGetItem(i);
            if (pItem != NULL &&
                (bDropableOnly == 0 || pItem->m_bDropable != 0))
                nCount++;
        }
    }

    return nCount;
}
// PC 0x0058A390
void CSWSPlaceable::ClosePlaceableForAllPlayers()
{
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pPosition;
    CSWSPlayer* pPlayer;
    CSWSObject* pPlayerObject;

    pPlayerList = g_pAppManager->m_pServerExoApp->GetPlayerList();
    for (pPosition = pPlayerList->GetHeadPos(); pPosition != NULL;
         pPlayerList->GetNext(pPosition))
    {
        pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer != NULL)
        {
            pPlayerObject = pPlayer->GetGameObject();
            if (pPlayerObject != NULL &&
                pPlayer->m_pContainerGUI->m_oidContainer == m_nId)
            {
                CloseInventory(pPlayerObject->m_nId, 1);
            }
        }
    }
}
// PC 0x0058A7E0
void CSWSPlaceable::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                                 void* pEventData, unsigned long nCalendarDay,
                                 unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_FEEDBACK_MESSAGE)
    {
        CSWCCMessageData* pMessageData = (CSWCCMessageData*)pEventData;
        if (pMessageData != NULL)
        {
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            if (pMessageData->m_nType == CSWCCMessageData::DATA_TYPE_SAVING_THROW)
            {
                if (pCreature != NULL)
                {
                    pCreature->BroadcastSavingThrowData(pMessageData);
                }
                delete pMessageData;
            }
            else if (pMessageData->m_nType == CSWCCMessageData::DATA_TYPE_FEEDBACK)
            {
                if (pCreature != NULL)
                {
                    pCreature->SendFeedbackMessage(pMessageData->GetInteger(0), pMessageData);
                }
                else
                {
                    delete pMessageData;
                }
            }
            else if (pCreature != NULL)
            {
                pCreature->SendFeedbackMessage(pMessageData->GetInteger(0), pMessageData);
            }
            else
            {
                delete pMessageData;
            }
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_ON_MELEE_ATTACKED)
    {
        CSWSCombatAttackData* pAttackData = (CSWSCombatAttackData*)pEventData;
        CSWSCreature* pAttacker = NULL;
        CSWSItem* pWeapon = NULL;

        if (pEventData != NULL)
        {
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, oidCaller);
            pMessageData->SetObjectID(1, GetId());
            if (g_bEnableCombatDebugging != 0)
            {
                pMessageData->SetString(0, pAttackData->m_sAttackDebugText);
            }
            pAttacker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            if (pAttacker != NULL)
            {
                pAttacker->BroadcastAttackDataToParty(pMessageData, pAttackData);
                pAttacker->RemoveCombatInvisibilityEffects();
            }
            delete pMessageData;
            SetLastAttackType(pAttackData->m_nAttackType);
            SetLastAttackMode(pAttackData->m_nAttackMode);
            if (pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_RIGHTHAND ||
                pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND)
            {
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
            }
            else if (pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND)
            {
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            }
            else if (pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
                     pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
            {
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_HANDS);
            }
            else if (pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE1)
            {
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON1);
            }
            else if (pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE2)
            {
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON2);
            }
            else if (pAttackData->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE3)
            {
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON3);
            }
        }
        else
        {

            pAttacker = (CSWSCreature*)oidCaller;
        }
        m_oidLastAttacker = oidCaller;
        SetLastHostileActor(oidCaller, 0);
        if (pAttacker != NULL)
        {
            pAttacker->BroadcastCombatStateToParty(OBJECT_INVALID);
            pAttacker->RemoveCombatInvisibilityEffects();
            if (pWeapon != NULL)
            {
                pAttacker->m_oidLastWeaponUsed = pWeapon->GetId();
            }
            else
            {
                pAttacker->m_oidLastWeaponUsed = OBJECT_INVALID;
            }
        }
        g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_ONMELEEATTACKED), m_nId, 1);
        if (m_nTrapFlag == 1 && pAttacker != NULL)
        {
            Vector vDistance = pAttacker->m_vPosition - m_vPosition;
            if (vDistance * vDistance <= 16.0f)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, oidCaller, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_ACQUIRE_ITEM)
    {
        CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidCaller);
        if (pItem != NULL)
        {
            AcquireItem(&pItem, OBJECT_INVALID, 1);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        CScriptEvent* pScriptEvent = (CScriptEvent*)pEventData;

        switch (pScriptEvent->m_nType)
        {
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN:
            m_oidLastOpenedBy = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONOPEN], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_CLOSE:
            m_oidLastClosedBy = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONCLOSED], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DAMAGED:
            m_oidLastDamager = oidCaller;
            SetLastHostileActor(oidCaller, 0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONDAMAGED], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH:
            SetAnimation(PLACEABLE_ANIMATION_DESTROYED);
            m_n154 = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONDEATH], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM:
            if (m_nTrapFlag != 0)
            {
                m_nTrapFlag = 0;
                m_oidLastDisarmedBy = oidCaller;
                g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONDISARM], GetId(), 1);
            }
            if (m_lstTrapDetectedBy.num > 0)
            {
                m_lstTrapDetectedBy.SetSize(0);
            }
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_HEARTBEAT:
            m_nHeartbeatDay = nCalendarDay;
            m_nHeartbeatTime = nTimeOfDay;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONHEARTBEAT], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_USED:
            if (m_bUseable != 0)
            {
                m_oidLastUsedBy = oidCaller;
                g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONUSED], GetId(), 1);
            }
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED:
            {
                int nTriggerFlag = pScriptEvent->GetInteger(0);
                int bTriggered = pScriptEvent->GetInteger(1);
                if (pScriptEvent->GetInteger(2) == 0 || bTriggered != 0)
                {
                    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
                    if ((pCreature != NULL &&
                         pCreature->m_pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_MINE, NULL) != 0) ||
                        (nTriggerFlag == 0 &&
                         (GetReputation(oidCaller, 0) >= MIN_FRIENDLY_REPUTATION || m_nFaction == pCreature->m_pStats->m_nFactionId)) ||
                        m_nTrapFlag == 0)
                    {
                        break;
                    }
                    if (pCreature != NULL)
                    {
                        pCreature->SendFeedbackMessage(FEEDBACK_TRAP_TRIGGERED, NULL);
                    }
                    m_oidLastTriggeredBy = oidCaller;
                    g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONTRAPTRIGGERED], GetId(), 1);
                    if (m_bTrapOneShot != 0)
                    {
                        m_nTrapFlag = 0;
                        if (GetArea() != NULL && &GetArea()->m_lstTrapObjectIds != NULL)
                        {
                            unsigned long oidPlaceable = GetId();
                            CSWSArea* pArea = GetArea();
                            if (pArea->m_lstTrapObjectIds.Contains(oidPlaceable) == 1)
                            {
                                unsigned long oidObject = GetId();
                                CSWSArea* pTrapArea = GetArea();
                                pTrapArea->m_lstTrapObjectIds.Remove(oidObject);
                            }
                        }
                        CSWSTrigger* pTrapTrigger = m_pTrapTrigger;
                        if (pTrapTrigger != NULL)
                        {
                            pTrapTrigger->SetFadeOutTime(1);
                            pTrapTrigger->RemoveFromArea();
                            delete pTrapTrigger;
                        }
                    }
                    if (m_lstTrapDetectedBy.num > 0)
                    {
                        m_lstTrapDetectedBy.SetSize(0);
                    }
                    break;
                }
                CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
                if (m_nTrapFlag == 0)
                    break;
                int nRoll = pCreature->m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, 0) / 20 + 20 +
                            pScriptEvent->GetInteger(3);
                if (nRoll >= GetOpenLockDC())
                {
                    m_bLocked = 0;
                    CScriptEvent* pUnlockEvent = new CScriptEvent;
                    pUnlockEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, pCreature->m_nId, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pUnlockEvent);
                }
                pCreature->SendFeedbackMessage(FEEDBACK_TRAP_TRIGGERED, NULL);
                m_oidLastTriggeredBy = oidCaller;
                m_nTrapFlag = 0;
                if (GetArea() != NULL)
                {
                    unsigned long oidPlaceable = GetId();
                    if (GetArea()->m_lstTrapObjectIds.Contains(oidPlaceable) == 1)
                    {
                        unsigned long oidObject = GetId();
                        GetArea()->m_lstTrapObjectIds.Remove(oidObject);
                    }
                }
                if (m_lstTrapDetectedBy.num > 0)
                {
                    m_lstTrapDetectedBy.SetSize(0);
                }
                CSWSTrigger* pTrapTrigger = m_pTrapTrigger;
                if (pTrapTrigger != NULL)
                {
                    unsigned char nTrapType = pTrapTrigger->m_nTrapType;
                    if (nTrapType >= 3 && nTrapType <= 5)
                    {
                        CSWCTrigger* pClientTrigger = pTrapTrigger->GetClientTrigger();
                        Vector vPosition = pClientTrigger->m_vPosition;
                        vPosition.m_fZ -= 2.0f;
                        pClientTrigger->SetPosition(vPosition);
                    }
                    pTrapTrigger->SetFadeOutTime(1);
                    pTrapTrigger->RemoveFromArea();
                    delete pTrapTrigger;
                }
                break;
            }
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE:
            m_oidLastSpeaker = oidCaller;
            m_nMatchedStringsNum = pScriptEvent->GetInteger(1);
            m_sLastListenText = pScriptEvent->GetString(0);
            g_pAppManager->GetServerExoApp()->SetLastConversation(pScriptEvent->GetString(0));
            m_nConversationType = pScriptEvent->GetInteger(2);
            g_pAppManager->GetServerExoApp()->SetActionConversation(1);
            g_pAppManager->GetServerExoApp()->SetIgnoreConversationStartRange(pScriptEvent->GetInteger(3));
            ClearMatchedExpressionStrings();
            for (int i = 1; i < pScriptEvent->GetInteger(0); i++)
            {
                AddMatchedExpressionString(pScriptEvent->GetString(i));
            }
            if (&m_sScripts[SCRIPT_ONDIALOG] != NULL &&
                (m_sScripts[SCRIPT_ONDIALOG].IsEmpty() != 0 || m_sScripts[SCRIPT_ONDIALOG] == "default"))
            {
                m_sScripts[SCRIPT_ONDIALOG] = "k_hen_dialogue01";
            }
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONDIALOG], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_INVENTORY_DISTURBED:
            m_oidLastDisturbed = oidCaller;
            m_n178 = pScriptEvent->GetInteger(0);
            m_oidInventoryDisturbItem = pScriptEvent->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONINVDISTURBED], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_LOCKED:
            m_oidLastLockedBy = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONLOCK], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED:
            m_oidLastUnlockedBy = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONUNLOCK], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_SPELLCASTAT:
            m_n1D8 = pScriptEvent->GetObjectID(0);
            m_n1DC = pScriptEvent->GetInteger(0);
            m_n1E0 = pScriptEvent->GetInteger(1);
            if (m_n1E0 != 0)
            {
                SetLastHostileActor(m_n1D8, 0);
                unsigned long oidCaster = m_n1D8;
                CSWSCreature* pCaster = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaster);
                if (pCaster != NULL)
                {
                    pCaster->BroadcastCombatStateToParty(OBJECT_INVALID);
                    pCaster->SetExcitedState(2);
                    pCaster->RemoveCombatInvisibilityEffects();
                }
            }
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONSPELLCASTAT], GetId(), 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT:
            m_nCustomScriptEventId = pScriptEvent->GetInteger(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ONUSERDEFINED], GetId(), 1);
            break;
        default:
            break;
        }
        delete pScriptEvent;
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        ClosePlaceableForAllPlayers();
        SpawnBodyBag();
        RemoveFromArea();
        if (this != NULL)
        {
            delete this;
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_REMOVE_FROM_AREA)
    {
        RemoveFromArea();
    }
    else if (nEventId == CServerAIMaster::EVENT_APPLY_EFFECT)
    {
        if (pEventData != NULL)
        {
            ApplyEffect((CGameEffect*)pEventData, 0, 0);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_REMOVE_EFFECT)
    {
        if (pEventData != NULL)
        {
            RemoveEffectById(*(unsigned __int64*)pEventData);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_PLAY_ANIMATION)
    {
        if (pEventData != NULL)
        {
            if (GetDead() == 0)
            {
                SetAnimation((int)pEventData);
            }
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
        {
            g_pVirtualMachine->RunScriptSituation(pEventData, GetId(), 1);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_SPELL_IMPACT)
    {
        CSWSSpellScriptData* pSpellData = (CSWSSpellScriptData*)pEventData;
        if (pSpellData != NULL && pSpellData->m_oidCaster == oidCaller)
        {
            m_nLastSpellId = pSpellData->m_nSpellId;
            m_oidSpellTargetObject = pSpellData->m_oidTarget;
            SetSpellTargetPosition(pSpellData->m_vTargetPosition);
            if (pSpellData->m_oidTarget != OBJECT_INVALID)
            {
                CGameObject* pTarget;
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(pSpellData->m_oidTarget,
                                                                                    &pTarget) ==
                        CGameObjectArray::SUCCESS &&
                    pTarget->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
                {
                    if (GetArea() == ((CSWSObject*)pTarget)->GetArea())
                    {
                        SetSpellTargetPosition(((CSWSObject*)pTarget)->m_vPosition);
                    }
                }
            }
            SetEffectSpellId(pSpellData->m_nSpellId);
            m_nSavingThrowSpellId = pSpellData->m_nSpellId;
            g_pVirtualMachine->RunScript(&pSpellData->m_sScript, GetId(), 1);
            SetEffectSpellId(-1);
        }
        delete pEventData;
    }
    else if (nEventId == CServerAIMaster::EVENT_OPEN_OBJECT)
    {
        if (pEventData != NULL)
        {
            OpenInventory(*(unsigned long*)pEventData, 0);
            delete (unsigned long*)pEventData;
        }
    }
    return;
}
// PC 0x0058B510
int CSWSPlaceable::AcquireItemsFromObject(unsigned long oidSource,
                                        int bDropableOnly)
{
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidSource);
    CSWSPlaceable* pPlaceable =
        g_pAppManager->m_pServerExoApp->GetPlaceableByGameObjectID(oidSource);

    if (pCreature == NULL && pPlaceable == NULL)
        return 0;
    if (pCreature != NULL &&
        (pCreature->m_pInventory == NULL ||
         pCreature->GetItemRepository(1) == NULL))
        return 0;
    if (pPlaceable != NULL && pPlaceable->m_pcItemRepository == NULL)
        return 0;

    CItemRepository* pRepository;
    if (pCreature != NULL)
        pRepository = pCreature->GetItemRepository(1);
    else if (pPlaceable != NULL)
    {
        pRepository = pPlaceable->m_pcItemRepository;
        bDropableOnly = 0;
    }

    int nCount = 0;
    CSWSItem* pItem;
    CExoArrayList<CSWSItem*> lstItems;
    if (pCreature != NULL)
    {
        for (unsigned char nSlot = 0;
             nSlot < CSWInventory::EQUIPMENT_SLOT_COUNT; nSlot++)
        {
            CSWInventory* pInventory = pCreature->m_pInventory;
            pItem = pInventory->GetItemInSlot(1 << nSlot);
            if (pItem != NULL &&
                (bDropableOnly == 0 || pItem->m_bDropable != 0))
                lstItems.Add(pItem);
        }
    }
    for (unsigned int i = 0; i < pRepository->m_lstItems.num; i++)
    {
        pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL &&
            (bDropableOnly == 0 || pItem->m_bDropable != 0))
            lstItems.Add(pItem);
    }
    for (int j = 0; j < lstItems.num; j++)
    {
        pItem = lstItems[j];
        if (pItem != NULL && AcquireItem(&pItem, oidSource, 1))
            nCount++;
    }

    if (pCreature != NULL && (unsigned long)pCreature->GetGold() > 0)
    {
        pItem = new CSWSItem(OBJECT_INVALID);
        CResRef cCredits("g_i_credits001");
        pItem->LoadFromTemplate(cCredits);
        pItem->m_nStackSize = pCreature->GetGold();
        pCreature->SetGold(0);
        AcquireItem(&pItem, OBJECT_INVALID, 1);
    }
    return nCount;
}
// PC 0x0057F6F0
void CSWSPlaceable::DoDamage(int nDamage)
{
    if (m_bPlotObject == 0)
    {
        CSWSObject::DoDamage(nDamage);
    }
}

static inline CServerExoApp* GetServerExoApp()
{
    return g_pAppManager->m_pServerExoApp;
}

static inline void NwsplaceableTrace1()
{
}

#ifdef _XBOX

void CSWSPlaceable::SetLightIsOn(int bLightIsOn)
{
    int nLightColor;
    if (m_bLightIsOn == bLightIsOn ||
        !g_pRules->m_p2DArrays->m_pPlaceables->GetINTEntry(m_nAppearance, CExoString("LightColor"), &nLightColor))
    {
        return;
    }
    m_n390 = 1;
    m_bLightIsOn = bLightIsOn;
}
#endif
