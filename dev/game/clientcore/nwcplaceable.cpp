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
// PC 0x00682270  Xbox 0x000B73E0
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
// PC 0x006823F0  Xbox 0x000B7560
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
// PC 0x006825F0
void CSWCPlaceable::UnloadModel()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x0060E760
void CSWCPlaceable::HilightPlaceable(int)
{
}
// PC 0x00682620  Xbox 0x000B7760
void CSWCPlaceable::AddFireAndForgetAnimation(unsigned short nAnimation,
                                             void* pData, unsigned short nFlags)
{
    float fSpeed = m_fAnimationSpeed;
    unsigned long nLength =
        (unsigned long)(GetAnimationLength(nAnimation) / fSpeed);
    InsertFireForgetAnimation(nAnimation, nLength, fSpeed, pData, 0);
}
// PC 0x00682660  Xbox 0x000B77A0
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
// PC 0x006826A0  Xbox 0x000B77E0
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
// PC 0x006826E0  Xbox 0x000B7820
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
// PC 0x00682BE0  Xbox 0x000B7D30
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
// PC 0x00682C60  Xbox 0x000B7DB0
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
// PC 0x00646650
void CSWCPlaceable::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x00682DD0  Xbox 0x000B7F20
void CSWCPlaceable::ActionMenuSecurity(unsigned long oidUser,
                                       CSWCCreature* pCreature)
{
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    if (pMessage != NULL)
    {
        pMessage->SendPlayerToServerInput_UnlockObject(m_nId, OBJECT_INVALID);
    }
}
// PC 0x00682E00  Xbox 0x000B7F80
CSWSPlaceable* CSWCPlaceable::GetServerPlaceable()
{
    if (GetServerObject() != NULL)
    {
        return GetServerObject()->AsSWSPlaceable();
    }
    return NULL;
}
// PC 0x00682E20  Xbox 0x000B7FA0
void CSWCPlaceable::SetAppearance(unsigned char nAppearance)
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
// PC 0x00682F20  Xbox 0x000B80A0
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
            unsigned char nAppearance = pPlaceable->m_nAppearance;
            bFailed = !pPlaceables->GetFLOATEntry(nAppearance, sColumn, &fHeight);
        }
        if (bFailed)
        {
            return 1.5f;
        }
    }
    return fHeight;
}
// PC 0x00682FF0  Xbox 0x000B8160
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
// PC 0x006831B0  Xbox 0x000B83B0
inline int CSWCPlaceable::IsHostile()
{
    if (GetServerPlaceable() != NULL)
    {
        return GetServerPlaceable()->m_bHostile;
    }
    return 0;
}
// PC 0x00683200  Xbox 0x000B8400
CSWCPlaceable::~CSWCPlaceable()
{
    UnloadLight();
    UnloadModel();
}
// PC 0x006836C0  Xbox 0x000B88D0
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
// PC 0x006832A0  Xbox 0x000B84A0
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
                CSWSPlaceable* pPlaceable = GetServerPlaceable();
                CSWCArea* pArea = m_pArea;
                if (pPlaceable != NULL && pPlaceable->m_bIsBodyBag &&
                    !pPlaceable->m_bIsBodyBagVisible && pArea != NULL &&
                    pArea->ParentCorpseExists(GetId()))
                {
                    nSoundAppType = CTwoDimArrays::PLACEABLE_SOUND_BODY_BAG;
                }
                else if (!bSound)
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
// PC 0x00683630  Xbox 0x000B8840
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
// PC 0x006837D0  Xbox 0x000B8C70
void CSWCPlaceable::GetTargetActions(CSWCCreature* pCreature, int nMenu,
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
        if (pPlaceable != NULL && pPlaceable->m_bHasInventory && m_bLocked &&
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
        break;
    }
}
