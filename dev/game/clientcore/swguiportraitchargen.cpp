#include <stdlib.h>
#include "aurcamera.h"

#include "exobase.h"
#include "exoresman.h"
#include "twodimarray.h"
#include "swguiquickpanel.h"

#include "nwccreature.h"
#include "swguicustompanel.h"
#include "aurscene.h"
#include "nwccreaturestats.h"
#include "nwscript.h"

#ifndef _XBOX
void CSWGuiPortraitCharGen::OnLeftArrowPressed(CSWGuiControl* pControl)  // PC 0x006D2FC0 GOG 0x006D2D50
{
    HandleInputEvent(GUIEVENT_LEFT, 1);
}
// PC 0x006D2FD0 GOG 0x006D2D60
void CSWGuiPortraitCharGen::OnRightArrowPressed(CSWGuiControl* pControl)
{
    HandleInputEvent(GUIEVENT_RIGHT, 1);
}
#endif
// PC 0x0076AAC0 GOG 0x0076A8E0
void CSWGuiPortraitCharGen::OnPanelAdded()
{
    int nRandom = rand();
    m_fBlinkTimer = (nRandom % 300) * 0.01f + 1.0f;
#ifdef _XBOX
    UpdatePortraitButton();
#endif
}
// PC 0x0076AAF0 GOG 0x0076A910
void CSWGuiPortraitCharGen::SetParentPanel(CSWGuiPanel* pParent,
                                           int nParentType)
{
    m_pParent = pParent;
    m_nParentType = nParentType;
}
// PC 0x0076AB10 GOG 0x0076A930
void CSWGuiPortraitCharGen::OnSelectDisplayButton()
{
#ifdef _XBOX
    m_pCreature->SetPortrait("");
#else
    CResRef cPortrait("");

    m_pCreature->SetPortrait(cPortrait);
#endif
    m_pCreature->SetPortraitId(m_lstPortraits[m_nPortraitIndex]);
    m_pCreature->m_pStats->m_nAppearance =
        m_lstAppearances[m_nPortraitIndex];
    m_pManager->PopModalPanel();
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    if (m_nParentType == 1)
    {
        ((CSWGuiQuickPanel*)m_pParent)->PortraitCreationCompleted();
    }
    else if (m_nParentType == 2)
    {
        ((CSWGuiCustomPanel*)m_pParent)->PortraitCreationCompleted();
    }
}
// PC 0x0076ABC0 GOG 0x0076A9E0
void CSWGuiPortraitCharGen::UpdatePortraitButton()
{
    int* pnPortrait = &m_lstPortraits[m_nPortraitIndex];

    if (m_pCreature->m_pStats->m_nPortraitId != *pnPortrait)
    {
        m_pCreature->SetPortraitId(*pnPortrait);
        CCAppearanceInfo cAppearance = *m_pCreature->m_pAppearance;
        cAppearance.m_nAppearance = m_lstAppearances[m_nPortraitIndex];
        m_pCreature->SetAppearance(&cAppearance, 2, 1, 1);
        m_pCreature->m_pAppearance->m_nAppearance =
            m_lstAppearances[m_nPortraitIndex];
        if (g_bDo3DGUIs != 0)
        {
            CAurObject* pHead = m_pCreature->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
            if (pHead != 0)
            {
                ((Gob*)pHead)->PlayAnimation("pause1", 1.0f, 2, 0);
            }
            CAurObject* pBody = m_pCreature->GetAnimationBase()->GetModel(254);
            if (pBody != 0)
            {
                ((Gob*)pBody)->PlayAnimation("pause1", 1.0f, 2, 0);
            }
        }
    }
    CResRef* pPortrait = &m_pCreature->GetPortrait(0);
    m_lblPortrait.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
    if (g_bDo3DGUIs != 0)
    {
        m_pCreature->AttachToScene(m_viewHead.m_cScene.m_pScene);
        CAurBehaviorAttachTranslate* pNode = (CAurBehaviorAttachTranslate*)m_viewHead.m_cScene.m_pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
        if (pNode != 0)
        {
            pNode->m_nWorldSpace = 0;
            pNode->m_vTranslation =
                Vector(0.0f, m_pCreature->GetHeadHeightOffset(), 0.0f);
        }
    }
}
// PC 0x0076AD50 GOG 0x0076AB70
void CSWGuiPortraitCharGen::Draw(float fTimeStep)
{
    int nCount = m_lstTimedControls.num;

    for (int i = nCount - 1; i >= 0; i--)
    {
        m_lstTimedControls.element[i].m_fTimeLeft -= fTimeStep;
        if (m_lstTimedControls.element[i].m_fTimeLeft <= 0.0f)
        {
            m_lstTimedControls.element[i].m_pControl->SetActive(0);
            m_lstTimedControls.DelIndex(i);
        }
    }
    if (m_pCreature != 0 && g_bDo3DGUIs != 0)
    {
        CAurObject* pHead = m_pCreature->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
        CAurScene* pScene = m_viewHead.m_cScene.m_pScene;
        if (((Gob*)pHead)->GetScene() != pScene)
        {
            pScene = m_viewHead.m_cScene.m_pScene;
            m_pCreature->AttachToScene(pScene);
            CAurBehaviorAttachTranslate* pNode =
                (CAurBehaviorAttachTranslate*)m_viewHead.m_cScene.m_pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
            if (pNode != 0)
            {
                pNode->m_nWorldSpace = 0;
                pNode->m_vTranslation =
                    Vector(0.0f, m_pCreature->GetHeadHeightOffset(), 0.0f);
            }
        }
        m_fBlinkTimer -= fTimeStep;
        if (m_fBlinkTimer <= 0.0f)
        {
            CExoString sAnimation;
            CExoString sIdle("pause1");
            CAurObject* pFace = m_pCreature->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
            int nChoice = rand() % 2;
            switch (nChoice)
            {
            case 0:
                sAnimation = "pause2";
                break;
            case 1:
                sAnimation = "listen";
                break;
            }
            if (pFace != 0)
            {
                ((Gob*)pFace)->AddAnimationFlag(1);
                ((Gob*)pFace)->PlayAnimation(sAnimation.CStr(), 1.0f, 33, 0);
                ((Gob*)pFace)->PlayAnimation(sIdle.CStr(), 1.0f, 96, 0);
            }
            CAurObject* pEyes = m_pCreature->GetAnimationBase()->GetModel(254);
            if (pEyes != 0)
            {
                ((Gob*)pEyes)->AddAnimationFlag(1);
                ((Gob*)pEyes)->PlayAnimation(sAnimation.CStr(), 1.0f, 33, 0);
                ((Gob*)pEyes)->PlayAnimation(sIdle.CStr(), 1.0f, 96, 0);
            }
            m_fBlinkTimer = (rand() % 300) * 0.01f + 1.0f;
        }
    }
    return CSWGuiPanel::Draw(fTimeStep);
}
// PC 0x0076B030 GOG 0x0076AE50
void CSWGuiPortraitCharGen::HandleCancelButton()
{
    if (m_pParent != 0)
    {
        int nIndex = m_lstPortraits.IndexOf(m_nOriginalPortrait);
        m_nPortraitIndex = nIndex;
        UpdatePortraitButton();
        m_pManager->PopModalPanel();
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        if (m_nParentType == 1)
        {
            ((CSWGuiQuickPanel*)m_pParent)->CancelledByChild();
        }
        else if (m_nParentType == 2)
        {
            ((CSWGuiCustomPanel*)m_pParent)->CancelledByChild();
        }
    }
}

