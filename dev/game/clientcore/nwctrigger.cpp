#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "worldtimer.h"
#include "nwsobject.h"
#include "aurmodel.h"
#include "aurtrigger.h"
#include "nwcanimbase.h"
#include "nwmessage.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "nwctrigger.h"
#include "nwcareaofeffectobject.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "swgui.h"
#include "gameobjectarray.h"
#include "aurcamera.h"
#include "auremitter.h"
#include "../../libsource/AURORA/aurpointer.h"
#include "guiingame.h"
#include "nwsarea.h"
#include <float.h>
// PC 0x006910D0  Xbox 0x000B92C0
int CSWCTrigger::LoadModel(const CResRef& cModel, unsigned char nModel,
                           unsigned char nFlags)
{
    if (m_pAnimBase == NULL)
    {
        m_pAnimBase = new CSWCAnimBaseTrigger;
        if (m_pAnimBase == NULL)
        {
            return 0;
        }
    }

    if (m_pAnimBase->LoadModel(cModel, nModel) == 0)
    {
        return 0;
    }

    Gob* pModel = static_cast<Gob*>(m_pAnimBase->GetModel(nModel));
    if (pModel != NULL)
    {
        pModel->SetExternalData(this);
        pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_TRIGGER);
    }

    return 1;
}
// PC 0x006825F0
void CSWCTrigger::UnloadModel()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x00646650
void CSWCTrigger::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x006911B0  Xbox 0x000B93A0
void CSWCTrigger::AddFireAndForgetAnimation(unsigned short nAnimation,
                                            void* pData, unsigned short nFlags)
{
    unsigned long nLength = (unsigned long)GetAnimationLength(nAnimation);
    InsertFireForgetAnimation(nAnimation, nLength, 1.0f, pData, 0);
}
// PC 0x006911E0  Xbox 0x000B93D0
CSWCTrigger::CSWCTrigger()
    : CSWCObject(CGameObject::OBJECT_TYPE_TRIGGER)
{
    unsigned long nDay;
    unsigned long nTime;
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    pApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_nNumVertices = 0;
    m_pvVertices = NULL;
    m_bTrap = 0;
    m_bAreaTransition = 0;
    m_bDisplayed = 0;
    m_fHighlightFade = 0.0f;
    m_nInitialValue = 0;
    m_pPartTrigger = NULL;
    m_cTriggerType = -1;
}
// PC 0x006912F0  Xbox 0x000B94E0
CSWCTrigger::~CSWCTrigger()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }

    if (m_pvVertices != NULL)
    {
        delete [] m_pvVertices;
        m_pvVertices = NULL;
    }

    if (m_pPartTrigger != NULL)
    {
        AurPartTriggerReleaseMemory(m_pPartTrigger);
        m_pPartTrigger = NULL;
    }
}
// PC 0x006913B0  Xbox 0x000B9590
void CSWCTrigger::UpdateTriggerColor(int nMode)
{
    if (m_bTrap != 0)
    {
        return;
    }
    Vector vColor;
    vColor.m_fZ = 1.0f;
    if (m_bAreaTransition != 0)
    {
        vColor.m_fX = 0.0f;
        vColor.m_fY = 0.0f;
        if (m_bDisplayed != 0)
        {
            AurPartTriggerSetHighlightParams(m_pPartTrigger, vColor, 0.25f,
                                             false, 0.0f, false);
        }
        else
        {
            AurPartTriggerSetHighlightParams(m_pPartTrigger, vColor, 0.0f,
                                             false, 0.0f, false);
        }
    }
    else
    {
        vColor.m_fY = 1.0f;
        vColor.m_fX = 0.3f;
        if (m_bDisplayed != 0)
        {
            AurPartTriggerSetHighlightParams(m_pPartTrigger, vColor, 0.25f,
                                             false, 0.0f, false);
        }
        else
        {
            AurPartTriggerSetHighlightParams(m_pPartTrigger, vColor, 0.0f,
                                             false, 0.0f, false);
        }
    }
    float fFade = m_fHighlightFade;
    AurPartTriggerSetHighlight(m_pPartTrigger, true, fFade, true);
}
// PC 0x00691470  Xbox 0x000B9670
void CSWCTrigger::DisplayTrigger(int bDisplay)
{
    if (bDisplay != m_bDisplayed)
    {
        m_bDisplayed = bDisplay;
        if (m_bTrap != 0)
        {
            if (m_bHostile != 0)
            {
                UpdateTriggerColor(0);
                return;
            }
        }
        else
        {
            if (m_bAreaTransition != 0)
            {
                UpdateTriggerColor(2);
                return;
            }
        }
        UpdateTriggerColor(1);
    }
}
// PC 0x006914D0  Xbox 0x000B96D0
void CSWCTrigger::SetCursor(unsigned char nTrigger)
{
    int nCursor;
    int bSuccess;
    {
        CExoString sColumn("CursorID");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        EXOXBOXASSERT(p2DArrays->m_pCursor);
        C2DA* pCursor = p2DArrays->m_pCursor;
        bSuccess = pCursor->GetINTEntry(nTrigger, sColumn, &nCursor);
    }
    if (bSuccess)
    {
        m_nCursor = (short)nCursor;
    }
}
// PC 0x00691560  Xbox 0x000B9770
void CSWCTrigger::UpdatePosition()
{
    Vector vMin = m_pvVertices[0];
    Vector vMax = m_pvVertices[0];

    for (int i = 1; i < m_nNumVertices; i++)
    {
        if (m_pvVertices[i].m_fX < vMin.m_fX)
        {
            vMin.m_fX = m_pvVertices[i].m_fX;
        }
        if (m_pvVertices[i].m_fY < vMin.m_fY)
        {
            vMin.m_fY = m_pvVertices[i].m_fY;
        }
        if (m_pvVertices[i].m_fZ < vMin.m_fZ)
        {
            vMin.m_fZ = m_pvVertices[i].m_fZ;
        }
        if (m_pvVertices[i].m_fX > vMax.m_fX)
        {
            vMax.m_fX = m_pvVertices[i].m_fX;
        }
        if (m_pvVertices[i].m_fY > vMax.m_fY)
        {
            vMax.m_fY = m_pvVertices[i].m_fY;
        }
        if (m_pvVertices[i].m_fZ > vMax.m_fZ)
        {
            vMax.m_fZ = m_pvVertices[i].m_fZ;
        }
    }
    Vector vSize(vMax.m_fX - vMin.m_fX, vMax.m_fY - vMin.m_fY,
                 vMax.m_fZ - vMin.m_fZ);
    Vector vHalf(0.5f * vSize.m_fX, 0.5f * vSize.m_fY, 0.5f * vSize.m_fZ);
    Vector vCenter(vHalf.m_fX + vMin.m_fX, vHalf.m_fY + vMin.m_fY,
                   vHalf.m_fZ + vMin.m_fZ);
    m_vPosition = vCenter;
}
// PC 0x006916B0  Xbox 0x000B98C0
unsigned char CSWCTrigger::GetCenter(Vector& vCenter)
{
    CAurCamera* pCamera = g_pAppManager->m_pClientExoApp->GetModuleCamera();
    int nResult = RETICLE_POINT_UNAVAILABLE;
    if (pCamera != NULL)
    {
        Vector vMin(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        Vector vCameraPosition = pCamera->GetPosition();
        Vector vView;
        vView = -pCamera->GetOrientation().zdir();
        for (int i = 0; i < m_nNumVertices; i++)
        {
            vMin.m_fX = min(vMin.m_fX, m_pvVertices[i].m_fX);
            vMin.m_fY = min(vMin.m_fY, m_pvVertices[i].m_fY);
            vMin.m_fZ = min(vMin.m_fZ, m_pvVertices[i].m_fZ);
            vMax.m_fX = max(vMax.m_fX, m_pvVertices[i].m_fX);
            vMax.m_fY = max(vMax.m_fY, m_pvVertices[i].m_fY);
            vMax.m_fZ = max(vMax.m_fZ, m_pvVertices[i].m_fZ);
        }
        Vector vPoint;
        vPoint = (vMax + vMin) / 2.0f;
        if (vView * vPoint > vView * vCameraPosition)
        {
            nResult = RETICLE_POINT_IN_FRONT;
        }
        else
        {
            nResult = RETICLE_POINT_BEHIND;
        }
        vPoint = pCamera->MapToScreen(vPoint, true);
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        vCenter.m_fX = pGuiManager->m_nWidth * vPoint.m_fX;
        vCenter.m_fY = pGuiManager->m_nHeight * vPoint.m_fY;
    }
    return nResult;
}
// PC 0x00691900  Xbox 0x000B9B70
void CSWCTrigger::ActionMenuDisableMine(unsigned long nAction,
                                        CSWCCreature* pCreature)
{
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();
    if (pMessage != NULL)
    {
        Vector vTarget;
        pMessage->SendPlayerToServerInput_UseSkill(CSWRules::SKILL_DEMOLITIONS, 0, m_nId, vTarget);
    }
}
// PC 0x00691950  Xbox 0x000B9BC0
void CSWCTrigger::ActionMenuRecoverMine(unsigned long nAction,
                                        CSWCCreature* pCreature)
{
    pCreature->PerformSkill(1, 0x65, GetId());
}
// PC 0x00691970  Xbox 0x000B9BE0
void CSWCTrigger::AnimationUpdate()
{
    unsigned short nAnimation;
    float fSpeed;
    unsigned long nLength;
    void* pData;

    if (AnimationFireAndForget(m_nCurrentAnimation) == 1 &&
        (m_nAnimationTimer -= m_nTimeDelta) < 0)
    {
        m_bFadeLocked = 1;
        m_nRemoveTimer = 500;
        m_nAnimationTimer = 0;
    }
    else
    {

        if (AnimationLooping(m_nCurrentAnimation) == 1)
        {
            if (GetTopFireForgetAnimation(&nAnimation, &fSpeed, &nLength,
                                          &pData) == 1)
            {
                m_nCurrentAnimation = nAnimation;
                m_pAnimBase->SetAnimation(nAnimation, fSpeed, 1, 0.0f);
                m_nAnimationTimer = nLength;
                RemoveTopFireForgetAnimation();
            }
            if (pData != NULL)
            {
                delete pData;
            }
        }
    }
}
// PC 0x00691A30  Xbox 0x000B9CA0
int CSWCTrigger::LineSegmentIntersectTrigger(const Vector& vStart,
                                             const Vector& vEnd, Vector* pvHit)
{
    CSWCArea* pArea = m_pArea;
    int i;
    int nNextPoint;
    float fIntersectionX;
    float fIntersectionY;

    if (pArea != NULL)
    {
        for (i = 0; i < m_nNumVertices; i++)
        {
            nNextPoint = i != m_nNumVertices - 1 ? i + 1 : 0;
            if (pArea->IntersectLineSegments(
                    vStart.m_fX, vStart.m_fY, vEnd.m_fX, vEnd.m_fY,
                    m_pvVertices[i].m_fX, m_pvVertices[i].m_fY,
                    m_pvVertices[nNextPoint].m_fX,
                    m_pvVertices[nNextPoint].m_fY, &fIntersectionY,
                    &fIntersectionX) == 1)
            {
                pvHit->m_fX = fIntersectionY;
                pvHit->m_fY = fIntersectionX;
                pvHit->m_fZ = 0.0f;
                return 1;
            }
        }
    }

    return 0;
}
// PC 0x00691B00  Xbox 0x000B9D80
unsigned char CSWCTrigger::GetReticlePoint(Vector& vPoint)
{
    int nResult;
    Gob* pModel = (Gob*)m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
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

            vPoint = g_pAppManager->m_pClientExoApp->GetModuleCamera()->MapToScreen(vPoint, true);
            vPoint.m_fX *=
                g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth;
            vPoint.m_fY *=
                g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight;
            goto done;
        }
    }

    nResult = GetCenter(vPoint);

