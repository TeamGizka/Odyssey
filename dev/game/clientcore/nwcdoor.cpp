#include "nwcdoor.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "worldtimer.h"
#include "clientaimaster.h"
#include "aurmodel.h"
#include "nwcanimbase.h"
#include "nwmessage.h"
#include "nwsdoor.h"
#include "nwsobject.h"
#include "nwcanimbase.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "exobase.h"
#include "nwcarea.h"
#include "nwsarea.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwskill.h"
#include "swgui.h"
#include "guiingame.h"
#include "aurcamera.h"
#include "aurscene.h"
#include "../../libsource/AURORA/aurpointer.h"

#include <math.h>
// PC 0x00683BB0  Xbox 0x000BFC30
CSWCDoor::CSWCDoor()
    : CSWCObject(CGameObject::OBJECT_TYPE_DOOR)
{
    unsigned long nDay;
    unsigned long nTime;

    m_nCurrentAnimation = DOOR_ANIMATION_CLOSED;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_bLockable = 0;
    m_bNoVisibleModel = 0;
    m_bTrapDetected = 0;
    m_bTrap = 0;
    m_bLocked = 0;
    m_bHilighted = 0;
    m_bAreaTransition = 0;
    m_bUseable = 1;
    m_nAppearanceType = INITIAL_APPEARANCE_TYPE;
    m_nGenericType = INITIAL_GENERIC_TYPE;
    m_sModel = "";
    m_nDoorData = 0;
    m_bStatic = 0;
}
// PC 0x006825F0
void CSWCDoor::UnloadModel()
{
    if (m_pAnimBase != 0)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = 0;
    }
}
// PC 0x0060E760
void CSWCDoor::HilightDoor(int)
{
}
// PC 0x00646650
void CSWCDoor::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x00683CC0  Xbox 0x000BFD40
void CSWCDoor::SetIsAreaTransition(int bAreaTransition)
{
    if (bAreaTransition == 0)
    {
        Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        if (pModel != 0)
        {
            pModel->Amputate("trans");
        }
    }
    m_bAreaTransition = bAreaTransition;
}
// PC 0x00683D00  Xbox 0x000BFD80
int CSWCDoor::SetAnimationWithoutTrigger(unsigned short nAnimation)
{
    m_nCurrentAnimation = nAnimation;
    if (AnimationLooping(nAnimation) != 0)
    {
        SetLoopingAnimation(nAnimation);
    }
    return m_pAnimBase->SetAnimation(nAnimation, 0.0f, 0, 0);
}
// PC 0x00683D40  Xbox 0x000BFDC0
void CSWCDoor::SetState(unsigned char nState)
{
    Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    m_nState = nState;
    if (pModel != 0)
    {
        if (nState != 0)
        {
            if (nState > 0 && nState <= 3)
            {
                pModel->SetIgnoreHitCheck(1);
            }
        }
        else
        {
            pModel->SetIgnoreHitCheck(0);
        }
    }
}
// PC 0x00683D90  Xbox 0x000BFE10
void CSWCDoor::ToggleDoorState(unsigned long oidDoor, CSWCCreature* pCreature)
{
    if (pCreature != 0)
    {
        pCreature->ClearAllActions();
    }
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (m_nCurrentAnimation == DOOR_ANIMATION_CLOSED)
    {
        pMessage->SendPlayerToServerInput_ChangeDoorState(m_nId, 10021);
        g_pAppManager->m_pClientExoApp->DelayNextEnemyAutoPause();
    }
    else if (m_nCurrentAnimation == DOOR_ANIMATION_OPEN1 ||
             m_nCurrentAnimation == DOOR_ANIMATION_OPEN2)
    {
        pMessage->SendPlayerToServerInput_ChangeDoorState(m_nId, DOOR_ANIMATION_CLOSED);
    }
}
// PC 0x00683E00  Xbox 0x000BFE80
void CSWCDoor::AddFireAndForgetAnimation(unsigned short nAnimation,
                                         void* pData, unsigned short nFlags)
{
    unsigned long nLength;
    if (!(m_fAnimationSpeed == 0.0f))
    {
        nLength = (unsigned __int64)(GetAnimationLength(nAnimation) /
                                     fabs(m_fAnimationSpeed));
    }
    else
    {
        nLength = (unsigned __int64)GetAnimationLength(nAnimation);
    }
    InsertFireForgetAnimation(nAnimation, nLength, m_fAnimationSpeed, pData, 0);
}
// PC 0x00683E50  Xbox 0x000BFEE0
void CSWCDoor::MenuActionSecurity(unsigned long oidDoor,
                                  CSWCCreature* pCreature)
{
    if (pCreature != 0)
    {
        pCreature->ClearAllActions();
    }
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    pMessage->SendPlayerToServerInput_UnlockObject(m_nId, OBJECT_INVALID);
    g_pAppManager->m_pClientExoApp->DelayNextEnemyAutoPause();
}
// PC 0x00683E90  Xbox 0x000BFF20
void CSWCDoor::MenuActionBash(unsigned long oidDoor, CSWCCreature* pCreature)
{
    if (pCreature != 0)
    {
        unsigned long oidTarget = m_nId;
        unsigned long oidCreature = pCreature->m_nId;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        if (pClientExoApp->ShowTutorialWindow(
                CClientExoApp::TUTORIAL_BASH, oidCreature, oidTarget,
                                              1) == 0)
        {
            pCreature->ClearAllActions();
            pCreature->PerformAttack(m_nId);
            g_pAppManager->m_pClientExoApp->DelayNextEnemyAutoPause();
        }
    }
}
// PC 0x00683EE0  Xbox 0x000BFF70
CSWSDoor* CSWCDoor::GetServerDoor()
{
    CSWSObject* pServerObject;
    if (GetServerObject() != 0)
    {
        pServerObject = GetServerObject();
        return pServerObject->AsSWSDoor();
    }
    return 0;
}
// PC 0x00683F00  Xbox 0x000BFF90
void MoveNextDoorState(CAurObject* pObject, char* sEvent, void* pData)
{
    if (stricmp(sEvent, "door") == 0 ||
        (strnicmp(sEvent, "done", 4) == 0 &&
         (strnicmp(sEvent + 4, "opening", 7) == 0 ||
          strnicmp(sEvent + 4, "closing", 7) == 0)))
    {
        CSWCObject* pDoor = (CSWCObject*)pData;
        if (pDoor->GetServerObject() != 0)
        {
            CSWSDoor* pServerDoor = pDoor->GetServerObject()->AsSWSDoor();
            if (pServerDoor != 0)
            {
                pServerDoor->MoveToNextOpenState();
            }
        }
    }
}
// PC 0x00683F90  Xbox 0x000C0020
CSWCDoor::~CSWCDoor()
{
    UnloadModel();
}
// PC 0x00684030  Xbox 0x000C00C0
void CSWCDoor::AnimationUpdate()
{
    unsigned short nAnimation;
    float fSpeed;
    unsigned long nLength;
    void* pData;

    if (AnimationFireAndForget(m_nCurrentAnimation) == 1)
    {
        m_nAnimationTimer -= m_nTimeDelta;
        if (m_nAnimationTimer < 0)
        {
            SetAnimation(GetLoopingAnimation(), 0, 0);
            m_nAnimationTimer = 0;
        }
    }
    if (AnimationLooping(m_nCurrentAnimation) == 1)
    {
        if (GetTopFireForgetAnimation(&nAnimation, &fSpeed, &nLength,
                                      &pData) == 1)
        {
            m_nCurrentAnimation = nAnimation;
            m_pAnimBase->SetAnimation(nAnimation, fSpeed, 1, 0.0f);
            m_nAnimationTimer = nLength;
            RemoveTopFireForgetAnimation();

            if (nAnimation == 334 || nAnimation == 335 ||
                nAnimation == 336 || nAnimation == 337 || nAnimation == 329)
            {
                CExoString sSound;
                int nSoundAppearance;
                int nAppearanceType = m_nAppearanceType;
                CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
                if (nAppearanceType == 0)
                {
                    nAppearanceType = m_nGenericType;
                    C2DA* pDoorTable = pArrays->m_pGenericDoors;
                    pDoorTable->GetINTEntry(
                        nAppearanceType, CTwoDimArrays::COL2DA_GENERICDOORS_SOUNDAPPTYPE,
                        &nSoundAppearance);
                }
                else
                {
                    C2DA* pDoorTable = pArrays->m_pDoorTypes;
                    pDoorTable->GetINTEntry(
                        nAppearanceType, CTwoDimArrays::COL2DA_DOORTYPES_SOUNDAPPTYPE,
                        &nSoundAppearance);
                }
                if (nAnimation == 334 || nAnimation == 335)
                {
                    CTwoDimArrays* pSoundArrays = g_pRules->m_p2DArrays;
                    C2DA* pSoundTable = pSoundArrays->m_pPlaceableSounds;
                    pSoundTable->GetCExoStringEntry(
                        nSoundAppearance, CTwoDimArrays::COL2DA_PLACEABLESOUNDS_OPENED,
                        &sSound);
                }
                else if (nAnimation == 336 || nAnimation == 337)
                {
                    CTwoDimArrays* pSoundArrays = g_pRules->m_p2DArrays;
                    C2DA* pSoundTable = pSoundArrays->m_pPlaceableSounds;
                    pSoundTable->GetCExoStringEntry(
                        nSoundAppearance, CTwoDimArrays::COL2DA_PLACEABLESOUNDS_CLOSED,
                        &sSound);
                }
                else if (nAnimation == 329)
                {
                    CTwoDimArrays* pSoundArrays = g_pRules->m_p2DArrays;
                    C2DA* pSoundTable = pSoundArrays->m_pPlaceableSounds;
                    pSoundTable->GetCExoStringEntry(
                        nSoundAppearance, CTwoDimArrays::COL2DA_PLACEABLESOUNDS_DESTROYED,
                        &sSound);
                }
                CResRef cSound(sSound);
                g_pExoSound->Play3DOneShotSound(cSound, m_vPosition, 1.5f,
                    CExoSound::PRIORITY_GROUP_DOOR, 0, 0, 0.0f, 0.0f);
            }
        }
        if (m_nCurrentAnimation == DOOR_ANIMATION_CLOSED)
        {
            SetState(0);
        }
        else if (m_nCurrentAnimation == DOOR_ANIMATION_OPEN1)
        {
            SetState(1);
        }
        else if (m_nCurrentAnimation == DOOR_ANIMATION_OPEN2)
        {
            SetState(2);
        }
        else if (m_nCurrentAnimation == DOOR_ANIMATION_DESTROYED)
        {
            SetState(3);
        }
        if (pData != 0)
        {
            delete pData;
        }
    }
}
// PC 0x006842E0  Xbox 0x000C03F0
int CSWCDoor::LoadModel(const CResRef& cModel, unsigned char nModel,
                        unsigned char nFlags)
{
    if (m_pAnimBase == 0)
    {
        m_pAnimBase = new CSWCAnimBaseDoor;
        m_pAnimBase->m_bUseable = m_bUseable;
        if (m_pAnimBase == 0)
        {
            return 0;
        }
    }
    if (m_pAnimBase->LoadModel(cModel, nModel) == 0)
    {
        return 0;
    }
    Gob* pModel = static_cast<Gob*>(m_pAnimBase->GetModel(nModel));
    if (pModel != 0)
    {
        pModel->SetExternalData(this);
        pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_DOOR);
        pModel->RegisterCallBack(MoveNextDoorState, "", this,
                                 MODEL_CALLBACK_DELAY, 0);
    }
    cModel.CopyToString(m_sModel);
    return 1;
}
// PC 0x00684410  Xbox 0x000C05C0
void CSWCDoor::GetTargetActions(CSWCCreature* pCreature, int nMenu,
                              CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    CSWSDoor* pServerDoor = GetServerDoor();
    int nRestrictMode = m_pArea->GetServerArea()->m_nPlayerRestrictMode;
    int nAction = lstActions.num;
    CExoString sAction;

    switch (nMenu)
    {
    case TARGET_ACTION_MENU_PRIMARY:
        if (GetCurrentAnimation() != DOOR_ANIMATION_OPEN1 &&
            GetCurrentAnimation() != DOOR_ANIMATION_OPEN2 &&
            !m_bStatic && !m_bLocked &&
            !nRestrictMode && m_bLockable)
        {
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(
                    CClientExoApp::GUI_STRING_BASH);
            lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCDoor::MenuActionBash;
#ifdef _XBOX
            lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
            lstActions[nAction].m_oidTarget = GetId();
#endif
            lstActions[nAction].m_resIcon = "i_attack";
        }
        break;
    case TARGET_ACTION_MENU_SKILLS:
        if (m_bLockable && pServerDoor != 0 && !pServerDoor->m_bKeyRequired &&
            pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY))
        {
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(
                    CClientExoApp::GUI_STRING_SECURITY);
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCDoor::MenuActionSecurity;
#ifdef _XBOX
            lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
            lstActions[nAction].m_oidTarget = GetId();
#endif
            lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_SECURITY;
            lstActions[nAction].m_resIcon =
                g_pRules->m_pSkills[CSWRules::SKILL_SECURITY].m_sIcon;
        }
        break;
    }
}
// PC 0x00684660  Xbox 0x000C0890
void CSWCDoor::UpdateAreaTransitionDisplay()
{
    CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    CSWCCreature* pPlayer =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();

    if (pGui == NULL || pPlayer == NULL)
    {
        return;
    }
    if (!m_bAreaTransition)
    {
        return;
    }
    if (m_nState != DOOR_STATE_OPEN1 && m_nState != DOOR_STATE_OPEN2)
    {
        return;
    }

    Vector vDirection;
    if (pGui->m_bCameraRelativeAreaTransition)
    {
        vDirection = g_pAppManager->m_pClientExoApp->GetModuleCamera()
                         ->GetOrientation().ydir();
        vDirection.m_fZ = 0.0f;
        vDirection = normalize(vDirection);
    }
    else
    {
        vDirection = pPlayer->m_vOrientation;
    }

    float fFacing = (m_vPosition - pPlayer->m_vPosition) * vDirection;
    if (fFacing <= 0.0f)
    {
        return;
    }

    if (GetModel(MODEL_SLOT_DEFAULT, 1) != NULL)
    {
        Quaternion qRotation;
        Vector vMinimum;
        Vector vMaximum;
        GetModel(MODEL_SLOT_DEFAULT, 1)->GetBoundingBox(vMinimum, vMaximum);
        Vector vForward(0.0f, 1.0f, 0.0f);
        vMinimum.m_fZ = 0.0f;
        vMinimum.m_fY = 0.0f;
        vMaximum.m_fZ = 0.0f;
        vMaximum.m_fY = 0.0f;
        qRotation = RotationArc(vForward, m_vOrientation);
        vMinimum = qRotation * vMinimum;
        vMaximum = qRotation * vMaximum;
        vMinimum = vMinimum + m_vPosition;
        vMaximum = vMaximum + m_vPosition;

        Vector vIntersection;
        Vector vStart = pPlayer->m_vPosition;
        Vector vEnd = pPlayer->m_vPosition + vDirection * 8.0f;
        vEnd.m_fZ = 0.0f;
        vStart.m_fZ = 0.0f;
        if (!IntersectLineSegments2d(vStart, vEnd, vMinimum, vMaximum,
                                     vIntersection))
        {
            return;
        }

        Gob* pPlayerModel = pPlayer->GetModel(254, 1);
        int bVisible = TRUE;
        if (pPlayerModel != NULL)
        {
            vIntersection.m_fZ = vStart.m_fZ =
                pPlayerModel->GetPosition().m_fZ;
        }
        else
        {
            vIntersection.m_fZ = vStart.m_fZ =
                pPlayer->m_vPosition.m_fZ + 1.0f;
        }

        CSWCArea* pArea = m_pArea;
        if (pArea != NULL)
        {
            CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
            CAurScene* pScene = pArea->m_pScene;
            CSWSArea* pServerArea = pArea->GetServerArea();
            cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
            if (pScene->HitCheck(vStart, vIntersection, NULL, &cHitInfo) ||
                (pServerArea != NULL &&
                 !pServerArea->NoNonWalkPolys(vStart, vIntersection,
                                              vStart.m_fZ, vStart.m_fZ, 0.0f,
                                              OBJECT_INVALID, NULL, 1)))
            {
                bVisible = FALSE;
            }
        }
        if (!bVisible)
        {
            return;
        }

        Vector vPrevious =
            pGui->GetAreaTranstionPosition() - pPlayer->m_vPosition;
        Vector vDelta = vStart - vIntersection;
        if (pGui->m_oidAreaTransition == OBJECT_INVALID ||
            vPrevious * vPrevious < vDelta * vDelta)
        {
            pGui->SetAreaTransition(m_nId, m_vPosition);
        }
    }
    else
    {
        Vector vDistance = m_vPosition - pPlayer->m_vPosition;
        float fDistanceSquared = vDistance * vDistance;
        if (fDistanceSquared <= 64.0f && fFacing >= 0.866)
        {
            Vector vPrevious =
                pGui->GetAreaTranstionPosition() - pPlayer->m_vPosition;
            if (pGui->m_oidAreaTransition == OBJECT_INVALID ||
                vPrevious * vPrevious > fDistanceSquared)
            {
                pGui->SetAreaTransition(m_nId, m_vPosition);
            }
        }
    }
}
// PC 0x00684CD0  Xbox 0x000C0F60
void CSWCDoor::AIUpdate()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nDeltaDay;
    unsigned long nDeltaTime;

    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime, &nDeltaDay,
        &nDeltaTime);
    m_nTimeDelta = nDeltaTime;

    if (m_nFeedbackTime != 0)
    {
        if (m_nFeedbackTime > m_nTimeDelta)
        {
            m_nFeedbackTime -= m_nTimeDelta;
        }
        else
        {
            m_nFeedbackTime = 0;
        }
    }

    UpdatePopUpBubble();
    AnimationUpdate();
    UpdateVisualEffectsList();
    UpdateFloatyText();
    UpdateAreaTransitionDisplay();

    if (FadeObject())
    {
        CClientAIMaster* pAIMaster =
            g_pAppManager->m_pClientExoApp->GetClientAIMaster();
        pAIMaster->m_aiList.RemoveObject(this);
        CGameObject* pGameObject;
        unsigned long nId = m_nId;
        pClient = g_pAppManager->m_pClientExoApp;
        if (pClient->GetGameObjectArray()->Delete(nId, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            delete this;
            return;
        }
    }

    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
}
