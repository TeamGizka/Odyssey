#include "exobase.h"
#include "nwcitem.h"
#include "nwccreature.h"
#include "nwsitem.h"
#include "nwrules.h"
#include "clientexoapp.h"
#include "nwcanimbase.h"
#include "nwitem.h"
#include "exobase.h"
#include "clientaimaster.h"
#include "nwcarea.h"
#include "serverexoapp.h"
#include "nwcmodule.h"
#include "guiingame.h"
#include "nwcanimbase.h"
#include "aurlightmanager.h"
#include "exotlktable.h"
#include "worldtimer.h"

static int enableitemgroundalignment = 0;
// PC 0x0069CAB0
CSWCItem::CSWCItem(unsigned long nBaseItem)
    : CSWItem(), CSWCObject(CGameObject::OBJECT_TYPE_ITEM)
{
    m_nUnknown150 = -1;
    m_nAttachSlot = INVENTORY_SLOT_INVALID;
    m_oidEquippedBy = OBJECT_INVALID;
    m_nEquipSlot = INVENTORY_SLOT_INVALID;
    m_nInventorySlot = INVENTORY_SLOT_INVALID;
    m_bAttached = 0;
    m_nContainerType = 0xff;
    m_nContainerSlot = 0xff;
    m_nBaseItem = nBaseItem;
    m_nUnknownContainer2 = 0xff;
    m_nUnknownContainer1 = 0xff;
    m_nNumActiveProperties = 0;
    m_pActiveProperties = 0;
    m_bIdentified = 0;
    m_bInContainer = 0;
    m_nStackSize = -1;
    m_bCanUse = 0;
    m_nArmorClass = 0;
    m_nModelVariation = 1;
    m_oidVisibleOwner = OBJECT_INVALID;
    for (int i = 0; i < 8; i++)
    {
        m_anItemPropertyUsesLeft[i] = 0;
    }
    m_nActivePropertyIndex = INVALID_ACTIVE_PROPERTY_INDEX;
    m_nBodyVariation = 0;
    m_pVisualEffect = 0;
    m_nUnknown198 = 0;
    m_pPowerSound = 0;
    m_bPowered = 0;
    m_nUnknown1A0 = 0;
}

#include "appmanager.h"