done:
    return nResult;
}
// PC 0x00691CC0  Xbox 0x000B9FE0
void CSWCTrigger::AddToRenderList(int nMode)
{
    CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    CExoArrayList<unsigned long>* pTriggers = &pArea->m_lstRenderTriggerIds;
    pTriggers->Add(m_nId);

    CAurScene* pScene = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo->m_pScene;
    float afColor[4];
    memset(afColor, 0, sizeof(afColor));
    m_pPartTrigger = AurPartTriggerCreate(pScene, m_pvVertices,
                                         m_nNumVertices, TRUE, afColor, this);
    UpdateTriggerColor(nMode);
}
// PC 0x00691D80  Xbox 0x000BA0A0
void CSWCTrigger::RemoveFromRenderList()
{
    CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    unsigned long oidTrigger = m_nId;

    if (pArea->m_lstRenderTriggerIds.Contains(oidTrigger) != 0)
    {
        pArea->m_lstRenderTriggerIds.Remove(oidTrigger);
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        AurPartTriggerDelete(pModule->m_pInfo->m_pScene, m_pPartTrigger, false);
    }
}
// PC 0x00691E00  Xbox 0x000BA110
void CSWCTrigger::SpecialDisplayTrigger(int bDisplay)
{
    Vector vColor;
    if (m_bTrap != 0)
    {
        return;
    }
    float fAlpha;
    if (m_bAreaTransition != 0)
    {
        if (m_pPartTrigger == NULL)
        {
            AddToRenderList(CSWCTrigger::RENDER_AREA_TRANSITION);
        }
        vColor.m_fX = 0.0f;
        vColor.m_fY = 0.0f;
        vColor.m_fZ = 1.0f;
        fAlpha = 0.25f;
    }
    else
    {
        if (m_pPartTrigger == NULL)
        {
            AddToRenderList(CSWCTrigger::RENDER_TRIGGER);
        }
        vColor.m_fX = 1.0f;
        vColor.m_fY = 1.0f;
        vColor.m_fZ = 0.0f;
        fAlpha = 0.35f;
    }
    if (bDisplay == 0 && m_bDisplayed == 0)
    {
        fAlpha = 0.0f;
    }
    AurPartTriggerSetHighlightParams(m_pPartTrigger, vColor, fAlpha, false,
                                     0.0f, false);
    float fFade = m_fHighlightFade;
    AurPartTriggerSetHighlight(m_pPartTrigger, true, fFade, true);
}
// PC 0x00691F00  Xbox 0x000BA210
void CSWCTrigger::GetTargetActions(CSWCCreature* pCreature, int nMenu,
                                   CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    int nAction = lstActions.num;

    switch (nMenu)
    {
    case TARGET_ACTION_MENU_PRIMARY:
        if (pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS) &&
            IsHostile())
        {
            lstActions.SetSize(nAction + 1);

            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_DISABLE_MINE);
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCTrigger::ActionMenuDisableMine;
#ifdef _XBOX
            lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
            lstActions[nAction].m_oidTarget = m_nId;
#endif
            lstActions[nAction].m_nAction = 1012;
            lstActions[nAction].m_resIcon = "i_disablemine";
        }
        break;
    case TARGET_ACTION_MENU_SKILLS:
        if (pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS))
        {
            lstActions.SetSize(nAction + 1);

            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_RECOVER_MINE);
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCTrigger::ActionMenuRecoverMine;
#ifdef _XBOX
            lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
            lstActions[nAction].m_oidTarget = m_nId;
