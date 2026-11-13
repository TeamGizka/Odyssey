#include <stdio.h>
#include <string.h>
#include "nwcobject.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "nwcanimbase.h"
#include "aurcamera.h"
#include "auremitter.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "exobase.h"
#include "clientoptions.h"
#include "nwccreature.h"
#include "nwcdoor.h"
#include "nwcplaceable.h"
#include "nwctrigger.h"
#include "nwcvisualeffectonobject.h"
#include "nwsarea.h"
#include "nwsplaceable.h"
#include "nwsobject.h"
#include "serverexoapp.h"
#include "swguiingame.h"
#include "nwclass.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "worldtimer.h"
#include "clientaimaster.h"

static inline void NwcobjectTrace1();

int RenderPosition = 0;
// PC 0x00695320 GOG 0x006950C0
CSWCObject::CSWCObject(unsigned char nObjectType)
    : CGameObject(nObjectType, OBJECT_INVALID)
{
    m_pArea = NULL;
    m_pAnimBase = NULL;
    m_pQueuedAnimationCallback = NULL;
    m_nCurrentAnimation = 0;
    m_nQueuedAnimation = 0;
    m_nLoopingAnimation = 0;
    m_nAnimationTimer = 0;
    m_nFireForgetCapacity = 8;
    m_pFireForgetAnimations = new CSWCFireForgetAnimation[8];
    m_nFireForgetCount = 0;
    m_pAIListPos = NULL;
    m_fAnimationSpeed = 1.0f;
    m_nFadeStartDay = 0;
    m_nFadeStartTime = 0;
    m_nFadeState = FADE_STATE_VISIBLE;
    m_nDesiredFadeState = FADE_STATE_VISIBLE;
    m_fFadeAlpha = 1.0f;
    m_bFadeFlag = 0;
    m_bVisibilityUpdated = 0;
    m_nFadeDelay = 0;
    m_bFadeHold = 0;
    m_bStoneTexture = 0;
    m_bBarkTexture = 0;
    m_bShadeTexture = 0;
    m_bOtherTexture = 0;
    m_pVocalization = NULL;
    m_pStreamingSound = NULL;
    m_nVocalizationFlag = 0;
    m_nIdleDay = 0;
    m_nIdleTime = 0;
    m_fHeadHitHeight = 2.1f;
    m_nIdleAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
    m_oidIdleTarget = CGameObjectArray::INVALIDOBJID;
    m_bShadowsVisible = 1;
    m_vGroundNormal = Vector(0.0f, 0.0f, 1.0f);
    m_nObjectIdState = 0;
    m_bFadeLocked = 0;
    m_nFeedbackTime = 0;
    m_bOutOfPlace = 0;
    m_bPlayedOutOfPlace = 0;
    m_pServerObject = NULL;
    m_nPortraitId = 0xFFFF;
}
// PC 0x00692CC0 GOG 0x00692A30
inline CSWCFireForgetAnimation::CSWCFireForgetAnimation()
{
    m_bActive = 0;
    m_nAnimation = ANIMATION_NONE;
    m_nEndDay = 0;
    m_nEndTime = 0;
    m_nLength = DEFAULT_LENGTH;
    m_fSpeed = 1.0f;
    m_pData = NULL;
}
// PC 0x00692CF0 GOG 0x00692A60
void CSWGuiInterfaceAction::Clear()
{
    m_sName = "";
    m_nAction = -1;
    m_pfnAction = NULL;
#ifdef _XBOX
    m_oidTarget = 0;
#else
    m_oidTarget = OBJECT_INVALID;
#endif
    m_resIcon = "";
    m_bEnabled = 1;
    m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_NONE;
#ifndef _XBOX
    m_nStackSize = 1;
    m_n38 = 0;
#endif
}
// PC 0x00692D60 GOG 0x00692AD0
unsigned short CSWCObject::GetCurrentAnimation()
{
    return m_nCurrentAnimation;
}
// PC 0x00692D70 GOG 0x00692AE0
CSWCAnimBase* CSWCObject::GetAnimationBase()
{
    return m_pAnimBase;
}
// PC 0x004015A0 GOG 0x004015A0
void CSWCObject::HidePopUpBubble()
{
}
// PC 0x004015A0 GOG 0x004015A0
void CSWCObject::KillPopUpBubble()
{
}
// PC 0x004015A0 GOG 0x004015A0
void CSWCObject::UpdatePopUpBubble()
{
}
// PC 0x004015A0 GOG 0x004015A0
void CSWCObject::UpdateFloatyText()
{
}
// PC 0x005EE1C0 GOG 0x005EE1A0
void CSWCObject::SetPopUpBubblePortrait(const CResRef& cPortrait)
{
}
// PC 0x005EE1C0 GOG 0x005EE1A0
void CSWCObject::SetPopUpBubbleDefaultText(const CExoString& sText)
{
}
// PC 0x005EE1C0 GOG 0x005EE1A0
void CSWCObject::SetPopUpBubbleDefaultCreatureID(unsigned long oidCreature)
{
}
// PC 0x00692D80 GOG 0x00692AF0
Gob* CSWCObject::GetModel(unsigned char nSlot, int bFlag)
{
    if (m_pAnimBase == NULL)
    {
        return NULL;
    }
    return (Gob*)m_pAnimBase->GetModel(nSlot);
}
// PC 0x00692DA0 GOG 0x00692B10
int CSWCObject::ReplaceTexture(unsigned char nModel, const CResRef& cOld,
                               const CResRef& cNew, int nFlags)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->ReplaceTexture(nModel, cOld, cNew, nFlags);
}
// PC 0x00692DC0 GOG 0x00692B30
int CSWCObject::ReplaceTexture(const CResRef& cOld, const CResRef& cNew,
                               int nFlags)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->ReplaceTexture(cOld, cNew, nFlags);
}
// PC 0x00692DE0 GOG 0x00692B50
float CSWCObject::GetAnimationLength(unsigned short nAnimation)
{
    if (m_pAnimBase == NULL)
    {
        return 1.0f;
    }
    return m_pAnimBase->GetAnimationLength(nAnimation);
}
// PC 0x00692E00 GOG 0x00692B70
unsigned short CSWCObject::GetLoopingAnimation()
{
    return m_nLoopingAnimation;
}
// PC 0x00692E10 GOG 0x00692B80
void CSWCObject::SetOrientation(const Vector& vOrientation)
{
    m_vOrientation = normalize(vOrientation);
    if (m_pAnimBase != NULL && m_bOutOfPlace == 0)
    {
        m_pAnimBase->SetOrientation(vOrientation);
    }
    UpdateWalkMesh();
}
// PC 0x00692E70 GOG 0x00692BE0
int CSWCObject::AnimationStationary(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_STATIONARY, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 1;
}
// PC 0x00692EE0 GOG 0x00692C50
int CSWCObject::AnimationPause(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_PAUSE, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x00692F50 GOG 0x00692CC0
int CSWCObject::AnimationWalking(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_WALKING, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x00692FC0 GOG 0x00692D30
int CSWCObject::AnimationRunning(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_RUNNING, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x006945B0 GOG 0x00694350
void CSWCObject::AttachToScene(CAurScene* pScene)
{
    NwcobjectTrace1();
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->AttachToScene(pScene);
    }
}
// PC 0x006945C0 GOG 0x00694360
int CSWCObject::SetLoopingAnimation(unsigned short nAnimation)
{
    int nResult = AnimationLooping(nAnimation);
    if (nResult == 1)
    {
        m_nLoopingAnimation = nAnimation;
    }
    return nResult;
}
// PC 0x00693030 GOG 0x00692DA0
int CSWCObject::AnimationLooping(unsigned short nAnimation)
{
    if (m_nObjectType == 5 && ((nAnimation >= 0x578 && nAnimation <= 0x5F7) ||
                               (nAnimation >= 0x640 && nAnimation <= 0x6BF)))
    {
        return 1;
    }
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_LOOPING, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x006930B0 GOG 0x00692E20
int CSWCObject::AnimationFireAndForget(unsigned short nAnimation)
{
    if ((m_nObjectType == 5 || m_nObjectType == 9) &&
        ((nAnimation >= 0x3E8 && nAnimation <= 0x467) ||
         (nAnimation >= 0x4B0 && nAnimation <= 0x52F)))
    {
        return 1;
    }
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_FIREFORGET, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x00693140 GOG 0x00692EB0
int CSWCObject::AnimationOverlay(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_OVERLAY, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x006931B0 GOG 0x00692F20
int CSWCObject::AnimationPlayOutOfPlace(unsigned short nAnimation)
{
    return (nAnimation >= 0x4B0 && nAnimation <= 0x52F) ||
           (nAnimation >= 0x640 && nAnimation <= 0x6BF);
}
// PC 0x006931E0 GOG 0x00692F50
int CSWCObject::AnimationDialog(unsigned short nAnimation)
{
    int nResult;

    if (g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->IsFireForgetDialogAnimation(nAnimation) == 0)
    {
        nResult = g_pAppManager->m_pClientExoApp->GetInGameGui()
                      ->IsLoopingDialogAnimation(nAnimation);
        if (nResult == 0)
        {
            return nResult;
        }
    }
    return 1;
}
// PC 0x00693230 GOG 0x00692FA0
int CSWCObject::AnimationParry(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_PARRY, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x006932A0 GOG 0x00693010
int CSWCObject::AnimationAttack(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_ATTACK, &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x006955D0 GOG 0x00695380
CSWCObject::~CSWCObject()
{
    g_pAppManager->m_pClientExoApp->RemoveObjectFromHitcheckIgnoreList(m_nId);
    KillAllVisualEffects();
    CClientAIMaster* pAIMaster = g_pAppManager->m_pClientExoApp->GetClientAIMaster();
    pAIMaster->RemoveObject(this);

    if (m_pVocalization != NULL)
    {
        delete m_pVocalization;
        m_pVocalization = NULL;
    }
    if (m_pStreamingSound != NULL)
    {
        delete m_pStreamingSound;
        m_pStreamingSound = NULL;
    }
    if (m_pFireForgetAnimations != NULL)
    {
        delete[] m_pFireForgetAnimations;
        m_pFireForgetAnimations = NULL;
    }
    if (m_pServerObject != NULL)
    {
        m_pServerObject->DetachFromClientObject();
        m_pServerObject = NULL;
    }
}
// PC 0x00695700 GOG 0x006954B0
int CSWCObject::SetAnimation(unsigned short nAnimation, int nFlags, int nParam)
{
    if (g_pAppManager->GetServerExoApp() != NULL && AsSWCCreature() != NULL)
    {
        CSWSCreature* pCreature = AsSWCCreature()->GetServerCreature();
        if ((pCreature == NULL || pCreature->GetDead() || pCreature->GetDeadTemp()) &&
            nAnimation != 10006 && nAnimation != 10008 && nAnimation != 10156)
            return 0;
    }
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 &&
        m_bPlayedOutOfPlace)
        return 0;
    if (m_pAnimBase == NULL)
        return 0;
    if (!m_pAnimBase->AnimationExists(nAnimation))
        return 0;

    int bTransformChanged = 0;
    if (m_bOutOfPlace)
    {
        Vector vPosition = m_vPosition;
        m_bOutOfPlace = 0;
        Vector vOrientation = m_vOrientation;
        m_pAnimBase->SetPosition(vPosition);
        m_pAnimBase->SetOrientation(vOrientation);
        m_pAnimBase->SetAlwaysRender(0);
        m_pAnimBase->SetInCutscene(0);
        bTransformChanged = 1;
    }
    if ((nAnimation >= CUTSCENE_ANIMATION_W_FIRST &&
         nAnimation <= CUTSCENE_ANIMATION_W_LAST) ||
        (nAnimation >= CUTSCENE_ANIMATION_WL_FIRST &&
         nAnimation <= CUTSCENE_ANIMATION_WL_LAST))
    {
        m_bOutOfPlace = 1;
        Vector vPosition;
        Quaternion qOrientation;
        m_pAnimBase->SetPosition(vPosition);
        m_pAnimBase->SetOrientation(qOrientation);
        m_pAnimBase->SetAlwaysRender(1);
        m_pAnimBase->SetInCutscene(1);
        bTransformChanged = 1;
        m_bPlayedOutOfPlace = 1;
    }
    if (AsSWCCreature() != NULL)
    {
        if (nAnimation != CREATURE_ANIMATION_COMBAT_IDLE)
            AsSWCCreature()->StopAmbientPauseAnim();
        if (AnimationHideEquippedItems(nAnimation))
            AsSWCCreature()->HideEquippedItems(CResRef(), CResRef());
        else
            TestAndReEquipweapons(m_nCurrentAnimation, nAnimation);
    }

    if (m_nLoopingAnimation == 10030)
    {
        m_pAnimBase->SetOverlayAnimation(10030, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == 10155)
    {
        m_pAnimBase->SetOverlayAnimation(10155, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == 10038)
    {
        m_pAnimBase->SetOverlayAnimation(10038, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == 10154)
    {
        m_pAnimBase->SetOverlayAnimation(10154, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_TALK_PLEADING)
    {
        m_pAnimBase->SetOverlayAnimation(10039, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_TALK_FORCEFUL)
    {
        m_pAnimBase->SetOverlayAnimation(10040, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_TALK_LAUGHING)
    {
        m_pAnimBase->SetOverlayAnimation(10041, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_TALK_SAD)
    {
        m_pAnimBase->SetOverlayAnimation(10042, 0);
        if (nParam == 1)
        {
            m_nLoopingAnimation = nAnimation;
            m_nCurrentAnimation = nAnimation;
            return 1;
        }
    }
    {
        int nPauseAnimation;
        if (AsSWCCreature() != NULL)
            nPauseAnimation = AsSWCCreature()->GetPauseReadyAnimation();
        else
            nPauseAnimation = CREATURE_ANIMATION_COMBAT_IDLE;

        if (AnimationLooping(nAnimation))
        {
            if (nAnimation != m_nLoopingAnimation)
            {
                if ((nAnimation == 0 && m_nLoopingAnimation == 1) ||
                    (nAnimation == 1 && m_nLoopingAnimation == 0))
                {
                    SetLoopingAnimation(nAnimation);
                    return 1;
                }
                for (int i = 0; i < m_nFireForgetCapacity; ++i)
                    RemoveTopFireForgetAnimation();

                int nTransition = nPauseAnimation;
                int nNextAnimation = nPauseAnimation;
                if (nAnimation == 10129)
                    nTransition = 10129;
                else if (nAnimation == 10130)
                    nTransition = 10130;
                else if (nAnimation == 10032 || nAnimation == 10033)
                    nTransition = 10237;
                if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_MEDITATE || m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_WORSHIP)
                    nTransition = 10238;
                if (nAnimation == DOOR_ANIMATION_CLOSED)
                    nTransition = (m_nCurrentAnimation != DOOR_ANIMATION_OPEN1) + 336;
                if (m_nLoopingAnimation == DOOR_ANIMATION_CLOSED)
                {
                    if (nAnimation == DOOR_ANIMATION_OPEN1)
                    {
                        nTransition = 334;
                    }
                    else
                        nTransition = 335;
                }
                if (!(nAnimation == 10106 || nAnimation == 10107 ||
                    nAnimation == 10108 || nAnimation == 10110 ||
                    nAnimation == 10111 || nAnimation == 10112 ||
                    nAnimation == 10113 || nAnimation == 10114 ||
                    nAnimation == 10115 || nAnimation == 10116))
                {
                if (nAnimation == PLACEABLE_ANIMATION_OPEN)
                    nTransition = 312;
                if (m_nLoopingAnimation == PLACEABLE_ANIMATION_OPEN)
                    nTransition = 313;
                if (nAnimation == 10074)
                    nTransition = 314;
                if (m_nLoopingAnimation == PLACEABLE_ANIMATION_DEACTIVATE)
                    nTransition = 315;
                if (nAnimation == 10073)
                    nTransition = 315;
                if (m_nLoopingAnimation == PLACEABLE_ANIMATION_ACTIVATE)
                    nTransition = 314;
                }
                if (nAnimation == 10139)
                    nTransition = 85;
                if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_PRONE)
                    nTransition = 86;
                if (nAnimation == 10418)
                    nTransition = 456;
                else if (nAnimation == 10424 || nAnimation == 10425 ||
                         nAnimation == 10426 || nAnimation == 10427 ||
                         nAnimation == 10428)
                    nTransition = 20;
                if (nAnimation == 10501)
                    nTransition = 559;
                else if (nAnimation == DOOR_ANIMATION_DESTROYED)
                {
                    if (m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                        nTransition = 306;
                    else if (m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                        nTransition = 329;
                }
                if (m_nLoopingAnimation == 10006)
                    nTransition = 10223;
                if (m_nLoopingAnimation == CREATURE_ANIMATION_AMBIENT_3)
                    nTransition = 10224;
                if (m_nLoopingAnimation == 10005)
                {
                    nTransition = 10223;
                    if (nAnimation == 10006)
                        nTransition = 10221;
                }
                if (m_nLoopingAnimation == 10007)
                {
                    nTransition = 10224;
                    if (nAnimation == 10008)
                        nTransition = 10222;
                }
                if (nAnimation == 10005)
                    nTransition = 10219;
                else if (nAnimation == 10007)
                    nTransition = 10220;
                else if (nAnimation == 10006)
                {
                    AbortFireForgetAnimation(1);
                    nTransition = 10221;
                    nNextAnimation = nPauseAnimation;
                }
                else if (nAnimation == 10008)
                {
                    AbortFireForgetAnimation(1);
                    nTransition = 10222;
                    nNextAnimation = nPauseAnimation;
                }
                if (nAnimation == 10006 || nAnimation == 10008)
                {
                    if (AsSWCCreature() != NULL)
                    {
                        int nDeathVFX;
                        int bDeathVFX;
                        {
                            CExoString sDeathVFXColumn("DeathVFX");
                            bDeathVFX = g_pRules->Get2DArrays()->m_pAppearance->GetINTEntry(
                                AsSWCCreature()->GetAppearance()->m_nAppearance,
                                sDeathVFXColumn, &nDeathVFX);
                        }
                        if (bDeathVFX)
                        {
                            CExoString sDeathVFXNode;
                            Gob* pModel = GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 0);
                            bool bMissingNode;
                            {
                                CExoString sDeathVFXNodeColumn("DeathVFXNode");
                                bMissingNode = !g_pRules->Get2DArrays()->m_pAppearance->GetCExoStringEntry(
                                    AsSWCCreature()->GetAppearance()->m_nAppearance,
                                    sDeathVFXNodeColumn, &sDeathVFXNode);
                            }
                            if (bMissingNode)
                                sDeathVFXNode = "impact";
                            if (pModel != NULL)
                            {
                                Quaternion qOrientation;
                                Vector vPosition;
                                if (!pModel->GetPartPosition((char*)sDeathVFXNode.CStr(),
                                                             &vPosition, &qOrientation))
                                    vPosition = m_vPosition + Vector(0.0f, 0.0f, 1.0f);
                                if (m_pArea != NULL)
                                    m_pArea->ApplyFireAndForgetVisualEffect(nDeathVFX, vPosition);
                            }
                        }
                    }
                }

                if (m_nLoopingAnimation == CREATURE_ANIMATION_AMBIENT_13)
                    nTransition = nPauseAnimation;
                if (m_nLoopingAnimation == CREATURE_ANIMATION_AMBIENT_6 || m_nLoopingAnimation == CREATURE_ANIMATION_AMBIENT_9)
                {
                    AbortFireForgetAnimation(1);
                    CSWCAnimBase* pAnimBase = m_pAnimBase;
                    if (pAnimBase->m_nAnimSet == 1)
                    {
                        nTransition = 85;
                        nNextAnimation = 86;
                    }
                    else
                    {
                        nTransition = 272;
                        nNextAnimation = 273;
                    }
                }
                if (nAnimation == 10156)
                {
                    CSWCAnimBase* pAnimBase = m_pAnimBase;
                    if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_PRONE)
                    {
                        nNextAnimation = nPauseAnimation;
                        if (pAnimBase->m_nAnimSet == 1)
                            nTransition = 374;
                        else
                            nTransition = nPauseAnimation;
                    }
                    else if (pAnimBase->m_nAnimSet == 1)
                    {
                        nTransition = 85;
                        nNextAnimation = 374;
                    }
                    else
                    {
                        nTransition = 272;
                        nNextAnimation = nPauseAnimation;
                    }
                }
                if (m_nLoopingAnimation == 10156)
                {
                    nNextAnimation = nPauseAnimation;
                    nTransition = 86;
                    if (GetAnimationBase()->m_nAnimSet != 1)
                        nTransition = 273;
                }
                if (nAnimation == 10163 || nAnimation == 10164)
                    nTransition = 383;
                if (m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_KNEEL_TALK_ANGRY || m_nLoopingAnimation == CREATURE_ANIMATION_LOOPING_KNEEL_TALK_SAD)
                {
                    if (nAnimation == 10006)
                        nTransition = 80;
                    else if (nAnimation != 10163 && nAnimation != 10164)
                        nTransition = 10271;
                }
                if (nTransition != nPauseAnimation)
                {
                    if (nNextAnimation != nPauseAnimation)
                    {
                        CSWCCombatAttackData* pData = new CSWCCombatAttackData;
                        pData->m_bHit = CSWCCombatAttackData::EVENT_ANIMATION_TRANSITION;
                        pData->m_nNextAnimation = nNextAnimation;
                        pData->m_nAnimationLength = GetAnimationLength(nTransition);
                        pData->m_fAnimationSpeed = m_fAnimationSpeed;
                        AddFireForgetAnimation(nTransition, pData);
                        SetLoopingAnimation(nAnimation);
                    }
                    else
                    {
                        if (AnimationLooping(nAnimation))
                            SetLoopingAnimation(nAnimation);
                        else
                            SetLoopingAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                        AddFireForgetAnimation(nTransition, NULL);
                    }
                }
                else
                {
                    SetLoopingAnimation(nAnimation);
                }
            }
            if (!AnimationLooping(m_nCurrentAnimation) &&
                AnimationStationary(m_nCurrentAnimation) == 1 && m_nAnimationTimer > 0)
                return 1;
        }
        if (nAnimation == CREATURE_ANIMATION_RUN)
            m_nIdleAnimation = CREATURE_ANIMATION_WALK;
        else
            m_nIdleAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
        if (m_nCurrentAnimation != nAnimation && AsSWCCreature() != NULL &&
            AsSWCCreature()->m_bBlurWeapon)
            AsSWCCreature()->BlurWeapon(0);

        float fBlend = 0.0f;
        float fLength = m_pAnimBase->GetAnimationLength(m_nCurrentAnimation);
        if (fLength != 0.0f && m_nCurrentAnimation != nAnimation)
        {
            float fTime = m_pAnimBase->GetAnimationTime(m_nCurrentAnimation);
            fBlend = fTime / fLength * m_pAnimBase->GetAnimationLength(nAnimation) / 1000.0;
        }
        m_nCurrentAnimation = nAnimation;
        int nResult;
        if (bTransformChanged)
            nResult = m_pAnimBase->SetAnimation(nAnimation, m_fAnimationSpeed, 2, fBlend);
        else
            nResult = m_pAnimBase->SetAnimation(nAnimation, m_fAnimationSpeed, 0, fBlend);
        return nResult;
    }
}
// PC 0x006945E0 GOG 0x00694380
void CSWCObject::TestAndReEquipweapons(int nFlag, unsigned short nAnimation)
{
    int bReEquip = 1;
    if (AnimationHideEquippedItems(nAnimation) != 0)
    {
        bReEquip = 0;
    }
    if (AsSWCCreature() != NULL && AsSWCCreature()->m_bEquippedItemsHidden != 0
        && bReEquip != 0
        && g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState
            == 0)
    {
        AsSWCCreature()->ShowEquippedItems();
    }
}
// PC 0x00693310 GOG 0x00693080
int CSWCObject::AnimationHideEquippedItems(unsigned short nAnimation)
{
    int nValue = 0;
    unsigned short nClient;
    if (m_pAnimBase != NULL)
    {
        nClient = m_pAnimBase->ServerToClientAnimation(nAnimation);
    }
    else
    {
        nClient = nAnimation;
    }
    C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
    if (pAnimations->GetINTEntry(
            nClient, CTwoDimArrays::COL2DA_ANIMATIONS_HIDEEQUIPPEDITEMS,
            &nValue) != 0)
    {
        return nValue != 0;
    }
    return 0;
}
// PC 0x00693380 GOG 0x006930F0
int CSWCObject::GetTopFireForgetAnimation(unsigned short* pnAnimation,
                                          float* pfSpeed,
                                          unsigned long* pnLength,
                                          void** ppData)
{
    unsigned long nEndDay = m_pFireForgetAnimations[0].m_nEndDay;
    unsigned long nEndTime = m_pFireForgetAnimations[0].m_nEndTime;
    unsigned long nElapsed = g_pAppManager->GetClientExoApp()->GetWorldTimer()
                                 ->GetTimeDifferenceFromWorldTime(nEndDay, nEndTime);
    if (m_pFireForgetAnimations[0].m_bActive == 0)
    {
        *pnAnimation = CSWCFireForgetAnimation::ANIMATION_NONE;
        *ppData = NULL;
        *pfSpeed = 1.0f;
        *pnLength = CSWCFireForgetAnimation::DEFAULT_LENGTH;
        return 0;
    }
    if (nElapsed == 0)
    {
        unsigned long nDay, nTime;
        g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
        if (g_pAppManager->GetClientExoApp()->GetWorldTimer()->CompareWorldTimes(
                nDay, nTime, nEndDay, nEndTime) == -1)
        {
            *pnAnimation = CSWCFireForgetAnimation::ANIMATION_NONE;
            *ppData = NULL;
            *pfSpeed = 1.0f;
            *pnLength = CSWCFireForgetAnimation::DEFAULT_LENGTH;
            return 0;
        }
    }
    *pnAnimation = m_pFireForgetAnimations[0].m_nAnimation;
    *ppData = m_pFireForgetAnimations[0].m_pData;
    unsigned long nLength = m_pFireForgetAnimations[0].m_nLength;
    int nRemaining = nLength - nElapsed;
    if (nRemaining < CSWCFireForgetAnimation::MIN_REMAINING_LENGTH)
    {
        nRemaining = CSWCFireForgetAnimation::MIN_REMAINING_LENGTH;
    }
    *pnLength = nRemaining;
    *pfSpeed = ((float)nLength) * m_pFireForgetAnimations[0].m_fSpeed / ((float)nRemaining);
    return 1;
}
// PC 0x00694650 GOG 0x006943F0
void CSWCObject::AddFireForgetAnimation(unsigned short nAnimation, void* pData)
{
    if (AsSWCCreature() != NULL)
    {
        AsSWCCreature()->AddFireForgetAnimation(nAnimation, pData);
    }
    else if (AsSWCDoor() != NULL)
    {
        AsSWCDoor()->AddFireAndForgetAnimation(nAnimation, pData, 0);
    }
    else if (AsSWCPlaceable() != NULL)
    {
        AsSWCPlaceable()->AddFireAndForgetAnimation(nAnimation, pData, 0);
    }
    else if (AsSWCTrigger() != NULL)
    {
        AsSWCTrigger()->AddFireAndForgetAnimation(nAnimation, pData, 0);
    }
    else
    {
        InsertFireForgetAnimation(nAnimation, 500, 1.0f, pData, 0);
    }
}
// PC 0x006934D0 GOG 0x00693240
void CSWCObject::InsertFireForgetAnimation(unsigned short nAnimation,
                                           unsigned long nLength, float fSpeed,
                                           void* pData, unsigned long nDelay)
{
    AnimationFireAndForget(nAnimation);
    unsigned long nDay, nTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    if (m_nFireForgetCount == m_nFireForgetCapacity)
    {
        CSWCFireForgetAnimation* pAnimations =
            new CSWCFireForgetAnimation[m_nFireForgetCapacity + 8];
        for (int i = 0; i < m_nFireForgetCapacity; i++)
        {
            pAnimations[i].m_bActive = m_pFireForgetAnimations[i].m_bActive;
            pAnimations[i].m_nAnimation = m_pFireForgetAnimations[i].m_nAnimation;
            pAnimations[i].m_nEndDay = m_pFireForgetAnimations[i].m_nEndDay;
            pAnimations[i].m_nEndTime = m_pFireForgetAnimations[i].m_nEndTime;
            pAnimations[i].m_nLength = m_pFireForgetAnimations[i].m_nLength;
            pAnimations[i].m_fSpeed = m_pFireForgetAnimations[i].m_fSpeed;
            pAnimations[i].m_pData = m_pFireForgetAnimations[i].m_pData;
        }
        delete[] m_pFireForgetAnimations;
        m_nFireForgetCapacity += 8;
        m_pFireForgetAnimations = pAnimations;
    }
    unsigned long nEndDay, nEndTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->AddWorldTimes(
        nDay, nTime, 0, nDelay, &nEndDay, &nEndTime);
    int i;
    for (i = m_nFireForgetCount - 1; i >= 0; i--)
    {
        if (g_pAppManager->GetClientExoApp()->GetWorldTimer()->CompareWorldTimes(
                nEndDay, nEndTime, m_pFireForgetAnimations[i].m_nEndDay,
                m_pFireForgetAnimations[i].m_nEndTime) != -1)
            break;
        m_pFireForgetAnimations[i + 1].m_bActive = m_pFireForgetAnimations[i].m_bActive;
        m_pFireForgetAnimations[i + 1].m_nAnimation = m_pFireForgetAnimations[i].m_nAnimation;
        m_pFireForgetAnimations[i + 1].m_nEndDay = m_pFireForgetAnimations[i].m_nEndDay;
        m_pFireForgetAnimations[i + 1].m_nEndTime = m_pFireForgetAnimations[i].m_nEndTime;
        m_pFireForgetAnimations[i + 1].m_nLength = m_pFireForgetAnimations[i].m_nLength;
        m_pFireForgetAnimations[i + 1].m_fSpeed = m_pFireForgetAnimations[i].m_fSpeed;
        m_pFireForgetAnimations[i + 1].m_pData = m_pFireForgetAnimations[i].m_pData;
    }
    i++;
    m_pFireForgetAnimations[i].m_bActive = 1;
    m_pFireForgetAnimations[i].m_nAnimation = nAnimation;
    m_pFireForgetAnimations[i].m_nEndDay = nEndDay;
    m_pFireForgetAnimations[i].m_nEndTime = nEndTime;
    m_pFireForgetAnimations[i].m_nLength = nLength;
    m_pFireForgetAnimations[i].m_fSpeed = fSpeed;
    m_pFireForgetAnimations[i].m_pData = pData;
    m_nFireForgetCount++;
}
// PC 0x00693730 GOG 0x006934A0
void CSWCObject::RemoveTopFireForgetAnimation()
{
    for (int i = 1; i < m_nFireForgetCount; i++)
    {
        m_pFireForgetAnimations[i - 1].m_bActive =
            m_pFireForgetAnimations[i].m_bActive;
        m_pFireForgetAnimations[i - 1].m_nAnimation =
            m_pFireForgetAnimations[i].m_nAnimation;
        m_pFireForgetAnimations[i - 1].m_nEndDay =
            m_pFireForgetAnimations[i].m_nEndDay;
        m_pFireForgetAnimations[i - 1].m_nEndTime =
            m_pFireForgetAnimations[i].m_nEndTime;
        m_pFireForgetAnimations[i - 1].m_nLength =
            m_pFireForgetAnimations[i].m_nLength;
        m_pFireForgetAnimations[i - 1].m_fSpeed =
            m_pFireForgetAnimations[i].m_fSpeed;
        m_pFireForgetAnimations[i - 1].m_pData =
            m_pFireForgetAnimations[i].m_pData;
    }
    if (m_nFireForgetCount >= 1)
    {
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_bActive = 0;
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_nAnimation = 10000;
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_nEndDay = 0;
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_nEndTime = 0;
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_nLength = 500;
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_fSpeed = 1.0f;
        m_pFireForgetAnimations[m_nFireForgetCount - 1].m_pData = NULL;
        m_nFireForgetCount--;
    }
}
// PC 0x00693820 GOG 0x00693590
void CSWCObject::RemoveAllFireForgetAnimations()
{
    int nIndex = 0;
    if (m_nFireForgetCapacity > 0)
    {
        do
        {
            RemoveTopFireForgetAnimation();
            nIndex++;
        } while (nIndex < m_nFireForgetCapacity);
    }
}
// PC 0x00693840 GOG 0x006935B0
int CSWCObject::AbortFireForgetAnimation(int bAbort)
{
    m_nAnimationTimer = 0;
    if (bAbort == 0)
    {
        return SetAnimation(m_nLoopingAnimation, 0, 0);
    }
    return bAbort;
}
// PC 0x00693870 GOG 0x006935E0
int CSWCObject::Hilite(const Vector& vColor, unsigned char nModel,
                       float fUnused, int bPulse)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->Hilite(vColor, nModel, bPulse);
}
// PC 0x006938A0 GOG 0x00693610
int CSWCObject::UnHilite(unsigned char nModel)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->UnHilite(nModel);
}
// PC 0x006938C0 GOG 0x00693630
int CSWCObject::SetAlpha(float fAlpha, unsigned char nModel)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->SetAlpha(fAlpha, nModel);
}
// PC 0x006938E0 GOG 0x00693650
int CSWCObject::SetEnvMap(int nType, int nFlags, const CResRef& cEnvMap)
{
    CExoString sEnvMap;

    if (m_pAnimBase == NULL)
    {
        return 0;
    }

    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (cEnvMap == "DEFAULT")
    {
        if (pModule != NULL)
        {
            CSWCArea* pArea = pModule->m_pInfo;
            if (pArea != NULL)
            {
                pArea->GetDefaultEnvMap().CopyToString(sEnvMap);
            }
        }
        if (sEnvMap.GetLength() == 0)
        {
            return 0;
        }
    }
    else
    {
        cEnvMap.CopyToString(sEnvMap);
    }

    return m_pAnimBase->SetEnvMap(nType, nFlags, sEnvMap);
}
// PC 0x00693A40 GOG 0x006937B0
int CSWCObject::SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& cEnvMap)
{
    CExoString sEnvMap;

    if (m_pAnimBase == NULL)
    {
        return 0;
    }

    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (cEnvMap == "DEFAULT")
    {
        if (pModule != NULL)
        {
            CSWCArea* pArea = pModule->m_pInfo;
            if (pArea != NULL)
            {
                pArea->GetDefaultEnvMap().CopyToString(sEnvMap);
            }
        }
        if (sEnvMap.GetLength() == 0)
        {
            return 0;
        }
    }
    else
    {
        cEnvMap.CopyToString(sEnvMap);
    }

    return m_pAnimBase->SetEnvMap(nModel, nType, nFlags, sEnvMap);
}
// PC 0x00693BA0 GOG 0x00693910
int CSWCObject::RestoreEnvMap()
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->RestoreEnvMap();
}
// PC 0x00693BC0 GOG 0x00693930
int CSWCObject::RestoreTexture()
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->RestoreTexture();
}
// PC 0x00693BD0 GOG 0x00693940
int CSWCObject::RestoreTexture(unsigned char nModel)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->RestoreTexture(nModel);
}
// PC 0x00693BF0 GOG 0x00693960
void CSWCObject::SetDesiredFadeState(unsigned char nState, int bFade,
                                     unsigned long nDelay, unsigned long nDuration)
{
    if (this == NULL || m_bFadeLocked != 1)
    {
        m_nDesiredFadeState = nState;
        m_bFadeFlag = bFade;
        m_nFadeDelay = nDelay;
        m_nFadeDuration = nDuration;
        if (bFade)
        {
            g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
                &m_nFadeStartDay, &m_nFadeStartTime);
        }
        unsigned char nShadowMode = g_pAppManager->GetClientExoApp()
            ->GetClientOptions()->m_nObjectShadowMode;
        if (nState == FADE_STATE_VISIBLE)
        {
            if (m_pAnimBase != NULL)
            {
                if (nShadowMode == CClientOptions::OBJECT_SHADOWS_ALL)
                {
                    TurnOnShadows();
                    HideShadowBlob();
                }
                else if (nShadowMode == CClientOptions::OBJECT_SHADOWS_PLAYER)
                {
                    unsigned long oidObject = m_nId;
                    if (oidObject == g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
                    {
                        TurnOnShadows();
                        HideShadowBlob();
                    }
                    else
                    {
                        TurnOffShadows();
                        ShowShadowBlob();
                    }
                }
                else
                {
                    TurnOffShadows();
                    ShowShadowBlob();
                }
            }
            m_bShadowsVisible = 1;
        }
        else
        {
            if (m_pAnimBase != NULL)
            {
                if (nShadowMode == CClientOptions::OBJECT_SHADOWS_ALL)
                {
                    TurnOffShadows();
                    HideShadowBlob();
                }
                else if (nShadowMode == CClientOptions::OBJECT_SHADOWS_PLAYER)
                {
                    unsigned long oidObject = m_nId;
                    if (oidObject == g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
                    {
                        TurnOffShadows();
                        HideShadowBlob();
                    }
                    else
                    {
                        TurnOffShadows();
                        HideShadowBlob();
                    }
                }
                else
                {
                    TurnOffShadows();
                    ShowShadowBlob();
                }
            }
            m_bShadowsVisible = 0;
        }
    }
}
// PC 0x00693D50 GOG 0x00693AC0
void CSWCObject::SetPopUpBubbleText(const CExoString&, int, float, float)
{
}
// PC 0x00693D60 GOG 0x00693AD0
void CSWCObject::SetPortrait(const CResRef& cPortrait)
{
    m_cPortrait.CResRef::operator=(cPortrait);
    GetPortrait(1);
}
// PC 0x00693DA0 GOG 0x00693B10
int CSWCObject::CalculateProjectileTimeToTarget(const Vector& vTarget,
                                                int bFast)
{
    Vector vDelta(vTarget.m_fX - m_vPosition.m_fX,
                  vTarget.m_fY - m_vPosition.m_fY,
                  vTarget.m_fZ - m_vPosition.m_fZ);
    float fDistance = magnitude(vDelta);
    float fSpeed;
    if (bFast != 0)
    {
        fSpeed = 37.0f;
    }
    else
    {
        fSpeed = 42.0f;
    }
    return (unsigned long)(fDistance / fSpeed * 1000.0f);
}
// PC 0x00693E00 GOG 0x00693B70
void CSWCObject::RemoveShadowBlob()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->RemoveShadowBlob();
        if (m_bShadowsVisible != 0)
        {
            m_pAnimBase->TurnOnShadows();
        }
    }
}
// PC 0x00693E30 GOG 0x00693BA0
void CSWCObject::SetGroundNormal(const Vector& vNormal)
{
    m_vGroundNormal = vNormal;
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->SetShadowBlobOrientation(
            RotationArc(Vector(0.0f, 0.0f, 1.0f), vNormal));
    }
}
// PC 0x00693ED0 GOG 0x00693C40
void CSWCObject::SetPortraitId(unsigned short nPortraitId)
{
    CSWPortrait cPortrait;
    if (cPortrait.SetPortraitId(nPortraitId) != 0)
    {
        SetPortrait(cPortrait.GetPortrait(0));
    }
    m_nPortraitId = nPortraitId;
}
// PC 0x00693F60 GOG 0x00693CD0
void CSWCObject::NewVocalization(const CResRef& cVocalization, int bFlag)
{
    if (m_pVocalization != NULL)
    {
        delete m_pVocalization;
    }
    m_pVocalization = new CExoSoundSource(cVocalization);
    m_nVocalizationFlag = bFlag;
}
// PC 0x00694000 GOG 0x00693D70
void CSWCObject::SetWind(int bEnabled, float fStrength)
{
    Gob* pModel = GetModel(MODEL_SLOT_DEFAULT, 0);
    if (pModel != NULL)
    {
        if (g_pAppManager->GetClientExoApp()->GetActivePauseState() ==
                CClientExoAppInternal::PAUSE_STATE_COMBAT)
        {
            pModel->SetWind(false, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            if (bEnabled)
            {
                pModel->SetWind(true, fStrength, 0.0f, 0.0f);
            }
            else
            {
                pModel->SetWind(false, 0.0f, 0.0f, 0.0f);
            }
        }
    }
}
// PC 0x0065B730 GOG 0x00693EF0
int CSWCObject::BarkBubble(unsigned long nStrRef, const CExoString& sText,
                           const CResRef& cSound, int nFlags)
{
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()->ShowBarkBubble(nStrRef, sText, cSound,
                                                  nFlags);
    return 1;
}
// PC 0x00694060 GOG 0x00693DD0
void CSWCObject::SetFeedbackInfo(const CExoString& sFeedback)
{
    m_sFeedbackInfo = sFeedback;
    m_nFeedbackTime = 5000;
}
// PC 0x00694090 GOG 0x00693E00
void CSWCObject::GetFeedbackInfo(CExoString& sFeedback, unsigned long& nTime)
{
    sFeedback = m_sFeedbackInfo;
    nTime = m_nFeedbackTime;
}
// PC 0x00694700 GOG 0x006944A0
int CSWCObject::AddVisualEffect(unsigned short nEffect, int nNode,
                               unsigned long oidCreator,
                               unsigned long oidTarget,
                               unsigned char nBeamFlag, unsigned char nFlags,
                               const Vector& vOffset)
{
    if (nEffect == CSWCVisualEffectOnObject::VFX_SINGLE_INSTANCE &&
        HasVisualEffectApplied(CSWCVisualEffectOnObject::VFX_SINGLE_INSTANCE))
        return 1;

    CSWCVisualEffectOnObject* pEffect = new CSWCVisualEffectOnObject;
    if (pEffect == NULL)
        return 0;
    if (!pEffect->LoadVisualEffect(nEffect, nNode, m_nId, oidCreator,
                                  oidTarget, nBeamFlag, nFlags, vOffset))
    {
        delete pEffect;
        return 0;
    }
    m_lstVisualEffects.AddTail(pEffect);
    pEffect->StartVisualEffect();
    pEffect->ShowVisualEffect(0);
    pEffect->DetachVisualEffect();
    ActivateHardCodedVisualWithHighestPriority(
        pEffect->GetHardCodedEffectClass(), pEffect);
    return 1;
}
// PC 0x00694830 GOG 0x006945D0
void CSWCObject::StopAllVisualEffects()
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();
    while (pEffect != NULL)
    {
        if (pEffect != NULL)
        {
            pEffect->StopVisualEffect(1);
        }
        pEffect = m_lstVisualEffects.GetNext(pPosition);
    }
}
// PC 0x00694870 GOG 0x00694610
void CSWCObject::KillAllVisualEffects()
{
    while (m_lstVisualEffects.GetHeadPos() != NULL)
    {
        CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.RemoveHead();
        if (pEffect != NULL)
        {
            pEffect->KillVisualEffect();
            delete pEffect;
        }
    }
}
// PC 0x006948B0 GOG 0x00694650
int CSWCObject::RefreshVisualEffects(int bFlag)
{
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();
    for (CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
         pPosition != NULL; pEffect = m_lstVisualEffects.GetNext(pPosition))
    {
        if (pEffect->m_bVisualEffectDone == 0 &&
            pEffect->m_bVisualEffectShown != 0)
        {
            pEffect->HideVisualEffect();
            pEffect->ShowVisualEffect(0);
        }
    }
    return 1;
}
// PC 0x00694920 GOG 0x006946C0
void CSWCObject::ActivateHardCodedVisualWithHighestPriority(
    unsigned char nEffectClass, CSWCVisualEffectOnObject* pEffect)
{
    unsigned char nEffectType = 255;
    CSWCVisualEffectOnObject* pHighestPriority = NULL;
    CSWCVisualEffectOnObject* pCurrent = m_lstVisualEffects.GetHead();
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
    while (pPosition != NULL)
    {
        if (pCurrent->m_bVisualEffectDone == 0 &&
            (unsigned char)pCurrent->GetHardCodedEffectClass() == nEffectClass)
        {
            if (pCurrent->m_nEffectType == 0)
            {
                if (pCurrent->m_bLightSourceShadows == 0 ||
                    g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo->GetIsDark() != 0)
                {
                    if (pCurrent->m_bLightSourceApplied == 0)
                        pCurrent->ReAttachVisualEffect();
                }
                else
                {
                    if (pCurrent->m_bLightSourceApplied != 0)
                        pCurrent->DetachVisualEffect();
                }
            }
            else if (pCurrent->m_nEffectType <= nEffectType)
            {
                nEffectType = pCurrent->m_nEffectType;
                pHighestPriority = pCurrent;
            }
        }
        pCurrent = m_lstVisualEffects.GetNext(pPosition);
    }

    pCurrent = m_lstVisualEffects.GetHead();
    pPosition = m_lstVisualEffects.GetHeadPos();
    while (pPosition != NULL)
    {
        if (pCurrent->m_bVisualEffectDone == 0 &&
            (unsigned char)pCurrent->GetHardCodedEffectClass() == nEffectClass &&
            pCurrent->m_nEffectType != 0)
        {
            if (pCurrent == pHighestPriority)
            {
                if (pCurrent->GetHardCodedEffectClass() ==
                        CSWCVisualEffectOnObject::HARDCODED_EFFECT_LIGHT)
                {
                    if (pCurrent->m_bLightSourceShadows == 0 ||
                        g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo->GetIsDark() != 0)
                    {
                        if (pCurrent->m_bVisualEffectStopped != 0)
                            pCurrent->StartVisualEffect();
                        if (pCurrent->m_bLightSourceApplied == 0)
                            pCurrent->ReAttachVisualEffect();
                    }
                    else if (pCurrent->m_bLightSourceApplied != 0 &&
                             pCurrent->m_bVisualEffectStopped == 0)
                        pCurrent->StopVisualEffect(0);
                }
                else if (pCurrent->m_bLightSourceApplied == 0)
                    pCurrent->ReAttachVisualEffect();
            }
            else if (pCurrent->m_bLightSourceApplied != 0)
                pCurrent->DetachVisualEffect();
        }
        pCurrent = m_lstVisualEffects.GetNext(pPosition);
    }
}
// PC 0x00696150 GOG 0x00695F00
void CSWCObject::SetPosition(const Vector& vPosition)
{
    m_vPosition = vPosition;
    if (RenderPosition && AsSWCCreature() != NULL)
    {

    }

    if (m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
        m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
        m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {

        int bCanFade = TRUE;
        if (m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CSWSPlaceable* pServerPlaceable =
                AsSWCPlaceable()->GetServerPlaceable();
            if (pServerPlaceable != NULL && pServerPlaceable->m_bIsBodyBag &&
                !pServerPlaceable->m_bIsBodyBagVisible)
            {
                bCanFade = FALSE;
            }
        }
        if (bCanFade && m_pArea != NULL &&
            m_pArea->GetObjectsAtPositionVisible(vPosition))
        {
            SetFadeState(CSWCObject::FADE_STATE_VISIBLE);
            SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 0, 0, 0);
        }
    }

    if (m_pAnimBase != NULL && m_bOutOfPlace == 0)
    {
        m_pAnimBase->SetPosition(vPosition);
    }
    UpdateWalkMesh();
}
// PC 0x00696210 GOG 0x00695FC0
int CSWCObject::DeleteVisualEffect(unsigned short nEffect)
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();
    while (pPosition != NULL)
    {
        if (pEffect->m_nVisualEffect == nEffect &&
            pEffect->m_bVisualEffectDone == 0)
        {
            DetachVisualEffectsMatchingHardCodedClass(pEffect);
            pEffect->StopVisualEffect(1);
            ActivateHardCodedVisualWithHighestPriority(
                pEffect->GetHardCodedEffectClass(), NULL);
            return 1;
        }
        pEffect = m_lstVisualEffects.GetNext(pPosition);
    }
    return 0;
}
// PC 0x006962A0 GOG 0x00696050
void CSWCObject::UpdateVisualEffectsList()
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();
    while (pEffect != NULL)
    {
        pEffect->Update(m_nTimeDelta);
        ActivateHardCodedVisualWithHighestPriority(
            pEffect->GetHardCodedEffectClass(), NULL);
        if (pEffect->IsDone())
        {
            if (pEffect->m_bPlayDurationAnimation == 0)
            {
                DetachVisualEffectsMatchingHardCodedClass(pEffect);
                ActivateHardCodedVisualWithHighestPriority(
                    pEffect->GetHardCodedEffectClass(), NULL);
            }
            CExoLinkedListNode* pNextPosition = pPosition;
            CSWCVisualEffectOnObject* pNextEffect =
                m_lstVisualEffects.GetNext(pNextPosition);
            m_lstVisualEffects.Remove(pPosition);
            pEffect->UnloadVisualEffect();
            delete pEffect;
            pEffect = pNextEffect;
            pPosition = pNextPosition;
        }
        else
        {
            pEffect = m_lstVisualEffects.GetNext(pPosition);
        }
    }
}
// PC 0x00696380 GOG 0x00696130
void CSWCObject::SetArea(CSWCArea* pArea)
{
    CSWCArea* pOldArea = m_pArea;

    if (pOldArea != pArea)
    {
        if (pOldArea != NULL)
        {
            pOldArea->m_lstObjectIds.Remove(m_nId);
        }
        if (pArea != NULL)
        {
            pArea->m_lstObjectIds.AddUnique(m_nId);
            SetObjectVisibilityOnSetArea(pArea);
        }
    }
    m_pArea = pArea;
}
// PC 0x00694AE0 GOG 0x00694880
void CSWCObject::DetachVisualEffectsMatchingHardCodedClass(
    CSWCVisualEffectOnObject* pNewEffect)
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetTailPos();
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetTail();
    while (pPosition != NULL)
    {
        if (pNewEffect->GetHardCodedEffectClass() !=
                CSWCVisualEffectOnObject::HARDCODED_EFFECT_BEAM &&
            pNewEffect->GetHardCodedEffectClass() ==
                pEffect->GetHardCodedEffectClass() &&
            pNewEffect != pEffect &&
            pEffect->m_nEffectType != 0 &&
            pEffect->m_bVisualEffectDone == 0)
        {
            if (pEffect->m_bLightSourceApplied != 0)
            {
                pEffect->DetachVisualEffect();
            }
            return;
        }
        pEffect = m_lstVisualEffects.GetPrev(pPosition);
    }
}
// PC 0x00694B80 GOG 0x00694920
int CSWCObject::HasVisualEffectApplied(unsigned long nEffect)
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetTailPos();
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetTail();
    while (pPosition != NULL)
    {
        if (pEffect->m_nVisualEffect == nEffect)
        {
            return 1;
        }
        pEffect = m_lstVisualEffects.GetPrev(pPosition);
    }
    return 0;
}
// PC 0x00694BE0 GOG 0x00694980
float CSWCObject::GetVisualEffectAlpha()
{
    CExoLinkedListNode* pPosition = m_lstVisualEffects.GetTailPos();

    const unsigned char nNoEffectType = 255;
    unsigned char nEffectType = nNoEffectType;
    float fAlpha = -1.0f;
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetTail();

    while (pPosition != NULL)
    {
        if (pEffect->GetHardCodedEffectClass() ==
                CSWCVisualEffectOnObject::HARDCODED_EFFECT_ALPHA &&
            pEffect->m_bVisualEffectDone == 0 &&
            pEffect->m_bLightSourceApplied != 0 &&
            pEffect->m_nEffectType < nEffectType)
        {
            fAlpha = pEffect->m_fAlpha;
            nEffectType = pEffect->m_nEffectType;
        }
        pEffect = m_lstVisualEffects.GetPrev(pPosition);
    }
    return fAlpha;
}
// PC 0x00694C70 GOG 0x00694A10
float CSWCObject::GetFinalFadeAlpha()
{
    BOOL bCheckFade = TRUE;
    if (bCheckFade) {} else goto visible;
    if (AsSWCCreature() != NULL)
    {
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
        if (pPlayer == NULL)
            goto visible;
        for (int i = 0; i < pPlayer->m_lstVisibilityList.num; i++)
        {
            if (pPlayer->m_lstVisibilityList[i]->m_oidObject == m_nId)
                return 1.0f;
        }
    }
check:
    if (m_nDesiredFadeState != FADE_STATE_VISIBLE)
        return 0.0f;
visible:
    {
        BOOL bVisible = TRUE;
        float fAlpha = GetVisualEffectAlpha();
        if (bVisible)
        {
            if (fAlpha != -1.0f)
                return fAlpha;
            return 1.0f;
        }
        goto check;
    }
}
// PC 0x00694CF0 GOG 0x00694A90
int CSWCObject::FadeObject()
{
    if (m_nFadeState != m_nDesiredFadeState)
    {
        unsigned long nTimeDelta =
            g_pAppManager->GetClientExoApp()->GetWorldTimer()->
                GetSnapshotTimeDifference() / 1000;

        if (m_nDesiredFadeState == FADE_STATE_VISIBLE)
        {
            if (m_fFadeAlpha >= GetFinalFadeAlpha())
            {
                m_fFadeAlpha = GetFinalFadeAlpha();
                m_nFadeState = FADE_STATE_VISIBLE;
            }
            else
            {
                m_fFadeAlpha += nTimeDelta / 2000.0f;
                m_nFadeState = FADE_STATE_FADING_IN;
            }
        }
        else if (m_nDesiredFadeState == FADE_STATE_HIDDEN)
        {
            if (m_nFadeDuration != 0)
            {
                unsigned long nDay, nTime, nElapsedDay, nElapsedTime;
                g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
                    &nDay, &nTime);
                g_pAppManager->GetClientExoApp()->GetWorldTimer()->SubtractWorldTimes(
                    nDay, nTime, m_nFadeStartDay, m_nFadeStartTime,
                    &nElapsedDay, &nElapsedTime);
                if (nElapsedTime <= m_nFadeDuration)
                {
                    return 0;
                }
            }

            CSWCCreature* pCreature = AsSWCCreature();
            if (pCreature != NULL && pCreature->m_bInParty != 0)
            {
                pCreature->SetInParty(0);
            }
            if (m_fFadeAlpha <= GetFinalFadeAlpha())
            {
                m_fFadeAlpha = GetFinalFadeAlpha();
                m_nFadeState = FADE_STATE_HIDDEN;
                if (m_bFadeFlag != 0)
                {
                    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
                        &m_nFadeStartDay, &m_nFadeStartTime);
                    m_pAnimBase->AttachToScene(NULL);
                }
                pCreature = AsSWCCreature();
                if (pCreature != NULL && pCreature->m_bInParty != 0)
                {
                    pCreature->SetInParty(0);
                }
            }
            else
            {
                m_fFadeAlpha -= nTimeDelta / 2000.0f;
                m_nFadeState = FADE_STATE_FADING_OUT;
            }
        }

        if (m_fFadeAlpha < 0.0f)
        {
            m_fFadeAlpha = 0.0f;
        }
        else if (m_fFadeAlpha > 1.0f)
        {
            m_fFadeAlpha = 1.0f;
        }
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->SetAlpha(m_fFadeAlpha, CSWCAnimBase::MODEL_SLOT_MAIN);
        }
    }
    else if (m_bFadeFlag != 0)
    {
        unsigned long nDay, nTime, nElapsedDay, nElapsedTime;
        g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
            &nDay, &nTime);
        g_pAppManager->GetClientExoApp()->GetWorldTimer()->SubtractWorldTimes(
            nDay, nTime, m_nFadeStartDay, m_nFadeStartTime,
            &nElapsedDay, &nElapsedTime);
        if (nElapsedTime >= m_nFadeDelay + m_nFadeDuration)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00694FB0 GOG 0x00694D50
