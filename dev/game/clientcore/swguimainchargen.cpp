#include <string.h>
#include "aurcamera.h"

#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwccreature.h"
#include "swgui.h"
#include "swguimainchargen.h"
#include "swguiportraitchargen.h"
#include "swguiquickpanel.h"
#include "swguiclassselection.h"
#include "swguicustompanel.h"

#include "aurscene.h"
#include "nwccreaturestats.h"
#include "nwclass.h"
#include "swguiportrait.h"

void CSWGuiMainCharGen::RemoveSubPanels()  // PC 0x0075B210 GOG 0x0075B100
{
    m_pManager->PopModalPanel();
#ifndef _XBOX
    if (m_pManager->PanelExists(m_pCustom) != 0)
#endif
    {
        CSWGuiPanel* pPanel = m_pCustom;
        pPanel->m_bDeferredRemoval = 0;
        pPanel->m_nExitState = EXITSTATE_DELETE;
    }
#ifndef _XBOX
    else
    {
        delete m_pCustom;
        m_pCustom = 0;
    }
    if (m_pManager->PanelExists(m_pQuickOrCustom) != 0)
#endif
    {
        CSWGuiPanel* pPanel = m_pQuickOrCustom;
        pPanel->m_bDeferredRemoval = 0;
        pPanel->m_nExitState = EXITSTATE_DELETE;
    }
#ifndef _XBOX
    else
    {
        delete m_pQuickOrCustom;
        m_pQuickOrCustom = 0;
    }
    if (m_pManager->PanelExists(m_pQuick) != 0)
#endif
    {
        CSWGuiPanel* pPanel = m_pQuick;
        pPanel->m_bDeferredRemoval = 0;
        pPanel->m_nExitState = EXITSTATE_DELETE;
    }
#ifndef _XBOX
    else
    {
        delete m_pQuick;
        m_pQuick = 0;
    }
#endif
    m_pManager->PopModalPanel();
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
}
// PC 0x0075B2F0 GOG 0x0075B030
void CSWGuiMainCharGen::SetVisiblePanel(int nPanel)
{
    if (nPanel != m_nVisiblePanel)
    {
        m_pManager->PopModalPanel();
        switch (m_nVisiblePanel)
        {
        case 1:
            m_pManager->RemovePanel(m_pQuickOrCustom);
            break;
        case 2:
            m_pManager->RemovePanel(m_pQuick);
            break;
        case 3:
            m_pManager->RemovePanel(m_pCustom);
            break;
        default:
            m_nVisiblePanel = 4;
            break;
        }
        if (m_nVisiblePanel != 4)
        {
            switch (nPanel)
            {
            case 1:
                m_pManager->AddPanel(m_pQuickOrCustom, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                break;
            case 2:
                m_pManager->AddPanel(m_pQuick, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                break;
            case 3:
                m_pManager->AddPanel(m_pCustom, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                break;
            default:
                m_nVisiblePanel = 4;
                break;
            }
            if (m_nVisiblePanel != 4)
            {
                m_nVisiblePanel = nPanel;
            }
        }
    }
}
// PC 0x0075B3A0 GOG 0x0075B1E0
void CSWGuiMainCharGen::DetermineClass()
{
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;

    m_sClass = pStats->GetClassesString(' ');
    m_sClass = m_sClass.Left(m_sClass.Find('(', 0));
}

#ifndef _XBOX
void CSWGuiMainCharGen::SetNameString(const CExoString& sName)  // PC 0x0075B440 GOG 0x0075B280
{
    m_sName = sName;
    m_lblName.m_cText.m_cParams.SetText(m_sName);
    m_lblClass.m_cText.m_cParams.SetText(m_sClass);
}
#endif
// PC 0x0075B480 GOG 0x0075B2C0
void CSWGuiMainCharGen::PortraitChanged()
{
    SetPortraitImage(&m_lblPortrait, m_pCreature->GetPortrait(0));
    m_lblPortrait.m_cText.m_cParams.SetText("");
    CAurCamera* pCamera = m_viewModel.m_cScene.m_pCamera;
    CCAppearanceInfo cAppearance = *m_pCreature->m_pAppearance;
    cAppearance.m_nAppearance = m_pCreature->m_pStats->m_nAppearance;
    if (g_bDo3DGUIs != 0)
    {
        m_pCreature->SetAppearance(&cAppearance, 3, 1);
        m_pCreature->AttachToScene(m_viewModel.m_cScene.m_pScene);
        m_pCreature->SetPosition(Vector());
        CAurBehaviorAttachTranslate* pNode = (CAurBehaviorAttachTranslate*)pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
        if (pNode != 0)
        {
            pNode->m_nWorldSpace = 1;
            pNode->m_vTranslation =
                Vector(0.0f, 0.0f, m_pCreature->GetHeadHeightOffset());
        }
    }
    Vector vCamera(2.5f, 0.0f, m_pCreature->GetHeadHeightOffset());
    pCamera->SetPosition(vCamera);
    vCamera -= Vector(0.0f, 0.0f, 1.0f);
    vCamera.Normalize();
    pCamera->SetOrientation(
        YawPitchRoll(Yaw(vCamera) + 180.0f, 90.0f - Pitch(vCamera), 0.0f));
}

inline unsigned char& CSWCLevelUpStats::MainCharGenAbility(int nAbility)
{
    return m_aAbilities[nAbility];
}
// PC 0x0075B6C0 GOG 0x0075B500
void CSWGuiMainCharGen::AbilitiesChanged()
{
    CSWClass* pClass;
    CSWCLevelUpStats* pStats;

    m_pCreature->m_pStats->SetFortSavingThrow();
    m_pCreature->m_pStats->SetWillSavingThrow();
    m_pCreature->m_pStats->SetReflexSavingThrow();
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->MainCharGenAbility(CSWCCreatureStats::ABILITY_CHARISMA));
        m_lblCharismaAbility.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->MainCharGenAbility(CSWCCreatureStats::ABILITY_STRENGTH));
        m_lblStrengthAbility.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->MainCharGenAbility(CSWCCreatureStats::ABILITY_INTELLIGENCE));
        m_lblIntelligenceAbility.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->MainCharGenAbility(CSWCCreatureStats::ABILITY_WISDOM));
        m_lblWisdomAbility.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->MainCharGenAbility(CSWCCreatureStats::ABILITY_CONSTITUTION));
        m_lblConstitutionAbility.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->MainCharGenAbility(CSWCCreatureStats::ABILITY_DEXTERITY));
        m_lblDexterityAbility.m_cText.m_cParams.SetText(sText);
    }
    pStats = m_pCreature->m_pStats;
    unsigned char nDexterity = pStats->m_nDexterity;
    int nDefense = (char)(pStats->CalcStatModifier(nDexterity) +
                          pStats->m_nDexterityBonus) +
                   10;
    for (int i = 0; i < m_pCreature->m_pStats->m_nClassCount; i++)
    {
        pClass = &g_pRules->m_pClasses[m_pCreature->m_pStats->GetClass(i)];
        pStats = m_pCreature->m_pStats;
        unsigned char nLevel = pStats->GetClassLevel(i);
        nDefense += pClass->GetArmorBonus(nLevel);
    }
    pStats = m_pCreature->m_pStats;
    pStats->m_nDefenseBase = nDefense;
    CSWCLevelUpStats* pStats2 = m_pCreature->m_pStats;
    unsigned char nHitDie = pClass->m_nHitDie;
    short nHitPoints = pStats2->m_nHitPointsBase;
    unsigned char nConstitution = pStats2->m_nConstitution;
    int nVitality =
        nHitDie + nHitPoints + pStats2->CalcStatModifier(nConstitution);
    {
        short nArmorClass = m_pCreature->m_pStats->GetArmorClass(0);
        CExoString sText(nArmorClass);
        m_lblDefense.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->m_nFortitudeSave);
        m_lblNewFortitude.m_cText.m_cParams.SetText(sText);
    }
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->m_nReflexSave);
        m_lblNewReflex.m_cText.m_cParams.SetText(sText);
    }
    m_lblVitality.m_cText.m_cParams.SetText(nVitality);
    {
        pStats = m_pCreature->m_pStats;
        CExoString sText(pStats->m_nWillSave);
        m_lblNewWill.m_cText.m_cParams.SetText(sText);
    }
}
// PC 0x0075BAA0 GOG 0x0075B8E0
void CSWGuiMainCharGen::ClearAbilities()
{
    m_lblCharismaAbility.m_cText.m_cParams.SetText("");
    m_lblStrengthAbility.m_cText.m_cParams.SetText("");
    m_lblIntelligenceAbility.m_cText.m_cParams.SetText("");
    m_lblWisdomAbility.m_cText.m_cParams.SetText("");
    m_lblConstitutionAbility.m_cText.m_cParams.SetText("");
    m_lblDexterityAbility.m_cText.m_cParams.SetText("");
    m_pCreature->m_pStats->SetSTRBase(8);
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    pStats->SetINTBase(8);
    m_pCreature->m_pStats->SetWISBase(8);
    m_pCreature->m_pStats->SetCONBase(8);
    m_pCreature->m_pStats->SetDEXBase(8, 1);
    m_pCreature->m_pStats->SetCHABase(8);
    m_pCreature->m_pStats->SetFortSavingThrow();
    m_pCreature->m_pStats->SetWillSavingThrow();
    m_pCreature->m_pStats->SetReflexSavingThrow();
    m_lblDefense.m_cText.m_cParams.SetText("");
    m_lblNewFortitude.m_cText.m_cParams.SetText("");
    m_lblNewReflex.m_cText.m_cParams.SetText("");
    m_lblVitality.m_cText.m_cParams.SetText("");
    m_lblNewWill.m_cText.m_cParams.SetText("");
    m_pCreature->m_pStats->ClearPackage();
}
#ifndef _XBOX
// PC 0x0075BDB0 GOG 0x0075BBF0
void CSWGuiMainCharGen::ClearFeats()
{
    m_pCreature->m_pStats->ClearFeats();
}
#endif
// PC 0x0075BDC0 GOG 0x0075BC00
void CSWGuiMainCharGen::BeginPlayGame()
{
    RemoveSubPanels();
    m_pClassSelection->BeginPlayGame();
}
// PC 0x0075BDE0 GOG 0x0075BC20
void CSWGuiMainCharGen::Draw(float fTimeStep)
{
    if (m_pCreature != 0 && g_bDo3DGUIs != 0 && (m_bNeedsDraw) != 0)
    {
        CAurObject* pHead = m_pCreature->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
        CAurScene* pScene = m_viewModel.m_cScene.m_pScene;
        if (((Gob*)pHead)->GetScene() != pScene)
        {
            m_pCreature->AttachToScene(m_viewModel.m_cScene.m_pScene);
            CAurBehaviorAttachTranslate* pNode =
                (CAurBehaviorAttachTranslate*)m_viewModel.m_cScene.m_pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
            if (pNode != 0)
            {
                pNode->m_nWorldSpace = 1;
                pNode->m_vTranslation =
                    Vector(0.0f, 0.0f, m_pCreature->GetHeadHeightOffset());
            }
        }
        m_pCreature->CGPauseCycle(fTimeStep, 0);
    }
    return CSWGuiPanel::Draw(fTimeStep);
}
#ifndef _XBOX
void CSWGuiMainCharGen::OnPanelAdded()  // PC 0x0075BEC0 GOG 0x0075B0E0
{
    m_pManager->AddPanel(m_pQuickOrCustom, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    CSWGuiPanel::OnPanelAdded();
}
#endif
// PC 0x0075BEE0 GOG 0x0075BD00
CSWGuiMainCharGen::CSWGuiMainCharGen(CSWGuiManager* pManager,
                                     CSWCCreature* pCreature,
                                     CSWGuiClassSelection* pClassSelection)
    : CSWGuiPanel(pManager)
{
    m_pClassSelection = pClassSelection;
    m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    m_pCreature = pCreature;
    DetermineClass();
#ifdef _XBOX
    StartLoadFromLayout("MAINCG");
#else
    StartLoadFromLayout("MAINCG_p");
#endif
    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblClass, "LBL_CLASS", 1);
    InitControl(m_lblStrength, "STR_LBL", 1);
    InitControl(m_lblDexterity, "DEX_LBL", 1);
    InitControl(m_lblConstitution, "CON_LBL", 1);
    InitControl(m_lblIntelligence, "INT_LBL", 1);
    InitControl(m_lblWisdom, "WIS_LBL", 1);
    InitControl(m_lblCharisma, "CHA_LBL", 1);
    InitControl(m_lblStrengthAbility, "STR_AB_LBL", 1);
    InitControl(m_lblDexterityAbility, "DEX_AB_LBL", 1);
    InitControl(m_lblConstitutionAbility, "CON_AB_LBL", 1);
    InitControl(m_lblIntelligenceAbility, "INT_AB_LBL", 1);
    InitControl(m_lblWisdomAbility, "WIS_AB_LBL", 1);
    InitControl(m_lblCharismaAbility, "CHA_AB_LBL", 1);
    InitControl(m_lblNewWill, "NEW_WILL_LBL", 1);
    InitControl(m_lblNewReflex, "NEW_REFL_LBL", 1);
    InitControl(m_lblNewFortitude, "NEW_FORT_LBL", 1);
    InitControl(m_lblBevelLeft, "LBL_BEVEL_L", 1);
    InitControl(m_lblBevelMiddle, "LBL_BEVEL_M", 1);
    InitControl(m_lblBevelRight, "LBL_BEVEL_R", 1);
    InitControl(m_lblFortitude, "LBL_FORTITUDE", 1);
    InitControl(m_lblReflex, "LBL_REFLEX", 1);
    InitControl(m_lblWill, "LBL_WILL", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
#endif
    InitControl(m_lblVitality, "LBL_VIT", 1);
    InitControl(m_lblDefense, "LBL_DEF", 1);
#ifdef _XBOX
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
#endif
    InitControl(m_viewModel, "MODEL_LBL", 1);
    InitControl(m_lblPortrait, "PORTRAIT_LBL", 1);
#ifndef _XBOX
    InitControl(m_lblStatsBack, "LBL_STATSBACK", 1);
    InitControl(m_lblStatsBorder, "LBL_STATSBORDER", 1);
    InitControl(m_lblPortBorder, "LBL_PORTBORDER", 1);
    InitControl(m_lblVitName, "LBL_VIT_NAME", 1);
    InitControl(m_lblDefName, "LBL_DEF_NAME", 1);
    InitControl(m_lblBar, "LBL_BAR1", 1);
#endif
    StopLoadFromLayout();
    m_pQuickOrCustom = new CSWGuiQuickOrCustomPanel(m_pManager, m_pCreature);
    m_pQuickOrCustom->m_pMainCharGen = this;
    m_pQuick = new CSWGuiQuickPanel(m_pManager, m_pCreature);
    m_pQuick->m_pMainCharGen = this;
    m_pCustom = new CSWGuiCustomPanel(m_pManager, m_pCreature);
    m_pCustom->SetParentPanel(this);
    m_nVisiblePanel = 1;
    SetNameString("");
    SetPortraitImage(&m_lblPortrait, m_pCreature->GetPortrait(0));
    CAurScene* pScene = m_viewModel.m_cScene.m_pScene;
    CAurCamera* pCamera = m_viewModel.m_cScene.m_pCamera;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        m_viewModel.m_cScene.AddModel("charrec_light", -1);
        m_pCreature->Setup3DScene(m_viewModel, 0, 0, 0);
        m_pCreature->AttachToScene(pScene);
        m_pCreature->SetPosition(Vector());
        CAurBehaviorAttachTranslate* pNode = (CAurBehaviorAttachTranslate*)pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
        if (pNode != 0)
        {
            pNode->m_nWorldSpace = 1;
            pNode->m_vTranslation =
                Vector(0.0f, 0.0f, m_pCreature->GetHeadHeightOffset());
        }
    }
}

#ifdef _XBOX

void CSWGuiMainCharGen::HandleInputEvent(int nEvent, int nState)
{
    if (nState && (nEvent == GUIEVENT_CANCEL || nEvent == GUIEVENT_CANCEL_ALTERNATE))
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        m_pManager->PopModalPanel();
        m_bDeferredRemoval = 0;
        m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

void CSWGuiMainCharGen::OnPanelAdded()
{
    m_pManager->AddPanel(m_pQuickOrCustom, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
#endif
