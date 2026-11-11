#include <stdio.h>
#include "../../libsource/AURORA/aurpointer.h"

#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "aurmodel.h"
#include "gameobjectarray.h"
#include "clientaimaster.h"
#include "worldtimer.h"
#include "exobase.h"
#include "nwcanimbase.h"
#include "nwcvisualeffect.h"
#include "nwcanimbase.h"
#include "clientoptions.h"
#include "nwcmodule.h"
#include "nwsobject.h"
#include "nwcanimbase.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwccreature.h"
#include "nwcarea.h"
#include "aurscene.h"
#include "nwrules.h"

float weirdanimspeed = 0.8f;
CExoLinkedList<CAurObject> CSWCVisualEffect::m_lstCachedVFX;
// PC 0x0068FE70  Xbox 0x000B3380
CSWCVisualEffect::CSWCVisualEffect(int bAddToGameObjectArray)
    : CSWCObject(SW_OBJECT_TYPE_AREA_OF_EFFECT)
{
    unsigned long nDay;
    unsigned long nTime;
    CClientExoApp* pExoApp;

    pExoApp = g_pAppManager->m_pClientExoApp;
    pExoApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_nDuration = 0;
    m_bDurationActive = 0;
    m_bFinished = 0;
    m_bOneShot = 0;
    m_bDeleteWhenFinished = 1;
    m_nAnimationDelay = 0;
    m_nAnimation = 0;
    if (bAddToGameObjectArray != 0)
    {
        pExoApp = g_pAppManager->m_pClientExoApp;
        pExoApp->GetGameObjectArray()->AddInternalObject(
            m_nId, this, 0);
        pExoApp = g_pAppManager->m_pClientExoApp;
        pExoApp->GetClientAIMaster()->AddObject(this);
    }
    m_pSoundSource = NULL;
    m_bGroundEffect = 0;
    m_bAttachWhenFinished = 0;
    m_bNoExpirationDelay = 0;
    m_bShadows = 0;
    m_nEffect = INVALID_EFFECT;
}
// PC 0x00690980  Xbox 0x000B4040
void CSWCVisualEffect::AIUpdate()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nSubDay;
    unsigned long nSubTime;
    CClientExoApp* pExoApp;

    if (m_bFinished != 0)
    {
        return;
    }
    pExoApp = g_pAppManager->m_pClientExoApp;
    pExoApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pExoApp = g_pAppManager->m_pClientExoApp;
    pExoApp->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime, &nSubDay, &nSubTime);
    m_nTimeDelta = nSubTime;
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    if (m_pSoundSource != NULL)
    {
        m_pSoundSource->SetPosition(m_vPosition, 1.5f);
    }
    if (m_nAnimationDelay > 0)
    {
        if (m_nAnimationDelay > m_nTimeDelta)
        {
            m_nAnimationDelay -= m_nTimeDelta;
        }
        else
        {
            m_nAnimationDelay = 0;
            if (m_nAnimation == 1)
            {
                if (m_bOneShot != 0)
                {
                    m_nDuration = m_bNoExpirationDelay != 0 ? 0 : 3000;
                    m_bDurationActive = 1;
                }
                else
                {
                    SetAnimation(2);
                }
            }
            else if (m_nAnimation == 3)
            {
                m_nDuration = 3000;
                m_bDurationActive = 1;
            }
        }
    }
    if (m_bDurationActive != 0)
    {
        if (m_nDuration <= m_nTimeDelta)
        {
            if (m_bDeleteWhenFinished != 0)
            {
                delete this;
                return;
            }
            if (m_bAttachWhenFinished != 0)
            {
                AttachToScene(NULL);
            }
            m_bFinished = 1;
            m_bDurationActive = 0;
        }
        else
        {
            m_nDuration -= m_nTimeDelta;
        }
    }
}
// PC 0x0068FF70  Xbox 0x000B3480
CSWCVisualEffect::~CSWCVisualEffect()
{
    g_pAppManager->m_pClientExoApp->GetGameObjectArray()->Delete(m_nId);
    if (m_pSoundSource != NULL)
    {
        delete m_pSoundSource;
    }
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x00690010  Xbox 0x000B3520
int CSWCVisualEffect::LoadEffect(unsigned long nEffect)
{
    CExoString sModel;
    CExoString sRow(nEffect);
    int bOrientWithGround;
    int bModelFound;
    int nShakeType;
    float fShakeDuration;
    float fShakeDelay;

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetINTEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_ORIENTWITHGROUND,
                &bOrientWithGround) == 0)
        {
            bOrientWithGround = 0;
        }
    }
    if (bOrientWithGround != 0)
    {
        m_bGroundEffect = 1;
    }
    else
    {
        m_bGroundEffect = 0;
    }

    bModelFound = 0;
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bHighQualityPersistentVFX == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_LOWQUALITY, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }
    if (bModelFound == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_HEADCON_NODE, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }
    if (bModelFound == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_IMPACT_NODE, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }
    if (bModelFound == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_M_NODE, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }
    if (bModelFound == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_S_NODE, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }
    if (bModelFound == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_L_NODE, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }
    if (bModelFound == 0)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetCExoStringEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_H_NODE, &sModel) != 0)
        {
            bModelFound = 1;
        }
    }

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetINTEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKETYPE, &nShakeType) == 0)
        {
            nShakeType = CTwoDimArrays::VISUALEFFECT_SHAKE_NONE;
        }
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetFLOATEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKEDURATION, &fShakeDuration) == 0)
        {
            fShakeDuration = 0.0f;
        }
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* p2DA = p2DArrays->m_pVisualEffect;
        if (p2DA->GetFLOATEntry(
                sRow, CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKEDELAY, &fShakeDelay) == 0)
        {
            fShakeDelay = 0.0f;
        }
    }
    if (nShakeType == CTwoDimArrays::VISUALEFFECT_SHAKE_BUMP)
    {
        CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
        pExoApp->GetModule()->BumpCamera(
            (unsigned long)(fShakeDelay * 1000.0f));
    }
    else if (nShakeType == CTwoDimArrays::VISUALEFFECT_SHAKE_CAMERA)
    {
        CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
        pExoApp->GetModule()->ShakeCamera(
            (unsigned long)(fShakeDelay * 1000.0f),
            (unsigned long)(fShakeDuration * 1000.0f));
    }
    if (bModelFound != 0)
    {
        CResRef cModel(sModel);
        if (LoadModel(cModel, CSWCAnimBase::MODEL_SLOT_MAIN,
                      LOAD_MODEL_FLAG_VISUAL_EFFECT) == 0)
        {
            return 0;
        }
    }
    if (nEffect == EFFECT_UNHILITED_FIRST || nEffect == EFFECT_UNHILITED_SECOND)
    {
        UnHilite(HILITE_CHANNEL_VISUAL_EFFECT);
    }
    m_nEffect = nEffect;
    return 1;
}
// PC 0x006903D0  Xbox 0x000B3A80
int CSWCVisualEffect::LoadModel(const CResRef& cModel, unsigned char nModel,
                                unsigned char nFlags)
{
    char szError[256];
    Gob* pModel;

    if (m_pAnimBase == NULL)
    {
        m_pAnimBase = new CSWCAnimBaseVisualEffect;
        if (m_pAnimBase == NULL)
        {
            return 0;
        }
    }
    cModel.GetResRefStr();
    if (m_pAnimBase->LoadModel(cModel, 255) == 0)
    {
        sprintf(szError,
                "CSWCVisualEffect::LoadModel: Failed to load visual effect "
                "model '%s'.",
                cModel.GetResRefStr());
        return 0;
    }
    pModel = static_cast<Gob*>(m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT));
    if (pModel == NULL)
    {
        return 0;
    }
    pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT);
    pModel->SetExternalData(this);
    pModel->SetLightPriority(Gob::LIGHT_PRIORITY_VISUAL_EFFECT);
    Vector vColor(1.0f, 1.0f, 1.0f);
    Hilite(vColor, 3, 0, 0);
    if (m_bShadows != 0)
    {
        pModel->EnableForceAnimations();
    }
    return 1;
}
// PC 0x006825F0
void CSWCVisualEffect::UnloadModel()
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x0063D820
void CSWCVisualEffect::AttachToScene(CAurScene* pScene)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->AttachToScene(pScene);
    }
}
// PC 0x00646650
void CSWCVisualEffect::UnloadModel(unsigned char nModel)
{
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x00690510  Xbox 0x000B3BE0
void CSWCVisualEffect::SetAnimation(unsigned short nAnimation)
{
    float fSpeed;

    m_nAnimationDelay = 0;
    m_nAnimation = nAnimation;
    CExoString sRow(m_nEffect);
    if (m_nEffect == 40)
    {
        fSpeed = weirdanimspeed;
    }
    else
    {
        fSpeed = 1.0f;
    }
    if (nAnimation == 1)
    {
        CExoString sSound;
        bool bMissing;

        {
            CExoString sColumn("SoundImpact");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            bMissing = p2DA->GetCExoStringEntry(sRow, sColumn, &sSound) == 0;
        }
        if (bMissing)
        {
            sSound = "";
        }
        {
            CResRef cSound(sSound);
            g_pExoSound->Play3DOneShotSound(cSound, m_vPosition, 1.5f, 13, 0, 0,
                                            0.0f, 0.0f);
        }
        if (m_pAnimBase != NULL)
        {
            m_nAnimationDelay =
                (unsigned long)m_pAnimBase->GetAnimationLength(1);
            if (m_bShadows != 0)
            {
                m_pAnimBase->SetAnimation(1, fSpeed, 0, 0);
            }
            else
            {
                m_pAnimBase->SetAnimation(1, fSpeed, 1, 0);
            }
        }
    }
    else if (nAnimation == 3)
    {
        CExoString sSound;
        bool bMissing;

        {
            CExoString sColumn("SoundCessation");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            bMissing = p2DA->GetCExoStringEntry(sRow, sColumn, &sSound) == 0;
        }
        if (bMissing)
        {
            sSound = "";
        }
        {
            CResRef cSound(sSound);
            g_pExoSound->Play3DOneShotSound(cSound, m_vPosition, 1.5f, 13, 0, 0,
                                            0.0f, 0.0f);
        }
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Stop();
            if (m_pSoundSource != NULL)
            {
                delete m_pSoundSource;
            }
            m_pSoundSource = NULL;
        }
        if (m_pAnimBase != NULL)
        {
            m_nAnimationDelay =
                (unsigned long)m_pAnimBase->GetAnimationLength(3);
            if (m_bShadows != 0)
            {
                m_pAnimBase->SetAnimation(3, fSpeed, 0, 0);
            }
            else
            {
                m_pAnimBase->SetAnimation(3, fSpeed, 1, 0);
            }
        }
    }
    else
    {
        CExoString sSound;
        bool bMissing;

        {
            CExoString sColumn("SoundDuration");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* p2DA = p2DArrays->m_pVisualEffect;
            bMissing = p2DA->GetCExoStringEntry(sRow, sColumn, &sSound) == 0;
        }
        if (bMissing)
        {
            sSound = "";
        }
        m_pSoundSource = new CExoSoundSource(sSound);
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Set3D(1);
            m_pSoundSource->SetPriorityGroup(CExoSound::PRIORITY_GROUP_VISUAL_EFFECT);
            m_pSoundSource->SetLooping(1);
            m_pSoundSource->Play();
        }
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->SetAnimation(nAnimation, fSpeed, 0, 0);
        }
    }
}
// PC 0x00690910  Xbox 0x000B3FD0
void CSWCVisualEffect::UnloadCachedVFX()
{
    CAurObject* pCachedObject;

    while (m_lstCachedVFX.m_pInternal->m_pHead != NULL)
    {
        pCachedObject = (CAurObject*)m_lstCachedVFX.m_pInternal->RemoveHead();
        delete pCachedObject;
    }
}
// PC 0x00690940  Xbox 0x000B4000
void CSWCVisualEffect::DoVFXRumble()
{
    g_pAppManager->m_pClientExoApp->LookUpAndPerformRumbleWithCutOff(
        m_nEffect, CClientExoApp::RUMBLE_TABLE_VISUALEFFECTS, m_vPosition);
}
// PC 0x00690B00  Xbox 0x000B41C0
void CSWCVisualEffect::SetPosition(const Vector& vPosition)
{
    if (m_pSoundSource != NULL)
    {
        m_pSoundSource->SetPosition(vPosition, 1.5f);
    }
    CSWCObject::SetPosition(vPosition);

    Quaternion qOrientation;
    if (m_nEffect == EFFECT_UNHILITED_FIRST ||
        m_nEffect == EFFECT_UNHILITED_SECOND)
    {
        if (g_pAppManager->m_pClientExoApp->GetPlayerCreature() != NULL)
        {
            Vector vDirection;
            Vector vPlayerPosition = g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_vPosition;
            vDirection = vPlayerPosition - vPosition;
            if (vDirection * vDirection > 0.00001f)
            {
                vDirection = normalize(vDirection);
                qOrientation = RotationArc(Vector(0.0f, 1.0f, 0.0f), vDirection);
                qOrientation = YawPitchRoll(Yaw(qOrientation), 0.0f, 0.0f);
            }
        }
        UnHilite(HILITE_CHANNEL_VISUAL_EFFECT);
    }
    if (m_bGroundEffect != 0)
    {
        CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
        CAurScene* pScene = pArea->m_pScene;
        if (pScene != NULL)
        {
            CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
            Vector vHit;
            cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
            cHitInfo.m_nFlags = g_pAppManager->GetClientExoApp()->GetHitCheckMaterialMask();
            Vector vStart = vPosition;
            vStart.m_fZ += 10.0f;
            Vector vEnd = vPosition;
            vEnd.m_fZ -= 10.0f;
            if (pScene->HitCheck(vStart, vEnd, &vHit, &cHitInfo) != 0)
            {
                qOrientation = RotationArc(Vector(0.0f, 0.0f, 1.0f),
                                           cHitInfo.m_vOutputNormal) * qOrientation;
            }
        }
    }
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->SetOrientation(qOrientation);
    }
}
