#include "nwmessage.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "nwcobject.h"
#include "nwcitem.h"
#include "nwcdoor.h"
#include "nwcplaceable.h"
#include "nwcvisualeffectonobject.h"
#include "nwccreature.h"
#include "nwsobject.h"
#include "nwcanimbase.h"
#include "nwcprojectile.h"
#include "nwspell.h"

#include "appmanager.h"
#include "aurora.h"
#include "aurlightmanager.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "exobase.h"
#include "aurscene.h"
#include "aurcamera.h"
#include "swbehaviorcamera.h"
#include "netlayer.h"
#include "clientoptions.h"
#include "nwccreatureappearance.h"
#include "twodimarray.h"
#include <math.h>
#include "nwrules.h"
#include "twodimarrays.h"

short CSWCVisualEffectOnObject::m_nMotionBlurVFXCount;
// PC 0x00700A20
int CSWCVisualEffectOnObject::IsDone()
{
    return m_bSelfIlluminationPulseDone != 0 && m_bAlphaDone != 0 &&
           m_bFOVDone != 0 && m_bVisualEffectDone != 0 &&
           m_nVisualEffectState == 0;
}
// PC 0x00700A60
void CSWCVisualEffectOnObject::ApplySelfIlluminationPulse(
    unsigned short nVisualEffect)
{
    nVisualEffect -= SELFILLUMINATION_PULSE_RED;
    switch (nVisualEffect)
    {
    case SELFILLUMINATION_PULSE_RED - SELFILLUMINATION_PULSE_RED:
        m_vSelfIlluminationPulseStart = Vector(1.0f, 0.0f, 0.0f);
        m_vSelfIlluminationPulseEnd = Vector(1.0f, 1.0f, 1.0f);
        break;
    case SELFILLUMINATION_PULSE_BLUE - SELFILLUMINATION_PULSE_RED:
        m_vSelfIlluminationPulseStart = Vector(0.0f, 0.0f, 1.0f);
        m_vSelfIlluminationPulseEnd = Vector(1.0f, 1.0f, 1.0f);
        break;
    }
}
// PC 0x00700B50
void CSWCVisualEffectOnObject::RemoveBeam(unsigned short nVisualEffect)
{
    if (m_pEffectModel != 0)
    {
        static_cast<Gob*>(m_pEffectModel)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        static_cast<Gob*>(m_pEffectModel)->AttachToScene(0);
    }
}
// PC 0x00700B80
void CSWCVisualEffectOnObject::SetEffectDone(CAurObject* pEffectModel)
{
    if (pEffectModel == m_apAttachedModels[0])
    {
        m_anAttachedModelFadeTime[0] = 3000;
    }
    else if (pEffectModel == m_apAttachedModels[1])
    {
        m_anAttachedModelFadeTime[1] = 3000;
    }
    else if (pEffectModel == m_apAttachedModels[2])
    {
        m_anAttachedModelFadeTime[2] = 3000;
    }
}

int CSWCVisualEffectOnObject::GetHardCodedCessationLength(
    unsigned short nVisualEffect)
{
    if (nVisualEffect == VFX_NONE)
    {
        return 0;
    }
    return HARDCODED_CESSATION_TIME;
}
// PC 0x00700BC0
int CSWCVisualEffectOnObject::GetHardCodedImpactLength(
    unsigned short nVisualEffect)
{
    float fTime;
    float fLength;

    if (nVisualEffect == 0xFFFF)
    {
        return 0;
    }
    if (m_pEffectModel != 0 && nVisualEffect == 1201)
    {
        static_cast<Gob*>(m_pEffectModel)
            ->GetAnimation("impact", &fTime, &fLength);
        return fLength * 1000.0f;
    }
    return 1000;
}
// PC 0x00700C20
int CSWCVisualEffectOnObject::LoadBeam(unsigned short nVisualEffect)
{
    CExoString sModel;
    nVisualEffect -= VFX_BEAM_BEGIN;
    switch (nVisualEffect)
    {
    case BEAM_LIGHTNING_SINGLE:
        sModel = "v_lightns_dur";
        break;
    case BEAM_LIGHTNING_MULTIPLE:
        sModel = "v_lightnx_dur";
        break;
    case BEAM_DROID_DISABLE:
        sModel = "v_drddisab_dur";
        break;
    case BEAM_DROID_KILL:
        sModel = "v_drdkill_dur";
        break;
    case BEAM_DEATH_FIELD:
        sModel = "v_deathfld_dur";
        break;
    case BEAM_DRAIN:
        sModel = "v_drain_dur";
        break;
    case BEAM_FLAME:
        sModel = "v_flame_dur";
        break;
    case BEAM_STUN_RAY:
        sModel = "v_stunray_dur";
        break;
    case BEAM_COLD_RAY:
        sModel = "v_coldray_dur";
        break;
    case BEAM_ION_RAY_1:
        sModel = "v_ionray01_dur";
        break;
    case BEAM_ION_RAY_2:
        sModel = "v_ionray02_dur";
        break;
    case BEAM_FORCE_STORM:
        sModel = "v_fstorm_dur";
        break;
    case BEAM_DROID_STUN:
        sModel = "v_drdstun_dur";
        break;
    case BEAM_FORCE_SHOCK:
        sModel = "v_fshock_dur";
        break;
    case BEAM_FLAMEP:
        sModel = "v_flamep_dur";
        break;
    case BEAM_FLAMEPL:
        sModel = "v_flamepl_dur";
        break;
    }
    m_pEffectModel = NewCAurObject((char*)sModel.CStr(), "fxbeam", 0, 0);
    if (m_pEffectModel == 0)
    {
        return 0;
    }
    static_cast<Gob*>(m_pEffectModel)->SetLightPriority(Gob::LIGHT_PRIORITY_VISUAL_EFFECT);
    static_cast<Gob*>(m_pEffectModel)->DisableFog();
    return 1;
}
// PC 0x00700DB0
int CSWCVisualEffectOnObject::LoadSpellVisual(unsigned short nVisualEffect)
{
    nVisualEffect -= VFX_SPELL_VISUAL_BEGIN;
    switch (nVisualEffect)
    {
    case SPELL_VISUAL_FIZZLE:
    case SPELL_VISUAL_FIZZLE_TIMED:
        m_pEffectModel = NewCAurObject("v_fizzle_imp", "fxfail", 0, 0);
        break;
    case SPELL_VISUAL_RESIST:
        m_pEffectModel = NewCAurObject("v_fresist_imp", "fxresist", 0, 0);
        break;
    default:
        return 0;
    }
    return m_pEffectModel != 0;
}
// PC 0x00700E10
int CSWCVisualEffectOnObject::LoadModelAttachment(unsigned short nVisualEffect)
{
    nVisualEffect -= VFX_MODEL_ATTACHMENT_BEGIN;
    switch (nVisualEffect)
    {
    case MODEL_ATTACHMENT_MEDAL:
        m_pEffectModel = NewCAurObject("v_medal_dur", "fxmedal", 0, 0);
        break;
    case MODEL_ATTACHMENT_REVAN_MASK_1:
        m_pEffectModel = NewCAurObject("v_revmask1_dur", "fxrevanmask1", 0, 0);
        break;
    case MODEL_ATTACHMENT_REVAN_MASK_2:
        m_pEffectModel = NewCAurObject("v_revmask2_dur", "fxrevanmask2", 0, 0);
        break;
    case MODEL_ATTACHMENT_MTHMST:
        m_pEffectModel = NewCAurObject("v_MthMst", "v_MthMst", 0, 0);
        break;
    }
    return m_pEffectModel != 0;
}
// PC 0x00700EA0
void CSWCVisualEffectOnObject::UnloadHardCodedVisualEffect()
{
    if (m_pEffectModel != 0)
    {
        static_cast<Gob*>(m_pEffectModel)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        static_cast<Gob*>(m_pEffectModel)->AttachToScene(0);
        delete m_pEffectModel;
        m_pEffectModel = 0;
    }
}
// PC 0x00700EF0
char CSWCVisualEffectOnObject::GetHardCodedEffectClass()
{
    return m_HardCodedEffectClass;
}
// PC 0x00700F00
char CSWCVisualEffectOnObject::GetHardCodedEffectClass(
    unsigned short nVisualEffect)
{
    if (nVisualEffect < 200)
    {
        return 2;
    }
    if (nVisualEffect < 300)
    {
        return 13;
    }
    if (nVisualEffect < 400)
    {
        return 4;
    }
    if (nVisualEffect >= 400 && nVisualEffect < 500)
    {
        return 13;
    }
    if (nVisualEffect >= 500 && nVisualEffect < 600)
    {
        return 13;
    }
    if (nVisualEffect < 700)
    {
        return 7;
    }
    if (nVisualEffect < 800)
    {
        return 8;
    }
    if (nVisualEffect < 900)
    {
        return 9;
    }
    if (nVisualEffect < 1000)
    {
        return 10;
    }
    if (nVisualEffect < 1100)
    {
        return 1;
    }
    if (nVisualEffect < 1200)
    {
        return 11;
    }
    if (nVisualEffect < 1300)
    {
        return 12;
    }
    if (nVisualEffect < 114)
    {
        return 14;
    }
    if (nVisualEffect < 1600)
    {
        return 15;
    }
    if (nVisualEffect < 116)
    {
        return 16;
    }
    return 0;
}
// PC 0x00700810
CSWCVisualEffectOnObject::CSWCVisualEffectOnObject()
{
    int i;

    m_sBeamTargetNode = "";
    m_oidTargetObject = OBJECT_INVALID;
    m_oidCreator = OBJECT_INVALID;
    m_oidTarget = OBJECT_INVALID;
    for (i = 0; i < 3; i++)
    {
        m_apAttachedModels[i] = 0;
        m_apVisualEffectModels[i] = 0;
        m_anAttachedModelAnimationTime[i] = 0;
        m_anAttachedModelAnimationState[i] = 0;
        m_anVisualEffectModelAnimationTime[i] = 0;
        m_anVisualEffectModelAnimationState[i] = 0;
    }
    m_anVisualEffect[0] = 0;
    m_anVisualEffect[1] = 0;
    m_anVisualEffect[2] = 0;
    m_bVisualEffectShown = 0;
    m_nVisualEffect = 0;
    m_bSelfIlluminationPulseDone = 0;
    m_bAlphaDone = 0;
    m_bFOVDone = 0;
    m_bCessationDone = 0;
    m_anAttachedModelFadeTime[0] = 0;
    m_anAttachedModelFadeTime[1] = 0;
    m_anAttachedModelFadeTime[2] = 0;
    m_nHardCodedEffectTime = 0;
    m_nVisualEffectState = 0;
    m_nAlphaState = ALPHA_FADING_OUT;
    m_pEffectModel = 0;
    m_bVisualEffectStopped = 0;
    m_bVisualEffectDone = 0;
    m_vSelfIlluminationPulseStart = Vector(0.0f, 0.0f, 0.0f);
    m_vSelfIlluminationPulseEnd = Vector(0.0f, 0.0f, 0.0f);
    m_vSelfIlluminationPulseCurrent = Vector(0.0f, 0.0f, 0.0f);
    m_fAlpha = 1.0f;
    m_fAlphaTarget = 0.0f;
    m_bLightSourceDynamic = 0;
    m_nEffectType = 0;
    m_HardCodedEffectClass = 0;
    m_bLightSourceShadows = 0;
    m_bLightSourceApplied = 0;
    m_fSavedFOV = 0.0f;
    m_fSavedCameraDistance = 0.0f;
    m_pSoundSource = 0;
    m_bRumblePerformed = 0;
    m_bHardCodedEffectApplied = 0;
    m_nF8 = 0;
}
// PC 0x00702DE0
void CSWCVisualEffectOnObject::UnloadVisualEffect()
{
    int i;

    UnloadHardCodedVisualEffect();
    for (i = 0; i < 3; i++)
    {
        if (m_apAttachedModels[i] != 0)
        {
            delete m_apAttachedModels[i];
            m_apAttachedModels[i] = 0;
        }
        if (m_apVisualEffectModels[i] != 0)
        {
            delete m_apVisualEffectModels[i];
            m_apVisualEffectModels[i] = 0;
        }
    }
}

CSWCObject* CSWCVisualEffectOnObject::GetTargetCSWCObject()
{
    CSWCObject* pObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_oidTargetObject);
    if (pObject == 0)
    {
        return 0;
    }
    return pObject->AsSWCObject();
}

CSWCObject* CSWCVisualEffectOnObject::GetSourceCSWCObject()
{
    CSWCObject* pObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_oidCreator);
    if (pObject == 0)
    {
        return 0;
    }
    return pObject->AsSWCObject();
}

void CSWCVisualEffectOnObject::DoVFXRumble()
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        g_pAppManager->GetClientExoApp()->LookUpAndPerformRumbleWithCutOff(
            m_nVisualEffect, 1, pObject->m_vPosition);
    }
}
// PC 0x00700FB0
void CSWCVisualEffectOnObject::DetachVisualEffect()
{
    int i;

    m_bLightSourceApplied = 0;
    for (i = 0; i < 3; i++)
    {
        if (m_apAttachedModels[i] != 0)
        {
            static_cast<Gob*>(m_apAttachedModels[i])->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            static_cast<Gob*>(m_apAttachedModels[i])->AttachToScene(0);
        }
    }
    if (m_pEffectModel != 0)
    {
        if (m_HardCodedEffectClass == 4)
        {
            CSWCObject* pObject = GetTargetCSWCObject();
            if (pObject != 0)
            {
                CSWCCreature* pCreature = pObject->AsSWCCreature();
                if (pCreature != 0 &&
                    pCreature ==
                        g_pAppManager->m_pClientExoApp->GetPlayerCreature())
                {
                    if (pCreature->m_pArea != 0)
                    {
                        pCreature->m_pArea->UpdateShadowingLights();
                    }
                }
            }
        }
        static_cast<Gob*>(m_pEffectModel)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        static_cast<Gob*>(m_pEffectModel)->AttachToScene(0);
    }
}
// PC 0x00701070
void CSWCVisualEffectOnObject::AttachBeam()
{
    CSWCObject* pTarget = GetTargetCSWCObject();
    CSWCObject* pCreator = GetSourceCSWCObject();
    if (pTarget != 0 && pCreator != 0 && m_pEffectModel != 0)
    {
        static_cast<Gob*>(m_pEffectModel)->AttachToObject(
            pCreator->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
            (char*)m_sBeamTargetNode.CStr(), 0);
        static_cast<Gob*>(m_pEffectModel)->ReattachReference(
            pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
            (char*)m_sBeamSourceNode.CStr());
    }
}