void CSWCObject::SetFadeState(unsigned char nState)
{
    if (this == NULL || m_bFadeLocked != 1)
    {
        m_nFadeState = nState;
        unsigned char nShadowMode = g_pAppManager->m_pClientExoApp
            ->GetClientOptions()->m_nObjectShadowMode;
        if (nState == FADE_STATE_VISIBLE)
        {
            if (m_pAnimBase != NULL)
            {
                if (nShadowMode == CClientOptions::OBJECT_SHADOWS_ALL)
                {
                    TurnOnShadows();
                    HideShadowBlob();
                }
                else if (nShadowMode == CClientOptions::OBJECT_SHADOWS_PLAYER)
                {
                    unsigned long oidObject = m_nId;
                    if (oidObject == g_pAppManager->m_pClientExoApp->GetPlayerCreatureId())
                    {
                        TurnOnShadows();
                        HideShadowBlob();
                    }
                    else
                    {
                        TurnOffShadows();
                        ShowShadowBlob();
                    }
                }
                else
                {
                    TurnOffShadows();
                    ShowShadowBlob();
                }
            }
            m_bShadowsVisible = 1;
            m_nDesiredFadeState = FADE_STATE_VISIBLE;
            m_fFadeAlpha = GetFinalFadeAlpha();
        }
        else
        {
            if (m_pAnimBase != NULL)
            {
                if (nShadowMode == CClientOptions::OBJECT_SHADOWS_ALL)
                {
                    TurnOffShadows();
                    HideShadowBlob();
                }
                else if (nShadowMode == CClientOptions::OBJECT_SHADOWS_PLAYER)
                {
                    unsigned long oidObject = m_nId;
                    if (oidObject == g_pAppManager->m_pClientExoApp->GetPlayerCreatureId())
                    {
                        TurnOffShadows();
                        HideShadowBlob();
                    }
                    else
                    {
                        TurnOffShadows();
                        HideShadowBlob();
                    }
                }
                else
                {
                    TurnOffShadows();
                    ShowShadowBlob();
                }
            }
            m_bShadowsVisible = 0;
            m_nDesiredFadeState = FADE_STATE_HIDDEN;
            m_fFadeAlpha = GetFinalFadeAlpha();
        }
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->SetAlpha(m_fFadeAlpha, CSWCAnimBase::MODEL_SLOT_MAIN);
        }
    }
}
// PC 0x006950E0 GOG 0x00694E80
void CSWCObject::SetObjectVisibilityOnSetArea(CSWCArea* pArea)
{
    Vector vPosition;

    if (m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
        m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
        m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        if (m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CSWSPlaceable* pServerPlaceable =
                AsSWCPlaceable()->GetServerPlaceable();
            if (pServerPlaceable != NULL && pServerPlaceable->m_bIsBodyBag &&
                !pServerPlaceable->m_bIsBodyBagVisible)
            {
                return;
            }
        }

        vPosition = m_vPosition;
        if (!equal(m_vPosition, Vector(0.0f, 0.0f, 0.0f)) &&
            pArea->GetObjectsAtPositionVisible(vPosition))
        {
            SetFadeState(FADE_STATE_VISIBLE);
            SetDesiredFadeState(FADE_STATE_VISIBLE, 0, 0, 0);
        }
        else
        {
            SetFadeState(FADE_STATE_HIDDEN);
            SetDesiredFadeState(FADE_STATE_HIDDEN, 0, 0, 0);
        }
    }
}
// PC 0x006951E0 GOG 0x00694F80
void CSWCObject::DetachVisualEffects()
{
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();
    for (CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
         pPosition != NULL; pEffect = m_lstVisualEffects.GetNext(pPosition))
    {
        if (pEffect->m_bVisualEffectDone == 0 &&
            pEffect->m_bVisualEffectShown != 0)
        {
            pEffect->DetachVisualEffect();
        }
    }
}
// PC 0x00695240 GOG 0x00694FE0
void CSWCObject::ReAttachVisualEffects()
{
    CSWCVisualEffectOnObject* pEffect = m_lstVisualEffects.GetHead();
    for (CExoLinkedListNode* pPosition = m_lstVisualEffects.GetHeadPos();
         pPosition != NULL; pEffect = m_lstVisualEffects.GetNext(pPosition))
    {
        if (pEffect->m_bVisualEffectDone == 0 &&
            pEffect->m_bVisualEffectShown != 0)
        {
            pEffect->ReAttachVisualEffect();
        }
    }
}
// PC 0x006952A0 GOG 0x00695040
void QueuedCallbackFunction(CAurObject* pObject, char* sEvent, void* pUserData)
{
    ((CSWCObject*)pUserData)->QueuedCallback(pObject, sEvent);
}
#ifndef _XBOX
// PC 0x006952C0 GOG 0x00695060
void CSWCObject::MenuActionUseSecuritySpike(unsigned long oidItem, CSWCCreature* pCreature)
{
    if (pCreature != NULL)
    {
        pCreature->ClearAllActions();
    }
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();
    pMessage->SendPlayerToServerInput_UnlockObject(GetId(), oidItem & ~OBJECT_LISTTYPE_MASK);
}
#endif
// PC 0x006940C0 GOG 0x00693E30
void CSWCObject::QueuedCallback(CAurObject* pObject, char* sEvent)
{
    char sDone[64];
    if (m_nQueuedAnimation != 0)
    {
        if (strnicmp(sEvent, "DONE", 4) == 0)
        {
            sprintf(sDone, "done%s",
                    m_pAnimBase->GetAnimationName(m_nCurrentAnimation).CStr());
            if (stricmp(sEvent, sDone) == 0)
            {
                m_nCurrentAnimation = m_nQueuedAnimation;
                m_nQueuedAnimation = 0;
            }
        }
    }
}
// PC 0x00694180 GOG 0x00693F20
void CSWCObject::ClientServerSyncHandler_Animation(int nAnimation, float fSpeed)
{
    m_fAnimationSpeed = fSpeed;
    if (nAnimation != m_nCurrentAnimation)
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
// PC 0x006941D0 GOG 0x00693F70
int CSWCObject::EnableBumpedOutShell(CResRef cShell, float fThickness)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->EnableBumpedOutShell(cShell, fThickness);
}
// PC 0x006941F0 GOG 0x00693F90
int CSWCObject::DisableBumpedOutShell()
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    return m_pAnimBase->DisableBumpedOutShell();
}
// PC 0x00694200 GOG 0x00693FA0
void CSWCObject::ClearAllActions()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    unsigned long nServerId = pServerExoApp->ClientToServerObjectId(GetId());
    pServerExoApp->ClearAllActions(nServerId);
}
// PC 0x00694220 GOG 0x00693FC0
void CSWCObject::ClearAllQueuedCombatActions()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    unsigned long nServerId = pServerExoApp->ClientToServerObjectId(m_nId);
    pServerExoApp->ClearAllQueuedCombatActions(nServerId);
}
// PC 0x00694240 GOG 0x00693FE0
CSWSObject* CSWCObject::GetServerObject()
{
    if (m_pServerObject == NULL)
    {
        if (m_bFadeLocked != 0)
        {
            return NULL;
        }
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp == NULL)
        {
            return NULL;
        }
        unsigned long nServerId = m_nId;
        nServerId = pServerExoApp->ClientToServerObjectId(nServerId);
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        CGameObject* pObject = pServerExoApp->GetGameObject(nServerId);
        if (pObject != NULL)
        {
            m_pServerObject = pObject->AsSWSObject();
            if (m_pServerObject != NULL)
            {
                m_pServerObject->AttachmentFromClientObject(this);
            }
        }
    }
    return m_pServerObject;
}
// PC 0x006942B0 GOG 0x00694050
void CSWCObject::AttachmentFromServerObject(CSWSObject* pServerObject)
{
    m_pServerObject = pServerObject;
}
// PC 0x006942C0 GOG 0x00694060
void CSWCObject::DetachFromServerObject()
{
    m_pServerObject = NULL;
}
// PC 0x006942D0 GOG 0x00694070
void CSWCObject::EnableDistortion()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->EnableDistortion();
    }
}
// PC 0x006942E0 GOG 0x00694080
void CSWCObject::DisableDistortion()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->DisableDistortion();
    }
}
// PC 0x006942F0 GOG 0x00694090
unsigned char CSWCObject::GetReticlePoint(Vector& vPoint)
{
    int nResult = RETICLE_POINT_UNAVAILABLE;
    if (m_pAnimBase != NULL)
    {
        Gob* pModel = m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
        if (pModel != NULL)
        {
            Quaternion qOrientation;
            Part* pHook = pModel->GetPart("lookathook");
            if (pHook != NULL)
            {
                pHook->GetWorld(vPoint, qOrientation);
                CAurCamera* pCamera =
                    g_pAppManager->m_pClientExoApp->GetModuleCamera();
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
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth;
                vPoint.m_fY *=
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight;
                return nResult;
            }
        }

        nResult = m_pAnimBase->GetCenterPoint(vPoint);
    }
    return nResult;
}
// PC 0x00694490 GOG 0x00694230
void CSWCObject::AcquireAnimationBase(CSWCObject* pSource)
{
    if (pSource != NULL)
    {
        if (m_pAnimBase != NULL)
        {
            delete m_pAnimBase;
        }
        if (pSource->AsSWCCreature() != NULL)
        {
            pSource->AsSWCCreature()->UnRegisterCallbacks();
        }
        m_pAnimBase = pSource->m_pAnimBase;
        static_cast<Gob*>(pSource->m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT))
            ->SetExternalData(this);
        static_cast<Gob*>(pSource->m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT))
            ->SetExternalDataType(m_nObjectType);
        pSource->m_pAnimBase = NULL;
    }
}

#ifndef _XBOX
float CSWCObject::GetStickyMouseThreshhold()  // PC 0x00694510 GOG 0x006942B0
{
    float fHeight = g_nScreenHeight * 0.1f;
    float fWidth = g_nScreenWidth * 0.1f;
    return fWidth * fWidth + fHeight * fHeight;
}
#endif

static inline void NwcobjectTrace1()
{
}

#ifdef _XBOX

void CSWCObject::MenuActionUseSecuritySpike(unsigned long oidItem, CSWCCreature* pCreature)
{
    if (pCreature != NULL)
    {
        pCreature->ClearAllActions();
    }
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();
    pMessage->SendPlayerToServerInput_UnlockObject(m_nId, oidItem & ~OBJECT_LISTTYPE_MASK);
}
#endif
