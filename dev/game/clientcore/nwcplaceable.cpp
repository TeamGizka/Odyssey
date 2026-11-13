#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "worldtimer.h"
#include "clientaimaster.h"
#include "nwcanimbase.h"
#include "guiingame.h"
#include "nwcplaceable.h"
#include "nwsplaceable.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwccreature.h"
#include "aurcamera.h"
#include "auremitter.h"
#include "swgui.h"
#include "nwsarea.h"
#include "nwccreaturestats.h"
#include "nwskill.h"
#include "serverexoapp.h"
#include "nwsplayer.h"
#include "nwsitem.h"
#include "nwspell.h"
#include "nwscreature.h"
#include "nwitem.h"
// PC 0x006D9560 GOG 0x006D9250
CSWCPlaceable::CSWCPlaceable()
    : CSWCObject(CGameObject::OBJECT_TYPE_PLACEABLE)
{
    unsigned long nDay;
    unsigned long nTime;

    m_nCurrentAnimation = PLACEABLE_ANIMATION_CLOSED;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay,
                                                                  &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_bTrap = 0;
    m_bTrapDetected = 0;
    m_bPlotObject = 0;
    m_nUnknown114 = 0;
    m_bLocked = 0;
    m_bUseable = 0;
    m_nUnknown12C = 1;
    m_nUnknown130 = 1;
    m_nUnknown13C = 0;
    m_nUnknown120 = 0;
    m_nAppearance = -1;
    m_nUnknown134 = -1;
    m_nUnknown138 = 0;
    m_sName = "";
    m_sTag = "";
    m_sModel = "";
    m_pLight = NULL;
    m_nUnknown140 = 0;
}
// PC 0x006D96E0 GOG 0x006D93A0
void CSWCPlaceable::AnimationUpdate()
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
        }
    }
    if (AnimationLooping(m_nCurrentAnimation) == 1)
    {
        if (GetTopFireForgetAnimation(&nAnimation, &fSpeed, &nLength,
                                      &pData) == 1)
        {
            m_nCurrentAnimation = nAnimation;
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
                m_bOutOfPlace = 1;
                Vector vPosition;
                Quaternion qOrientation;
                m_pAnimBase->SetPosition(vPosition);
                m_pAnimBase->SetOrientation(qOrientation);
                m_pAnimBase->SetAlwaysRender(1);
                m_pAnimBase->SetInCutscene(1);
                m_bPlayedOutOfPlace = 1;
            }
            m_pAnimBase->SetAnimation(nAnimation, fSpeed, 1, 0.0f);
            m_nAnimationTimer = nLength;
            RemoveTopFireForgetAnimation();

            if (nAnimation == PLACEABLE_ANIMATION_CLIENT_OPEN ||
                nAnimation == PLACEABLE_ANIMATION_CLIENT_CLOSED ||
                nAnimation == PLACEABLE_ANIMATION_CLIENT_DESTROYED ||
                nAnimation == PLACEABLE_ANIMATION_CLIENT_ACTIVATE ||
                nAnimation == PLACEABLE_ANIMATION_CLIENT_USED)
            {
                CExoString sSound;
                int nSoundAppType;
                int bSound;
                {
                    int nAppearance = m_nAppearance;
                    CExoString sColumn("SoundAppType");
                    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
                    C2DA* pPlaceables = pArrays->m_pPlaceables;
                    bSound = pPlaceables->GetINTEntry(nAppearance, sColumn,
                                                      &nSoundAppType);
                }
                if (!bSound)
                {
                    return;
                }
                if (nAnimation == PLACEABLE_ANIMATION_CLIENT_OPEN)
                {
                    return;
                }
                else if (nAnimation == PLACEABLE_ANIMATION_CLIENT_CLOSED)
                {
                    CExoString sColumn("Closed");
                    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
                    C2DA* pSounds = pArrays->m_pPlaceableSounds;
                    bSound = pSounds->GetCExoStringEntry(nSoundAppType, sColumn,
                                                         &sSound);
                }
                else if (nAnimation == PLACEABLE_ANIMATION_CLIENT_DESTROYED)
                {
                    CExoString sColumn("Destroyed");
                    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
                    C2DA* pSounds = pArrays->m_pPlaceableSounds;
                    bSound = pSounds->GetCExoStringEntry(nSoundAppType, sColumn,
                                                         &sSound);
                }
                else if (nAnimation == PLACEABLE_ANIMATION_CLIENT_ACTIVATE ||
                         nAnimation == PLACEABLE_ANIMATION_CLIENT_USED)
                {
                    CExoString sColumn("Used");
                    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
                    C2DA* pSounds = pArrays->m_pPlaceableSounds;
                    bSound = pSounds->GetCExoStringEntry(nSoundAppType, sColumn,
                                                         &sSound);
                }
                else
                {
                    return;
                }
                if (bSound)
                {
                    CResRef cSound(sSound);
                    g_pExoSound->Play3DOneShotSound(cSound, m_vPosition, 0.0f,
                        CExoSound::PRIORITY_GROUP_PLACEABLE, 0, 0, 0.0f, 0.0f);
                }
            }
        }
    }
}
// PC 0x006D9A20 GOG 0x006D96E0
int CSWCPlaceable::LoadModel(const CResRef& cModel, unsigned char nModel,
                             unsigned char nFlags)
{
    if (m_pAnimBase == NULL)
    {
        m_pAnimBase = new CSWCAnimBasePlaceable;
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
        pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_PLACEABLE);
    }

    cModel.CopyToString(m_sModel);
    m_sModel = m_sModel.SubString(4, -1);

    Quaternion qOrientation;
    Vector vPosition;
    CExoString sPart;
    sPart = m_sModel + "_head_hit";
    if (pModel->GetPartLocalPosition((char*)sPart.CStr(), &vPosition,
                                     &qOrientation))
    {
        m_fHeadHitHeight = vPosition.m_fZ;
    }
    else
    {
        m_fHeadHitHeight = 1.5f;
    }

    return 1;
}
// PC 0x006DB7A0 GOG 0x0069CD00
void CSWCPlaceable::UnloadModel()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x005EE1C0 GOG 0x005EE1A0
void CSWCPlaceable::HilightPlaceable(int)
{
}
// PC 0x006D9C20 GOG 0x006D98E0
void CSWCPlaceable::AddFireAndForgetAnimation(unsigned short nAnimation,
                                             void* pData, unsigned short nFlags)
{
    float fSpeed = m_fAnimationSpeed;
    unsigned long nLength =
        (unsigned long)(GetAnimationLength(nAnimation) / fSpeed);
    InsertFireForgetAnimation(nAnimation, nLength, fSpeed, pData, 0);
}
// PC 0x006D9C60 GOG 0x006D9920
void CSWCPlaceable::UsePlaceable(unsigned long oidUser, CSWCCreature* pCreature)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    CGuiInGame* pInGame =
        (CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pInGame->GetCanClick())
    {
        pMessage->SendPlayerToServerInput_UseObject(m_nId);
    }
}
// PC 0x006D9CA0 GOG 0x006D9960
void CSWCPlaceable::BashPlaceable(unsigned long oidUser,
                                  CSWCCreature* pCreature)
{
    if (pCreature != NULL)
    {
        unsigned long oidTarget = m_nId;
        unsigned long oidCreature = pCreature->m_nId;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        if (pClientExoApp->ShowTutorialWindow(
                CClientExoApp::TUTORIAL_BASH, oidCreature, oidTarget,
                                              0) == 0)
        {
            pCreature->PerformAttack(m_nId);
        }
    }
}
// PC 0x006D9CE0 GOG 0x006D99A0
int CSWCPlaceable::LoadLight()
{

    Vector vColor;
    CExoString sLightNode;
    CExoString sCommand;
    int nLightColor;
    bool bFailed;
    {
        CExoString sColumn("LightColor");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetINTEntry(m_nAppearance, sColumn,
                                            &nLightColor);
    }
    if (bFailed)
    {
        return TRUE;
    }

    CSWCArea* pArea = NULL;
    if (g_pAppManager->GetClientExoApp()->GetModule() != NULL)
    {
        pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
        if (pArea != NULL)
        {
            m_pLight = pArea->GetPlaceableObjectLight(GetId());
        }
    }
    {
        CExoString sColumn("LightNode");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetCExoStringEntry(m_nAppearance, sColumn,
                                                   &sLightNode);
    }
    if (bFailed)
    {
        sLightNode = "impact";
    }
    Gob* pModel = GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
    if (pModel == NULL)
    {
        return FALSE;
    }
    if (m_pLight == NULL)
    {
        Quaternion qOrientation;
        Vector vRootPosition;
        Vector vOffset;
        m_pLight = static_cast<Gob*>(
            NewCAurObject("fx_placeable01", "placeablelight", NULL, NULL));
        if (m_pLight == NULL)
        {
            return FALSE;
        }
        {
            CExoString sColumn("RED");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLightColor = p2DArrays->m_pLightColor;
            pLightColor->GetFLOATEntry(nLightColor, sColumn, &vColor.m_fX);
        }
        {
            CExoString sColumn("GREEN");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLightColor = p2DArrays->m_pLightColor;
            pLightColor->GetFLOATEntry(nLightColor, sColumn, &vColor.m_fY);
        }
        {
            CExoString sColumn("BLUE");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLightColor = p2DArrays->m_pLightColor;
            pLightColor->GetFLOATEntry(nLightColor, sColumn, &vColor.m_fZ);
        }
        {
            CExoString sColumn("LightOffsetX");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            bFailed = !pPlaceables->GetFLOATEntry(m_nAppearance, sColumn,
                                                  &vOffset.m_fX);
        }
        if (bFailed)
        {
            vOffset.m_fX = 0.0f;
        }
        {
            CExoString sColumn("LightOffsetY");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            bFailed = !pPlaceables->GetFLOATEntry(m_nAppearance, sColumn,
                                                  &vOffset.m_fY);
        }
        if (bFailed)
        {
            vOffset.m_fY = 0.0f;
        }
        {
            CExoString sColumn("LightOffsetZ");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            bFailed = !pPlaceables->GetFLOATEntry(m_nAppearance, sColumn,
                                                  &vOffset.m_fZ);
        }
        if (bFailed)
        {
            vOffset.m_fZ = 3.0f;
        }
        m_pLight->GetPartLocalPosition("root", &vRootPosition, &qOrientation);
        m_pLight->SetPartLocalPosition("root", vRootPosition + vOffset,
                                       qOrientation);
    }
    m_pLight->AttachToObject(pModel, "root", Gob::ATTACH_NORMAL);
    if (pArea != NULL)
    {
        pArea->RemovePlaceableObjectLight(GetId(), 0);
    }
    return TRUE;
}
// PC 0x006DA1E0 GOG 0x006D9EA0
void CSWCPlaceable::UnloadLight()
{
    if (g_pAppManager->GetClientExoApp()->GetModule() != NULL)
    {
        CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
        if (pModule->m_pInfo != NULL)
        {
            pModule->m_pInfo->RemovePlaceableObjectLight(m_nId, 0);
        }
    }
    if (m_pLight != NULL)
    {
        m_pLight->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
        m_pLight->AttachToScene(NULL);
        delete m_pLight;
        m_pLight = NULL;
    }
}
// PC 0x006DA260 GOG 0x006D9F20
int CSWCPlaceable::ApplyShadowBlob()
{
    CExoString sShadowModel;
    if (m_pAnimBase == NULL)
    {
        return 0;
    }

    int nShadowSize;
    bool bFailed;
    {
        CExoString sColumn("ShadowSize");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetINTEntry(m_nAppearance, sColumn,
                                            &nShadowSize);
    }
    if (bFailed)
    {
        return 1;
    }

    switch (nShadowSize)
    {
    case 0:
        sShadowModel = "Fx_Shadow_S";
        break;
    case 1:
        sShadowModel = "Fx_Shadow_M";
        break;
    case 2:
        sShadowModel = "Fx_Shadow_L";
        break;
    default:
        sShadowModel = "Fx_Shadow_M";
        break;
    }

    if (m_bShadowsVisible)
    {
        m_pAnimBase->TurnOffShadows();
    }
    int bResult = m_pAnimBase->ApplyShadowBlob(sShadowModel);
    if (!m_bShadowsVisible)
    {
        HideShadowBlob();
    }
    return bResult;
}
// PC 0x0069CF50 GOG 0x0069CD30
void CSWCPlaceable::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x006DA3D0 GOG 0x006DA090
void CSWCPlaceable::ActionMenuSecurity(unsigned long oidUser,
                                       CSWCCreature* pCreature)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_UnlockObject(m_nId, OBJECT_INVALID);
    }
}
#ifndef _XBOX
// PC 0x006DA400 GOG 0x006DA0C0
void CSWCPlaceable::MenuActionUseItem(unsigned long oidItem, CSWCCreature* pCreature)
{
    pCreature->PerformUseItem(oidItem & ~OBJECT_LISTTYPE_MASK, 0, m_nId, m_vPosition);
}
// PC 0x006DA430 GOG 0x006DB450
void CSWCPlaceable::MenuActionSetPlaceableMine(unsigned long oidItem, CSWCCreature* pCreature)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pPlayer =
        pServerExoApp->GetCreatureByGameObjectID(pServerExoApp->GetPlayerCreatureId());
    if (pPlayer != NULL && pPlayer->GetArea() != NULL &&
        !pPlayer->GetArea()->GetPlayerCanSetMines())
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(STRREF_CANNOT_SET_MINES);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
            pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }
    oidItem &= ~OBJECT_LISTTYPE_MASK;
    if (g_pAppManager->GetClientExoApp()->ShowTutorialWindow(TUTORIAL_SET_MINE, GetId(), oidItem, 0) == 0)
    {
        pCreature->PerformUseItem(oidItem, 0, GetId(), m_vPosition);
    }
}
#endif
// PC 0x006DA500 GOG 0x006DA0F0
CSWSPlaceable* CSWCPlaceable::GetServerPlaceable()
{
    if (GetServerObject() != NULL)
    {
        return GetServerObject()->AsSWSPlaceable();
    }
    return NULL;
}
// PC 0x006DA520 GOG 0x006DA110
void CSWCPlaceable::SetAppearance(int nAppearance)
{
    m_nAppearance = nAppearance;
    int bHitCheck = 0;
    {
        CExoString sColumn("HitCheck");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        pPlaceables->GetINTEntry(nAppearance, sColumn, &bHitCheck);
    }

    Gob* pModel = GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
    if (pModel != NULL)
    {
        pModel->SetIgnoreHitCheck(bHitCheck == 0);
    }

    if (m_pAnimBase != NULL)
    {
        int bNonCull;
        CExoString sColumn("NonCull");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        pPlaceables->GetINTEntry(nAppearance, sColumn, &bNonCull);
    }
}
// PC 0x006DA620 GOG 0x006DA210
float CSWCPlaceable::GetCanSeeHeight()
{
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pPlaceables = p2DArrays->m_pPlaceables;
    float fHeight = 1.5f;
    CSWSPlaceable* pPlaceable = GetServerPlaceable();
    if (pPlaceable != NULL)
    {
        bool bFailed;
        {
            CExoString sColumn("CanSeeHeight");
            unsigned long nAppearance = pPlaceable->m_nAppearance;
            bFailed = !pPlaceables->GetFLOATEntry(nAppearance, sColumn, &fHeight);
        }
        if (bFailed)
        {
            return 1.5f;
        }
    }
    return fHeight;
}
// PC 0x006DA6F0 GOG 0x006DA2E0
unsigned char CSWCPlaceable::GetReticlePoint(Vector& vPoint)
{
    CSWSPlaceable* pPlaceable = GetServerPlaceable();
    if (pPlaceable != NULL && pPlaceable->m_bIsCorpse != 0)
    {
        Gob* pModel = (Gob*)m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
        if (pModel != NULL)
        {
            Part* pRoot = pModel->GetPart("rootdummy");
            if (pRoot != NULL)
            {
                Quaternion qOrientation;
                pRoot->GetWorld(vPoint, qOrientation);

                CAurCamera* pCamera =
                    g_pAppManager->m_pClientExoApp->GetModuleCamera();
                Vector vView = -pCamera->GetOrientation().zdir();

                unsigned char nResult;
                if (vView * vPoint > vView * pCamera->GetPosition())
                {
                    nResult = 1;
                }
                else
                {
                    nResult = 2;
                }

                vPoint = pCamera->MapToScreen(vPoint, true);
                vPoint.m_fX *=
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth;
                vPoint.m_fY *=
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight;
                return nResult;
            }
        }
    }

    return CSWCObject::GetReticlePoint(vPoint);
}
// PC 0x006DA8B0 GOG 0x006DA4A0
inline int CSWCPlaceable::IsHostile()
{
    if (GetServerPlaceable() != NULL)
    {
        return GetServerPlaceable()->m_bHostile;
    }
    return 0;
}
// PC 0x006DA900 GOG 0x006DA4F0
CSWCPlaceable::~CSWCPlaceable()
{
    UnloadLight();
    UnloadModel();
}
// PC 0x006DA9A0 GOG 0x006DA590
void CSWCPlaceable::AIUpdate()
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
// PC 0x006DAAB0 GOG 0x006DA6A0
int CSWCPlaceable::SetAnimation(unsigned short nAnimation, int nA, int nB)
{
    if (GetServerObject() != NULL)
    {
        CSWSPlaceable* pPlaceable = GetServerObject()->AsSWSPlaceable();
        if (pPlaceable != NULL && pPlaceable->m_bIsCorpse != 0)
        {
            return 1;
        }
    }

    if (nAnimation == 306 || nAnimation == 10072)
    {
        UnloadLight();
    }

    return CSWCObject::SetAnimation(nAnimation, nA, nB);
}
#ifndef _XBOX
static inline int GetActionCount(const CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    return lstActions.num;
}
// PC 0x006DAB40 GOG 0x006DA730
void CSWCPlaceable::GetUseItems(CSWCCreature* pCreature,
                                CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    CSWSPlaceable* pPlaceable = GetServerObject() != NULL ?
        GetServerObject()->AsSWSPlaceable() : NULL;
    int nFirstAction = lstActions.num;
    if (m_bLocked)
    {
        if (pServerCreature != NULL &&
            pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY))
            pServerCreature->GetSpikeDetails(lstActions, 1);
        for (int i = nFirstAction; i < GetActionCount(lstActions); ++i)
        {
            lstActions[i].m_pfnAction =
                (PFNMENUACTION)&CSWCObject::MenuActionUseSecuritySpike;
            lstActions[i].m_oidTarget = GetId();
        }
    }
    CItemRepository* pRepository = pServerCreature->GetItemRepository(1);
    CExoString sName;
    int nAction = 0;
    int nLanguage = g_pAppManager->GetClientExoApp()->GetClientLanguage();
    for (unsigned long i = 0; i < pRepository->m_lstItems.num; ++i)
    {
        CSWItemProperty* pProperty;
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL && pItem->m_bPlotObject &&
            pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
        {
            int nSubtype = pProperty->m_nSubtype;
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSubtype);
            if (pSpell != NULL)
            {
                unsigned char nTargeting = pSpell->m_nItemTargeting;
                if (nTargeting == CSWSpell::ITEM_TARGETING_PLACEABLE)
                {
                    lstActions.SetSize(nAction + 1);
                    pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
                    lstActions[nAction].m_sName = sName;
                    lstActions[nAction].m_pfnAction =
                        (PFNMENUACTION)&CSWCPlaceable::MenuActionUseItem;
                    lstActions[nAction].m_oidTarget = GetId();
                    lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
                    lstActions[nAction].m_resIcon = pItem->GetIcon();
                    lstActions[nAction].m_nStackSize = pItem->m_nStackSize > pItem->m_nCharges ?
                        pItem->m_nStackSize : pItem->m_nCharges;
                    nAction++;
                }
            }
        }
    }
}
// PC 0x006DADA0 GOG 0x006DA990
void CSWCPlaceable::GetPlaceableInterfaceMines(CSWCCreature* pCreature,
                                               CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    int nFlag11C = m_n11C;
    if (nFlag11C)
    {
        return;
    }
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    if (pServerCreature == NULL || pServerCreature->GetArea()->m_nPlayerRestrictMode ||
        !pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS))
    {
        return;
    }
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    int nAction = lstActions.num;
    int nLanguage = pClientExoApp->GetClientLanguage();
    CItemRepository* pRepository = pServerCreature->GetItemRepository(1);
    CExoString sName;
    CExoString sCount;
    CExoString sSetMine;
    sSetMine.Format(" (%s)", pClientExoApp->GetGUIString(STRREF_LOCK));
    for (unsigned long i = 0; i < pRepository->m_lstItems.num; ++i)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem->m_bPlotObject)
        {
            continue;
        }
        CSWBaseItem* pBaseItem = pItem->GetBaseItem();
        if (pBaseItem->m_nItemType != CSWBaseItem::ITEM_TYPE_MINE)
        {
            continue;
        }
        switch (pItem->GetActiveProperty(0)->m_nSubtype)
        {
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 14:
        case 15:
        case 16:
        case 19:
        case 20:
        case 21:
        case 22:
        case 25:
        case 26:
            pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
            if (pItem->m_nStackSize > 1)
            {
                sCount.Format(" (%d)", pItem->m_nStackSize);
            }
            else
            {
                sCount = "";
            }
            sName = sName + sSetMine + sCount;
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName = sName;
            lstActions[nAction].m_pfnAction =
                (PFNMENUACTION)&CSWCPlaceable::MenuActionSetPlaceableMine;
            lstActions[nAction].m_oidTarget = GetId();
            lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
            lstActions[nAction].m_resIcon = pItem->GetIcon();
            nAction++;
            break;
        }
    }
}
void CSWCPlaceable::GetTargetActions(CSWCCreature* pCreature, int nMenu,  // PC 0x006DB0C0 GOG 0x006DACB0
                                    CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    CSWSPlaceable* pPlaceable = GetServerPlaceable();
    int nRestrictMode = m_pArea->GetServerArea()->m_nPlayerRestrictMode;
    int nAction = lstActions.num;
    CExoString sAction;

    switch (nMenu)
    {
    case TARGET_ACTION_MENU_PRIMARY:
        if (pPlaceable != NULL && pPlaceable->m_bHasInventory)
        {
            if (!m_bPlotObject && !nRestrictMode && m_bLocked)
            {
                lstActions.SetSize(nAction + 1);
                lstActions[nAction].m_sName =
                    g_pAppManager->GetClientExoApp()->GetGUIString(
                        CClientExoApp::GUI_STRING_BASH);
                lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
                lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::BashPlaceable;
#ifdef _XBOX
                lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
                lstActions[nAction].m_oidTarget = GetId();
#endif
                lstActions[nAction].m_resIcon = "i_attack";

                unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
                unsigned long oidPCObject = pPlayer->m_oidPCObject;
                unsigned long oidCreature = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidPCObject);
                CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidCreature);
                if (pObject != NULL && pObject->AsSWCCreature() != NULL)
                {
                    CSWCCreature* pPlayerCreature = pObject->AsSWCCreature();
                    if (pCreature->GetId() != oidCreature && pPlayerCreature->m_bInParty &&
                        pPlayerCreature->GetServerCreature() != NULL &&
                        pPlayerCreature->GetServerCreature()->GetDeadTemp() &&
                        GetServerPlaceable() != NULL && !GetServerPlaceable()->m_bPartyInteract)
                    {
                        lstActions[nAction].m_bEnabled = 0;
                        lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_DEAD;
                    }
                }
            }
        }
        else if (!m_bPlotObject && !nRestrictMode && m_bLocked)
        {
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(
                    CClientExoApp::GUI_STRING_BASH);
            lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::BashPlaceable;
#ifdef _XBOX
            lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
            lstActions[nAction].m_oidTarget = GetId();
#endif
            lstActions[nAction].m_resIcon = "i_attack";
        }
        break;
    case TARGET_ACTION_MENU_SKILLS:
    {
        if (pPlaceable == NULL)
        {
            break;
        }
        int bHasInventory = pPlaceable->m_bHasInventory;
        if (bHasInventory && m_bLocked &&
            pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY))
        {
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(
                    CClientExoApp::GUI_STRING_SECURITY);
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::ActionMenuSecurity;
#ifdef _XBOX
            lstActions[nAction].m_oidTarget = (unsigned long)this;
#else
            lstActions[nAction].m_oidTarget = GetId();
#endif
            lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_SECURITY;
            lstActions[nAction].m_resIcon = g_pRules->m_pSkills[CSWRules::SKILL_SECURITY].m_sIcon;
        }
        if (pPlaceable->m_bHasInventory && m_bLocked &&
            pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY))
        {
            GetUseItems(pCreature, lstActions);
        }
        break;
    }
    case TARGET_ACTION_MENU_MINES:
        if (m_bLocked)
        {
            GetPlaceableInterfaceMines(pCreature, lstActions);
        }
        break;
    }
}
#endif