CSWCObject* CSWCVisualEffectOnObject::GetOwnerCSWCObject()
{
    CSWCObject* pObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_oidTarget);
    if (pObject == 0)
    {
        return 0;
    }
    return pObject->AsSWCObject();
}
// PC 0x00701130
void CSWCVisualEffectOnObject::AttachLight()
{
    Quaternion qOrientation;
    Vector vPosition;
    CExoString sAttachNode;

    CSWCObject* pTarget = GetTargetCSWCObject();
    if (pTarget == 0 || m_pEffectModel == 0)
    {
        return;
    }
    Gob* pTargetModel = pTarget->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    if (pTargetModel == 0)
    {
        return;
    }
    CSWCPlaceable* pPlaceable = pTarget->AsSWCPlaceable();
    CSWCDoor* pDoor = pTarget->AsSWCDoor();
    if (pTarget->AsSWCCreature() != 0)
    {
        pTargetModel->GetPartLocalPosition("headconjure", &vPosition, &qOrientation);
        vPosition.m_fY += 0.75f;
        vPosition.m_fZ += 0.5f;
    }
    else if (pPlaceable != 0)
    {
        CExoString sPart;
        sPart = pPlaceable->m_sModel + "_head_hit";
        pTargetModel->GetPartLocalPosition((char*)sPart.CStr(), &vPosition, &qOrientation);
        vPosition.m_fY += 0.375f;
        vPosition.m_fZ += 0.25f;
    }
    else if (pDoor != 0)
    {
        CExoString sPart;
        sPart = pDoor->m_sModel + "hhit";
        pTargetModel->GetPartLocalPosition((char*)sPart.CStr(), &vPosition, &qOrientation);
        vPosition.m_fY += 0.375f;
        vPosition.m_fZ += 0.25f;
    }
    else
    {
        qOrientation = Quaternion();
        vPosition = Vector();
    }
    static_cast<Gob*>(m_pEffectModel)->SetPartLocalPosition("root", vPosition, qOrientation);
    if (pPlaceable != 0)
    {
        sAttachNode = pPlaceable->m_sModel + "_ground";
    }
    else if (pDoor != 0)
    {
        sAttachNode = pDoor->m_sModel + "grnd";
    }
    else
    {
        sAttachNode = "root";
    }
    static_cast<Gob*>(m_pEffectModel)->AttachToObject(
        pTargetModel, (char*)sAttachNode.CStr(), Gob::ATTACH_NORMAL);
    CSWCCreature* pCreature = pTarget->AsSWCCreature();
    if (pCreature != 0 && pCreature == g_pAppManager->GetClientExoApp()->GetPlayerCreature())
    {
        if (pCreature->m_pArea != 0)
        {
            pCreature->m_pArea->UpdateShadowingLights();
        }
    }
}
// PC 0x00701560
void CSWCVisualEffectOnObject::ApplyBumpedOutShell(unsigned short nVisualEffect)
{
    CExoString sTexture;
    m_nEffectType = 20;
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        nVisualEffect -= 1400;
        switch (nVisualEffect)
        {
        case 1:
            sTexture = "fx_tex_01";
            break;
        case 2:
            sTexture = "fx_tex_02";
            break;
        case 3:
            sTexture = "fx_tex_03";
            break;
        case 4:
            sTexture = "fx_tex_04";
            break;
        case 5:
            sTexture = "fx_tex_05";
            break;
        case 6:
            sTexture = "fx_tex_06";
            break;
        case 7:
            sTexture = "fx_tex_07";
            break;
        case 8:
            sTexture = "fx_tex_08";
            break;
        case 9:
            sTexture = "fx_tex_09";
            break;
        case 10:
            sTexture = "fx_tex_10";
            break;
        case 11:
            sTexture = "fx_tex_11";
            break;
        case 12:
            sTexture = "fx_tex_12";
            break;
        case 13:
            sTexture = "fx_tex_14";
            break;
        case 14:
            sTexture = "fx_tex_15";
            break;
        case 15:
            sTexture = "fx_tex_16";
            break;
        case 16:
            sTexture = "fx_tex_17";
            break;
        case 17:
            sTexture = "fx_tex_18";
            break;
        case 18:
            sTexture = "fx_tex_19";
            break;
        case 19:
            sTexture = "fx_tex_20";
            break;
        case 20:
            sTexture = "fx_tex_21";
            break;
        case 21:
            sTexture = "fx_tex_22";
            break;
        case 22:
            sTexture = "fx_tex_23";
            break;
        case 23:
            sTexture = "fx_tex_24";
            break;
        case 24:
            sTexture = "fx_tex_25";
            break;
        case 25:
            sTexture = "fx_tex_26";
            break;
        case 26:
            sTexture = "fx_tex_stealth";
            break;
        default:
            return;
        }
        pObject->EnableBumpedOutShell(sTexture, 0.02f);
        CSWCCreature* pCreature = pObject->AsSWCCreature();
        if (pCreature != 0)
        {
            CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
            CSWCItem* pItem = pExoApp->GetItemByGameObjectID(
                pCreature->GetEquippedItemID(INVENTORY_SLOT_HEAD));
            if (pItem != 0)
            {
                pItem->EnableBumpedOutShell(sTexture, 0.02f);
            }
        }
    }
}
// PC 0x007017B0
void CSWCVisualEffectOnObject::RemoveBumpedOutShell()
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        pObject->DisableBumpedOutShell();
        CSWCCreature* pCreature = pObject->AsSWCCreature();
        if (pCreature != 0)
        {
            CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
            CSWCItem* pItem = pExoApp->GetItemByGameObjectID(
                pCreature->GetEquippedItemID(INVENTORY_SLOT_HEAD));
            if (pItem != 0)
            {
                pItem->DisableBumpedOutShell();
            }
        }
    }
}
// PC 0x00701820
void CSWCVisualEffectOnObject::ApplyMotionBlur(unsigned short nVisualEffect)
{
    CSWCObject* pGameObject;
    CSWCCreature* pPlayerCreature;

    pGameObject = GetTargetCSWCObject();
    if (pGameObject != 0)
    {
        nVisualEffect -= 1600;
        switch (nVisualEffect)
        {
        case 1:
            ++m_nMotionBlurVFXCount;
            pPlayerCreature =
                g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            if ((pPlayerCreature == 0 ||
                 pPlayerCreature->m_nId == pGameObject->m_nId) &&
                m_nMotionBlurVFXCount == 1 &&
                g_pAppManager->m_pClientExoApp->GetMiniGame() == 0)
            {
                AurSetSpeedBlurRatio(0.75f);
                AurEnableSpeedBlur();
            }
            break;
        case 2:
            ++m_nMotionBlurVFXCount;
            pPlayerCreature =
                g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            if ((pPlayerCreature == 0 ||
                 pPlayerCreature->m_nId == pGameObject->m_nId) &&
                m_nMotionBlurVFXCount == 1 &&
                g_pAppManager->m_pClientExoApp->GetMiniGame() == 0)
            {
                AurSetSpeedBlurRatio(0.75f);
                AurEnableSpeedBlur();
            }
            break;
        }
    }
}
// PC 0x007018B0
void CSWCVisualEffectOnObject::RemoveMotionBlur()
{
    CSWCObject* pGameObject;

    pGameObject = GetTargetCSWCObject();
    if (pGameObject != 0)
    {
        if (--m_nMotionBlurVFXCount < 0)
        {
            m_nMotionBlurVFXCount = 0;
        }
        else if (m_nMotionBlurVFXCount != 0)
        {
            return;
        }
        if (g_pAppManager->m_pClientExoApp->GetMiniGame() == 0)
        {
            AurDisableSpeedBlur();
        }
    }
}
// PC 0x00701910
void CSWCVisualEffectOnObject::ApplyDistortion(unsigned short nVisualEffect)
{
    CSWCObject* pGameObject;

    pGameObject = GetTargetCSWCObject();
    if (pGameObject != 0)
    {
        nVisualEffect -= 1800;
        if (nVisualEffect == 0)
        {
            pGameObject->EnableDistortion();
        }
    }
}
// PC 0x00701950
void CSWCVisualEffectOnObject::RemoveDistortion(unsigned short nVisualEffect)
{
    CSWCObject* pGameObject;

    pGameObject = GetTargetCSWCObject();
    if (pGameObject != 0)
    {
        nVisualEffect -= 1800;
        if (nVisualEffect == 0)
        {
            pGameObject->DisableDistortion();
        }
    }
}
// PC 0x00701990
void CSWCVisualEffectOnObject::AttachVFXModelToTargetModel(
    CAurObject* pEffectModel, CAurObject* pTargetModel,
    CExoString const& sNode)
{
    int nOrientationOff;
    if (!g_pRules->Get2DArrays()->m_pVisualEffect->GetINTEntry(
            CExoString(m_nVisualEffect), "OrientationOff",
            &nOrientationOff))
    {
        nOrientationOff = 0;
    }
    if (nOrientationOff != 0)
    {
        Quaternion qOrientation;
        Vector vPosition;
        static_cast<Gob*>(pEffectModel)->AttachToObject(
            pTargetModel, (char*)sNode.CStr(), Gob::ATTACH_NO_ORIENTATION);
        static_cast<Gob*>(pEffectModel)->GetPartPosition(
            "root", &vPosition, &qOrientation);
        static_cast<Gob*>(pEffectModel)->SetPartPosition(
            "root", vPosition, Quaternion());
    }
    else
    {
        static_cast<Gob*>(pEffectModel)->AttachToObject(
            pTargetModel, (char*)sNode.CStr(), Gob::ATTACH_NORMAL);
    }
}
// PC 0x007058E0
CSWCVisualEffectOnObject::~CSWCVisualEffectOnObject()
{
    if (m_pSoundSource != 0)
    {
        delete m_pSoundSource;
    }
    UnloadVisualEffect();
}
// PC 0x00701B50
int CSWCVisualEffectOnObject::LoadVisualEffect(
    unsigned short nEffect, int bImpactOnly, unsigned long oidObject,
    unsigned long oidCreator, unsigned long oidTarget,
    unsigned char nSourceHook, unsigned char nTargetHook, Vector vOffset)
{
    CExoString sTypeFd;
    CExoString sImpHeadconNode;
    CExoString sImpImpactNode;
    CExoString sImpRootNode;

    m_nVisualEffect = nEffect;
    m_oidTargetObject = oidObject;
    m_oidCreator = oidCreator;
    m_oidTarget = oidTarget;

    CSWCObject* pTarget = GetTargetCSWCObject();
    if (pTarget == 0)
    {
        return 0;
    }
    CSWCObject* pSource = GetSourceCSWCObject();
    CExoString sRow(nEffect);

    {
        CExoString sColumn("Type_FD");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        p2DA->GetCExoStringEntry(sRow, sColumn, &sTypeFd);
    }
    {
        CExoString sColumn("Imp_HeadCon_Node");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        p2DA->GetCExoStringEntry(sRow, sColumn, &sImpHeadconNode);
    }
    if (sImpHeadconNode != "")
    {
        CExoString sLowQuality;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()
                ->m_bHighQualityPersistentVFX == 0)
        {
            int bEntryFound;
            {
                CExoString sColumn("LowQuality");
                CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                C2DA* p2DA = p2DArrays->m_pVisualEffect;
                bEntryFound = p2DA->GetCExoStringEntry(sRow, sColumn, &sLowQuality);
            }
            if (bEntryFound)
            {
                sImpHeadconNode = sLowQuality;
            }
        }
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()
                ->m_nGoreLevel < CClientOptions::GORE_LEVEL_FULL)
        {
            int bEntryFound;
            {
                CExoString sColumn("LowViolence");
                CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                C2DA* p2DA = p2DArrays->m_pVisualEffect;
                bEntryFound = p2DA->GetCExoStringEntry(sRow, sColumn, &sLowQuality);
            }
            if (bEntryFound)
            {
                sImpHeadconNode = sLowQuality;
            }
        }
    }
    {
        CExoString sColumn("Imp_Impact_Node");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        p2DA->GetCExoStringEntry(sRow, sColumn, &sImpImpactNode);
    }
    if (sImpImpactNode != "")
    {
        CExoString sLowQuality;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()
                ->m_bHighQualityPersistentVFX == 0)
        {
            int bEntryFound;
            {
                CExoString sColumn("LowQuality");
                CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                C2DA* p2DA = p2DArrays->m_pVisualEffect;
                bEntryFound = p2DA->GetCExoStringEntry(sRow, sColumn, &sLowQuality);
            }
            if (bEntryFound)
            {
                sImpImpactNode = sLowQuality;
            }
        }
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()
                ->m_nGoreLevel < CClientOptions::GORE_LEVEL_FULL)
        {
            CExoString sLowViolence;
            int bEntryFound;
            {
                CExoString sColumn("LowViolence");
                CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                C2DA* p2DA = p2DArrays->m_pVisualEffect;
                bEntryFound = p2DA->GetCExoStringEntry(sRow, sColumn, &sLowViolence);
            }
            if (bEntryFound)
            {
                {
                    CExoString sColumn("Imp_Impact_Node");
                    CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                    C2DA* p2DA = p2DArrays->m_pVisualEffect;
                    p2DA->GetCExoStringEntry(sLowViolence, sColumn, &sImpImpactNode);
                }
            }
        }
    }

    int nSizeCategory;
    CSWCCreature* pCreature = pTarget->AsSWCCreature();
    if (pCreature != 0)
    {
        unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
        if (!g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
                nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY,
                &nSizeCategory))
        {
            nSizeCategory = CREATURE_SIZE_MEDIUM;
        }
    }
    else
    {
        nSizeCategory = CREATURE_SIZE_MEDIUM;
    }
    if (nSizeCategory >= CREATURE_SIZE_HUGE)
    {
        {
            CExoString sColumn("Imp_Root_H_Node");
            CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            p2DA->GetCExoStringEntry(sRow, sColumn, &sImpRootNode);
        }
    }
    if (nSizeCategory >= CREATURE_SIZE_LARGE && sImpRootNode.GetLength() == 0)
    {
        {
            CExoString sColumn("Imp_Root_L_Node");
            CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            p2DA->GetCExoStringEntry(sRow, sColumn, &sImpRootNode);
        }
    }
    if (nSizeCategory >= CREATURE_SIZE_TINY && sImpRootNode.GetLength() == 0)
    {
        {
            CExoString sColumn("Imp_Root_M_Node");
            CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            p2DA->GetCExoStringEntry(sRow, sColumn, &sImpRootNode);
        }
    }
    if (sImpRootNode != "")
    {
        CExoString sLowQuality;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()
                ->m_bHighQualityPersistentVFX == 0)
        {
            int bEntryFound;
            {
                CExoString sColumn("LowQuality");
                CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                C2DA* p2DA = p2DArrays->m_pVisualEffect;
                bEntryFound = p2DA->GetCExoStringEntry(sRow, sColumn, &sLowQuality);
            }
            if (bEntryFound)
            {
                sImpRootNode = sLowQuality;
            }
        }
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()
                ->m_nGoreLevel < CClientOptions::GORE_LEVEL_FULL)
        {
            CExoString sLowViolence;
            int bEntryFound;
            {
                CExoString sColumn("LowViolence");
                CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                C2DA* p2DA = p2DArrays->m_pVisualEffect;
                bEntryFound = p2DA->GetCExoStringEntry(sRow, sColumn, &sLowViolence);
            }
            if (bEntryFound)
            {
                if (nSizeCategory >= CREATURE_SIZE_HUGE)
                {
                    {
                        CExoString sColumn("Imp_Root_H_Node");
                        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                        C2DA* p2DA = p2DArrays->m_pVisualEffect;
                        p2DA->GetCExoStringEntry(sLowViolence, sColumn, &sImpRootNode);
                    }
                }
                if (nSizeCategory >= CREATURE_SIZE_LARGE &&
                    sImpRootNode.GetLength() == 0)
                {
                    {
                        CExoString sColumn("Imp_Root_L_Node");
                        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                        C2DA* p2DA = p2DArrays->m_pVisualEffect;
                        p2DA->GetCExoStringEntry(sLowViolence, sColumn, &sImpRootNode);
                    }
                }
                if (nSizeCategory >= CREATURE_SIZE_TINY &&
                    sImpRootNode.GetLength() == 0)
                {
                    {
                        CExoString sColumn("Imp_Root_M_Node");
                        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
                        C2DA* p2DA = p2DArrays->m_pVisualEffect;
                        p2DA->GetCExoStringEntry(sLowViolence, sColumn, &sImpRootNode);
                    }
                }
            }
        }
    }

    int nProgfxImpact;
    int nProgfxDuration;
    int nProgfxCessation;
    int bProgfxImpact;
    int bProgfxDuration;
    int bProgfxCessation;
    if (sImpRootNode.GetLength() == 0)
    {
        {
            CExoString sColumn("ProgFX_Impact");
            CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            bProgfxImpact = p2DA->GetINTEntry(sRow, sColumn, &nProgfxImpact);
        }
        {
            CExoString sColumn("ProgFX_Duration");
            CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            bProgfxDuration = p2DA->GetINTEntry(sRow, sColumn, &nProgfxDuration);
        }
        {
            CExoString sColumn("ProgFX_Cessation");
            CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            bProgfxCessation = p2DA->GetINTEntry(sRow, sColumn, &nProgfxCessation);
        }
    }
    else
    {
        bProgfxImpact = 0;
        bProgfxDuration = 0;
        bProgfxCessation = 0;
    }

    CSWCPlaceable* pSourcePlaceable;
    CSWCDoor* pSourceDoor;
    if (pSource != 0)
    {
        pSourcePlaceable = pSource->AsSWCPlaceable();
        pSourceDoor = pSource->AsSWCDoor();
    }
    else
    {
        pSourcePlaceable = 0;
        pSourceDoor = 0;
    }
    if (nSourceHook == BEAM_HOOK_HAND)
    {
        if (pSourcePlaceable != 0)
            m_sBeamTargetNode = pSourcePlaceable->m_sModel + "_hand";
        else if (pSourceDoor != 0)
            m_sBeamTargetNode = pSourceDoor->m_sModel + "impc";
        else
            m_sBeamTargetNode = "handconjure";
    }
    else if (nSourceHook == BEAM_HOOK_IMPACT)
    {
        if (pSourcePlaceable != 0)
            m_sBeamTargetNode = pSourcePlaceable->m_sModel + "_impact";
        else if (pSourceDoor != 0)
            m_sBeamTargetNode = pSourceDoor->m_sModel + "impc";
        else
            m_sBeamTargetNode = "impact";
    }
    else if (nSourceHook == BEAM_HOOK_HEAD)
    {
        if (pSourcePlaceable != 0)
            m_sBeamTargetNode = pSourcePlaceable->m_sModel + "_head";
        else if (pSourceDoor != 0)
            m_sBeamTargetNode = pSourceDoor->m_sModel + "impc";
        else
            m_sBeamTargetNode = "headconjure";
    }
    else if (nSourceHook == BEAM_HOOK_LEFT_HAND)
    {
        if (pSourcePlaceable != 0)
            m_sBeamTargetNode = pSourcePlaceable->m_sModel + "_ground";
        else if (pSourceDoor != 0)
            m_sBeamTargetNode = pSourceDoor->m_sModel + "grnd";
        else
            m_sBeamTargetNode = "lhand";
    }
    else if (nSourceHook == BEAM_HOOK_RIGHT_HAND)
    {
        if (pSourcePlaceable != 0)
            m_sBeamTargetNode = pSourcePlaceable->m_sModel + "_ground";
        else if (pSourceDoor != 0)
            m_sBeamTargetNode = pSourceDoor->m_sModel + "grnd";
        else
            m_sBeamTargetNode = "rhand";
    }
    else
    {
        if (pSourcePlaceable != 0)
            m_sBeamTargetNode = pSourcePlaceable->m_sModel + "_ground";
        else if (pSourceDoor != 0)
            m_sBeamTargetNode = pSourceDoor->m_sModel + "grnd";
        else
            m_sBeamTargetNode = "root";
    }

    CSWCPlaceable* pTargetPlaceable = pTarget->AsSWCPlaceable();
    CSWCDoor* pTargetDoor = pTarget->AsSWCDoor();
    if (nTargetHook == BEAM_HOOK_HAND || nTargetHook == BEAM_HOOK_IMPACT)
    {
        if (pTargetPlaceable != 0)
            m_sBeamSourceNode = pTargetPlaceable->m_sModel + "_impact";
        else if (pTargetDoor != 0)
            m_sBeamSourceNode = pTargetDoor->m_sModel + "impc";
        else
            m_sBeamSourceNode = "impact";
    }
    else
    {
        if (pTargetPlaceable != 0)
            m_sBeamSourceNode = pTargetPlaceable->m_sModel + "_ground";
        else if (pTargetDoor != 0)
            m_sBeamSourceNode = pTargetDoor->m_sModel + "grnd";
        else
            m_sBeamSourceNode = "root";
    }
    if (pSource != 0)
    {
        Gob* pSourceModel = pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        if (pSourceModel != 0 &&
            pSourceModel->GetPart((char*)m_sBeamTargetNode.CStr()) == 0)
        {
            m_sBeamTargetNode = "root";
        }
    }
    Gob* pTargetModel = pTarget->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    if (pTargetModel != 0 &&
        pTargetModel->GetPart((char*)m_sBeamSourceNode.CStr()) == 0)
    {
        m_sBeamSourceNode = "root";
    }

    m_vOffset = vOffset;
    m_bSelfIlluminationPulseDone = 1;
    m_bAlphaDone = 1;
    m_bFOVDone = 1;
    m_anVisualEffect[0] = VFX_NONE;
    m_anVisualEffect[1] = VFX_NONE;
    m_anVisualEffect[2] = VFX_NONE;
    m_bPlayDurationAnimation = bImpactOnly == 0;
    if (bProgfxImpact)
        m_anVisualEffect[0] = nProgfxImpact;
    if (bImpactOnly == 0)
    {
        if (bProgfxDuration)
            m_anVisualEffect[1] = nProgfxDuration;
        if (bProgfxCessation)
            m_anVisualEffect[2] = nProgfxCessation;
    }
    if (sImpRootNode.GetLength() > 0)
    {
        m_apAttachedModels[0] = NewCAurObject(
            (char*)sImpRootNode.CStr(), "fxground", 0, 0);
        if (m_apAttachedModels[0] != 0)
        {
            m_bSelfIlluminationPulseDone = 0;
            static_cast<Gob*>(m_apAttachedModels[0])->SetLightPriority(
                Gob::LIGHT_PRIORITY_VISUAL_EFFECT);
            static_cast<Gob*>(m_apAttachedModels[0])->DisableFog();
        }
    }
    if (sImpImpactNode.GetLength() > 0)
    {
        m_apAttachedModels[1] = NewCAurObject(
            (char*)sImpImpactNode.CStr(), "fximpact", 0, 0);
        if (m_apAttachedModels[1] != 0)
        {
            m_bAlphaDone = 0;
            static_cast<Gob*>(m_apAttachedModels[1])->SetLightPriority(
                Gob::LIGHT_PRIORITY_VISUAL_EFFECT);
            static_cast<Gob*>(m_apAttachedModels[1])->DisableFog();
        }
    }
    if (sImpHeadconNode.GetLength() > 0)
    {
        m_apAttachedModels[2] = NewCAurObject(
            (char*)sImpHeadconNode.CStr(), "fxhead", 0, 0);
        if (m_apAttachedModels[2] != 0)
        {
            m_bFOVDone = 0;
            static_cast<Gob*>(m_apAttachedModels[2])->SetLightPriority(
                Gob::LIGHT_PRIORITY_VISUAL_EFFECT);
            static_cast<Gob*>(m_apAttachedModels[2])->DisableFog();
        }
    }
    m_nHardCodedEffectTime = 0;
    if (m_anVisualEffect[0] != VFX_NONE)
    {
        m_nHardCodedEffectTime = GetHardCodedImpactLength(m_anVisualEffect[0]);
    }
    else if (m_anVisualEffect[1] == VFX_NONE && m_anVisualEffect[2] != VFX_NONE)
    {
        m_nHardCodedEffectTime = CESSATION_ONLY_IMPACT_TIME;
    }
    m_bVisualEffectStopped = 1;
    m_bRumblePerformed = 0;
    return 1;
}
// PC 0x00702E60
void CSWCVisualEffectOnObject::ApplyCheat(unsigned short nVisualEffect)
{
    CExoString sModel;
    CSWCObject* pTarget = GetTargetCSWCObject();
    CSWCObject* pSource = GetSourceCSWCObject();
    if (pTarget == 0 || pSource == 0)
        return;

    CSWCProjectile* pProjectile = new CSWCProjectile;
    switch (nVisualEffect)
    {
    case CHEAT_KUNG_POW:
        sModel = "vpr_kngpow";
        break;
    }
    if (!pProjectile->LoadModel(sModel, CSWCObject::MODEL_SLOT_DEFAULT, 0))
    {
        delete pProjectile;
        return;
    }

    Vector vPosition = pSource->m_vPosition + 4.0f * pSource->m_vOrientation;
    vPosition.m_fZ += 1.0f;
    pProjectile->SetPosition(vPosition);
    pProjectile->SetOrientation(pSource->m_vOrientation);
    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (pModule != 0 && pModule->m_pInfo != 0)
    {
        CAurScene* pScene = pModule->m_pInfo->m_pScene;
        pProjectile->AttachToScene(pScene);
    }
    pProjectile->SetAnimation(CSWCProjectile::PROJECTILE_ANIMATION_TRAVEL);
    pProjectile->SetProjectileSpellId(11000);
    if (equal(m_vOffset, Vector()))
    {
        pProjectile->m_vDesiredPosition = pTarget->m_vPosition;
        pProjectile->SetDesiredTargetObject(pTarget->GetId(), "");
    }
    else
    {
        pProjectile->m_vDesiredPosition = m_vOffset;
    }
    switch (nVisualEffect)
    {
    case CHEAT_KUNG_POW:
    {
        float fDistance = magnitude(pTarget->m_vPosition - pSource->m_vPosition);
        pProjectile->SetDesiredPositionTimer((int)
            (fDistance / ((float)log(fDistance) * 3.0f + 2.0f) * 1000.0f));
        pProjectile->m_nOrientationType =
            CSWCProjectile::PROJECTILE_ORIENTATION_MOVEMENT;
        pProjectile->SetProjectileType(CSWCProjectile::PROJECTILE_PATH_TYPE_BURST_UP);
        CResRef cSound("c_cow_atk1");
        g_pExoSound->Play3DOneShotSound(cSound,
            pSource->m_vPosition, 1.5f, CExoSound::PRIORITY_GROUP_VISUAL_EFFECT,
            0, 0, 0.0f, 0.0f);
        break;
    }
    }
}
// PC 0x00703180
void CSWCVisualEffectOnObject::ApplySpellVisual(unsigned short nVisualEffect)
{
    nVisualEffect -= VFX_SPELL_VISUAL_BEGIN;
    switch (nVisualEffect)
    {
    case SPELL_VISUAL_FIZZLE:
        m_sBeamSourceNode = "headconjure";
        break;
    case SPELL_VISUAL_FIZZLE_TIMED:
        m_sBeamSourceNode = "handconjure";
        break;
    case SPELL_VISUAL_RESIST:
        m_sBeamSourceNode = "impact";
        break;
    }
    if (m_pEffectModel != 0)
    {
        if (nVisualEffect == SPELL_VISUAL_RESIST)
        {
            Quaternion qOrientation;
            Vector vPosition;
            CSWCObject* pCreator = GetSourceCSWCObject();
            CSWCObject* pTarget = GetTargetCSWCObject();
            static_cast<Gob*>(m_pEffectModel)->GetPartPosition(
                "root", &vPosition, &qOrientation);
            if (pCreator != 0 && pTarget != 0)
            {
                Vector vDirection;
                vDirection = pCreator->m_vPosition - pTarget->m_vPosition;
                if (vDirection * vDirection <= SPELL_VISUAL_DIRECTION_EPSILON_SQUARED)
                {
                    vDirection = Vector(0.0f, 0.0f, 0.0f);
                }
                vDirection = normalize(vDirection);
                qOrientation = YawPitchRoll(Yaw(vDirection), 0.0f, 0.0f);
            }
            static_cast<Gob*>(m_pEffectModel)->SetPartPosition(
                "root", vPosition, qOrientation);
            pTarget = GetTargetCSWCObject();
            static_cast<Gob*>(m_pEffectModel)->AttachToObject(
                pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
                (char*)m_sBeamSourceNode.CStr(), Gob::ATTACH_NO_ORIENTATION);
        }
        else
        {
            CSWCObject* pTarget = GetTargetCSWCObject();
            static_cast<Gob*>(m_pEffectModel)->AttachToObject(
                pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
                (char*)m_sBeamSourceNode.CStr(), 0);
        }
        static_cast<Gob*>(m_pEffectModel)->PlayAnimation(
            "impact", 1.0f, AnimRun::FLAG_SUPPRESS_KEY_REPORTS, 0.0f);
    }
}
// PC 0x00703450
void CSWCVisualEffectOnObject::ApplyLightsaberThrow(unsigned short nVisualEffect)
{
    CSWCObject* pTarget = GetTargetCSWCObject();
    CSWCObject* pSource = GetSourceCSWCObject();
    CSWCObject* pOwner = GetOwnerCSWCObject();
    float fThrowRate = 37.0f;
    if (pOwner != 0 && pOwner->AsSWCCreature() != 0)
    {
        fThrowRate = pOwner->AsSWCCreature()->GetLightSaberThrowRate();
    }
    if (pTarget != 0 && pSource != 0 && pOwner != 0 &&
        pOwner->AsSWCCreature() != 0)
    {
        CSWCCreature* pCreature = pOwner->AsSWCCreature();
        CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
            pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
        if (pItem != 0)
        {
            CSWCProjectile* pProjectile = new CSWCProjectile;
            pProjectile->SetProjectileSpellId(CSWSpell::FORCE_POWER_LIGHT_SABER_THROW);
            pProjectile->m_nSourceTargetId = pOwner->GetId();
            {
                CResRef cModel((char*)pItem->m_cModelResRef.GetResRef());
                pProjectile->LoadModel(cModel, CSWCObject::MODEL_SLOT_DEFAULT, 0);
            }
            pProjectile->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->DisableForceAnimations();
            Vector vPosition;
            Quaternion qOrientation;
            if (pSource->GetId() == pOwner->GetId())
            {
                pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition(
                    "rhand", &vPosition, &qOrientation);
            }
            else
            {
                pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition(
                    "impact", &vPosition, &qOrientation);
            }
            pProjectile->SetPosition(vPosition);
            pCreature->SetThrownLightsaber(pProjectile->GetId());
            Vector vOrientation = pSource->m_vOrientation;
            Vector vAxis = cross(vOrientation, Vector(0.0f, 0.0f, 1.0f));
            Quaternion qRotation(vAxis, 1.2217305f);
            pProjectile->SetOrientation(qRotation * vOrientation);
            pProjectile->SetDesiredPosition(pTarget->m_vPosition);
            if (pOwner == pTarget)
            {
                pProjectile->SetDesiredTargetObject(pTarget->GetId(), "rhand");
            }
            else
            {
                pProjectile->SetDesiredTargetObject(pTarget->GetId(), "");
            }
            float fDistance = magnitude(pTarget->m_vPosition - pSource->m_vPosition);
            int nThrowTime = (int)(fDistance / fThrowRate * LIGHTSABER_THROW_TIME_SCALE);
            pProjectile->SetDesiredPositionTimer(nThrowTime);
            pProjectile->SetAnimation(CSWCProjectile::PROJECTILE_ANIMATION_THROWOUT);
            pProjectile->SetProjectileType(CSWCProjectile::PROJECTILE_PATH_TYPE_HOMING);
            pProjectile->m_nImpactType = CSWCProjectile::PROJECTILE_IMPACT_NONE;
            CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
            if (pModule != 0 && pModule->GetArea() != 0)
            {
                pProjectile->AttachToScene(pModule->GetArea()->m_pScene);
            }
        }
    }
}
// PC 0x00703990
void CSWCVisualEffectOnObject::ApplyTextureReplace(unsigned short nVisualEffect)
{
    CExoString sTexture;
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject == 0)
    {
        return;
    }
    nVisualEffect -= 1000;
    switch (nVisualEffect)
    {
    case 10:
        sTexture = "vdu_tex_stone";
        pObject->m_bStoneTexture = 1;
        break;
    case 20:
        sTexture = "vdu_tex_grstone";
        pObject->m_bStoneTexture = 1;
        break;
    case 30:
        sTexture = "vdu_tex_bark";
        pObject->m_bBarkTexture = 1;
        break;
    case 40:
        sTexture = "vdu_tex_shade";
        pObject->m_bShadeTexture = 1;
        break;
    default:
        return;
    }
    pObject->ReplaceTexture("", sTexture, 0);
    CSWCCreature* pCreature = pObject->AsSWCCreature();
    if (pCreature != 0)
    {
        CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
        CSWCItem* pItem =
            pExoApp->GetItemByGameObjectID(pCreature->GetEquippedItemID(1));
        if (pItem != 0)
        {
            CResRef cItemNew(sTexture);
            CResRef cItemOld("");
            pItem->ReplaceTexture(cItemOld, cItemNew, 0);
        }
    }
}
// PC 0x00703B40
void CSWCVisualEffectOnObject::ApplyEnvironmentMap(unsigned short nVisualEffect)
{
    CExoString sEnvMap;
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        nVisualEffect -= 100;
        sEnvMap.Format("vdu_envmap%03u", nVisualEffect);
        pObject->ReplaceTexture("", "", 0);
        pObject->SetEnvMap(1, 0, sEnvMap);
    }
}
// PC 0x00703C20
void CSWCVisualEffectOnObject::ApplySelfIllumination(unsigned short nVisualEffect)
{
    Vector vColor;
    CClientExoApp* pClientExoApp = g_pAppManager->GetClientExoApp();
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        m_nEffectType = EFFECT_TYPE_SELFILLUMINATION;
        nVisualEffect -= SELFILLUMINATION_RED;
        switch (nVisualEffect)
        {
        case SELFILLUMINATION_RED - SELFILLUMINATION_RED:
            vColor = Vector(1.0f, 0.0f, 0.0f);
            break;
        case SELFILLUMINATION_GREEN - SELFILLUMINATION_RED:
            vColor = Vector(0.0f, 1.0f, 0.0f);
            break;
        case SELFILLUMINATION_BLUE - SELFILLUMINATION_RED:
            vColor = Vector(0.0f, 0.0f, 1.0f);
            break;
        case SELFILLUMINATION_PURPLE - SELFILLUMINATION_RED:
            vColor = Vector(1.0f, 0.0f, 1.0f);
            break;
        case SELFILLUMINATION_YELLOW - SELFILLUMINATION_RED:
            vColor = Vector(1.0f, 1.0f, 0.0f);
            break;
        case SELFILLUMINATION_WHITE - SELFILLUMINATION_RED:
            vColor = Vector(1.0f, 1.0f, 1.0f);
            break;
        default:
            return;
        }
        pObject->Hilite(vColor, CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT,
                        0.0f, 1);
    }
}
// PC 0x00703DC0
void CSWCVisualEffectOnObject::ApplyLightSource(unsigned short nVisualEffect)
{
    CExoString sAnimation;
    CSWCObject* pTarget = GetTargetCSWCObject();
    if (pTarget == 0 || m_pEffectModel == 0)
    {
        return;
    }

    nVisualEffect -= VFX_LIGHT_SOURCE_BEGIN;
    int bDynamic = 1;
    m_nEffectType = LIGHT_SOURCE_TYPE_NORMAL;
    switch (nVisualEffect)
    {
        case LIGHT_SOURCE_WHITE_5M:
        case LIGHT_SOURCE_WHITE_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "White_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_WHITE_10M:
        case LIGHT_SOURCE_WHITE_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "White_10m";
            break;
        case LIGHT_SOURCE_WHITE_15M:
        case LIGHT_SOURCE_WHITE_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "White_15m";
            break;
        case LIGHT_SOURCE_WHITE_20M:
        case LIGHT_SOURCE_WHITE_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "White_20m";
            break;
        case LIGHT_SOURCE_BLUE_5M:
        case LIGHT_SOURCE_BLUE_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Blue_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_BLUE_10M:
        case LIGHT_SOURCE_BLUE_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Blue_10m";
            break;
        case LIGHT_SOURCE_BLUE_15M:
        case LIGHT_SOURCE_BLUE_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Blue_15m";
            break;
        case LIGHT_SOURCE_BLUE_20M:
        case LIGHT_SOURCE_BLUE_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Blue_20m";
            break;
        case LIGHT_SOURCE_YELLOW_5M:
        case LIGHT_SOURCE_YELLOW_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Yello_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_YELLOW_10M:
        case LIGHT_SOURCE_YELLOW_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Yello_10m";
            break;
        case LIGHT_SOURCE_YELLOW_15M:
        case LIGHT_SOURCE_YELLOW_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Yello_15m";
            break;
        case LIGHT_SOURCE_YELLOW_20M:
        case LIGHT_SOURCE_YELLOW_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Yello_20m";
            break;
        case LIGHT_SOURCE_RED_5M:
        case LIGHT_SOURCE_RED_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Red_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_RED_10M:
        case LIGHT_SOURCE_RED_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Red_10m";
            break;
        case LIGHT_SOURCE_RED_15M:
        case LIGHT_SOURCE_RED_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Red_15m";
            break;
        case LIGHT_SOURCE_RED_20M:
        case LIGHT_SOURCE_RED_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Red_20m";
            break;
        case LIGHT_SOURCE_ORANGE_5M:
        case LIGHT_SOURCE_ORANGE_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Orng_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_ORANGE_10M:
        case LIGHT_SOURCE_ORANGE_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Orng_10m";
            break;
        case LIGHT_SOURCE_ORANGE_15M:
        case LIGHT_SOURCE_ORANGE_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Orng_15m";
            break;
        case LIGHT_SOURCE_ORANGE_20M:
        case LIGHT_SOURCE_ORANGE_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Orng_20m";
            break;
        case LIGHT_SOURCE_PURPLE_5M:
        case LIGHT_SOURCE_PURPLE_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Purp_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_PURPLE_10M:
        case LIGHT_SOURCE_PURPLE_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Purp_10m";
            break;
        case LIGHT_SOURCE_PURPLE_15M:
        case LIGHT_SOURCE_PURPLE_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Purp_15m";
            break;
        case LIGHT_SOURCE_PURPLE_20M:
        case LIGHT_SOURCE_PURPLE_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Purp_20m";
            break;
        case LIGHT_SOURCE_GREEN_5M:
        case LIGHT_SOURCE_GREEN_5M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Gren_5m";
            bDynamic = 0;
            break;
        case LIGHT_SOURCE_GREEN_10M:
        case LIGHT_SOURCE_GREEN_10M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Gren_10m";
            break;
        case LIGHT_SOURCE_GREEN_15M:
        case LIGHT_SOURCE_GREEN_15M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Gren_15m";
            break;
        case LIGHT_SOURCE_GREEN_20M:
        case LIGHT_SOURCE_GREEN_20M + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Gren_20m";
            break;
        case LIGHT_SOURCE_DARKVISION:
        case LIGHT_SOURCE_DARKVISION + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Dark_Vision";
            bDynamic = 0;
            m_nEffectType = LIGHT_SOURCE_TYPE_DARKVISION;
            m_bLightSourceShadows = 1;
            break;
        case LIGHT_SOURCE_BLINDVISION:
        case LIGHT_SOURCE_BLINDVISION + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Blind_Vision";
            bDynamic = 0;
            m_nEffectType = LIGHT_SOURCE_TYPE_BLINDVISION;
            m_bLightSourceShadows = 1;
            break;
        case LIGHT_SOURCE_LOWLIGHTVISION:
        case LIGHT_SOURCE_LOWLIGHTVISION + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Low_Light_Vision";
            bDynamic = 0;
            m_nEffectType = LIGHT_SOURCE_TYPE_LOWLIGHTVISION;
            m_bLightSourceShadows = 1;
            break;
        case LIGHT_SOURCE_ULTRAVISION:
        case LIGHT_SOURCE_ULTRAVISION + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "Ultra_Vision";
            bDynamic = 0;
            m_nEffectType = LIGHT_SOURCE_TYPE_ULTRAVISION;
            m_bLightSourceShadows = 0;
            break;
        case LIGHT_SOURCE_DARKNESS:
        case LIGHT_SOURCE_DARKNESS + LIGHT_SOURCE_REVERSE_BEGIN:
            sAnimation = "darkness";
            bDynamic = 0;
            m_nEffectType = LIGHT_SOURCE_TYPE_DARKNESS;
            m_bLightSourceShadows = 1;
            break;
        case LIGHT_SOURCE_BLINDNESS:
            if (g_pAppManager->GetClientExoApp() != 0 &&
                g_pAppManager->GetClientExoApp()->GetPlayerCreature() != 0 &&
                m_oidTargetObject ==
                    g_pAppManager->GetClientExoApp()->GetPlayerCreature()->GetObjectId())
            {
                if (g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
                        m_oidTargetObject) != 0)
                {
                    sAnimation = "Blind_Vision";
                    bDynamic = 0;
                    m_nEffectType = LIGHT_SOURCE_TYPE_BLINDNESS;
                    m_bLightSourceShadows = 0;
                }
                CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
                if (pModule != 0 && pModule->m_pInfo != 0)
                {
                    pModule->m_pInfo->m_pScene->SetBlindLights(m_pEffectModel);
                }
            }
            break;
        default:
            return;
    }

    AttachLight();
    float fSpeed = 1.0f;
    if (nVisualEffect >= LIGHT_SOURCE_REVERSE_BEGIN)
    {
        fSpeed = -0.31f;
    }
    static_cast<Gob*>(m_pEffectModel)->PlayAnimation(
        sAnimation.CStr(), fSpeed,
        Gob::ANIMATION_FLAG_NO_LOOP | Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);
    if (m_pEffectModel != 0)
    {
        m_bLightSourceDynamic = bDynamic != 0;
    }

    CSWCCreature* pCreature = pTarget->AsSWCCreature();
    CSWCCreature* pPlayerCreature = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (pCreature != 0 && pCreature == pPlayerCreature)
    {
        static_cast<Gob*>(m_pEffectModel)->SetLightPriority(LIGHT_PRIORITY_PLAYER);
    }
    else
    {
        static_cast<Gob*>(m_pEffectModel)->SetLightPriority(LIGHT_PRIORITY_OTHER_OBJECT);
    }
}

    enum { LIGHT_PRIORITY_PLAYER = 2, LIGHT_PRIORITY_OTHER_OBJECT = 4 };