void CSWGuiPortraitCharGen::HandleLeftButton()
{
    m_nPortraitIndex--;
    if (m_nPortraitIndex < 0)
    {
        m_nPortraitIndex = m_lstPortraits.num - 1;
    }
    UpdatePortraitButton();
#ifdef _XBOX
    ShowTimedControl(&m_lblhArrL);
#endif
}
// PC 0x0076B0B0 GOG 0x0076AED0
void CSWGuiPortraitCharGen::HandleRightButton()
{
    int nCount = m_lstPortraits.num;

    m_nPortraitIndex++;
    if (m_nPortraitIndex >= nCount)
    {
        m_nPortraitIndex = 0;
    }
    UpdatePortraitButton();
#ifdef _XBOX
    ShowTimedControl(&m_lblhArrR);
#endif
}
// PC 0x0076B0E0 GOG 0x0076AF00
void CSWGuiPortraitCharGen::HandleInputEvent(int nEvent, int bPressed)
{
    if (bPressed != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnSelectDisplayButton();
            break;
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleCancelButton();
            break;
        case 47:
        case 53:
        case 63:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            HandleLeftButton();
            break;
        case 48:
        case 54:
        case 64:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            HandleRightButton();
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, bPressed);
}
// PC 0x0076B1E0 GOG 0x0076B000
void CSWGuiPortraitCharGen::GetPortraitsList(int nGender)
{
    C2DA* p2DA = new C2DA("PORTRAITS", 0);
#ifdef _XBOX
    EXOXBOXASSERT(p2DA->Load2DArray());
#else
    p2DA->Load2DArray();
#endif
    int nRows = p2DA->m_nNumRows;
    CExoString sBaseResRef;
    CResRef cResRef;

    CExoArrayList<int>* pPortraits = &m_lstPortraits;
    CExoArrayList<int>* pAppearances = &m_lstAppearances;

    pAppearances->num = 0;
    pPortraits->num = 0;
    for (int i = 0; i < nRows; i++)
    {
        int nSex = -1;
        int nAppearance = -1;
        p2DA->GetINTEntry(i, "Sex", &nSex);
        if (nSex == nGender)
        {
            int nForPC;
            p2DA->GetINTEntry(i, "ForPC", &nForPC);
            if (nForPC == 1)
            {
                switch (m_pCreature->GetStats()->GetClass(0))
                {
                case CLASS_TYPE_JEDICONSULAR:
                    p2DA->GetINTEntry(i, "Appearance_S", &nAppearance);
                    break;
                case CLASS_TYPE_JEDISENTINEL:
                    p2DA->GetINTEntry(i, "AppearanceNumber", &nAppearance);
                    break;
                case CLASS_TYPE_JEDIGUARDIAN:
                    p2DA->GetINTEntry(i, "Appearance_L", &nAppearance);
                    break;
                default:
                    p2DA->GetINTEntry(i, "AppearanceNumber", &nAppearance);
                    break;
                }
                p2DA->GetCExoStringEntry(i, "BaseResRef", &sBaseResRef);
                if (m_pCreature->GetStats()->m_nPortraitId == i)
                {
                    int nIndex = pPortraits->num;
                    m_nPortraitIndex = nIndex;
                }
                pPortraits->Add(i);
                pAppearances->Add(nAppearance);
            }
        }
    }
    delete p2DA;
}
// PC 0x0076B520 GOG 0x0076B340
CSWGuiPortraitCharGen::CSWGuiPortraitCharGen(CSWGuiManager* pManager,
                                             CSWCCreature* pCreature)
    : CSWGuiPanel(pManager)
{
    m_pCreature = pCreature;
#ifdef _XBOX
    StartLoadFromLayout("PORTCUST");
#else
    StartLoadFromLayout("PORTCUST_p");
#endif
    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
#ifdef _XBOX
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#endif
    InitControl(m_lblPortrait, "LBL_PORTRAIT", 1);
    InitControl(m_viewHead, "LBL_HEAD", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
#endif
    InitControl(m_lblBevel[0], "LBL_BEVEL_L", 1);
    InitControl(m_lblBevel[1], "LBL_BEVEL_M", 1);
    InitControl(m_lblBevel[2], "LBL_BEVEL_R", 1);
    InitControl(m_lblBevel[3], "LBL_BEVEL_T", 1);
    InitControl(m_lblBevel[4], "LBL_BEVEL_B", 1);
#ifdef _XBOX
    InitControl(m_lblhArrL, "LBLH_ARRL", 1);
    InitControl(m_lblhArrR, "LBLH_ARRR", 1);
#else
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnAccept, "BTN_ACCEPT", 1);
    InitControl(m_btnLeft, "BTN_ARRL", 1);
    InitControl(m_btnRight, "BTN_ARRR", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
    InitControl(m_alblBar[5], "LBL_BAR6", 1);
    InitControl(m_alblBar[6], "LBL_BAR7", 1);
    InitControl(m_alblBar[7], "LBL_BAR8", 1);
#endif
    StopLoadFromLayout();

    m_nOriginalPortrait = m_pCreature->GetStats()->m_nPortraitId;
    m_nPortraitIndex = 0;
    GetPortraitsList(m_pCreature->GetStats()->m_nGender);

    CAurScene* pScene = m_viewHead.m_cScene.m_pScene;
    CAurCamera* pCamera = m_viewHead.m_cScene.m_pCamera;
#ifndef _XBOX
    if (pCamera != 0)
    {
        pCamera->SetViewAngle(65.0f);
    }
#endif
    if (g_bDo3DGUIs != 0)
    {
        CExoString sCameraHook;
        sCameraHook.Format("camerahook%c",
                           m_pCreature->GetStats()->m_nGender == 0 ? 'm' : 'f');
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        CAurObject* pLight;
        pLight = m_viewHead.m_cScene.AddModel("cghead_light", -1);
        if (pLight != 0)
        {
            ((Gob*)pLight)->PlayAnimation("default", 1.0f, 0, 0);
            pCamera->AttachToObject(pLight, sCameraHook.CStr(), Gob::ATTACH_TRANSLATE);
            pCamera->SetViewAngle(22.726f);
        }
    }
#ifndef _XBOX
    CSWGuiButton* pButton = &m_btnBack;
    pButton->m_bFocusable = 0;
    pButton->AddEvent(39, this,
                      (CSWGuiEventHandler)&CSWGuiPanel::OnBButtonPressed);
    pButton = &m_btnAccept;
    pButton->m_bFocusable = 0;
    pButton->AddEvent(39, this,
                      (CSWGuiEventHandler)&CSWGuiPanel::OnAButtonPressed);
    pButton = &m_btnRight;
    pButton->m_bFocusable = 0;
    pButton->AddEvent(
        39, this,
        (CSWGuiEventHandler)&CSWGuiPortraitCharGen::OnRightArrowPressed);
    pButton = &m_btnLeft;
    pButton->m_bFocusable = 0;
    pButton->AddEvent(
        39, this,
        (CSWGuiEventHandler)&CSWGuiPortraitCharGen::OnLeftArrowPressed);
    UpdatePortraitButton();
#endif
    m_fBlinkTimer = 0.0f;
}

#ifdef _XBOX
void CSWGuiPortraitCharGen::ShowTimedControl(CSWGuiControl* pControl)
{
    int nCount = m_lstTimedControls.num;

    for (int i = 0; i < nCount; i++)
    {
        if (m_lstTimedControls.element[i].m_pControl == pControl)
        {
            m_lstTimedControls.element[i].m_fTimeLeft = 0.25f;
            return;
        }
    }
    m_lstTimedControls.SetSize(nCount + 1);
    m_lstTimedControls.element[nCount].m_pControl = pControl;
    m_lstTimedControls.element[nCount].m_fTimeLeft = 0.25f;
    pControl->SetActive(1);
}
#endif