#ifdef _XBOX
#include "nwsitem.h"
#include "nwspell.h"
#include "nwsplayer.h"
#include "nwscreature.h"

enum { XBOX_TARGET_ACTION_MENU_USE_ITEMS = 5 };

enum { XBOX_ITEM_TARGETING_PLACEABLE = 2 };

void CSWCPlaceable::MenuActionUseItem(unsigned long oidItem, CSWCCreature* pCreature)
{
    pCreature->PerformUseItem(oidItem & ~OBJECT_LISTTYPE_MASK, 0, m_nId, m_vPosition);
}

void CSWCPlaceable::GetUseItems(CSWCCreature* pCreature,
                                CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    CSWSPlaceable* pPlaceable = GetServerObject() != NULL ?
        GetServerObject()->AsSWSPlaceable() : NULL;
    int nFirstAction = lstActions.num;
    if (m_bLocked)
    {
        if (pServerCreature != NULL &&
            pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY))
            pServerCreature->GetSpikeDetails(lstActions, 1);
        for (int i = nFirstAction; i < lstActions.num; ++i)
        {
            lstActions[i].m_pfnAction =
                (PFNMENUACTION)&CSWCObject::MenuActionUseSecuritySpike;
            lstActions[i].m_oidTarget = (unsigned long)this;
        }
    }
    CItemRepository* pRepository = pServerCreature->GetItemRepository(1);
    CExoString sName;
    int nAction = 0;
    int nLanguage = g_pAppManager->GetClientExoApp()->GetClientLanguage();
    for (unsigned long i = 0; i < pRepository->m_lstItems.num; ++i)
    {
        CSWItemProperty* pProperty;
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL && pItem->m_bPlotObject &&
            pItem->GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
        {
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(pProperty->m_nSubtype);
            if (pSpell != NULL && pSpell->m_nItemTargeting == XBOX_ITEM_TARGETING_PLACEABLE)
            {
                lstActions.SetSize(nAction + 1);
                pItem->m_sLocalizedName.GetString(nLanguage, &sName, 0);
                if (pItem->m_nStackSize > 1)
                    sName.Format("%s (%d)", sName.CStr(), pItem->m_nStackSize);
                else if (pItem->m_nCharges > 1)
                    sName.Format("%s (%d)", sName.CStr(), pItem->m_nCharges);
                lstActions[nAction].m_sName = sName;
                lstActions[nAction].m_pfnAction =
                    (PFNMENUACTION)&CSWCPlaceable::MenuActionUseItem;
                lstActions[nAction].m_oidTarget = (unsigned long)this;
                lstActions[nAction].m_nAction = pItem->GetId() | OBJECT_LISTTYPE_MASK;
                lstActions[nAction].m_resIcon = pItem->GetIcon();
                nAction++;
            }
        }
    }
}