static inline void NwcitemTrace1();
#define NWCITEM_TRACE() NwcitemTrace1()
// PC 0x0069CC10
void CSWCItem::AIUpdate()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nDeltaDay;
    unsigned long nDeltaTime;

    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
        &nDay, &nTime);
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime,
        &nDeltaDay, &nDeltaTime);
    m_nTimeDelta = nDeltaTime;

    UpdatePopUpBubble();
    if (FadeObject())
    {
        CSWCObject* pObject = this;
        CClientAIMaster* pAIMaster =
            g_pAppManager->GetClientExoApp()->GetClientAIMaster();
        pAIMaster->m_aiList.RemoveObject(pObject);
        CGameObject* pGameObject;
        unsigned long nId = m_nId;
        if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->Delete(
                nId, &pGameObject) == CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            delete this;
            return;
        }
    }

    if (m_pVisualEffect != NULL)
    {
        float fTime = 0.0f;
        m_pVisualEffect->GetAnimation("impact", &fTime, NULL);
        if (fTime == -1.0f)
        {
            m_pVisualEffect->AttachToObject(NULL, "", Gob::ATTACH_NORMAL);
            delete m_pVisualEffect;
            m_pVisualEffect = NULL;
            g_pAppManager->GetClientExoApp()->GetClientAIMaster()->RemoveObject(this);
        }
    }

    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
}
// PC 0x0069CD80
int CSWCItem::LoadModel(const CResRef& cModel, unsigned char nModel,
                        unsigned char nFlags)
{
    if (m_pAnimBase == NULL)
    {
        m_pAnimBase = new CSWCAnimBase;
        if (m_pAnimBase == NULL)
        {
            return 0;
        }
    }

    if (GetAnimationBase()->LoadModel(cModel, nModel) == 0)
    {
        return 0;
    }

    Gob* pModel = static_cast<Gob*>(GetAnimationBase()->GetModel(nModel));
    if (pModel != NULL)
    {
        pModel->SetExternalData(this);
        pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_ITEM);
    }
    pModel->SetLightPriority(5);

    if (nModel == CSWCAnimBase::MODEL_SLOT_MAIN)
    {
        m_fHeadHitHeight = 0.5f;
    }

    return 1;
}
// PC 0x0069CE80
void CSWCItem::SetEquippedBy(CSWCCreature* pCreature)
{
    Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    if (pCreature != NULL)
    {
        m_oidEquippedBy = pCreature->m_nId;
        CSWSCreature* pServerCreature = pCreature->GetServerCreature();
        if (pServerCreature != NULL)
        {
            if (pModel != NULL)
            {
                if (pServerCreature->m_n24C)
                    pModel->EnableHologram(1);
                else
                    pModel->EnableHologram(0);
                if (pServerCreature->m_n254)
                    pModel->EnableRendering(0);
                else
                    pModel->EnableRendering(1);
            }
        }
        else if (pModel != NULL)
        {
            pModel->EnableHologram(0);
            pModel->EnableRendering(1);
        }
    }
    else
    {
        m_oidEquippedBy = 2130706432;
        if (pModel != NULL)
        {
            pModel->EnableHologram(0);
            pModel->EnableRendering(1);
        }
    }
}
// PC 0x0069CF30
CSWCCreature* CSWCItem::GetEquippedBy()
{
    unsigned long nEquippedBy = m_oidEquippedBy;
    if (nEquippedBy == 2130706432)
    {
        return NULL;
    }
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    return pClientExoApp->GetCreatureByGameObjectID(nEquippedBy);
}
// PC 0x0069CF50
void CSWCItem::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x0069CF60
void CSWCItem::SetNumActiveProperties(int nNumActiveProperties)
{
    m_nNumActiveProperties = nNumActiveProperties;
    if (m_pActiveProperties != NULL)
    {
        delete[] m_pActiveProperties;
    }
    m_pActiveProperties = new CSWItemProperty[m_nNumActiveProperties];
}
// PC 0x0069CFA0
CSWItemProperty* CSWCItem::GetActiveProperty(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nNumActiveProperties)
    {
        return 0;
    }
    return m_pActiveProperties + nIndex;
}
// PC 0x0069CFD0
void CSWCItem::PlayDropSound()
{
    CResRef cSound;
    if (GetBaseItem()->m_nItemType >= CSWBaseItem::ITEM_TYPE_ARMOR_FIRST &&
        GetBaseItem()->m_nItemType <= CSWBaseItem::ITEM_TYPE_ARMOR_LAST)
    {
        switch (m_nArmorType)
        {
        case ARMOR_TYPE_CLOTH:
            cSound = "it_materialcloth";
            break;
        case ARMOR_TYPE_LEATHER:
            cSound = "it_armorleather";
            break;
        case ARMOR_TYPE_CHAIN:
            cSound = "it_armorchain";
            break;
        case ARMOR_TYPE_PLATE:
            cSound = "it_armorplate";
            break;
        }
    }
    else
    {
        unsigned char nSoundType =
            g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nInvSoundType;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        cSound = pClientExoApp->GetItemDropSound(nSoundType);
    }
    g_pExoSound->PlayOneShotSound(cSound, CExoSound::SOUND_TYPE_GUI,
                                 0, 0, 0.0f, 0.0f);
}
// PC 0x0069D0E0
int CSWCItem::GetActivePropertySpellID(int nIndex)
{
    if (nIndex >= m_nNumActiveProperties)
    {
        return -1;
    }
    if (m_pActiveProperties[nIndex].m_nPropertyType != CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
    {
        return -1;
    }
    return m_pActiveProperties[nIndex].m_nSubtype;
}
// PC 0x0069D110
void CSWCItem::SetStackSize(int nStackSize)
{
    m_nStackSize = nStackSize;
    g_pRules->m_pBaseItems->GetBaseItem(m_nBaseItem);
}

enum
{
    ITEM_VISUAL_EFFECT_ACID = 1,
    ITEM_VISUAL_EFFECT_FROST = 2,
    ITEM_VISUAL_EFFECT_ELECTRICAL = 3,
    ITEM_VISUAL_EFFECT_FIRE = 4,
    ITEM_VISUAL_EFFECT_SONIC = 5,
    ITEM_VISUAL_EFFECT_HOLY = 6,
    ITEM_VISUAL_EFFECT_NEGATIVE = 7
};

int CSWCItem::LoadVisualEffect(unsigned char nEffect)
{
    CExoString sModel;
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(m_nBaseItem);
    if (pBaseItem == NULL)
    {
        return 0;
    }
    sModel = pBaseItem->m_sItemClass;
    switch (nEffect)
    {
    case ITEM_VISUAL_EFFECT_ACID:
        sModel = sModel + "_fxacid";
        break;
    case ITEM_VISUAL_EFFECT_FROST:
        sModel = sModel + "_fxfrost";
        break;
    case ITEM_VISUAL_EFFECT_ELECTRICAL:
        sModel = sModel + "_fxelect";
        break;
    case ITEM_VISUAL_EFFECT_FIRE:
        sModel = sModel + "_fxfire";
        break;
    case ITEM_VISUAL_EFFECT_SONIC:
        sModel = sModel + "_fxsonic";
        break;
    case ITEM_VISUAL_EFFECT_HOLY:
        sModel = sModel + "_fxholy";
        break;
    case ITEM_VISUAL_EFFECT_NEGATIVE:
        sModel = sModel + "_fxneg";
        break;
    default:
        return 0;
    }
    if (m_pAnimBase->LoadModel(CResRef(sModel), MODEL_SLOT_VISUAL_EFFECT) == 0)
    {
        return 0;
    }
    m_nUnknown198 = 1;
    return 1;
}
// PC 0x0069D130
void CSWCItem::ShowVisualEffect()
{
    if (m_pAnimBase != NULL)
    {
        Gob* pModel = GetModel(CSWCAnimBase::MODEL_WINGS, 1);
        if (pModel != NULL)
        {
            Gob* pHead = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            if (pHead != NULL)
            {
                pModel->AttachToObject(pHead, "root", Gob::ATTACH_NORMAL);
            }
        }
    }
}

inline void CSWCItem::HideVisualEffect()
{
    if (m_pAnimBase != NULL)
    {
        Gob* pModel = GetModel(CSWCAnimBase::MODEL_WINGS, 1);
        if (pModel != NULL)
        {
            pModel->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
            pModel->AttachToScene(NULL);
        }
    }
}
// PC 0x0069D180
void CSWCItem::SpawnVisualEffect(const CExoString& sModel,
                                 const CExoString& sHook)
{
    if (m_pVisualEffect != NULL)
    {
        delete m_pVisualEffect;
        m_pVisualEffect = NULL;
    }
    m_pVisualEffect = (Gob*)NewCAurObject((char*)sModel.CStr(),
                                         "specialattack", NULL, NULL);
    if (m_pVisualEffect != NULL)
    {
        m_pVisualEffect->AttachToObject(
            GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1), (char*)sHook.CStr(), 0);
        m_pVisualEffect->PlayAnimation("impact", 1.0f, 1, 0.0f);
    }
    g_pAppManager->m_pClientExoApp->GetClientAIMaster()->AddObject(this);
}