#endif
            lstActions[nAction].m_nAction = 1026;
            lstActions[nAction].m_resIcon = "i_recovermine";
        }
        break;
    }
}
// PC 0x006920B0  Xbox 0x000BA3C0
void CSWCTrigger::UpdateAreaTransitionDisplay()
{
    CGuiInGame* pGuiInGame = g_pAppManager->GetClientExoApp()->GetInGameGui();
    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (pGuiInGame == NULL || pPlayer == NULL || m_bAreaTransition == 0)
    {
        return;
    }

    Vector vHit;
    Vector vDirection;
    if (pGuiInGame->m_bCameraRelativeAreaTransition != 0)
    {
        CAurCamera* pCamera = g_pAppManager->GetClientExoApp()->GetModuleCamera();
        vDirection = pCamera->GetOrientation().ydir();
        vDirection.m_fZ = 0.0f;
        vDirection = normalize(vDirection);
    }
    else
    {
        vDirection = pPlayer->m_vOrientation;
    }

    Vector vStart = pPlayer->m_vPosition;
    Vector vEnd = pPlayer->m_vPosition + vDirection * 8.0f;
    vEnd.m_fZ = 0.0f;
    vStart.m_fZ = 0.0f;
    if (LineSegmentIntersectTrigger(vStart, vEnd, &vHit) == 0)
    {
        return;
    }

    Gob* pHead = pPlayer->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
    int bVisible = TRUE;
    if (pHead != NULL)
    {
        vHit.m_fZ = vStart.m_fZ = pHead->GetPosition().m_fZ;
    }
    else
    {
        vHit.m_fZ = vStart.m_fZ = pPlayer->m_vPosition.m_fZ + 1.0f;
    }

    CSWCArea* pArea = m_pArea;
    if (pArea != NULL)
    {
        CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
        CAurScene* pScene = pArea->m_pScene;
        CSWSArea* pServerArea = pArea->GetServerArea();
        cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
        if (pScene->HitCheck(vStart, vHit, NULL, &cHitInfo) ||
            (pServerArea != NULL &&
             !pServerArea->NoNonWalkPolys(vStart, vHit, vStart.m_fZ,
                                          vStart.m_fZ, 0.0f, OBJECT_INVALID,
                                          NULL, TRUE)))
        {
            bVisible = FALSE;
        }
    }
    if (!bVisible)
    {
        return;
    }

    Vector vToTransition =
        pGuiInGame->GetAreaTranstionPosition() - pPlayer->m_vPosition;
    Vector vToHit = vHit - vStart;
    if (pGuiInGame->m_oidAreaTransition == OBJECT_INVALID ||
        vToHit * vToHit < vToTransition * vToTransition)
    {
        pGuiInGame->SetAreaTransition(m_nId, vHit);
    }
}
// PC 0x00692410  Xbox 0x000BA710
void CSWCTrigger::AIUpdate()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nDeltaDay;
    unsigned long nDeltaTime;

    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime,
        &nDeltaDay, &nDeltaTime);

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
    m_nTimeDelta = nDeltaTime;

    if (m_bFadeLocked != 0)
    {
        if (m_nRemoveTimer <= m_nTimeDelta)
        {
            CGameObject* pGameObject;
            CGameObjectArray* pObjects =
                g_pAppManager->m_pClientExoApp->GetGameObjectArray();
            if (pObjects->Delete(
                    GetId(), &pGameObject) == CGameObjectArray::SUCCESS &&
                pGameObject != NULL)
            {
                delete this;
            }
        }
        else
        {
            m_nRemoveTimer -= m_nTimeDelta;
        }
    }
    else
    {
        UpdatePosition();
        UpdatePopUpBubble();
        UpdateAreaTransitionDisplay();
        AnimationUpdate();
        g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
        m_nLastUpdateDay = nDay;
        m_nLastUpdateTime = nTime;
    }
}
