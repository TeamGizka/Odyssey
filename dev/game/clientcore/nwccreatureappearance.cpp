#include "nwccreatureappearance.h"
#include "exobase.h"
#include "twodimarray.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "nwccreature.h"
#include "nwcanimbase.h"
#include "auremitter.h"
#include "appmanager.h"
#include "gameobjectarray.h"
#include "clientexoapp.h"
#include "nwcitem.h"
#include "nwcanimbase.h"
#include "nwitem.h"
#include "nwsobject.h"
#include "swmain.h"
#include <string.h>
#include <stdio.h>
#include "nwrules.h"
// PC 0x006F0430
CSWCCreatureAppearance::CSWCCreatureAppearance(CSWCCreature* pCreature)
{
    m_nUnmapped10 = CGameObjectArray::INVALIDOBJID;
    memset(m_anArmorModelPart, 0, sizeof(m_anArmorModelPart));
    m_nBodyVariation = 0;
    m_nTextureVar = 1;
    m_nAppearance = -1;
    m_nGender = -1;
    m_nHeadVariation = -1;
    m_nHead = -1;
    m_bUseBodyVariation = 0;
    m_nUnknown1D = 0;
    m_nUnknown1E = 0;
    m_nUnknown1F = 0;
    m_nUnknown20 = 0;
    m_nTail = 0;
    m_nWing = 0;
    m_fCameraHeight = 0.6f;
    m_fDefaultHeight = 0.5f;
    m_nUnknown30 = 0;
    m_nUnknown34 = -1;
    m_nAlignment = 50;
    m_pCreature = pCreature;
    m_nUnknown40 = 0;
    m_nUnknown44 = 0;
    m_nUnknown48 = 0;
    m_nUnknown4C = 0;
    m_nUnknown50 = 0;
    m_nUnknown54 = 0;
    m_bForceAnimate = 0;
    m_bGroundTilt = 0;
}
// PC 0x004015A0
CSWCCreatureAppearance::~CSWCCreatureAppearance()
{
}
// PC 0x00651540
int CSWCCreatureAppearance::RequestTailAppearance(CCAppearanceInfo* pAppearance)
{
    return 1;
}
// PC 0x00651540
int CSWCCreatureAppearance::RequestWingAppearance(CCAppearanceInfo* pAppearance)
{
    return 1;
}
// PC 0x006F04C0
int CSWCCreatureAppearance::CreateHead(unsigned char nType, CCAppearanceInfo* pAppearance)
{
    CResRef cHeadModel;
    CResRef cHeadTexture;
    CResRef cEnvMap;
    CExoString sHEAD;
    CExoString sTexture;
    CExoString sEntry;
    int bEnvMap;
    char sHeadNumber[4];

    if (pAppearance->m_nHead == 0 &&
        pAppearance->m_nUnmapped10 == CGameObjectArray::INVALIDOBJID)
    {
        m_pCreature->UnloadMonkVisuals();
        m_pCreature->UnloadModel(CSWCAnimBase::MODEL_SLOT_HEAD);
        m_pCreature->UpdateMonkVisuals();
        m_nUnmapped10 = pAppearance->m_nUnmapped10;
        m_nHead = pAppearance->m_nHead;
        return 1;
    }

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(
            pAppearance->m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_ENVMAP,
            &sEntry);
    }
    if (sEntry.GetLength() > 0)
    {
        cEnvMap = sEntry;
        bEnvMap = 1;
    }
    else
    {
        bEnvMap = 0;
    }

    if (nType == CSWCAnimBase::ANIMATION_TYPE_BTYPE)
    {
        int nHead;
        if (pAppearance->m_nUnknown34 != CCAppearanceInfo::HEAD_OVERRIDE_DEFAULT)
        {
            nHead = pAppearance->m_nUnknown34;
            m_nUnknown40 = 1;
        }
        else if (pAppearance->m_nUnknown30)
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
            pAppearance2DA->GetINTEntry(
                pAppearance->m_nAppearance,
                CTwoDimArrays::COL2DA_APPEARANCETABLE_BACKUPHEAD,
                &nHead);
        }
        else
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
            pAppearance2DA->GetINTEntry(
                pAppearance->m_nAppearance,
                CTwoDimArrays::COL2DA_APPEARANCETABLE_NORMALHEAD,
                &nHead);
        }
        {
            CExoString sColumn("HEAD");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pHeads2DA = p2DArrays->m_pHeads;
            pHeads2DA->GetCExoStringEntry(nHead, sColumn, &sHEAD);
        }
        if (sHEAD.IsEmpty())
        {
            return 0;
        }
        cHeadModel = sHEAD;
        cHeadTexture = sHEAD;
    }
    else
    {
        sHEAD = "P";
        {
            int nGender = pAppearance->m_nGender == CCAppearanceInfo::GENDER_FEMALE;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pGender2DA = p2DArrays->m_pGender;
            pGender2DA->GetCExoStringEntry(
                nGender, CTwoDimArrays::COL2DA_GENDER_GENDER, &sEntry);
        }
        sHEAD = sHEAD + sEntry;
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
            pAppearance2DA->GetCExoStringEntry(
                pAppearance->m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_RACE, &sEntry);
        }
        sHEAD = sHEAD + sEntry;
        sTexture = sHEAD;
        sHEAD = sHEAD + CExoString(pAppearance->m_nHeadVariation);
        sHEAD = sHEAD + "_HEAD";
        sTexture = sTexture + "0_HEAD";
        sprintf(sHeadNumber, "%03u", pAppearance->m_nHead);
        sHEAD = sHEAD + sHeadNumber;
        sTexture = sTexture + sHeadNumber;
        cHeadModel = sHEAD;
        cHeadTexture = sTexture;
    }

    if (!m_nUnknown40 && m_nAppearance == pAppearance->m_nAppearance &&
        m_nGender == pAppearance->m_nGender && m_nHead == pAppearance->m_nHead &&
        m_nUnmapped10 == pAppearance->m_nUnmapped10 &&
        m_nUnknown34 == pAppearance->m_nUnknown34 &&
        m_nUnknown30 == pAppearance->m_nUnknown30)
    {
        if (bEnvMap)
        {
            m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_HEAD, 1, 1, cEnvMap);
        }
        else
        {
            m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_HEAD, 0, 1, "DEFAULT");
        }
        return 1;
    }

    m_pCreature->UnloadMonkVisuals();
    m_pCreature->UnloadModel(CSWCAnimBase::MODEL_SLOT_HEAD);
    m_pCreature->UpdateMonkVisuals();
    m_nHead = pAppearance->m_nHead;
    m_nUnknown34 = pAppearance->m_nUnknown34;
    m_nUnknown30 = pAppearance->m_nUnknown30;
    if (!m_pCreature->LoadModel(cHeadModel, CSWCAnimBase::MODEL_SLOT_HEAD, nType))
    {
        return 0;
    }

    if (nType == CSWCAnimBase::ANIMATION_TYPE_HEAD ||
        nType == CSWCAnimBase::ANIMATION_TYPE_BTYPE)
    {
        if (m_nUnknown40 || m_nUnmapped10 != pAppearance->m_nUnmapped10)
        {
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(m_nUnmapped10);
            if (pItem)
            {
                Gob* pItemModel = pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
                if (pItemModel)
                {
                    pItemModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
                    pItemModel->SetObjectScale(1.0f, 1);
                    pItemModel->SetCanDownSample(1);
                }
                if (pItem->m_pArea)
                {
                    pItem->AttachToScene(0);
                }
            }
            if (pAppearance->m_nUnmapped10 != CGameObjectArray::INVALIDOBJID)
            {
                pClientExoApp = g_pAppManager->m_pClientExoApp;
                pItem = pClientExoApp->GetItemByGameObjectID(pAppearance->m_nUnmapped10);
                if (!pItem)
                {
                    return 0;
                }
                Gob* pHeadModel = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
                Gob* pItemModel = pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
                if (pItemModel)
                {
                    pItemModel->AttachToObject(pHeadModel, "GoggleHook", Gob::ATTACH_NORMAL);
                    pItemModel->PlayAnimation("default", 1.0f, 0, 0.0f);
                }
            }
        }
        m_pCreature->GetAnimationBase()->SetHeadItem(pAppearance->m_nUnmapped10);
    }
    m_nUnmapped10 = pAppearance->m_nUnmapped10;
    if (g_pAppManager->m_pClientExoApp->GetPlayerCreature() == m_pCreature)
    {
        m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1)->SetCanDownSample(0);
    }
    if (bEnvMap)
    {
        m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_HEAD, 1, 1, cEnvMap);
    }
    else
    {
        m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_HEAD, 0, 1, "DEFAULT");
    }
    m_nUnknown50 = 1;
    m_nUnknown40 = 0;
    m_bForceAnimate = g_pAppManager->m_pClientExoApp->GetPausedByCombat();
    return 1;
}
// PC 0x006F0E70
char CSWCCreatureAppearance::GetGenderChar()
{
    CExoString sGender;

    {
        CExoString sColumn("GENDER");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pGender = p2DArrays->m_pGender;

        pGender->GetCExoStringEntry(m_nGender, sColumn, &sGender);
    }
    return sGender[0];
}
// PC 0x004065A0
int CSWCCreatureAppearance::GetArmourClass()
{
    return 0;
}
// PC 0x006F1BF0
int CSWCCreatureAppearance::CreateHeadAppearance(CCAppearanceInfo* pAppearance)
{
    CExoString sModelType;

    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

    pAppearance2DA->GetCExoStringEntry(
        pAppearance->m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_MODELTYPE,
        &sModelType);
    if (GetAnimType(sModelType) != 3)
    {
        m_nHead = pAppearance->m_nHead;
        m_pCreature->UnloadModel(254);
        return 1;
    }

    return CreateHead(3, pAppearance);
}
// PC 0x006F1CD0
int CSWCCreatureAppearance::CreateTailAppearance(CCAppearanceInfo* pAppearance)
{
    CExoString sModelType;

    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

    pAppearance2DA->GetCExoStringEntry(
        pAppearance->m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_MODELTYPE,
        &sModelType);
    GetAnimType(sModelType);
    if (sModelType.Find('T', 0) == -1)
    {
        m_nTail = pAppearance->m_nTail;
        m_pCreature->UnloadModel(253);
    }

    return 1;
}
// PC 0x006F1D90
int CSWCCreatureAppearance::CreateWingAppearance(CCAppearanceInfo* pAppearance)
{
    CExoString sModelType;

    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

    pAppearance2DA->GetCExoStringEntry(
        pAppearance->m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_MODELTYPE,
        &sModelType);
    GetAnimType(sModelType);
    if (sModelType.Find('W', 0) == -1)
    {
        m_nWing = pAppearance->m_nWing;
        m_pCreature->UnloadModel(252);
    }

    return 1;
}
// PC 0x006F1E50
int CSWCCreatureAppearance::RequestBType(CCAppearanceInfo* pAppearance)
{
    CResRef cModel;

    GetBTypeModelAndTextureResRef(pAppearance, &cModel, 0);

    CResRef cTexture("");

    m_pCreature->RequestModel(cModel, cTexture, 3);
    return 1;
}
// PC 0x006F1EB0
int CSWCCreatureAppearance::CreateBTypeBody(unsigned char nType, CCAppearanceInfo* pAppearance)
{

    CResRef cBodyModel;
    CResRef cBodyTexture;
    CResRef cEnvMap;
    CExoString sRace;
    CExoString sENVMAP;
    CExoString sEntry;
    int bEnvMap;

    {
        CExoString sColumn("RACE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(
            pAppearance->m_nAppearance, sColumn, &sRace);
    }
    {
        CExoString sColumn("ENVMAP");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(
            pAppearance->m_nAppearance, sColumn, &sENVMAP);
    }
    if (sENVMAP.GetLength() > 0)
    {
        cEnvMap = sENVMAP;
        bEnvMap = 1;
    }
    else
    {
        bEnvMap = 0;
    }

    GetBTypeModelAndTextureResRef(pAppearance, &cBodyModel, &cBodyTexture);
    if (m_nAppearance == pAppearance->m_nAppearance &&
        m_bUseBodyVariation == pAppearance->m_bUseBodyVariation &&
        m_nBodyVariation == pAppearance->m_nBodyVariation &&
        m_nTextureVar == pAppearance->m_nTextureVar &&
        m_nGender == pAppearance->m_nGender)
    {
        return 1;
    }

    m_nBodyVariation = pAppearance->m_nBodyVariation;
    m_nTextureVar = pAppearance->m_nTextureVar;
    m_nAppearance = pAppearance->m_nAppearance;
    m_bUseBodyVariation = pAppearance->m_bUseBodyVariation;
    m_nGender = pAppearance->m_nGender;

    Gob* pHeadModel = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
    if (pHeadModel)
    {
        pHeadModel->SetBehavior(0);
    }
    int bQueuedCallback = m_pCreature->ClearQuededAnimationCallback();
    m_pCreature->UnloadModel(CSWCAnimBase::MODEL_SLOT_MAIN);
    if (!m_pCreature->LoadModel(cBodyModel, CSWCAnimBase::MODEL_SLOT_MAIN, nType))
    {
        char sBuffer[128];
        sprintf(sBuffer, "CSWCCreatureAppearance::CreateBTypeBody(): Failed to load model '%s'.",
                cBodyModel.GetResRefStr());
        EXOLOG("%s\n", sBuffer);
        return 0;
    }

    Gob* pBodyModel = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
    if (pBodyModel)
    {
        pBodyModel->SetPosition(m_pCreature->m_vPosition);
    }
    if (bQueuedCallback)
    {
        m_pCreature->SetQuededAnimationCallback();
    }
    if (bEnvMap)
    {
        m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_MAIN, 1, 1, cEnvMap);
    }
    else
    {
        m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_MAIN, 0, 1, "DEFAULT");
    }
    if (!g_pAppManager->GetServerExoApp() ||
        g_pAppManager->GetClientExoApp()->IsPlayerCreature(m_pCreature))
    {
        m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->SetCanDownSample(0);
    }
    if (cBodyTexture != "")
    {
        m_pCreature->ReplaceTexture(CSWCAnimBase::MODEL_SLOT_MAIN, "", cBodyTexture, 1);
    }
    m_nAppearance = pAppearance->m_nAppearance;
    m_nUnknown40 = 1;
    m_nUnknown44 = 1;
    m_nUnknown48 = 1;
    SwitchWeaponEvent(0, "AppearanceForceUpdate", m_pCreature);
    if (pHeadModel)
    {
        Gob* pMainModel = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
        pHeadModel->AttachToObject(pMainModel, "HeadHook", Gob::ATTACH_NORMAL);
    }
    m_nUnknown50 = 1;
    m_bForceAnimate = g_pAppManager->m_pClientExoApp->GetPausedByCombat();
    return 1;
}
// PC 0x006F2330
void CSWCCreatureAppearance::SetAppearanceAlignment(short nAlignment)
{
    if (m_nUnknown50 == 0 && m_nAlignment == nAlignment)
    {
        return;
    }

    m_nAlignment = nAlignment;
    if (m_pCreature == 0 ||
        m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 0) == 0)
    {
        return;
    }

    CExoString sHeadTexture;
    CExoString sBodyTexture;
    int bHeadTexture = 0;
    int bBodyTexture = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

    if (pAppearance2DA == 0)
    {
        return;
    }

    int nHead;

    m_nUnknown50 = 0;
    if (m_nUnknown34 != 0xFF)
    {
        nHead = m_nUnknown34;
        m_nUnknown40 = 1;
    }
    else if (m_nUnknown30 != 0)
    {
        pAppearance2DA->GetINTEntry(m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_BACKUPHEAD, &nHead);
    }
    else
    {
        pAppearance2DA->GetINTEntry(m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_NORMALHEAD, &nHead);
    }

    if (nAlignment <= 10)
    {
        CExoString sColumn("HEADTEXVVVE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pHeads = p2DArrays->m_pHeads;

        bHeadTexture = pHeads->GetCExoStringEntry(nHead, sColumn, &sHeadTexture);
    }
    else if (nAlignment <= 20)
    {
        CExoString sColumn("HEADTEXVVE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pHeads = p2DArrays->m_pHeads;

        bHeadTexture = pHeads->GetCExoStringEntry(nHead, sColumn, &sHeadTexture);
    }
    else if (nAlignment <= 30)
    {
        CExoString sColumn("HEADTEXVE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pHeads = p2DArrays->m_pHeads;

        bHeadTexture = pHeads->GetCExoStringEntry(nHead, sColumn, &sHeadTexture);
    }
    else if (nAlignment <= 40)
    {
        CExoString sColumn("HEADTEXE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pHeads = p2DArrays->m_pHeads;

        bHeadTexture = pHeads->GetCExoStringEntry(nHead, sColumn, &sHeadTexture);
    }

    if (m_bUseBodyVariation == 0 && nAlignment <= 30)
    {
        CExoString sColumn("TEXAEVIL");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

        bBodyTexture = pAppearance2DA->GetCExoStringEntry(m_nAppearance,
                                                          sColumn,
                                                          &sBodyTexture);
    }

    if (bHeadTexture)
    {
        if (g_pExoResMan->Exists(sHeadTexture, RESTYPE_TGA,
                                 (unsigned long*)0) ||
            g_pExoResMan->Exists(sHeadTexture, RESTYPE_TPC,
                                 (unsigned long*)0))
        {
            m_pCreature->ReplaceTexture(CSWCAnimBase::MODEL_SLOT_HEAD, "",
                                        sHeadTexture, 1);
        }
    }
    else
    {
        m_pCreature->RestoreTexture(CSWCAnimBase::MODEL_SLOT_HEAD);
    }

    if (bBodyTexture)
    {
        char sBuffer[32];

        sprintf(sBuffer, "%s01", sBodyTexture.CStr());
        sBodyTexture = sBuffer;
        if (g_pExoResMan->Exists(sBodyTexture, RESTYPE_TGA,
                                 (unsigned long*)0) ||
            g_pExoResMan->Exists(sBodyTexture, RESTYPE_TPC,
                                 (unsigned long*)0))
        {
            m_pCreature->ReplaceTexture(CSWCAnimBase::MODEL_SLOT_MAIN, "",
                                        sBodyTexture, 1);
        }
    }
    else
    {
        CResRef cTexture;

        GetBTypeModelAndTextureResRef(this, 0, &cTexture);
        if (cTexture != "")
        {
            m_pCreature->ReplaceTexture(CSWCAnimBase::MODEL_SLOT_MAIN, "",
                                        cTexture, 1);
        }

        if (g_pAppManager->m_pServerExoApp == 0 ||
            g_pAppManager->GetClientExoApp()->IsPlayerCreature(m_pCreature))
        {
            m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)
                ->SetCanDownSample(0);
        }
    }
}
// PC 0x006F2D30
int CSWCCreatureAppearance::CreateSimple(unsigned char nType,
                                       CCAppearanceInfo* pAppearance)
{
    CResRef cModel;
    CResRef cEnvMap;
    CExoString sRace;
    CExoString sEntry;

    {
        CExoString sColumn("RACE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance,
                                          sColumn, &sRace);
    }
    {
        CExoString sColumn("ENVMAP");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance,
                                          sColumn, &sEntry);
    }
    int bEnvMap;
    if (sEntry.GetLength() > 0)
    {
        cEnvMap = sEntry;
        bEnvMap = 1;
    }
    else
    {
        bEnvMap = 0;
    }
    cModel = sRace;
    if (m_nAppearance == pAppearance->m_nAppearance)
    {
        return 1;
    }

    List<CAurObject*> aModels;
    Gob* pItem = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_RIGHT_ITEM, 1);
    if (pItem != 0)
    {
        pItem->AttachToScene(0);
        pItem->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
    }
    pItem = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_LEFT_ITEM, 1);
    if (pItem != 0)
    {
        pItem->AttachToScene(0);
        pItem->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
    }
    m_pCreature->UnloadMonkVisuals();
    int bAnimationCallback = m_pCreature->ClearQuededAnimationCallback();
    m_pCreature->UnloadModel(CSWCAnimBase::MODEL_SLOT_MAIN);
    m_pCreature->UpdateMonkVisuals();
    if (!m_pCreature->LoadModel(cModel, CSWCAnimBase::MODEL_SLOT_MAIN, nType))
    {
        return 0;
    }
    if (bAnimationCallback)
    {
        m_pCreature->SetQuededAnimationCallback();
    }
    if (bEnvMap)
    {
        m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_MAIN, 1, 1, cEnvMap);
    }
    else
    {
        m_pCreature->SetEnvMap(CSWCAnimBase::MODEL_SLOT_MAIN, 0, 1, "DEFAULT");
    }
    if (g_pAppManager->GetClientExoApp()->GetPlayerCreature() == m_pCreature)
    {
        m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->SetCanDownSample(0);
    }
    CCAppearanceInfo::operator=(*pAppearance);
    m_nUnknown40 = 1;
    m_nUnknown44 = 1;
    m_nUnknown48 = 1;

    CExoString sModelType;
    {
        CExoString sColumn("MODELTYPE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance,
                                          sColumn, &sModelType);
    }
    nType = GetAnimType(sModelType);
    if ((nType == CSWCAnimBase::ANIMATION_TYPE_WIELD ||
         nType == CSWCAnimBase::ANIMATION_TYPE_BTYPE) &&
        sModelType.Find('L', 0) == -1)
    {
        if (m_pCreature->GetAnimationBase() != 0)
        {
            m_pCreature->GetAnimationBase()->SetWieldLimited(0);
        }
        m_pCreature->SetCreatureType(CSWCCreature::CREATURE_TYPE_UNLIMITED_WIELD);
    }
    else
    {
        if (0 != m_pCreature->GetAnimationBase())
        {
            m_pCreature->GetAnimationBase()->SetWieldLimited(1);
        }
        m_pCreature->SetCreatureType(CSWCCreature::CREATURE_TYPE_LIMITED_WIELD);
    }
    if (nType != CSWCAnimBase::ANIMATION_TYPE_BTYPE)
    {
        int bRaceTex;
        {
            CExoString sColumn("RaceTex");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
            bRaceTex = pAppearance2DA->GetCExoStringEntry(
                pAppearance->m_nAppearance, sColumn, &sEntry);
        }
        if (bRaceTex &&
            (g_pExoResMan->Exists(sEntry, RESTYPE_TGA, (unsigned long*)0) ||
             g_pExoResMan->Exists(sEntry, RESTYPE_TPC, (unsigned long*)0)))
        {
            m_pCreature->ReplaceTexture("", sEntry, 1);
        }
    }
    m_pCreature->UpdateWeaponClass();
    SwitchWeaponEvent(0, "AppearanceForceUpdate", m_pCreature);
    m_bForceAnimate = g_pAppManager->GetClientExoApp()->GetPausedByCombat();
    return 1;
}
// PC 0x006F0F10
unsigned char CSWCCreatureAppearance::GetAnimType(const CExoString& sModelType)
{
    int nResult;
    bool bBType;
    bool bSimple;

    nResult = sModelType.Find('S', 0);
    if (nResult != -1)
    {
        return 11;
    }

    {
        bBType = sModelType.Find("B", 0) != -1;

        if (bBType)
        {
            return 3;
        }

        nResult = sModelType.Find('F', 0);
        if (nResult != -1 || (nResult = sModelType.Find('L', 0),
                              bSimple = nResult == -1, nResult = 11, !bSimple))
        {
            return 2;
        }

        return nResult;
    }
}
// PC 0x006F0FE0
int CSWCCreatureAppearance::RequestSimple(CCAppearanceInfo* pAppearance)
{
    CExoString sResRef;
    CExoString sModelType;

    {
        CExoString sColumn("RACE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance, sColumn,
                                           &sResRef);
    }
    {
        CExoString sColumn("MODELTYPE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance, sColumn,
                                           &sModelType);
    }

    unsigned char nAnimType = GetAnimType(sModelType);

#ifdef _XBOX
    m_pCreature->RequestModel(sResRef, "", nAnimType);
#else
    {
        CResRef cTexture("");
        CResRef cModel(sResRef);

        m_pCreature->RequestModel(cModel, cTexture, nAnimType);
    }
#endif
    return 1;
}
// PC 0x006F1130
int CSWCCreatureAppearance::RequestHeadAppearance(CCAppearanceInfo* pAppearance)
{

    CExoString sModel;
    CExoString sTexture;
    CExoString sEntry;

    {
        CExoString sColumn("MODELTYPE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance, sColumn,
                                           &sEntry);
    }

    if (GetAnimType(sEntry) == CSWCAnimBase::ANIMATION_TYPE_BTYPE)
    {
        if (pAppearance->m_nHead == 0 &&
            pAppearance->m_nUnmapped10 == CGameObjectArray::INVALIDOBJID)
        {
            return 1;
        }
        if (pAppearance->m_nUnmapped10 == CGameObjectArray::INVALIDOBJID)
        {
            sModel = "P";
            int nGender = pAppearance->m_nGender == CCAppearanceInfo::GENDER_FEMALE;
            {
                CExoString sColumn("GENDER");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pGender2DA = p2DArrays->m_pGender;
                pGender2DA->GetCExoStringEntry(nGender, sColumn, &sEntry);
            }
            sModel = sModel + sEntry;
            {
                CExoString sColumn("RACE");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
                pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance,
                                                   sColumn, &sEntry);
            }
            sModel = sModel + sEntry;
            sTexture = sModel;
            sModel = sModel + CExoString(pAppearance->m_nHeadVariation);
            sModel = sModel + "_HEAD";
            sTexture = sTexture + "0_HEAD";

            char szHead[4];
            sprintf(szHead, "%03u", pAppearance->m_nHead);
            sModel = sModel + szHead;
            sTexture = sTexture + szHead;

            {
                CExoString sColumn("MODELTYPE");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
                pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance,
                                                   sColumn, &sEntry);
            }
            unsigned char nAnimType = GetAnimType(sEntry);
            m_pCreature->RequestModel(sModel, sTexture, nAnimType);
        }
    }
    return 1;
}
// PC 0x006F1580
float CSWCCreatureAppearance::GetHeight()
{
    Vector vPosition;
    Quaternion qOrientation;
    Gob* pModel = m_pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);

    if (pModel != 0)
    {
        pModel->GetPartPosition("HEAD_G", &vPosition, &qOrientation);
        if (vPosition.m_fZ == 0.0f)
        {
            return m_fDefaultHeight;
        }

        float fHeight = vPosition.m_fZ - m_pCreature->m_vPosition.m_fZ;
        if (fHeight < 0.0f)
        {
            return m_fDefaultHeight;
        }

        return fHeight;
    }

    return 0.0f;
}
// PC 0x006F1630
CAurObject* CSWCCreatureAppearance::GetCameraFollowPart()
{
    Gob* pModel = m_pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    Part* pPart = pModel->GetPart("CAMERAHOOK");

    if (pPart != 0)
    {
        return pPart->GetAurObject();
    }

    return pModel;
}
// PC 0x006F1670
int CSWCCreatureAppearance::GetFollowingBody()
{
    Gob* pModel = m_pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);

    return pModel->GetPart("CAMERAHOOK") != 0;
}
// PC 0x006F16A0
float CSWCCreatureAppearance::GetHeadHeightOffset()
{
    Gob* pModel = m_pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);

    if (pModel != NULL)
    {
        if (pModel->GetPart("CAMERAHOOK") != NULL)
        {
            Vector vPosition;
            Quaternion qOrientation;
            pModel->GetPartLocalPosition("CAMERAHOOK", &vPosition, &qOrientation);
            return vPosition.m_fZ;
        }

        return GetHeight();
    }
    return 0.0f;
}
// PC 0x006F1750
float CSWCCreatureAppearance::GetCameraOffset()
{
    float fOffset = 0.0f;
    C2DA* pAppearance = g_pRules->m_p2DArrays->m_pAppearance;

    if (pAppearance->GetFLOATEntry(
            m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_CAMERAHEIGHTOFFSET,
            &fOffset) != 0)
    {
        return fOffset;
    }

    return 0.0f;
}
// PC 0x006F17A0
float CSWCCreatureAppearance::GetRunLooksAwfulRate()
{
    float fRunDist;

    g_pRules->m_p2DArrays->m_pAppearance->GetFLOATEntry(
        m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_RUNDIST, &fRunDist);
    return (fRunDist * 1000.0f) * 0.37f;
}
// PC 0x006F17E0
void CSWCCreatureAppearance::GetBTypeModelAndTextureResRef(
    CCAppearanceInfo* pAppearance, CResRef* pModel, CResRef* pTexture)
{
    char sModelColumn[] = "ModelA";
    char sTextureColumn[] = "TEXA";
    int nBodyVariation = max(1, min(10, pAppearance->m_nBodyVariation));
    int nTextureVar = max(1, pAppearance->m_nTextureVar);

    if (pAppearance->m_bUseBodyVariation)
    {
        char cBodyVariation = 'A' + nBodyVariation - 1;
        sModelColumn[5] = cBodyVariation;
        sTextureColumn[3] = cBodyVariation;
    }

    CExoString sEntry;

    if (pModel != 0)
    {
        bool bMissing;

        {
            CExoString sColumn(sModelColumn);
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

            bMissing = !pAppearance2DA->GetCExoStringEntry(
                pAppearance->m_nAppearance, sColumn, &sEntry);
        }
        if (bMissing)
        {
            sEntry = "";
        }
        *pModel = sEntry;
    }

    if (pTexture != 0)
    {
        int bFound;
        {
            CExoString sColumn(sTextureColumn);
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
            bFound = pAppearance2DA->GetCExoStringEntry(
                pAppearance->m_nAppearance, sColumn, &sEntry);
        }
        if (bFound)
        {
            CResRef cTexture;
            CExoString sTexture;

            sTexture.Format("%s%02d", sEntry.CStr(), nTextureVar);
            cTexture = sTexture;
            if (!g_pExoResMan->Exists(cTexture, RESTYPE_TGA, (unsigned long*)0) &&
                !g_pExoResMan->Exists(cTexture, RESTYPE_TPC, (unsigned long*)0))
            {
                sTexture.Format("%s%02d", sEntry.CStr(), 1);
                cTexture = sTexture;
            }
            *pTexture = cTexture;
        }
        else
        {
            *pTexture = "";
        }
    }
}
// PC 0x006F1A70
void CSWCCreatureAppearance::ForceAnimate()
{
    if (m_bForceAnimate)
    {
        Gob* pModel = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
        if (pModel != 0)
        {
            pModel->EnableForceAnimations();
            pModel->AnimateExternal(1.0f);
            pModel->DisableForceAnimations();
        }

        pModel = m_pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
        if (pModel != 0)
        {
            pModel->EnableForceAnimations();
            pModel->AnimateExternal(1.0f);
            pModel->DisableForceAnimations();

            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(m_nUnmapped10);
            if (pItem != 0)
            {
                pModel = pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
                if (pModel != 0)
                {
                    pModel->EnableForceAnimations();
                    pModel->AnimateExternal(1.0f);
                    pModel->DisableForceAnimations();
                }
            }
        }

        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(
            m_pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
        if (pItem != 0)
        {
            pModel = pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
            if (pModel != 0)
            {
                pModel->EnableForceAnimations();
                pModel->AnimateExternal(1.0f);
                pModel->DisableForceAnimations();
            }
        }

        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pItem = pClientExoApp->GetItemByGameObjectID(
            m_pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
        if (pItem != 0)
        {
            pModel = pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
            if (pModel != 0)
            {
                pModel->EnableForceAnimations();
                pModel->AnimateExternal(1.0f);
                pModel->DisableForceAnimations();
            }
        }
    }
}
// PC 0x006F3230
int CSWCCreatureAppearance::RequestBodyAppearance(CCAppearanceInfo* pAppearance)
{
    CExoString sModelType;
    int bResult;

    {
        CExoString sColumn("MODELTYPE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;

        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance, sColumn,
                                           &sModelType);
    }

    switch (GetAnimType(sModelType))
    {
    case 2:
    case 11:
        bResult = RequestSimple(pAppearance);
        break;
    case 3:
        bResult = RequestBType(pAppearance);
        break;
    default:
        bResult = 0;
        break;
    }

    return bResult;
}
// PC 0x006F3300
int CSWCCreatureAppearance::CreateBodyAppearance(CCAppearanceInfo* pAppearance)
{
    CExoString sModelType;

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetFLOATEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEACCl, &m_fDriveAcceleration);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetFLOATEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEMAXSPEED, &m_fDriveMaxSpeed);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetFLOATEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEANIMWALK, &m_fDriveAnimWalk);
    }
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetFLOATEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEANIMRUN, &m_fDriveAnimRun);
    }

    if (m_fDriveAcceleration == 0.0f)
    {
        m_fDriveAcceleration = 25.0f;
    }
    if (m_fDriveMaxSpeed == 0.0f)
    {
        m_fDriveMaxSpeed = 6.0f;
    }
    if (m_fDriveAnimWalk == 0.0f)
    {
        m_fDriveAnimWalk = 2.0f;
    }
    if (m_fDriveAnimRun == 0.0f)
    {
        m_fDriveAnimRun = 4.0f;
    }

    CSWSCreature* pServerCreature = m_pCreature->GetServerCreature();
    if (pServerCreature != 0)
    {
        m_fDriveMaxSpeed *= pServerCreature->GetMovementRateFactor();
    }

    float fValue;
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetFLOATEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_PERSPACE, &fValue);
    }
    m_fCameraHeight = fValue;
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetCExoStringEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_MODELTYPE, &sModelType);
    }
    unsigned char nAnimType = GetAnimType(sModelType);
    int bResult;
    switch (nAnimType)
    {
    case CSWCAnimBase::ANIMATION_TYPE_WIELD:
    case CSWCAnimBase::ANIMATION_TYPE_TAIL_WINGS:
        bResult = CreateSimple(nAnimType, pAppearance);
        break;
    case CSWCAnimBase::ANIMATION_TYPE_BTYPE:
        bResult = CreateBTypeBody(nAnimType, pAppearance);
        break;
    default:
        bResult = 0;
        break;
    }

    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetFLOATEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_HEIGHT, &fValue);
    }
    m_fDefaultHeight = fValue;
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        pAppearance2DA->GetINTEntry(pAppearance->m_nAppearance,
            CTwoDimArrays::COL2DA_APPEARANCETABLE_GROUNDTILT, &m_bGroundTilt);
    }

    if (bResult)
    {
        if ((nAnimType == CSWCAnimBase::ANIMATION_TYPE_WIELD ||
             nAnimType == CSWCAnimBase::ANIMATION_TYPE_BTYPE) &&
            sModelType.Find("L", 0) == -1)
        {
            if (m_pCreature->GetAnimationBase() != 0)
            {
                m_pCreature->GetAnimationBase()->SetWieldLimited(0);
            }
            m_pCreature->SetCreatureType(CSWCCreature::CREATURE_TYPE_UNLIMITED_WIELD);
        }
        else
        {
            if (m_pCreature->GetAnimationBase() != 0)
            {
                m_pCreature->GetAnimationBase()->SetWieldLimited(1);
            }
            m_pCreature->SetCreatureType(CSWCCreature::CREATURE_TYPE_LIMITED_WIELD);
        }
        m_pCreature->UpdateWeaponClass();
    }
    return bResult;
}