inline void CSWCItem::ComputeZValue(Vector* pPosition, float fOffset,
                                        Vector* pNormal)
{
    float fZ = 0.0f;
    if (m_pArea != NULL)
    {
        fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
            m_pArea->m_cAreaResRef, *pPosition, 1);
    }
    pPosition->m_fZ = fZ + fOffset;
}
// PC 0x0069D230
void CSWCItem::SetArea(CSWCArea* pArea)
{
    if (pArea != NULL)
    {
        HideVisualEffect();
    }
    else
    {
        ShowVisualEffect();
    }
    int bMoved = (pArea != NULL && m_pArea != pArea);
    CSWCObject::SetArea(pArea);
    if (bMoved)
    {
        SetPosition(m_vPosition);
    }
}
// PC 0x0069D2B0
void CSWCItem::SetItemPropertyUsesLeft(int nIndex, unsigned short nUsesLeft)
{
    if (nIndex < 8)
    {
        m_anItemPropertyUsesLeft[nIndex] = nUsesLeft;
        if (nIndex == m_nActivePropertyIndex && nUsesLeft == 0)
        {
            int i;
            for (i = 0; i < 8; i++)
            {
                CSWItemProperty* pProperty = GetActiveProperty(i);
                if (pProperty != NULL && pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM &&
                    m_anItemPropertyUsesLeft[i] > 0)
                {
                    m_nActivePropertyIndex = i;
                    break;
                }
            }
            if (i == 8)
            {
                m_nActivePropertyIndex = -1;
            }
        }
    }
}
// PC 0x0069D330
void CSWCItem::UpdateOrientation(const Vector& vNormal)
{
    Quaternion qOrientation;
    float fRoll = 0.0f;
    float fPitch = 0.0f;

    if (g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nRotateOnGround ==
        CSWBaseItem::GROUND_ROTATION_ROLL)
    {
        fRoll = 90.0f;
    }
    else if (g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nRotateOnGround ==
             CSWBaseItem::GROUND_ROTATION_PITCH)
    {
        fPitch = 90.0f;
    }

    qOrientation = YawPitchRoll(Yaw(m_vOrientation), fPitch, fRoll);
    if (enableitemgroundalignment != 0)
    {
        qOrientation = RotationArc(Vector(0.0f, 0.0f, 1.0f), vNormal) * qOrientation;
    }
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->SetOrientation(qOrientation);
    }
}
// PC 0x006DB7A0
void CSWCItem::UnloadModel()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x0069D520
void CSWCItem::ResolveCreaturePoweredAnimations(int bPower)
{
    CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()->
        GetCreatureByGameObjectID(m_oidEquippedBy);
    if (pCreature != NULL)
    {
        unsigned char nPoweredAnimations = 0;
        if (pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND) == GetId())
        {
            nPoweredAnimations = CSWCAnimBase::POWERED_ANIMATION_RIGHTHAND;
        }
        else if (pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND) == GetId())
        {
            nPoweredAnimations = CSWCAnimBase::POWERED_ANIMATION_LEFTHAND;
        }
        if (pCreature->GetAnimationBase() != NULL)
        {
            if (bPower && !m_bPowered)
            {
                nPoweredAnimations |= pCreature->GetAnimationBase()->m_nPoweredAnimations;
            }
            else if (!bPower && m_bPowered)
            {
                nPoweredAnimations = ~nPoweredAnimations &
                    pCreature->GetAnimationBase()->m_nPoweredAnimations;
            }
            else if (!bPower && !m_bPowered)
            {
                nPoweredAnimations &= ~(CSWCAnimBase::POWERED_ANIMATION_RIGHTHAND |
                    CSWCAnimBase::POWERED_ANIMATION_LEFTHAND);
            }

            if (GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_POWERED_HAND_39 &&
                GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_POWERED_HAND_40 &&
                GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_POWERED_HAND_41)
            {
                nPoweredAnimations |= CSWCAnimBase::POWERED_ANIMATION_OTHER;
            }
            else
            {
                nPoweredAnimations &= ~CSWCAnimBase::POWERED_ANIMATION_OTHER;
            }
            pCreature->GetAnimationBase()->m_nPoweredAnimations = nPoweredAnimations;
        }
    }
}
// PC 0x0069D610
CSWSItem* CSWCItem::GetServerItem()
{
    if (GetServerObject() != NULL)
    {
        return GetServerObject()->AsSWSItem();
    }
    return NULL;
}
// PC 0x0069D650
CSWCItem::~CSWCItem()
{
    NWCITEM_TRACE();
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(MODEL_SLOT_VISUAL_EFFECT);
    }
    UnloadModel();

    if (m_pActiveProperties != NULL)
    {
        delete[] m_pActiveProperties;
        m_pActiveProperties = NULL;
    }
    if (m_pVisualEffect != NULL)
    {
        m_pVisualEffect->AttachToObject(NULL, "", Gob::ATTACH_NORMAL);
        delete m_pVisualEffect;
        m_pVisualEffect = NULL;
    }
    if (m_pPowerSound != NULL)
    {
        delete m_pPowerSound;
    }
    m_pPowerSound = NULL;
}
// PC 0x0069D750
void CSWCItem::SetOrientation(const Vector& vOrientation)
{
    NWCITEM_TRACE();
    m_vOrientation = vOrientation;
    Vector vNormal(0.0f, 0.0f, 1.0f);
    if (enableitemgroundalignment && m_pArea)
    {
        Vector vPosition = m_vPosition;
        vPosition.m_fZ = 0.0f;
        ComputeZValue(&vPosition, 0.0f, &vNormal);
    }
    UpdateOrientation(vNormal);
}
// PC 0x0069D7F0
CExoString CSWCItem::GetName(int bFullName)
{
    NWCITEM_TRACE();
    STR_RES strRes;
    CExoString sName;

    if (!bFullName)
    {
        return m_sName;
    }

    if (m_sName.GetLength() > 0 && m_bIdentified)
    {
        return m_sName;
    }

    int nNameStrRef =
        g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nName;
    g_pTlkTable->Fetch(nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    sName = strRes.m_sString;

    if (!m_bIdentified)
    {

        g_pTlkTable->Fetch(0, strRes, CTlkTable::GENDER_MALE);
        CExoString sSpace(" ");
        sName = sName + sSpace + strRes.m_sString;
    }

    return sName;
}
// PC 0x0069D9D0
void CSWCItem::SetPosition(const Vector& vPosition)
{
    NWCITEM_TRACE();
    Vector vNormal(0.0f, 0.0f, 1.0f);
    Vector vNewPosition = vPosition;

    vNewPosition.m_fZ = 0.0f;

    if (enableitemgroundalignment != 0)
    {
        ComputeZValue(&vNewPosition, 0.1f, &vNormal);
    }
    else
    {
        int nStaticTrimeshParts = hitcheck_static_trimesh_parts;
        hitcheck_static_trimesh_parts = 1;
        ComputeZValue(&vNewPosition, 0.1f, &vNormal);
        hitcheck_static_trimesh_parts = nStaticTrimeshParts;
    }

    CSWCObject::SetPosition(vNewPosition);

    if (enableitemgroundalignment != 0)
    {
        if (m_vOrientation * m_vOrientation > 0.00001f)
        {
            UpdateOrientation(vNormal);
        }
    }
}
// PC 0x0069DAD0
void CSWCItem::PowerItem(int bPower, int bOverride, int bAnimate,
                         int bSilent)
{
    NWCITEM_TRACE();
    CSWCArea* pModuleArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    CSWMiniGame* pMiniGame = pModuleArea->m_pMiniGame;
    if (pMiniGame != NULL)
    {
        return;
    }
    if (GetEquippedBy() != NULL &&
        GetEquippedBy()->m_bOverrideLightsaberState == 1 && !bOverride)
    {
        return;
    }
    if (bPower == 1 &&
        g_pAppManager->m_pClientExoApp->GetAreaNotReadyToDisplay() == 1 &&
        !g_pAppManager->m_pClientExoApp->GetInGameGui()->IsGlobalFading())
    {
        ResolveCreaturePoweredAnimations(0);
        return;
    }
    ResolveCreaturePoweredAnimations(bPower);
    if (g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_bPoweredItem != 1)
    {
        return;
    }
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetCreatureByGameObjectID(m_oidEquippedBy);
    CSWCAnimBase* pAnimBase = GetAnimationBase();
    if (pAnimBase != NULL && bPower != m_bPowered)
    {
        Vector vPosition = pAnimBase->GetModel(
            CSWCAnimBase::MODEL_SLOT_MAIN)->GetPosition();
        if (m_pPowerSound == NULL)
        {
            m_pPowerSound = new CExoSoundSource(
                g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_sPoweredSnd);
            m_pPowerSound->Set3D(1);
            m_pPowerSound->SetPosition(vPosition, 0.0f);
        }
        unsigned short nTransitionAnimation;
        unsigned short nIdleAnimation;
        if (bPower)
        {
            nTransitionAnimation = ITEM_ANIMATION_POWER_UP;
            nIdleAnimation = ITEM_ANIMATION_POWERED;
            if (!bSilent && m_pPowerSound != NULL)
            {
                m_pPowerSound->SetPriorityGroup(CExoSound::PRIORITY_GROUP_POWERED_ITEM);
                m_pPowerSound->SetLooping(1);
                m_pPowerSound->Set3D(1);
                m_pPowerSound->SetPosition(vPosition, 0.0f);
                m_pPowerSound->Play();
            }
        }
        else
        {
            nTransitionAnimation = ITEM_ANIMATION_POWER_DOWN;
            nIdleAnimation = ITEM_ANIMATION_UNPOWERED;
            if (m_pPowerSound != NULL)
            {
                m_pPowerSound->Stop();
            }
        }
        if (bAnimate)
        {
            if (!m_bPowered)
            {
                if (m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN) != NULL)
                {
                    m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN)->PlayAnimation(
                        "NULL", 1.0f, 0, 0.0f);
                }
                if (!bSilent)
                {
                    g_pExoSound->Play3DOneShotSound(
                        g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_sPowerUpSnd,
                        vPosition, 0.0f, CExoSound::PRIORITY_GROUP_POWERED_ITEM,
                        0, 0, 0.0f, 0.0f);
                }
            }
            else if (!bSilent)
            {
                g_pExoSound->Play3DOneShotSound(
                    g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_sPowerDownSnd,
                    vPosition, 0.0f, CExoSound::PRIORITY_GROUP_POWERED_ITEM,
                    0, 0, 0.0f, 0.0f);
            }
            m_pAnimBase->SetAnimation(nTransitionAnimation, 1.0f,
                                      AnimRun::FLAG_SUPPRESS_KEY_REPORTS, 0.0f);
            m_pAnimBase->SetAnimation(nIdleAnimation, 1.0f,
                                      AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED, 0.0f);
        }
        else
        {
            m_pAnimBase->SetAnimation(nIdleAnimation, 1.0f, 0, 0.0f);
        }
    }
    m_bPowered = bPower;
}

static inline void NwcitemTrace1()
{
}
