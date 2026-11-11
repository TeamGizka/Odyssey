#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "worldtimer.h"
#include "exobase.h"
#include "nwcareaofeffectobject.h"
#include "nwcanimbase.h"
#include "nwcvisualeffect.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "gameobjectarray.h"
#include "nwcanimbase.h"
#include "nwcmodule.h"
#include "nwcarea.h"
#include "aurscene.h"
#include "../../libsource/AURORA/aurpointer.h"
#include <math.h>
#include "nwrules.h"
// PC 0x007460D0
CSWCAreaOfEffectObject::CSWCAreaOfEffectObject()
    : CSWCObject(0xB)
{
    unsigned long nDay;
    unsigned long nTime;

    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_vOrientation = Vector(0.0f, 1.0f, 0.0f);
    m_vPosition = Vector(0.0f, 0.0f, 0.0f);
    m_fRadius = 0.0f;
    m_fWidth = 0.0f;
    m_fLength = 0.0f;
    m_cShape = 0;
    for (unsigned char i = 0; i < 3; i++)
    {
        m_anTypeChance[i] = 0;
        m_anInstanceCount[i] = 0;
        m_anDuration[i] = 0;
    }
    m_pnLifetime = NULL;
    m_nEndTimer = 0;
    m_bFinished = 0;
    m_nNumInstances = 0;
    m_bStopping = 0;
    m_pbAttached = NULL;
    m_pScene = NULL;
    m_pnAnimTimer = NULL;
    m_pnAnimation = NULL;
    m_pnDelay = NULL;
    m_pVisualEffect = NULL;
    m_bVisualAttached = 0;
    m_pSoundSource = NULL;
    m_cSoundResRef = "";
    m_nSoundChance = 0;
    m_bSnapToGround = 0;
    m_bSoundStarted = 0;
    m_oidCreator = OBJECT_INVALID;
    m_nAreaEffectId = -1;
}
// PC 0x00746250
int CSWCAreaOfEffectObject::LoadEffect(unsigned long nAreaEffectId)
{
    CExoString asModels[3];
    CExoString sShape;
    CExoString sDurationVFX;
    int i;
    int bOrientWithGround;
    float afEdgeWeight[3];
    float fSoundChance;

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        if (pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_ORIENTWITHGROUND, &bOrientWithGround) == 0)
        {
            bOrientWithGround = 0;
        }
    }
    if (bOrientWithGround != 0)
    {
        m_bSnapToGround = 1;
    }
    else
    {
        m_bSnapToGround = 0;
    }

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        if (pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATIONVFX, &sDurationVFX) != 0)
        {
            CExoString sModel;
            int bModelFound = 0;
            if (bModelFound == 0)
            {
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pVisualEffect;
                if (pTable->GetCExoStringEntry(sDurationVFX,
                CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_HEADCON_NODE, &sModel) != 0)
                {
                    bModelFound = 1;
                }
            }
            if (bModelFound == 0)
            {
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pVisualEffect;
                if (pTable->GetCExoStringEntry(sDurationVFX,
                CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_IMPACT_NODE, &sModel) != 0)
                {
                    bModelFound = 1;
                }
            }
            if (bModelFound == 0)
            {
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pVisualEffect;
                if (pTable->GetCExoStringEntry(sDurationVFX,
                CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_M_NODE, &sModel) != 0)
                {
                    bModelFound = 1;
                }
            }
            if (bModelFound == 0)
            {
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pVisualEffect;
                if (pTable->GetCExoStringEntry(sDurationVFX,
                CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_S_NODE, &sModel) != 0)
                {
                    bModelFound = 1;
                }
            }
            if (bModelFound == 0)
            {
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pVisualEffect;
                if (pTable->GetCExoStringEntry(sDurationVFX,
                CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_L_NODE, &sModel) != 0)
                {
                    bModelFound = 1;
                }
            }
            if (bModelFound == 0)
            {
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pVisualEffect;
                if (pTable->GetCExoStringEntry(sDurationVFX,
                CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_H_NODE, &sModel) != 0)
                {
                    bModelFound = 1;
                }
            }
            if (bModelFound == 0)
            {
                return 0;
            }
            CSWCVisualEffect* pEffect = new CSWCVisualEffect(0);
            if (pEffect != NULL)
            {
                CResRef cModel = sModel;
                if (pEffect->LoadModel(cModel, MODEL_SLOT_DEFAULT,
                LOAD_MODEL_FLAG_VISUAL_EFFECT) == 0)
                {
                    delete pEffect;
                    pEffect = NULL;
                }
                else
                {
                    m_bVisualAttached = 0;
                    pEffect->m_bDeleteWhenFinished = 0;
                    pEffect->m_bOneShot = 0;
                    pEffect->SetAnimation(VFX_ANIMATION_START);
                }
            }
            m_pVisualEffect = pEffect;
        }
    }

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_SHAPE, &sShape);
    }
    m_cShape = sShape.CStr()[0];
    if (m_cShape == SHAPE_CIRCLE)
    {
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            if (pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_RADIUS, &m_fRadius) == 0)
            {
                m_fRadius = 1.0f;
            }
        }
    }
    else if (m_cShape == SHAPE_RECTANGLE)
    {
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            if (pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_WIDTH, &m_fWidth) == 0)
            {
                m_fWidth = 1.0f;
            }
        }
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            if (pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_LENGTH, &m_fLength) == 0)
            {
                m_fLength = 1.0f;
            }
        }
    }

    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bHighQualityPersistentVFX == 0)
    {
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            if (pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODELMIN01, &asModels[0]) == 0)
            {
                {
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
                    pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL01, &asModels[0]);
                }
            }
        }
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            if (pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODELMIN02, &asModels[1]) == 0)
            {
                {
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
                    pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL02, &asModels[1]);
                }
            }
        }
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            if (pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODELMIN03, &asModels[2]) == 0)
            {
                {
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
                    pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL03, &asModels[2]);
                }
            }
        }
    }
    else
    {
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL01, &asModels[0]);
        }
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL02, &asModels[1]);
        }
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL03, &asModels[2]);
        }
    }

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_NUMACT01, &m_anInstanceCount[0]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_NUMACT02, &m_anInstanceCount[1]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_NUMACT03, &m_anInstanceCount[2]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATION01, &m_anDuration[0]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATION02, &m_anDuration[1]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetINTEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATION03, &m_anDuration[2]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT01, &afEdgeWeight[0]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT02, &afEdgeWeight[1]);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT03, &afEdgeWeight[2]);
    }
    m_nTimeDelta = 0;
    for (i = 0; i < 3; i++)
    {
        m_anTypeChance[i] = (unsigned char)(afEdgeWeight[i] * 100.0f);
        if (m_anTypeChance[i] > 100)
        {
            m_anTypeChance[i] = 100;
        }
    }

    CExoString sSoundLoop;
    CExoString sSoundOneShot;
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        if (pTable->GetCExoStringEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOT, &sSoundOneShot) != 0)
        {
            m_cSoundResRef = sSoundOneShot;
        }
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        if (pTable->GetFLOATEntry(nAreaEffectId, CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOTPERCENTAGE, &fSoundChance) == 0)
        {
            fSoundChance = 0.0f;
        }
    }
    m_nSoundChance = (int)(fSoundChance * 100.0f);
    m_nNumInstances = m_anInstanceCount[0] + m_anInstanceCount[1] + m_anInstanceCount[2];
    if (m_nNumInstances < 1)
    {
        return 1;
    }

    m_pAnimBase = new CSWCAnimBaseVisualEffect[m_nNumInstances];
    if (m_pAnimBase == NULL)
    {
        UnloadModel();
        return 0;
    }
    m_pnLifetime = new unsigned long[m_nNumInstances];
    if (m_pnLifetime == NULL)
    {
        UnloadModel();
        return 0;
    }
    m_pnDelay = new unsigned long[m_nNumInstances];
    if (m_pnDelay == NULL)
    {
        UnloadModel();
        return 0;
    }
    m_pbAttached = new int[m_nNumInstances];
    if (m_pbAttached == NULL)
    {
        UnloadModel();
        return 0;
    }
    m_pnAnimTimer = new unsigned long[m_nNumInstances];
    if (m_pnAnimTimer == NULL)
    {
        UnloadModel();
        return 0;
    }
    m_pnAnimation = new unsigned short[m_nNumInstances];
    if (m_pnAnimation == NULL)
    {
        UnloadModel();
        return 0;
    }
    for (i = 0; i < m_nNumInstances; i++)
    {
        m_pbAttached[i] = 0;
        m_pnDelay[i] = 0;
        m_pnLifetime[i] = 0;
        m_pnAnimation[i] = 0;
        m_pnAnimTimer[i] = 0;
    }
    int nInstance = 0;
    for (i = 0; i < m_anInstanceCount[0]; i++)
    {
        CResRef cModel = asModels[0];
        if (m_pAnimBase[nInstance].LoadModel(cModel,
        CSWCAnimBase::MODEL_SLOT_MAIN) != 0)
        {
            m_pnDelay[nInstance] = i * 300;
            m_pAnimBase[nInstance].Hilite(Vector(1.0f, 1.0f, 1.0f),
            HILITE_CHANNEL_VISUAL_EFFECT, 0);
        }
        nInstance++;
    }
    for (i = 0; i < m_anInstanceCount[1]; i++)
    {
        CResRef cModel = asModels[1];
        if (m_pAnimBase[nInstance].LoadModel(cModel,
        CSWCAnimBase::MODEL_SLOT_MAIN) != 0)
        {
            m_pnDelay[nInstance] = 300 + i * 400;
            m_pAnimBase[nInstance].Hilite(Vector(1.0f, 1.0f, 1.0f),
            HILITE_CHANNEL_VISUAL_EFFECT, 0);
        }
        nInstance++;
    }
    for (i = 0; i < m_anInstanceCount[2]; i++)
    {
        CResRef cModel = asModels[2];
        if (m_pAnimBase[nInstance].LoadModel(cModel,
        CSWCAnimBase::MODEL_SLOT_MAIN) != 0)
        {
            m_pnDelay[nInstance] = 700 + i * 350;
            m_pAnimBase[nInstance].Hilite(Vector(1.0f, 1.0f, 1.0f),
            HILITE_CHANNEL_VISUAL_EFFECT, 0);
        }
        nInstance++;
    }
    if (nAreaEffectId == AOE_PER_WALLBLADE)
    {
        CResRef cEnvMap("DEFAULT");
        SetEnvMap(1, 1, cEnvMap);
    }
    m_nAreaEffectId = nAreaEffectId;
    return 1;
}
// PC 0x00746E80
void CSWCAreaOfEffectObject::UnloadModel()
{
    if (m_pnAnimTimer != NULL)
    {
        delete m_pnAnimTimer;
        m_pnAnimTimer = NULL;
    }
    if (m_pnAnimation != NULL)
    {
        delete m_pnAnimation;
        m_pnAnimation = NULL;
    }
    if (m_pbAttached != NULL)
    {
        delete m_pbAttached;
        m_pbAttached = NULL;
    }
    if (m_pnDelay != NULL)
    {
        delete m_pnDelay;
        m_pnDelay = NULL;
    }
    if (m_pnLifetime != NULL)
    {
        delete m_pnLifetime;
        m_pnLifetime = NULL;
    }
    if (m_pAnimBase != NULL)
    {
        for (int i = 0; i < m_nNumInstances; i++)
        {
            m_pAnimBase[i].UnloadModel();
        }
        delete[] m_pAnimBase;
        m_pAnimBase = NULL;
    }
    if (m_pVisualEffect != NULL)
    {
        delete m_pVisualEffect;
        m_pVisualEffect = NULL;
    }
}
// PC 0x00746F80
void CSWCAreaOfEffectObject::UnloadModel(unsigned char nModel)
{
    if (nModel == 0xFF)
    {
        UnloadModel();
    }
}
// PC 0x00746FA0
int CSWCAreaOfEffectObject::SetAnimation(unsigned short nAnimation,
                                         unsigned char nInstance)
{
    if (m_pAnimBase == NULL)
    {
        return 0;
    }
    m_pnAnimation[nInstance] = nAnimation;
    if (nAnimation == 1)
    {
        m_pnAnimTimer[nInstance] =
            (unsigned long)m_pAnimBase[nInstance].GetAnimationLength(1);
        if (m_pnAnimTimer[nInstance] == 0)
        {
            m_pnAnimTimer[nInstance] = 1;
        }
        return m_pAnimBase[nInstance].SetAnimation(1, 1.0f, 1, 0);
    }
    else if (nAnimation == 3)
    {
        m_pnAnimTimer[nInstance] =
            (unsigned long)m_pAnimBase[nInstance].GetAnimationLength(3);
        if (m_pnAnimTimer[nInstance] == 0)
        {
            m_pnAnimTimer[nInstance] = 1;
        }
        return m_pAnimBase[nInstance].SetAnimation(3, 1.0f, 1, 0);
    }
    else
    {
        m_pnAnimTimer[nInstance] = 0;
        return m_pAnimBase[nInstance].SetAnimation(nAnimation, 1.0f, 0, 0);
    }
}
// PC 0x007470B0
void CSWCAreaOfEffectObject::ResetTimer(int nIndex)
{
    if (m_pAnimBase != NULL)
    {
        int nTotal = 0;
        unsigned char i;
        for (i = 0; i < 3; i++)
        {
            nTotal += m_anInstanceCount[i];
            if (nIndex < nTotal)
            {
                break;
            }
        }
        if (i != 3)
        {
            m_pnLifetime[nIndex] = m_anDuration[i];
        }
    }
}
// PC 0x00747100
void CSWCAreaOfEffectObject::StopEffect()
{
    CExoString sSoundCessation;
    int bMissing;
    {
        CExoString sColumn("SoundCessation");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
        bMissing = pTable->GetCExoStringEntry(
            m_nAreaEffectId, sColumn, &sSoundCessation);
    }
    if (bMissing != 0)
    {
        sSoundCessation = "";
    }
    {
    CResRef cSoundResRef = sSoundCessation;
    g_pExoSound->Play3DOneShotSound(
        cSoundResRef, m_vPosition, 1.5f, SOUND_PRIORITY_AREA_EFFECT,
        0, 0, 0.0f, 0.0f);
    }
    if (m_pSoundSource != NULL)
    {
        m_pSoundSource->Stop();
        delete m_pSoundSource;
        m_pSoundSource = NULL;
    }
    m_bStopping = 1;
    if (m_pVisualEffect != NULL)
    {
        m_pVisualEffect->SetAnimation(VFX_ANIMATION_END);
    }
    for (int i = 0; i < m_nNumInstances; i++)
    {
        SetAnimation(VFX_ANIMATION_END, i);
        m_pnLifetime[i] = 0;
    }
    m_nEndTimer = 3000;
}
// PC 0x007472D0
void CSWCAreaOfEffectObject::SetMoveTimer(unsigned long nTime,
                                          CAurObject* pObject)
{
    if (m_pAnimBase != NULL)
    {
        int i = 0;
        while (i < m_nNumInstances)
        {
            if (m_pAnimBase[i].GetModel(255) == pObject)
            {
                break;
            }
            i++;
        }
        if (i != m_nNumInstances)
        {
            m_pnDelay[i] = nTime;
        }
    }
}
// PC 0x00747340
int CSWCAreaOfEffectObject::SetEnvMap(int nType, int nFlags,
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

    for (int i = 0; i < m_nNumInstances; i++)
    {
        CResRef cMap(sEnvMap);
        m_pAnimBase[i].SetEnvMap(nType, nFlags, cMap);
    }
    return 1;
}
// PC 0x007474A0
int CSWCAreaOfEffectObject::SetEnvMap(unsigned char nModel, int nType,
                                     int nFlags, const CResRef& cEnvMap)
{
    CExoString sEnvMap;

    if (m_pAnimBase == NULL || nModel != CSWCAnimBase::MODEL_SLOT_MAIN)
    {
        return 0;
    }

    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
    if (cEnvMap == "DEFAULT")
    {
        if (pModule != NULL && pModule->m_pInfo != NULL)
        {
            pModule->m_pInfo->GetDefaultEnvMap().CopyToString(sEnvMap);
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

    for (int i = 0; i < m_nNumInstances; i++)
    {
        CResRef cMap(sEnvMap);
        m_pAnimBase[i].SetEnvMap(nType, nFlags, cMap);
    }
    return 1;
}
// PC 0x007475F0
CSWCAreaOfEffectObject::~CSWCAreaOfEffectObject()
{
    if (m_pSoundSource != NULL)
    {
        delete m_pSoundSource;
    }
    UnloadModel();
}
// PC 0x00747680
int CSWCAreaOfEffectObject::SetInstanceRandomPosition(unsigned char nInstance)
{
    Vector vPosition;
    if (m_pAnimBase == NULL)
    {
        return 0;
    }

    int nInstances = 0;
    unsigned char nType;
    for (nType = 0; nType < 3; nType++)
    {
        nInstances += m_anInstanceCount[nType];
        if (nInstance < nInstances)
        {
            break;
        }
    }
    if (nType == 3)
    {
        return 0;
    }

    if (rand() % 100 < m_anTypeChance[nType])
    {
        if (m_cShape == SHAPE_CIRCLE)
        {
            if (rand() % 2)
            {
                if (rand() % 2)
                {
                    vPosition.m_fX = (rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f;
                }
                else
                {
                    vPosition.m_fX = -((rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f);
                }
                if (rand() % 2)
                {
                    vPosition.m_fY = sqrt(m_fRadius * m_fRadius - vPosition.m_fX * vPosition.m_fX);
                }
                else
                {
                    vPosition.m_fY = -sqrt(m_fRadius * m_fRadius - vPosition.m_fX * vPosition.m_fX);
                }
            }
            else
            {
                if (rand() % 2)
                {
                    vPosition.m_fY = (rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f;
                }
                else
                {
                    vPosition.m_fY = -((rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f);
                }
                if (rand() % 2)
                {
                    vPosition.m_fX = sqrt(m_fRadius * m_fRadius - vPosition.m_fY * vPosition.m_fY);
                }
                else
                {
                    vPosition.m_fX = -sqrt(m_fRadius * m_fRadius - vPosition.m_fY * vPosition.m_fY);
                }
            }
        }
        else if (m_cShape == SHAPE_RECTANGLE)
        {
            if (rand() % 2)
            {
                if (rand() % 2)
                {
                    vPosition.m_fX = (rand() % (unsigned long)(m_fWidth * 50.0f)) / 100.0f;
                }
                else
                {
                    vPosition.m_fX = -((rand() % (unsigned long)(m_fWidth * 50.0f)) / 100.0f);
                }
                if (rand() % 2)
                {
                    vPosition.m_fY = m_fLength * 0.5f;
                }
                else
                {
                    vPosition.m_fY = m_fLength * -0.5f;
                }
            }
            else
            {
                if (rand() % 2)
                {
                    vPosition.m_fY = (rand() % (unsigned long)(m_fLength * 50.0f)) / 100.0f;
                }
                else
                {
                    vPosition.m_fY = -((rand() % (unsigned long)(m_fLength * 50.0f)) / 100.0f);
                }
                if (rand() % 2)
                {
                    vPosition.m_fX = m_fWidth * 0.5f;
                }
                else
                {
                    vPosition.m_fX = m_fWidth * -0.5f;
                }
            }
        }
    }
    else
    {
        if (m_cShape == SHAPE_CIRCLE)
        {
            float fRadius;
            if (rand() % 2)
            {
                if (rand() % 2)
                {
                    vPosition.m_fX = (rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f - 0.5f;
                }
                else
                {
                    vPosition.m_fX = 0.5f - (rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f;
                }
                fRadius = sqrt(m_fRadius * m_fRadius - vPosition.m_fX * vPosition.m_fX);
                if (rand() % 2)
                {
                    vPosition.m_fY = (rand() % (unsigned long)(fRadius * 100.0f)) / 100.0f - 0.5f;
                }
                else
                {
                    vPosition.m_fY = 0.5f - (rand() % (unsigned long)(fRadius * 100.0f)) / 100.0f;
                }
            }
            else
            {
                if (rand() % 2)
                {
                    vPosition.m_fY = (rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f - 0.5f;
                }
                else
                {
                    vPosition.m_fY = 0.5f - (rand() % (unsigned long)(m_fRadius * 100.0f)) / 100.0f;
                }
                fRadius = sqrt(m_fRadius * m_fRadius - vPosition.m_fY * vPosition.m_fY);
                if (rand() % 2)
                {
                    vPosition.m_fX = (rand() % (unsigned long)(fRadius * 100.0f)) / 100.0f - 0.5f;
                }
                else
                {
                    vPosition.m_fX = 0.5f - (rand() % (unsigned long)(fRadius * 100.0f)) / 100.0f;
                }
            }
        }
        else if (m_cShape == SHAPE_RECTANGLE)
        {
            if (rand() % 2)
            {
                vPosition.m_fX = (rand() % (unsigned long)(m_fWidth * 50.0f)) / 100.0f - 0.5f;
            }
            else
            {
                vPosition.m_fX = 0.5f - (rand() % (unsigned long)(m_fWidth * 50.0f)) / 100.0f;
            }
            if (rand() % 2)
            {
                vPosition.m_fY = (rand() % (unsigned long)(m_fLength * 50.0f)) / 100.0f - 0.5f;
            }
            else
            {
                vPosition.m_fY = 0.5f - (rand() % (unsigned long)(m_fLength * 50.0f)) / 100.0f;
            }
        }
    }

    vPosition = RotationArc(Vector(0.0f, 1.0f, 0.0f), m_vOrientation) * vPosition;
    vPosition = vPosition + m_vPosition;
    CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    Vector vHit;
    CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    CAurScene* pScene = pArea->m_pScene;
    cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    cHitInfo.m_nFlags = g_pAppManager->m_pClientExoApp->GetHitCheckMaterialMask();
    Vector vStart = vPosition;
    vStart.m_fZ += 10.0f;
    Vector vEnd = vPosition;
    vEnd.m_fZ -= 10.0f;
    if (!pScene->HitCheck(vStart, vEnd, &vHit, &cHitInfo))
    {
        return 0;
    }
    m_pAnimBase[nInstance].SetPosition(vHit);
    if (m_bSnapToGround != 0)
    {
        m_pAnimBase[nInstance].SetOrientation(
            RotationArc(Vector(0.0f, 0.0f, 1.0f), cHitInfo.m_vOutputNormal));
    }
    return 1;
}
// PC 0x00748200
void CSWCAreaOfEffectObject::UpdateInstancePosition()
{
    int nFinishedInstances = 0;
    if (m_pAnimBase != NULL)
    {
        for (int i = 0; i < m_nNumInstances; i++)
        {
            if (m_pnLifetime[i] == 0)
            {
                if (m_bStopping != 0)
                {
                    nFinishedInstances++;
                }
                else if (m_pnDelay[i] > 0)
                {
                    if (m_pnDelay[i] <= m_nTimeDelta)
                    {
                        if (SetInstanceRandomPosition(i))
                        {
                            if (m_pbAttached[i] == 0)
                            {
                                m_pAnimBase[i].AttachToScene(m_pScene);
                                m_pbAttached[i] = 1;
                            }
                            SetAnimation(VFX_ANIMATION_START, i);
                            ResetTimer(i);
                            m_pnDelay[i] = 0;
                            if (m_cSoundResRef.GetResRef()[0] != 0 &&
                                rand() % 100 < m_nSoundChance &&
                                m_pAnimBase[i].GetModel(CSWCAnimBase::MODEL_SLOT_MAIN) != NULL)
                            {
                                g_pExoSound->Play3DOneShotSound(m_cSoundResRef,
                                    m_pAnimBase[i].GetModel(CSWCAnimBase::MODEL_SLOT_MAIN)->GetPosition(),
                                    1.5f, SOUND_PRIORITY_AREA_EFFECT, 0, 0, 0.0f, 0.0f);
                            }
                        }
                    }
                    else
                    {
                        m_pnDelay[i] -= m_nTimeDelta;
                    }
                }
            }
            else if (m_pnAnimation[i] == VFX_ANIMATION_LOOP)
            {
                if (m_pnLifetime[i] <= m_nTimeDelta)
                {
                    SetAnimation(VFX_ANIMATION_END, i);
                    m_pnLifetime[i] = 0;
                }
                else
                {
                    m_pnLifetime[i] -= m_nTimeDelta;
                }
            }
        }
    }
    int bVisualFinished = 1;
    if (m_pVisualEffect != NULL)
    {
        bVisualFinished = m_pVisualEffect->m_bFinished;
    }
    if (m_bStopping != 0 && bVisualFinished != 0 &&
        nFinishedInstances == m_nNumInstances)
    {
        m_bFinished = 1;
    }
}
// PC 0x007483F0
void CSWCAreaOfEffectObject::SetPosition(const Vector& vPosition)
{
    CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    Vector vHit;
    Vector vStart;
    Vector vEnd;
    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CAurScene* pScene = pArea->m_pScene;
    if (m_pVisualEffect != NULL)
    {
        m_pVisualEffect->SetPosition(vPosition);
        if (pScene != NULL && m_bSnapToGround != 0)
        {
            cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
            cHitInfo.m_nFlags = g_pAppManager->GetClientExoApp()->GetHitCheckMaterialMask();
            vStart = vPosition;
            vStart.m_fZ += 10.0f;
            vEnd = vPosition;
            vEnd.m_fZ -= 10.0f;
            if (pScene->HitCheck(vStart, vEnd, &vHit, &cHitInfo))
            {
                m_pVisualEffect->GetAnimationBase()->SetOrientation(
                    RotationArc(Vector(0.0f, 0.0f, 1.0f), cHitInfo.m_vOutputNormal));
            }
        }
    }

    Vector vOffset = vPosition - m_vPosition;
    if (m_pAnimBase != NULL)
    {
        for (int i = 0; i < m_nNumInstances; i++)
        {
            Gob* pModel = m_pAnimBase[i].GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
            if (pModel != NULL)
            {
                Vector vModelPosition;
                vModelPosition = pModel->GetPosition();
                Vector vNewPosition = vModelPosition + vOffset;
                pModel->SetPosition(vNewPosition);
                if (pScene != NULL && m_bSnapToGround != 0)
                {
                    cHitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    cHitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                    cHitInfo.m_nFlags = g_pAppManager->GetClientExoApp()->GetHitCheckMaterialMask();
                    vStart = vNewPosition;
                    vStart.m_fZ += 10.0f;
                    vEnd = vNewPosition;
                    vEnd.m_fZ -= 10.0f;
                    if (pScene->HitCheck(vStart, vEnd, &vHit, &cHitInfo))
                    {
                        m_pAnimBase[i].SetOrientation(
                            RotationArc(Vector(0.0f, 0.0f, 1.0f), cHitInfo.m_vOutputNormal));
                    }
                }
            }
        }
    }
    m_vPosition = vPosition;
}
// PC 0x00748880
void CSWCAreaOfEffectObject::AIUpdate()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nDeltaDay;
    unsigned long nDeltaTime;

    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pClient = g_pAppManager->m_pClientExoApp;
    pClient->GetWorldTimer()->SubtractWorldTimes(nDay, nTime,
        m_nLastUpdateDay, m_nLastUpdateTime, &nDeltaDay, &nDeltaTime);
    m_nTimeDelta = nDeltaTime;

    if (m_oidCreator != OBJECT_INVALID)
    {
        CSWCObject* pCreator =
            g_pAppManager->m_pClientExoApp->GetGameObject(m_oidCreator);
        if (pCreator != NULL)
        {
            CSWCObject* pObject = pCreator->AsSWCObject();
            SetPosition(pObject->m_vPosition);
        }
    }

    if (m_bSoundStarted == 0)
    {
        CExoString sSoundImpact;
        CExoString sSoundDuration;

        bool bMissing;
        {
            CExoString sColumn("SoundImpact");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            bMissing = pTable->GetCExoStringEntry(m_nAreaEffectId, sColumn,
                                                 &sSoundImpact) == 0;
        }
        if (bMissing)
        {
            sSoundImpact = "";
        }
        {
            CExoString sColumn("SoundDuration");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pVisualEffectsPersistent;
            bMissing = pTable->GetCExoStringEntry(m_nAreaEffectId, sColumn,
                                                 &sSoundDuration) == 0;
        }
        if (bMissing)
        {
            sSoundDuration = "";
        }
        {
            CResRef cSound(sSoundImpact);
            g_pExoSound->Play3DOneShotSound(
                cSound, m_vPosition, 1.5f, SOUND_PRIORITY_AREA_EFFECT,
                0, 0, 0.0f, 0.0f);
        }
        m_pSoundSource = new CExoSoundSource(sSoundDuration);
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Set3D(1);
            m_pSoundSource->SetPriorityGroup(SOUND_PRIORITY_AREA_EFFECT);
            m_pSoundSource->SetLooping(1);
            m_pSoundSource->SetPosition(m_vPosition, 1.5f);
            m_pSoundSource->Play();
        }
        m_bSoundStarted = 1;
    }

    for (unsigned char i = 0; i < m_nNumInstances; i++)
    {
        if (m_pnAnimTimer[i] > 0)
        {
            if (m_pnAnimTimer[i] > m_nTimeDelta)
            {
                m_pnAnimTimer[i] -= m_nTimeDelta;
            }
            else
            {
                m_pnAnimTimer[i] = 0;
                if (m_pnAnimation[i] == VFX_ANIMATION_START)
                {
                    if (m_pAnimBase != NULL)
                    {
                        m_pnAnimation[i] = VFX_ANIMATION_LOOP;
                        m_pnAnimTimer[i] = 0;
                        m_pAnimBase[i].SetAnimation(VFX_ANIMATION_LOOP,
                                                   1.0f, 0, 0);
                    }
                }
                else if (m_pnAnimation[i] == VFX_ANIMATION_END)
                {
                    SetMoveTimer(1, m_pAnimBase[i].GetModel(
                                      CSWCAnimBase::MODEL_SLOT_MAIN));
                }
            }
        }
    }

    if (m_bFinished != 0)
    {
        if (m_nEndTimer <= m_nTimeDelta)
        {
            m_nEndTimer = 0;
            AttachToScene(NULL);
            unsigned long nId = m_nId;
            g_pAppManager->m_pClientExoApp->GetGameObjectArray()->Delete(nId);
            delete this;
            return;
        }
        m_nEndTimer -= m_nTimeDelta;
    }
    else
    {
        UpdateInstancePosition();
    }

    if (m_pVisualEffect != NULL)
    {
        if (m_bVisualAttached == 0)
        {
            m_pVisualEffect->AttachToScene(m_pScene);
            m_bVisualAttached = 1;
        }
        m_pVisualEffect->AIUpdate();
    }
    if (m_pSoundSource != NULL)
    {
        m_pSoundSource->SetPosition(m_vPosition, 1.5f);
    }
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
}