// PC 0x00704360
void CSWCVisualEffectOnObject::ApplyAlpha(unsigned short nVisualEffect)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        m_nEffectType = EFFECT_TYPE_STANDARD;
        nVisualEffect -= VFX_ALPHA_BEGIN;
        switch (nVisualEffect)
        {
        case ALPHA_HALF:
            pObject->SetAlpha(m_fAlphaTarget, CSWCObject::MODEL_SLOT_DEFAULT);
            m_fAlpha = m_fAlphaTarget;
            break;
        case ALPHA_HALF_BLUE_HILITE:
            pObject->SetAlpha(m_fAlphaTarget, CSWCObject::MODEL_SLOT_DEFAULT);
            m_fAlpha = m_fAlphaTarget;
            pObject->Hilite(Vector(0.25f, 0.25f, 1.0f),
                            CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT, 0.0f, 1);
            break;
        case ALPHA_HALF_PURPLE_HILITE:
            pObject->SetAlpha(m_fAlphaTarget, CSWCObject::MODEL_SLOT_DEFAULT);
            m_fAlpha = m_fAlphaTarget;
            pObject->Hilite(Vector(0.5f, 0.0f, 1.0f),
                            CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT, 0.0f, 1);
            break;
        case ALPHA_FADE:
        case ALPHA_FADE_PARTIAL:
            m_nAlphaState = ALPHA_FADING_OUT;
            break;
        }
    }
}
int CSWCVisualEffectOnObject::ApplyBeam(unsigned short nVisualEffect)  // PC 0x007044B0
{
    AttachBeam();
    return static_cast<Gob*>(m_pEffectModel)
        ->PlayAnimation("cast01", 1.0f, 0, 0.0f);
}
// PC 0x007044E0
void CSWCVisualEffectOnObject::ApplyDeleteModel(unsigned short nVisualEffect)
{
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nGoreLevel >= 2)
    {
        CSWCObject* pObject = GetTargetCSWCObject();
        if (pObject != 0)
        {
            CSWCCreature* pCreature = pObject->AsSWCCreature();
            if (pCreature != 0)
            {
                pCreature->KillPopUpBubble();
            }
            pObject->GetAnimationBase()->UnloadModel();
        }
    }
}
// PC 0x00705950
void CSWCVisualEffectOnObject::ApplyChunk(unsigned short nVisualEffect)
{
#ifdef _XBOX
    Vector vPosition;
#endif
    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nGoreLevel >=
        CClientOptions::GORE_LEVEL_FULL)
    {
        CSWCObject* pObject = GetTargetCSWCObject();
        if (pObject->m_pArea != 0)
        {
            CSWCCreature* pCreature = pObject->AsSWCCreature();
            if (pCreature != 0)
            {
#ifdef _XBOX
                vPosition = pObject->m_vPosition;
#endif
                if (pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1) != 0)
                {
                    Quaternion qOrientation;
                    Vector vImpact;
                    pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)
                        ->GetPartLocalPosition("impact", &vImpact, &qOrientation);
#ifdef _XBOX
                    vPosition += vImpact;
#endif
                }
                CExoString sBloodColor;
                {
                    CExoString sColumn("bloodcolr");
                    unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
                    g_pRules->Get2DArrays()->m_pAppearance->GetCExoStringEntry(
                        nAppearance,
                        sColumn, &sBloodColor);
                }
                int nSizeCategory;
                unsigned short nAppearance = pCreature->GetAppearance()->m_nAppearance;
                C2DA* pAppearanceTable = g_pRules->Get2DArrays()->m_pAppearance;
                pAppearanceTable->GetINTEntry(
                    nAppearance,
                    CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY,
                    &nSizeCategory);
                if (nSizeCategory == CREATURE_SIZE_INVALID)
                {
                    return;
                }
                if (FALSE)
                {
                    CExoString sChunkModel;
                    sChunkModel = sBloodColor;
                }
            }
        }
    }
}
// PC 0x00704540
void CSWCVisualEffectOnObject::ApplyMirv(unsigned short nVisualEffect)
{
    Quaternion qOrientation;
    Vector vPosition;
    CExoString sModel;
    CSWCObject* pTarget = GetTargetCSWCObject();
    CSWCObject* pSource = GetSourceCSWCObject();
    if (pTarget == 0 || pSource == 0)
        return;

    CSWCProjectile* pProjectile = new CSWCProjectile;
    pProjectile->SetProjectileSpellId(340);
    nVisualEffect -= VFX_MIRV_BEGIN;
    switch (nVisualEffect)
    {
    case MIRV_MAGIC_MISSILE:
        sModel = "vpr_magmisl";
        break;
    case MIRV_FIRE_ARROW:
        sModel = "vpr_arofire";
        pProjectile->m_nOrientationType =
            CSWCProjectile::PROJECTILE_ORIENTATION_MOVEMENT;
        break;
    case MIRV_ACID_ARROW:
        sModel = "vpr_aroacid";
        pProjectile->m_nOrientationType =
            CSWCProjectile::PROJECTILE_ORIENTATION_MOVEMENT;
        break;
    }
    if (!pProjectile->LoadModel(sModel, 255, 0))
    {
        delete pProjectile;
        return;
    }
    if (!pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition(
            "handconjure", &vPosition, &qOrientation))
    {
        vPosition = pSource->m_vPosition;
    }
    pProjectile->SetPosition(vPosition);
    pProjectile->SetOrientation(pSource->m_vOrientation);
    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (pModule != 0 && pModule->m_pInfo != 0)
    {
        CAurScene* pScene = pModule->m_pInfo->m_pScene;
        pProjectile->AttachToScene(pScene);
    }
    pProjectile->SetAnimation(1);
    if (equal(m_vOffset, Vector()))
    {
        pProjectile->m_vDesiredPosition = pTarget->m_vPosition;
        pProjectile->SetDesiredTargetObject(pTarget->GetId(), "");
    }
    else
    {
        pProjectile->m_vDesiredPosition = m_vOffset;
    }
    switch (nVisualEffect)
    {
    case MIRV_MAGIC_MISSILE:
    {
        float fDistance = magnitude(pTarget->m_vPosition - pSource->m_vPosition);
        unsigned long nTravelTime = (unsigned long)(
            fDistance / ((float)log(fDistance) * 3.0f + 2.0f) * 1000.0f);
        pProjectile->SetDesiredPositionTimer(nTravelTime);
        pProjectile->SetProjectileType(CSWCProjectile::PROJECTILE_PATH_TYPE_BURST_UP);
        break;
    }
    case MIRV_FIRE_ARROW:
    {
        float fDistance = magnitude(pTarget->m_vPosition - pSource->m_vPosition);
        unsigned long nTravelTime = (unsigned long)(
            fDistance / ((float)log(fDistance) * 3.0f + 2.0f) * 1000.0f);
        pProjectile->SetDesiredPositionTimer(nTravelTime);
        pProjectile->SetProjectileType(CSWCProjectile::PROJECTILE_PATH_TYPE_BURST_UP);
        break;
    }
    case MIRV_ACID_ARROW:
        pProjectile->SetDesiredPositionTimer(
            2 * pProjectile->CalculateProjectileTimeToTarget(pTarget->m_vPosition, 0));
        pProjectile->SetProjectileType(CSWCProjectile::PROJECTILE_PATH_TYPE_HOMING);
        break;
    }
}
// PC 0x00704880
void CSWCVisualEffectOnObject::ApplyFOV(unsigned short nVisualEffect)
{
    CSWCCreature* pPlayerCreature =
        g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pPlayerCreature == 0 || pObject == 0 ||
        pPlayerCreature->m_nId != pObject->m_nId)
    {
        return;
    }
    CAurCamera* pCamera = g_pAppManager->GetClientExoApp()->GetModuleCamera();
    if (pCamera != 0)
    {
        nVisualEffect -= FOV_ZOOM;
        switch (nVisualEffect)
        {
        case 0:
            m_fSavedFOV = pCamera->GetViewAngle();
            CAurBehavior* pBehavior = reinterpret_cast<CAurBehavior*>(
                g_pAppManager->GetClientExoApp()->GetModuleCamera()
                    ->GetBehavior(-1));
            if (pBehavior != 0)
            {
                if (pBehavior->GetType() == AUR_BEHAVIOR_CAMERA_ONASTICK)
                {
                    CSWCameraOnAStick* pOnAStick =
                        static_cast<CSWCameraOnAStick*>(pBehavior);
                    m_fSavedCameraDistance = pOnAStick->m_fDistance;
                    pOnAStick->m_bKeepFramingOnViewAngle = 1;
                }
                if (pBehavior->GetType() == AUR_BEHAVIOR_CAMERA_FREELOOK)
                {

                }
            }
            pCamera->BeginViewAngleAnimation(135.0f, 0.75f, 0);
            m_nAlphaState = ALPHA_FADING_OUT;
            m_nHardCodedEffectTime = FOV_DURATION;
            break;
        }
    }
}
// PC 0x00704980
void CSWCVisualEffectOnObject::ApplyModelAttachment(
    unsigned short nVisualEffect)
{
    nVisualEffect -= VFX_MODEL_ATTACHMENT_BEGIN;
    switch (nVisualEffect)
    {
    case MODEL_ATTACHMENT_MEDAL:
        m_sBeamSourceNode = "medalhook";
        break;
    case MODEL_ATTACHMENT_REVAN_MASK_1:
        m_sBeamSourceNode = "revmask1hook";
        break;
    case MODEL_ATTACHMENT_REVAN_MASK_2:
        m_sBeamSourceNode = "revmask2hook";
        break;
    case MODEL_ATTACHMENT_MTHMST:
        m_sBeamSourceNode = "maskhook";
        break;
    }
    if (m_pEffectModel != 0)
    {
        switch (nVisualEffect)
        {
        case MODEL_ATTACHMENT_MTHMST:
        {
            Gob* pModel = GetTargetCSWCObject()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
            if (pModel != 0)
            {
                m_nF8 = 1;
            }
            else
            {
                pModel = GetTargetCSWCObject()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
            }
            static_cast<Gob*>(m_pEffectModel)->AttachToObject(
                pModel, (char*)m_sBeamSourceNode.CStr(), 0);
            m_pEffectModel->PlayAnimation("impact", 1.0f, 4, 0);
            break;
        }
        default:
        {
            CSWCObject* pObject = GetTargetCSWCObject();
            static_cast<Gob*>(m_pEffectModel)->AttachToObject(
                pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1),
                (char*)m_sBeamSourceNode.CStr(), 0);
            break;
        }
        }
    }
}
// PC 0x00704AD0
void CSWCVisualEffectOnObject::RemoveTextureReplace(
    unsigned short nVisualEffect)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        if (pObject->m_bStoneTexture == 1)
        {
            pObject->m_bStoneTexture = 0;
        }
        if (pObject->m_bBarkTexture == 1)
        {
            pObject->m_bBarkTexture = 0;
        }
        if (pObject->m_bShadeTexture == 1)
        {
            pObject->m_bShadeTexture = 0;
        }
        pObject->RestoreTexture();
        CSWCCreature* pCreature = pObject->AsSWCCreature();
        if (pCreature != 0)
        {
            CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
            CSWCItem* pItem = pExoApp->GetItemByGameObjectID(
                pCreature->GetEquippedItemID(INVENTORY_SLOT_HEAD));
            if (pItem != 0)
            {
                pItem->RestoreTexture();
            }
        }
    }
}
// PC 0x00704B60
void CSWCVisualEffectOnObject::RemoveEnvironmentMap(
    unsigned short nVisualEffect)
{
    CSWCObject* pGameObject;

    pGameObject = GetTargetCSWCObject();
    if (pGameObject != 0)
    {
        pGameObject->RestoreEnvMap();
        pGameObject->RestoreTexture();
    }
}
// PC 0x00704D10
void CSWCVisualEffectOnObject::RemoveSelfIllumination(
    unsigned short nVisualEffect)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        pObject->UnHilite(CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT);
    }
}
// PC 0x00704D10
void CSWCVisualEffectOnObject::RemoveSelfIlluminationPulse(
    unsigned short nVisualEffect)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        pObject->UnHilite(CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT);
    }
}
// PC 0x00704BA0
void CSWCVisualEffectOnObject::RemoveLightSource(unsigned short nVisualEffect)
{
    CSWCObject* pGameObject;
    CSWCCreature* pCreature;

    if (m_pEffectModel != 0)
    {
        pGameObject = GetTargetCSWCObject();
        nVisualEffect -= 300;
        if (m_bLightSourceApplied != 0 && pGameObject != 0)
        {
            pCreature = pGameObject->AsSWCCreature();
            if (pCreature != 0 &&
                pCreature ==
                    g_pAppManager->m_pClientExoApp->GetPlayerCreature())
            {
                if (pCreature->m_pArea != 0)
                {
                    pCreature->m_pArea->UpdateShadowingLights();
                }
            }
        }
        if (nVisualEffect == 33 &&
            g_pAppManager->m_pClientExoApp->GetPlayerCreature() != 0 &&
            m_oidTargetObject ==
                g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_nId)
        {
            CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
            if (pModule != 0)
            {
                if (pModule->m_pInfo != 0)
                {
                    pModule->m_pInfo->m_pScene->SetBlindLights(0);
                }
            }
        }
        static_cast<Gob*>(m_pEffectModel)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        static_cast<Gob*>(m_pEffectModel)->AttachToScene(0);
    }
}
// PC 0x00704C90
void CSWCVisualEffectOnObject::RemoveAlpha(unsigned short nVisualEffect)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        nVisualEffect -= 400;
        switch (nVisualEffect)
        {
        case 2:
        case 3:
            pObject->UnHilite(CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT);
        case 0:
        case 1:
        case 4:
            pObject->SetAlpha(1.0f, CSWCObject::MODEL_SLOT_DEFAULT);
            break;
        }
    }
}
// PC 0x00704D50
void CSWCVisualEffectOnObject::RemoveFOV(unsigned short nVisualEffect,
                                         int bForce)
{
    CSWCCreature* pPlayerCreature =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pPlayerCreature == 0 || pObject == 0)
    {
        return;
    }
    if (bForce == 0 && pPlayerCreature->m_nId != pObject->m_nId)
    {
        return;
    }
    CAurCamera* pCamera = g_pAppManager->m_pClientExoApp->GetModuleCamera();
    if (pCamera != 0)
    {
        nVisualEffect -= FOV_ZOOM;
        switch (nVisualEffect)
        {
        case 0:
            pCamera->EndViewAngleAnimation(false);
            pCamera->SetViewAngle(m_fSavedFOV);
            CAurBehavior* pBehavior = reinterpret_cast<CAurBehavior*>(
                g_pAppManager->m_pClientExoApp->GetModuleCamera()
                    ->GetBehavior(-1));
            if (pBehavior != 0)
            {
                if (pBehavior->GetType() == AUR_BEHAVIOR_CAMERA_ONASTICK)
                {
                    CSWCameraOnAStick* pOnAStick =
                        static_cast<CSWCameraOnAStick*>(pBehavior);
                    pOnAStick->m_fDistance = m_fSavedCameraDistance;
                    pOnAStick->m_bKeepFramingOnViewAngle = 0;
                }
                if (pBehavior->GetType() == AUR_BEHAVIOR_CAMERA_FREELOOK)
                {

                }
            }
            break;
        }
    }
}
// PC 0x00704E30
void CSWCVisualEffectOnObject::UpdateSelfIlluminationPulse(unsigned long nTimeElapsed)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        if (m_nHardCodedEffectTime > SELFILLUMINATION_PULSE_PERIOD)
        {
            m_nHardCodedEffectTime -= SELFILLUMINATION_PULSE_PERIOD;
            m_nAlphaState = !m_nAlphaState;
        }
        if (m_nAlphaState == ALPHA_FADING_OUT)
        {
            m_vSelfIlluminationPulseCurrent =
                (1.0f - m_nHardCodedEffectTime * 0.001f) *
                    m_vSelfIlluminationPulseStart +
                (m_nHardCodedEffectTime * 0.001f) *
                    m_vSelfIlluminationPulseEnd;
        }
        else
        {
            m_vSelfIlluminationPulseCurrent =
                (m_nHardCodedEffectTime * 0.001f) *
                    m_vSelfIlluminationPulseStart +
                (1.0f - m_nHardCodedEffectTime * 0.001f) *
                    m_vSelfIlluminationPulseEnd;
        }
        pObject->Hilite(m_vSelfIlluminationPulseCurrent,
                        CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT, 0.0f, 1);
        m_nHardCodedEffectTime += nTimeElapsed;
    }
}
// PC 0x00704FB0
void CSWCVisualEffectOnObject::UpdateAlpha(unsigned long nTimeElapsed)
{
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject != 0)
    {
        unsigned short nVisualEffect = m_anVisualEffect[1] - VFX_ALPHA_BEGIN;
        if (nVisualEffect == ALPHA_FADE || nVisualEffect == ALPHA_FADE_PARTIAL)
        {
            if (nVisualEffect == ALPHA_FADE)
            {
                if (m_nHardCodedEffectTime >= ALPHA_FADE_PERIOD)
                {
                    m_nAlphaState += m_nHardCodedEffectTime / ALPHA_FADE_PERIOD;
                    m_nAlphaState %= ALPHA_FADE_STATE_COUNT;
                    m_nHardCodedEffectTime %= ALPHA_FADE_PERIOD;
                }
            }
            else
            {
                if (m_nHardCodedEffectTime >= ALPHA_PARTIAL_FADE_PERIOD)
                {
                    m_nAlphaState += m_nHardCodedEffectTime / ALPHA_PARTIAL_FADE_PERIOD;
                    m_nAlphaState %= ALPHA_FADE_STATE_COUNT;
                    m_nHardCodedEffectTime %= ALPHA_PARTIAL_FADE_PERIOD;
                    if (m_nAlphaState == ALPHA_FADED_OUT)
                        m_nAlphaState = ALPHA_FADING_IN;
                    else if (m_nAlphaState == ALPHA_FADED_IN)
                        m_nAlphaState = ALPHA_FADING_OUT;
                }
            }
            switch (m_nAlphaState)
            {
            case ALPHA_FADING_OUT:
                if (nVisualEffect == ALPHA_FADE)
                {
                    m_fAlpha = 1.0f - m_nHardCodedEffectTime / float(ALPHA_FADE_PERIOD);
                    if (m_fAlpha < 0.0f)
                        m_fAlpha = 0.0f;
                }
                else
                {
                    m_fAlpha = 1.0f - (0.5f * (m_nHardCodedEffectTime / float(ALPHA_PARTIAL_FADE_PERIOD)) + 0.2f);
                    if (m_fAlpha < 0.3f)
                        m_fAlpha = 0.3f;
                }
                break;
            case ALPHA_FADED_OUT:
                if (nVisualEffect == ALPHA_FADE)
                    m_fAlpha = 0.0f;
                else
                    m_fAlpha = 0.3f;
                break;
            case ALPHA_FADING_IN:
                if (nVisualEffect == ALPHA_FADE)
                {
                    m_fAlpha = m_nHardCodedEffectTime / float(ALPHA_FADE_PERIOD);
                    if (m_fAlpha > 1.0f)
                        m_fAlpha = 1.0f;
                }
                else
                {
                    m_fAlpha = 0.5f * (m_nHardCodedEffectTime / float(ALPHA_PARTIAL_FADE_PERIOD)) + 0.3f;
                    if (m_fAlpha > 0.8f)
                        m_fAlpha = 0.8f;
                }
                break;
            case ALPHA_FADED_IN:
                if (nVisualEffect == ALPHA_FADE)
                    m_fAlpha = 1.0f;
                else
                    m_fAlpha = 0.8f;
                break;
            }
            pObject->SetAlpha(m_fAlpha, CSWCObject::MODEL_SLOT_DEFAULT);
            m_nHardCodedEffectTime += nTimeElapsed;
        }
    }
}
// PC 0x00705220
void CSWCVisualEffectOnObject::UpdateFOV(
    unsigned short nVisualEffect, unsigned long nTimeElapsed)
{
    CSWCCreature* pPlayerCreature =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pPlayerCreature == 0 || pObject == 0 ||
        pObject->m_nId != pPlayerCreature->m_nId ||
        g_pAppManager->m_pClientExoApp->GetPausedByCombat())
    {
        RemoveFOV(nVisualEffect, 1);
    }
    else if (m_nAlphaState == ALPHA_FADING_OUT)
    {
        CAurCamera* pCamera =
            g_pAppManager->m_pClientExoApp->GetModuleCamera();
        if (pCamera != 0 && m_nAlphaState == ALPHA_FADING_OUT &&
            m_nHardCodedEffectTime <= float(FOV_RETURN_TIME))
        {
            nVisualEffect -= FOV_ZOOM;
            switch (nVisualEffect)
            {
            case 0:
                pCamera->EndViewAngleAnimation(false);
                pCamera->BeginViewAngleAnimation(
                    m_fSavedFOV, m_nHardCodedEffectTime * 0.001f, 0);
                break;
            }
            m_nAlphaState = ALPHA_FADED_OUT;
        }
    }
}
// PC 0x00705350
int CSWCVisualEffectOnObject::LoadLightSource(unsigned short nVisualEffect)
{
    if (GetTargetCSWCObject() == 0)
    {
        return 0;
    }
    m_pEffectModel = NewCAurObject("fx_light_clr", "fx_light", 0, 0);
    if (m_pEffectModel == 0)
    {
        return 0;
    }
    static_cast<Gob*>(m_pEffectModel)->SetLightPriority(Gob::LIGHT_PRIORITY_VISUAL_EFFECT);
    return 1;
}
// PC 0x007053B0
void CSWCVisualEffectOnObject::ReAttachVisualEffect()
{
    CExoString sAttachNode;
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject == 0)
    {
        return;
    }
    CAurObject* pTargetModel;
    if (m_nF8)
    {
        pTargetModel = pObject->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
    }
    else
    {
        pTargetModel = pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
    }
    if (pTargetModel == 0)
    {
        return;
    }
    m_bLightSourceApplied = 1;
    CSWCPlaceable* pPlaceable = pObject->AsSWCPlaceable();
    CSWCDoor* pDoor = pObject->AsSWCDoor();

    if (m_apAttachedModels[0] != 0)
    {
        if (pPlaceable != 0)
        {
            sAttachNode = pPlaceable->m_sModel + "_ground";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "grnd";
        }
        else
        {
            sAttachNode = "root";
        }
        AttachVFXModelToTargetModel(m_apAttachedModels[0], pTargetModel,
                                    sAttachNode);
    }
    if (m_apAttachedModels[1] != 0)
    {
        if (pPlaceable != 0)
        {
            sAttachNode = pPlaceable->m_sModel + "_impact";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "impc";
        }
        else
        {
            sAttachNode = "impact";
        }
        AttachVFXModelToTargetModel(m_apAttachedModels[1], pTargetModel,
                                    sAttachNode);
    }
    if (m_apAttachedModels[2] != 0)
    {
        if (pPlaceable != 0)
        {
            sAttachNode = pPlaceable->m_sModel + "_head_hit";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "hhit";
        }
        else
        {
            sAttachNode = "talkdummy";
        }
        CAurObject* pHeadModel;
        if (pObject->GetAnimationBase() != 0 &&
            pObject->GetAnimationBase()->m_nAnimationType ==
                CSWCAnimBase::ANIMATION_TYPE_BTYPE)
        {
            pHeadModel = pObject->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
        }
        else
        {
            pHeadModel = pTargetModel;
        }
        AttachVFXModelToTargetModel(m_apAttachedModels[2], pHeadModel,
                                    sAttachNode);
    }
    unsigned short nVisualEffect;
    if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
    {
        nVisualEffect = m_anVisualEffect[0];
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_DURATION)
    {
        nVisualEffect = m_anVisualEffect[1];
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
    {
        nVisualEffect = m_anVisualEffect[2];
    }
    else
    {
        nVisualEffect = VFX_NONE;
    }
    if (m_pEffectModel != 0)
    {
        if (nVisualEffect >= VFX_BEAM_BEGIN && nVisualEffect < VFX_BEAM_END)
        {
            AttachBeam();
        }
        else if (nVisualEffect >= VFX_LIGHT_SOURCE_BEGIN &&
                 nVisualEffect < VFX_LIGHT_SOURCE_END)
        {
            AttachLight();
        }
        else if (nVisualEffect >= VFX_SPELL_VISUAL_BEGIN &&
                 nVisualEffect < VFX_SPELL_VISUAL_END)
        {
            ApplySpellVisual(nVisualEffect);
        }
        else
        {
            static_cast<Gob*>(m_pEffectModel)->AttachToObject(
                pTargetModel, (char*)m_sBeamSourceNode.CStr(), Gob::ATTACH_NORMAL);
        }
    }
    if (nVisualEffect >= VFX_ENVIRONMENT_MAP_BEGIN &&
        nVisualEffect < VFX_ENVIRONMENT_MAP_END)
    {
        ApplyEnvironmentMap(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_SELFILLUMINATION_BEGIN &&
             nVisualEffect < VFX_SELFILLUMINATION_END)
    {
        ApplySelfIllumination(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_ALPHA_BEGIN && nVisualEffect < VFX_ALPHA_END)
    {
        if (nVisualEffect - VFX_ALPHA_BEGIN != ALPHA_FADE &&
            nVisualEffect - VFX_ALPHA_BEGIN != ALPHA_FADE_PARTIAL)
        {
            ApplyAlpha(nVisualEffect);
        }
        else
        {
            pObject->SetAlpha(m_fAlpha, CSWCObject::MODEL_SLOT_DEFAULT);
        }
    }
    else if (nVisualEffect >= VFX_SELFILLUMINATION_PULSE_BEGIN &&
             nVisualEffect < VFX_SELFILLUMINATION_PULSE_END)
    {
        pObject->Hilite(m_vSelfIlluminationPulseCurrent,
                        CSWCObject::HILITE_CHANNEL_VISUAL_EFFECT, 0.0f, 1);
    }
    else if (nVisualEffect >= VFX_TEXTURE_REPLACE_BEGIN &&
             nVisualEffect < VFX_TEXTURE_REPLACE_END)
    {
        ApplyTextureReplace(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_BUMPED_OUT_SHELL_BEGIN &&
             nVisualEffect < VFX_BUMPED_OUT_SHELL_END)
    {
        ApplyBumpedOutShell(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_DISTORTION_BEGIN &&
             nVisualEffect < VFX_DISTORTION_END)
    {
        ApplyDistortion(nVisualEffect);
    }
}
// PC 0x00705F40
void CSWCVisualEffectOnObject::ApplyHardCodedVisualEffect(
    unsigned short nVisualEffect)
{
    if (m_bVisualEffectShown && !m_bHardCodedEffectApplied)
    {
        if (nVisualEffect < VFX_ENVIRONMENT_MAP_END)
            ApplyEnvironmentMap(nVisualEffect);
        else if (nVisualEffect < VFX_SELFILLUMINATION_END)
            ApplySelfIllumination(nVisualEffect);
        else if (nVisualEffect < VFX_LIGHT_SOURCE_END)
            ApplyLightSource(nVisualEffect);
        else if (nVisualEffect < VFX_ALPHA_END)
            ApplyAlpha(nVisualEffect);
        else if (nVisualEffect < VFX_SELFILLUMINATION_PULSE_END)
            ApplySelfIlluminationPulse(nVisualEffect);
        else if (nVisualEffect < VFX_BEAM_END)
            ApplyBeam(nVisualEffect);
        else if (nVisualEffect < VFX_DELETE_MODEL_END)
            ApplyDeleteModel(nVisualEffect);
        else if (nVisualEffect < VFX_CHUNK_END)
            ApplyChunk(nVisualEffect);
        else if (nVisualEffect < VFX_MIRV_END)
            ApplyMirv(nVisualEffect);
        else if (nVisualEffect < VFX_TEXTURE_REPLACE_END)
            ApplyTextureReplace(nVisualEffect);
        else if (nVisualEffect < VFX_CHEAT_END)
            ApplyCheat(nVisualEffect);
        else if (nVisualEffect < VFX_SPELL_VISUAL_END)
            ApplySpellVisual(nVisualEffect);
        else if (nVisualEffect < VFX_LIGHTSABER_THROW_END)
            ApplyLightsaberThrow(nVisualEffect);
        else if (nVisualEffect < VFX_BUMPED_OUT_SHELL_END)
            ApplyBumpedOutShell(nVisualEffect);
        else if (nVisualEffect < VFX_FOV_END)
            ApplyFOV(nVisualEffect);
        else if (nVisualEffect < VFX_MOTION_BLUR_END)
            ApplyMotionBlur(nVisualEffect);
        else if (nVisualEffect < VFX_MODEL_ATTACHMENT_END)
            ApplyModelAttachment(nVisualEffect);
        else if (nVisualEffect < VFX_DISTORTION_END)
            ApplyDistortion(nVisualEffect);

        m_bHardCodedEffectApplied = 1;
    }
}
#ifndef _XBOX
// PC 0x00705AF0
void CSWCVisualEffectOnObject::RemoveModelAttachment(unsigned short nVisualEffect)
{
    if (m_pEffectModel == 0)
    {
        return;
    }
    nVisualEffect -= VFX_MODEL_ATTACHMENT_BEGIN;
    if (nVisualEffect != MODEL_ATTACHMENT_MTHMST)
    {
        m_pEffectModel->AttachToObject(0, 0, 0);
        m_pEffectModel->AttachToScene(0);
        return;
    }
    m_pEffectModel->PlayAnimation("stop", 0.0f, 1, 0.0f);
    m_pEffectModel->AttachToObject(0, 0, 0);
    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (pModule != 0 && pModule->m_pInfo != 0)
    {
        pModule->m_pInfo->AddToBeDeletedEmitter(m_pEffectModel);
        m_pEffectModel = 0;
        return;
    }
    m_pEffectModel->AttachToScene(0);
}
#endif
// PC 0x00705BA0
void CSWCVisualEffectOnObject::RemoveHardCodedVisualEffect(
    unsigned short nVisualEffect)
{
    if (m_bVisualEffectShown && m_bHardCodedEffectApplied)
    {
        if (nVisualEffect >= VFX_ENVIRONMENT_MAP_BEGIN &&
                 nVisualEffect < VFX_ENVIRONMENT_MAP_END)
        {
            RemoveEnvironmentMap(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_SELFILLUMINATION_BEGIN &&
                 nVisualEffect < VFX_SELFILLUMINATION_END)
        {
            RemoveSelfIllumination(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_LIGHT_SOURCE_BEGIN &&
                 nVisualEffect < VFX_LIGHT_SOURCE_END)
        {
            RemoveLightSource(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_ALPHA_BEGIN &&
                 nVisualEffect < VFX_ALPHA_END)
        {
            RemoveAlpha(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_SELFILLUMINATION_PULSE_BEGIN &&
                 nVisualEffect < VFX_SELFILLUMINATION_PULSE_END)
        {
            RemoveSelfIlluminationPulse(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_BEAM_BEGIN &&
                 nVisualEffect < VFX_BEAM_END)
        {
            RemoveBeam(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_TEXTURE_REPLACE_BEGIN &&
                 nVisualEffect < VFX_TEXTURE_REPLACE_END)
        {
            RemoveTextureReplace(nVisualEffect);
        }
        else if (nVisualEffect >= VFX_BUMPED_OUT_SHELL_BEGIN &&
                 nVisualEffect < VFX_BUMPED_OUT_SHELL_END)
        {
            RemoveBumpedOutShell();
        }
        else if (nVisualEffect >= VFX_FOV_BEGIN &&
                 nVisualEffect < VFX_FOV_END)
        {
            RemoveFOV(nVisualEffect, 0);
        }
        else if (nVisualEffect >= VFX_MOTION_BLUR_BEGIN &&
                 nVisualEffect < VFX_MOTION_BLUR_END)
        {
            RemoveMotionBlur();
        }
#ifndef _XBOX
        else if (nVisualEffect >= VFX_MODEL_ATTACHMENT_BEGIN &&
                 nVisualEffect < VFX_MODEL_ATTACHMENT_END)
        {
            RemoveModelAttachment(nVisualEffect);
        }
#endif
        else if (nVisualEffect >= VFX_DISTORTION_BEGIN &&
                 nVisualEffect < VFX_DISTORTION_END)
        {
            RemoveDistortion(nVisualEffect);
        }
        m_bHardCodedEffectApplied = 0;
    }
}

#include "nwrules.h"

#include "twodimarrays.h"
// PC 0x00705D60
int CSWCVisualEffectOnObject::LoadHardCodedVisualEffect(
    unsigned short nVisualEffect)
{
    if (nVisualEffect >= VFX_LIGHT_SOURCE_BEGIN &&
        nVisualEffect < VFX_LIGHT_SOURCE_END)
    {
        return LoadLightSource(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_ALPHA_BEGIN &&
             nVisualEffect < VFX_ALPHA_END)
    {
        unsigned short nAlphaEffect = nVisualEffect - VFX_ALPHA_BEGIN;
        switch (nAlphaEffect)
        {
        case ALPHA_HALF:
        case ALPHA_HALF_BLUE_HILITE:
        case ALPHA_HALF_PURPLE_HILITE:
            m_fAlphaTarget = 0.5f;
            break;
        }
    }
    else if (nVisualEffect >= VFX_BEAM_BEGIN &&
             nVisualEffect < VFX_BEAM_END)
    {
        return LoadBeam(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_SPELL_VISUAL_BEGIN &&
             nVisualEffect < VFX_SPELL_VISUAL_END)
    {
        return LoadSpellVisual(nVisualEffect);
    }
    else if (nVisualEffect >= VFX_MODEL_ATTACHMENT_BEGIN &&
             nVisualEffect < VFX_MODEL_ATTACHMENT_END)
    {
        return LoadModelAttachment(nVisualEffect);
    }
    return 1;
}
// PC 0x00706160
void CSWCVisualEffectOnObject::UpdateMotionBlur(unsigned long nTimeElapsed)
{
    CSWCCreature* pPlayerCreature =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    if (pPlayerCreature != 0)
    {
        unsigned short nVisualEffect;
        if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
        {
            nVisualEffect = m_anVisualEffect[0];
        }
        else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
        {
            nVisualEffect = m_anVisualEffect[2];
        }
        else if (m_nVisualEffectState == VISUAL_EFFECT_DURATION)
        {
            nVisualEffect = m_anVisualEffect[1];
        }

        CSWCObject* pTargetObject = GetTargetCSWCObject();
        if (pTargetObject != 0)
        {
            int bRemove = 0;
            if (pTargetObject->m_nId != pPlayerCreature->m_nId)
            {
                bRemove = 1;
            }
            CSWCCreature* pTargetCreature = pTargetObject->AsSWCCreature();
            CSWSCreature* pServerCreature;
            if (bRemove ||
                (pTargetCreature != 0 &&
                 (pServerCreature = pTargetCreature->GetServerCreature()) != 0 &&
                 pServerCreature->IsDebilitated(0) &&
                 pTargetObject->m_nId == pPlayerCreature->m_nId))
            {
                RemoveHardCodedVisualEffect(nVisualEffect);
                return;
            }
        }

        ApplyHardCodedVisualEffect(nVisualEffect);
        float fSpeedRatio = pPlayerCreature->m_fSWAnimationSpeed /
                            pPlayerCreature->m_pAppearance->m_fDriveMaxSpeed;
        if (fSpeedRatio > 1.0f)
        {
            fSpeedRatio = 1.0f;
        }
        AurSetSpeedBlurRatio(fSpeedRatio * 0.75f);
    }
}
// PC 0x00706280
int CSWCVisualEffectOnObject::ShowVisualEffect(int bForce)
{
    if (!bForce && m_bVisualEffectShown)
    {
        return 1;
    }
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    CGameObject* pGameObject;
    if (pObjectArray == 0)
    {
        return 0;
    }
    if (pObjectArray->GetGameObject(m_oidTargetObject, &pGameObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    CSWCObject* pObject = pGameObject->AsSWCObject();
    CAurObject* pTargetModel =
        pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
    CExoString sAttachNode;
    CSWCPlaceable* pPlaceable = pObject->AsSWCPlaceable();
    CSWCDoor* pDoor = pObject->AsSWCDoor();

    if (m_apAttachedModels[0] != 0)
    {
        if (pPlaceable != 0)
        {
            sAttachNode = pPlaceable->m_sModel + "_ground";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "grnd";
        }
        else
        {
            sAttachNode = "root";
        }
        AttachVFXModelToTargetModel(m_apAttachedModels[0], pTargetModel,
                                    sAttachNode);
    }
    if (m_apAttachedModels[1] != 0)
    {
        if (pPlaceable != 0)
        {
            sAttachNode = pPlaceable->m_sModel + "_impact";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "impc";
        }
        else
        {
            sAttachNode = "impact";
        }
        AttachVFXModelToTargetModel(m_apAttachedModels[1], pTargetModel,
                                    sAttachNode);
    }
    if (m_apAttachedModels[2] != 0)
    {
        if (pPlaceable != 0)
        {
            sAttachNode = pPlaceable->m_sModel + "_head_hit";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "hhit";
        }
        else
        {
            sAttachNode = "talkdummy";
        }
        CAurObject* pHeadModel;
        if (pObject->GetAnimationBase() != 0 &&
            pObject->GetAnimationBase()->m_nAnimationType ==
                CSWCAnimBase::ANIMATION_TYPE_BTYPE)
        {
            pHeadModel = pObject->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
        }
        else
        {
            pHeadModel = pTargetModel;
        }
        AttachVFXModelToTargetModel(m_apAttachedModels[2], pHeadModel,
                                    sAttachNode);
    }
    if (m_pEffectModel != 0)
    {
        CSWCPlaceable* pEffectPlaceable = pObject->AsSWCPlaceable();
        if (pEffectPlaceable != 0)
        {
            sAttachNode = pEffectPlaceable->m_sModel + "_ground";
        }
        else if (pDoor != 0)
        {
            sAttachNode = pDoor->m_sModel + "grnd";
        }
        else
        {
            sAttachNode = "root";
        }
        static_cast<Gob*>(m_pEffectModel)->AttachToObject(
            pTargetModel, (char*)sAttachNode.CStr(), Gob::ATTACH_NORMAL);
    }
    m_bVisualEffectShown = 1;
    m_bLightSourceApplied = 1;
    unsigned short nVisualEffect;
    if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
    {
        nVisualEffect = m_anVisualEffect[0];
        if (nVisualEffect != VFX_NONE)
        {
            ApplyHardCodedVisualEffect(nVisualEffect);
        }
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_DURATION)
    {
        nVisualEffect = m_anVisualEffect[1];
        if (nVisualEffect != VFX_NONE)
        {
            ApplyHardCodedVisualEffect(nVisualEffect);
        }
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
    {
        nVisualEffect = m_anVisualEffect[2];
        if (nVisualEffect != VFX_NONE)
        {
            ApplyHardCodedVisualEffect(nVisualEffect);
        }
    }
    return 1;
}
// PC 0x00705DF0
void CSWCVisualEffectOnObject::HideVisualEffect()
{
    int i;
    unsigned short nVisualEffect;

    if (m_bVisualEffectShown != 0)
    {
        for (i = 0; i < 3; i++)
        {
            if (m_apAttachedModels[i] != 0)
            {
                static_cast<Gob*>(m_apAttachedModels[i])
                    ->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
                static_cast<Gob*>(m_apAttachedModels[i])->AttachToScene(0);
            }
        }
        if (m_pEffectModel != 0)
        {
            static_cast<Gob*>(m_pEffectModel)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            static_cast<Gob*>(m_pEffectModel)->AttachToScene(0);
        }
        if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
        {
            nVisualEffect = m_anVisualEffect[0];
            if (nVisualEffect != 0xFFFF)
            {
                RemoveHardCodedVisualEffect(nVisualEffect);
            }
        }
        else if (m_nVisualEffectState == VISUAL_EFFECT_DURATION)
        {
            nVisualEffect = m_anVisualEffect[1];
            if (nVisualEffect != 0xFFFF)
            {
                RemoveHardCodedVisualEffect(nVisualEffect);
            }
        }
        else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
        {
            nVisualEffect = m_anVisualEffect[2];
            if (nVisualEffect != 0xFFFF)
            {
                RemoveHardCodedVisualEffect(nVisualEffect);
            }
        }
        m_bVisualEffectShown = 0;
        m_bLightSourceApplied = 0;
    }
}
// PC 0x00706740
void CSWCVisualEffectOnObject::StartVisualEffect()
{
    int bDone = 1;
    float fTime;
    float fLength;

    if (!m_bVisualEffectStopped || m_bVisualEffectDone)
    {
        return;
    }
    for (int i = 0; i < 3; ++i)
    {
        m_anAttachedModelAnimationTime[i] = 0;
        if (m_apAttachedModels[i] != 0)
        {
            bDone = 0;
            if (static_cast<Gob*>(m_apAttachedModels[i])->PlayAnimation(
                    "impact", 1.0f, 1, 0.0f))
            {
                m_anAttachedModelAnimationState[i] = VISUAL_EFFECT_IMPACT;
                static_cast<Gob*>(m_apAttachedModels[i])->GetAnimation(
                    "impact", &fTime, &fLength);
                m_anAttachedModelAnimationTime[i] = fLength * 1000.0f;
            }
            else if (m_bPlayDurationAnimation == 1)
            {
                m_anAttachedModelAnimationState[i] = VISUAL_EFFECT_DURATION;
                static_cast<Gob*>(m_apAttachedModels[i])->PlayAnimation(
                    "duration", 1.0f, 0, 0.0f);
            }
            else
            {
                m_anAttachedModelAnimationState[i] = VISUAL_EFFECT_IMPACT;
                m_anAttachedModelAnimationTime[i] = ATTACHED_MODEL_IMPACT_FALLBACK_TIME;
            }
        }
    }
    m_nVisualEffectState = 0;
    if (m_anVisualEffect[0] != VFX_NONE)
    {
        UnloadHardCodedVisualEffect();
        LoadHardCodedVisualEffect(m_anVisualEffect[0]);
        ApplyHardCodedVisualEffect(m_anVisualEffect[0]);
        m_nVisualEffectState = VISUAL_EFFECT_IMPACT;
        m_HardCodedEffectClass = GetHardCodedEffectClass(m_anVisualEffect[0]);
        m_nHardCodedEffectTime = GetHardCodedImpactLength(m_anVisualEffect[0]);
        bDone = 0;
    }
    else if (m_anVisualEffect[1] != VFX_NONE)
    {
        UnloadHardCodedVisualEffect();
        LoadHardCodedVisualEffect(m_anVisualEffect[1]);
        ApplyHardCodedVisualEffect(m_anVisualEffect[1]);
        m_nVisualEffectState = VISUAL_EFFECT_DURATION;
        m_HardCodedEffectClass = GetHardCodedEffectClass(m_anVisualEffect[1]);
        bDone = 0;
    }
    else if (m_anVisualEffect[2] != VFX_NONE)
    {
        UnloadHardCodedVisualEffect();
        LoadHardCodedVisualEffect(m_anVisualEffect[2]);
        ApplyHardCodedVisualEffect(m_anVisualEffect[2]);
        m_nVisualEffectState = VISUAL_EFFECT_CESSATION;
        m_HardCodedEffectClass = GetHardCodedEffectClass(m_anVisualEffect[2]);
        m_nHardCodedEffectTime = GetHardCodedCessationLength(m_anVisualEffect[2]);
        bDone = 0;
    }
    if (m_nEffectType == 0)
    {
        CExoString sRow(m_nVisualEffect);
        int nShakeType;
        float fShakeDuration;
        float fShakeDelay;
        {
        C2DA* pVisualEffect = g_pRules->Get2DArrays()->m_pVisualEffect;
        if (!pVisualEffect->GetINTEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKETYPE, &nShakeType))
        {
            nShakeType = CTwoDimArrays::VISUALEFFECT_SHAKE_NONE;
        }
        }
        {
        C2DA* pVisualEffect = g_pRules->Get2DArrays()->m_pVisualEffect;
        if (!pVisualEffect->GetFLOATEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKEDURATION,
                &fShakeDuration))
        {
            fShakeDuration = 0.0f;
        }
        }
        {
        C2DA* pVisualEffect = g_pRules->Get2DArrays()->m_pVisualEffect;
        if (!pVisualEffect->GetFLOATEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKEDELAY, &fShakeDelay))
        {
            fShakeDelay = 0.0f;
        }
        }
        if (nShakeType == CTwoDimArrays::VISUALEFFECT_SHAKE_BUMP)
        {
            g_pAppManager->GetClientExoApp()->GetModule()->BumpCamera(
                (unsigned long)(fShakeDelay * 1000.0f));
        }
        else if (nShakeType == CTwoDimArrays::VISUALEFFECT_SHAKE_CAMERA)
        {
            g_pAppManager->GetClientExoApp()->GetModule()->ShakeCamera(
                (unsigned long)(fShakeDelay * 1000.0f),
                (unsigned long)(fShakeDuration * 1000.0f));
        }
    }

    CExoString sSoundImpact;
    CExoString sSoundDuration;
    CSWCObject* pObject = GetTargetCSWCObject();
    CExoString sRow(m_nVisualEffect);
    {
        C2DA* pVisualEffect = g_pRules->Get2DArrays()->m_pVisualEffect;
        if (!pVisualEffect->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SOUNDIMPACT, &sSoundImpact))
        {
            sSoundImpact = "";
        }
    }
    {
        C2DA* pVisualEffect = g_pRules->Get2DArrays()->m_pVisualEffect;
        if (!pVisualEffect->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SOUNDDURATION, &sSoundDuration))
        {
            sSoundDuration = "";
        }
    }
    if (pObject != 0 && sSoundImpact.GetLength() > 0)
    {
        CResRef resSound(sSoundImpact);
        g_pExoSound->Play3DOneShotSound(
            resSound, pObject->m_vPosition, 1.5f,
            CExoSound::PRIORITY_GROUP_VISUAL_EFFECT, 0, 0, 0.0f, 0.0f);
    }
    if (sSoundDuration.GetLength() > 0)
    {
        m_pSoundSource = new CExoSoundSource(sSoundDuration);
        if (m_pSoundSource != 0)
        {
            m_pSoundSource->Set3D(1);
            m_pSoundSource->SetPriorityGroup(CExoSound::PRIORITY_GROUP_VISUAL_EFFECT);
            m_pSoundSource->SetLooping(1);
            m_pSoundSource->Play();
        }
    }
    m_bVisualEffectStopped = 0;
    if (bDone)
    {
        m_bVisualEffectDone = 1;
    }
    if (!m_bRumblePerformed)
    {
        DoVFXRumble();
        m_bRumblePerformed = 1;
    }
}
// PC 0x00706D10
void CSWCVisualEffectOnObject::StopVisualEffect(int bDone)
{
    if (m_bVisualEffectStopped)
    {
        m_bVisualEffectDone = bDone;
        return;
    }
    m_bVisualEffectStopped = 1;
    m_bVisualEffectDone = bDone;

    float fTime;
    float fLength;
    int bCessationAnimation = 0;
    for (int i = 0; i < 3; ++i)
    {
        m_anAttachedModelAnimationTime[i] = 0;
        if (m_apAttachedModels[i] != 0)
        {
            static_cast<Gob*>(m_apAttachedModels[i])->GetAnimation(
                "cessation", &fTime, &fLength);
            if (static_cast<Gob*>(m_apAttachedModels[i])->PlayAnimation(
                    "cessation", 1.0f, 1, 0.0f))
            {
                m_anAttachedModelAnimationState[i] = VISUAL_EFFECT_CESSATION;
                m_anAttachedModelAnimationTime[i] = fLength * 1000.0f;
                m_anAttachedModelFadeTime[0] =
                    ATTACHED_MODEL_FADE_TIME + (unsigned long)(fLength * 1000.0f);
                bCessationAnimation = 1;
            }
            else
            {
                m_bSelfIlluminationPulseDone = 1;
            }
        }
    }
    if (bDone && !bCessationAnimation)
    {
        m_bCessationDone = 1;
    }

    if (m_nVisualEffectState == VISUAL_EFFECT_DURATION)
    {
        if (m_anVisualEffect[1] >= VFX_LIGHT_SOURCE_END ||
            m_anVisualEffect[1] < VFX_LIGHT_SOURCE_BEGIN ||
            m_anVisualEffect[2] >= VFX_LIGHT_SOURCE_END ||
            m_anVisualEffect[2] < VFX_LIGHT_SOURCE_BEGIN)
        {
            RemoveHardCodedVisualEffect(m_anVisualEffect[1]);
            UnloadHardCodedVisualEffect();
        }
        if (m_anVisualEffect[2] == VFX_NONE)
        {
            m_nVisualEffectState = 0;
        }
        else
        {
            m_nVisualEffectState = VISUAL_EFFECT_CESSATION;
            m_HardCodedEffectClass = GetHardCodedEffectClass(m_anVisualEffect[2]);
            m_nHardCodedEffectTime = HARDCODED_CESSATION_TIME;
        }
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
    {
        RemoveHardCodedVisualEffect(m_anVisualEffect[0]);
        UnloadHardCodedVisualEffect();
    }
    if (m_anVisualEffect[2] != VFX_NONE)
    {
        if (m_anVisualEffect[1] >= VFX_LIGHT_SOURCE_END ||
            m_anVisualEffect[1] < VFX_LIGHT_SOURCE_BEGIN ||
            m_anVisualEffect[2] >= VFX_LIGHT_SOURCE_END ||
            m_anVisualEffect[2] < VFX_LIGHT_SOURCE_BEGIN)
        {
            LoadHardCodedVisualEffect(m_anVisualEffect[2]);
        }
        ApplyHardCodedVisualEffect(m_anVisualEffect[2]);
    }

    CExoString sSoundCessation;
    CSWCObject* pObject = GetTargetCSWCObject();
    CExoString sRow(m_nVisualEffect);
    bool bSoundCessationMissing;
    {
        CExoString sColumn("SoundCessation");
        C2DA* pVisualEffect = g_pRules->Get2DArrays()->m_pVisualEffect;
        bSoundCessationMissing = !pVisualEffect->GetCExoStringEntry(
            sRow, sColumn, &sSoundCessation);
    }
    if (bSoundCessationMissing)
    {
        sSoundCessation = "";
    }
    if (pObject != 0 && sSoundCessation.GetLength() > 0)
    {
        CResRef resSound(sSoundCessation);
        g_pExoSound->Play3DOneShotSound(
            resSound, pObject->m_vPosition, 1.5f,
            CExoSound::PRIORITY_GROUP_VISUAL_EFFECT, 0, 0, 0.0f, 0.0f);
    }
    if (m_pSoundSource != 0)
    {
        m_pSoundSource->Stop();
        delete m_pSoundSource;
        m_pSoundSource = 0;
    }
    if (!m_bLightSourceApplied)
    {
        DetachVisualEffect();
    }
}
// PC 0x00705EB0
void CSWCVisualEffectOnObject::KillVisualEffect()
{
    unsigned short nVisualEffect;

    if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
    {
        nVisualEffect = m_anVisualEffect[0];
        if (nVisualEffect != 0xFFFF)
        {
            RemoveHardCodedVisualEffect(nVisualEffect);
        }
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_DURATION)
    {
        nVisualEffect = m_anVisualEffect[1];
        if (nVisualEffect != 0xFFFF)
        {
            RemoveHardCodedVisualEffect(nVisualEffect);
        }
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
    {
        nVisualEffect = m_anVisualEffect[2];
        if (nVisualEffect != 0xFFFF)
        {
            RemoveHardCodedVisualEffect(nVisualEffect);
        }
    }
    UnloadVisualEffect();
    m_nVisualEffectState = 0;
    m_HardCodedEffectClass = 0;
    if (m_pSoundSource != 0)
    {
        m_pSoundSource->Stop();
        if (m_pSoundSource != 0)
        {
            delete m_pSoundSource;
        }
        m_pSoundSource = 0;
    }
}
// PC 0x007070E0
void CSWCVisualEffectOnObject::UpdateHardCodedVisualEffect(
    unsigned short nVisualEffect, unsigned long nTimeElapsed)
{
    if (nVisualEffect >= 400 && nVisualEffect < 500)
    {
        UpdateAlpha(nTimeElapsed);
        return;
    }
    if (nVisualEffect >= 500 && nVisualEffect < 600)
    {
        UpdateSelfIlluminationPulse(nTimeElapsed);
        return;
    }
    if (nVisualEffect >= 1500 && nVisualEffect < 1600)
    {
        UpdateFOV(nVisualEffect, nTimeElapsed);
        return;
    }
    if (nVisualEffect >= 1600 && nVisualEffect < 1700)
    {
        UpdateMotionBlur(nTimeElapsed);
        return;
    }
}
// PC 0x00707150
void CSWCVisualEffectOnObject::Update(unsigned long nTimeElapsed)
{
    CExoString sAnimation;
    CSWCObject* pObject = GetTargetCSWCObject();
    if (pObject == 0)
    {
        return;
    }

    if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT ||
        m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
    {
        if (m_nHardCodedEffectTime <= nTimeElapsed)
        {
            m_nHardCodedEffectTime = 0;
            if (m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
            {
                RemoveHardCodedVisualEffect(m_anVisualEffect[0]);
                UnloadHardCodedVisualEffect();
                if (!m_bPlayDurationAnimation)
                {
                    m_nVisualEffectState = 0;
                    m_bVisualEffectDone = 1;
                }
                else if (!m_bVisualEffectDone)
                {
                    m_nVisualEffectState = VISUAL_EFFECT_DURATION;
                    m_HardCodedEffectClass =
                        GetHardCodedEffectClass(m_anVisualEffect[1]);
                    LoadHardCodedVisualEffect(m_anVisualEffect[1]);
                    ApplyHardCodedVisualEffect(m_anVisualEffect[1]);
                    if (!m_bLightSourceApplied)
                    {
                        DetachVisualEffect();
                    }
                }
            }
            else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
            {
                if (m_bLightSourceApplied)
                {
                    RemoveHardCodedVisualEffect(m_anVisualEffect[2]);
                }
                UnloadHardCodedVisualEffect();
                m_nVisualEffectState = 0;
            }
        }
        else
        {
            m_nHardCodedEffectTime -= nTimeElapsed;
            if (m_bVisualEffectShown && m_bLightSourceApplied)
            {
                if (!m_bVisualEffectDone &&
                    m_nVisualEffectState == VISUAL_EFFECT_IMPACT)
                {
                    UpdateHardCodedVisualEffect(m_anVisualEffect[0], nTimeElapsed);
                }
                else if (m_nVisualEffectState == VISUAL_EFFECT_CESSATION)
                {
                    UpdateHardCodedVisualEffect(m_anVisualEffect[2], nTimeElapsed);
                }
            }
        }
    }
    else if (m_nVisualEffectState == VISUAL_EFFECT_DURATION &&
             m_bVisualEffectShown && m_bLightSourceApplied &&
             !m_bVisualEffectDone)
    {
        UpdateHardCodedVisualEffect(m_anVisualEffect[1], nTimeElapsed);
    }

    for (int i = 0; i < 3; ++i)
    {
        if (m_anAttachedModelAnimationTime[i] > 0)
        {
            if (m_anAttachedModelAnimationTime[i] > nTimeElapsed)
            {
                m_anAttachedModelAnimationTime[i] -= nTimeElapsed;
            }
            else
            {
                m_anAttachedModelAnimationTime[i] = 0;
                if (m_anAttachedModelAnimationState[i] == VISUAL_EFFECT_IMPACT)
                {
                    if (!m_bPlayDurationAnimation)
                    {
                        SetEffectDone(m_apAttachedModels[i]);
                        m_bCessationDone = 1;
                        m_bVisualEffectDone = 1;
                    }
                    else
                    {
                        m_anAttachedModelAnimationState[i] = VISUAL_EFFECT_DURATION;
                        static_cast<Gob*>(m_apAttachedModels[i])->PlayAnimation(
                            "duration", 1.0f, 0, 0.0f);
                    }
                }
                else if (m_anAttachedModelAnimationState[i] == VISUAL_EFFECT_CESSATION)
                {
                    SetEffectDone(m_apAttachedModels[i]);
                    m_bCessationDone = 1;
                    m_bVisualEffectDone = 1;
                }
            }
        }
    }
    if (m_pSoundSource != 0)
    {
        m_pSoundSource->SetPosition(pObject->m_vPosition, 0.0f);
    }
    if (m_bCessationDone)
    {
        if (!m_bSelfIlluminationPulseDone)
        {
            if (m_anAttachedModelFadeTime[0] <= nTimeElapsed)
                m_bSelfIlluminationPulseDone = 1;
            else
                m_anAttachedModelFadeTime[0] -= nTimeElapsed;
        }
        if (!m_bAlphaDone)
        {
            if (m_anAttachedModelFadeTime[1] <= nTimeElapsed)
                m_bAlphaDone = 1;
            else
                m_anAttachedModelFadeTime[1] -= nTimeElapsed;
        }
        if (!m_bFOVDone)
        {
            if (m_anAttachedModelFadeTime[2] <= nTimeElapsed)
                m_bFOVDone = 1;
            else
                m_anAttachedModelFadeTime[2] -= nTimeElapsed;
        }
    }
}