void CSWCPlaceable::GetTargetActions(CSWCCreature* pCreature, int nMenu,
                                    CExoArrayList<CSWGuiInterfaceAction>& lstActions)
{
    CSWSPlaceable* pPlaceable = GetServerObject() != NULL ?
        GetServerObject()->AsSWSPlaceable() : NULL;
    int nRestrictMode = m_pArea->GetServerArea()->m_nPlayerRestrictMode;
    int nAction = lstActions.num;
    CExoString sAction;

    switch (nMenu)
    {
    case TARGET_ACTION_MENU_PRIMARY:
        if (pPlaceable != NULL && pPlaceable->m_bHasInventory)
        {
            if (m_bUseable)
            {
                lstActions.SetSize(nAction + 1);
                lstActions[nAction].m_sName =
                    g_pAppManager->GetClientExoApp()->GetGUIString(
                        CClientExoApp::GUI_STRING_OPEN);
                lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_USE_PLACEABLE;
                lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::UsePlaceable;
                lstActions[nAction].m_oidTarget = (unsigned long)this;
                lstActions[nAction].m_resIcon = "i_openplace";

                unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
                unsigned long oidPCObject = pPlayer->m_oidPCObject;
                unsigned long oidCreature = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidPCObject);
                CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidCreature);
                if (pObject != NULL && pObject->AsSWCCreature() != NULL)
                {
                    CSWCCreature* pPlayerCreature = pObject->AsSWCCreature();
                    if (pCreature->GetId() != oidCreature && pPlayerCreature->m_bInParty &&
                        pPlayerCreature->GetServerCreature() != NULL &&
                        pPlayerCreature->GetServerCreature()->GetDeadTemp() &&
                        GetServerPlaceable() != NULL && !GetServerPlaceable()->m_bPartyInteract)
                    {
                        lstActions[nAction].m_bEnabled = 0;
                        lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_DEAD;
                    }
                }
                nAction++;
            }
            if (!m_bPlotObject && !nRestrictMode && m_bLocked)
            {
                lstActions.SetSize(nAction + 1);
                lstActions[nAction].m_sName =
                    g_pAppManager->GetClientExoApp()->GetGUIString(
                        CClientExoApp::GUI_STRING_BASH);
                lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
                lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::BashPlaceable;
                lstActions[nAction].m_oidTarget = (unsigned long)this;
                lstActions[nAction].m_resIcon = "i_attack";

                unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
                unsigned long oidPCObject = pPlayer->m_oidPCObject;
                unsigned long oidCreature = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidPCObject);
                CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidCreature);
                if (pObject != NULL && pObject->AsSWCCreature() != NULL)
                {
                    CSWCCreature* pPlayerCreature = pObject->AsSWCCreature();
                    if (pCreature->GetId() != oidCreature && pPlayerCreature->m_bInParty &&
                        pPlayerCreature->GetServerCreature() != NULL &&
                        pPlayerCreature->GetServerCreature()->GetDeadTemp() &&
                        GetServerPlaceable() != NULL && !GetServerPlaceable()->m_bPartyInteract)
                    {
                        lstActions[nAction].m_bEnabled = 0;
                        lstActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_DEAD;
                    }
                }
            }
        }
        else
        {
            if (m_bUseable && pPlaceable != NULL && !pPlaceable->m_bIsCorpse)
            {
                lstActions.SetSize(nAction + 1);
                lstActions[nAction].m_sName =
                    g_pAppManager->GetClientExoApp()->GetGUIString(
                        CClientExoApp::GUI_STRING_USE);
                lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_USE_PLACEABLE;
                lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::UsePlaceable;
                lstActions[nAction].m_oidTarget = (unsigned long)this;
                lstActions[nAction].m_resIcon = "i_useplace";
                nAction++;
            }
            if (!m_bPlotObject && !nRestrictMode)
            {
                lstActions.SetSize(nAction + 1);
                lstActions[nAction].m_sName =
                    g_pAppManager->GetClientExoApp()->GetGUIString(
                        CClientExoApp::GUI_STRING_BASH);
                lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
                lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::BashPlaceable;
                lstActions[nAction].m_oidTarget = (unsigned long)this;
                lstActions[nAction].m_resIcon = "i_attack";
            }
        }
        break;
    case TARGET_ACTION_MENU_SKILLS:
        if (pPlaceable != NULL && pPlaceable->m_bHasInventory && m_bLocked &&
            pCreature->m_pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY))
        {
            lstActions.SetSize(nAction + 1);
            lstActions[nAction].m_sName =
                g_pAppManager->GetClientExoApp()->GetGUIString(
                    CClientExoApp::GUI_STRING_SECURITY);
            lstActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::ActionMenuSecurity;
            lstActions[nAction].m_oidTarget = (unsigned long)this;
            lstActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_SECURITY;
            lstActions[nAction].m_resIcon = g_pRules->m_pSkills[CSWRules::SKILL_SECURITY].m_sIcon;
        }
        break;
    case XBOX_TARGET_ACTION_MENU_USE_ITEMS:
        if (pPlaceable != NULL && pPlaceable->m_bHasInventory)
            GetUseItems(pCreature, lstActions);
        break;
    }
}
#endif
