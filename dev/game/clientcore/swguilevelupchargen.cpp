#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwccreaturestats.h"
#include "nwmessage.h"
#include "nwclass.h"
#include "exobase.h"
#include "aurcamera.h"
#include "nwsobject.h"
#include "swguilevelupchargen.h"
#include "swguileveluppanel.h"

#include "nwccreature.h"
#include "guiingame.h"
#include "aurscene.h"

static inline void SwguilevelupchargenTrace3();
static inline void SwguilevelupchargenTrace2();
static inline void SwguilevelupchargenTrace1();
#define SWGUILEVELUPCHARGEN_TRACE() SwguilevelupchargenTrace3()
// PC 0x00756EA0
void CSWGuiLevelUpCharGen::OnPanelAdded()
{
#ifdef _XBOX
    m_pLevelUpPanel->SetCanCancel(m_nInputClass != 0);
    m_pManager->AddPanel(m_pLevelUpPanel, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#else
    m_pLevelUpPanel->SetCanCancel(0);
    m_pManager->AddPanel(m_pLevelUpPanel, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x00758490
CSWGuiLevelUpCharGen::CSWGuiLevelUpCharGen(CSWGuiManager* pManager,
                                         unsigned long nCreature,
                                         CSWCCreature* pCreature)
    : CSWGuiPanel(pManager)
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pOriginal = pApp->GetCreatureByGameObjectID(nCreature);
    m_pManager = pApp->GetSWGuiManager();
    m_pCreature = new CSWCCreature;
    m_nCreatureId = nCreature;
    CopyCreatureStats(pOriginal, m_pCreature);
    LevelUpCreature();
    DetermineClass();
    StartLoadFromLayout("MAINCG");
    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblClass, "LBL_CLASS", 1);
    InitControl(m_lblLevel, "LBL_LEVEL", 1);
    InitControl(m_lblLevelValue, "LBL_LEVEL_VAL", 1);
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
    InitControl(m_lblVitality, "LBL_VIT", 1);
    InitControl(m_lblDefense, "LBL_DEF", 1);
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
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
#endif
    InitControl(m_viewModel, "MODEL_LBL", 1);
    InitControl(m_lblPortrait, "PORTRAIT_LBL", 1);
    StopLoadFromLayout();
    m_lblTitle.m_cText.m_cParams.SetStrRef(1071);
    m_pLevelUpPanel = new CSWGuiLevelUpPanel(m_pManager, m_nCreatureId);
    m_pLevelUpPanel->m_pParent = this;
    SetNameString(m_pCreature->GetName());
    AbilitiesChanged();
    {
        CResRef* pPortrait = &pOriginal->GetPortrait(1);
        m_lblPortrait.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
    }
    CAurScene* pScene = m_viewModel.m_cScene.m_pScene;
    CAurCamera* pCamera = m_viewModel.m_cScene.m_pCamera;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        m_viewModel.m_cScene.AddModel("charrec_light", -1);
        pCreature->Setup3DScene(m_viewModel, pOriginal, 0, 1);
        pCreature->AttachToScene(pScene);
        pCreature->SetPosition(Vector());
        CAurBehaviorAttachTranslate* pNode = (CAurBehaviorAttachTranslate*)pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
        if (pNode != 0)
        {
            pNode->m_nWorldSpace = 1;
            pNode->m_vTranslation = Vector(0.0f, 0.0f, pCreature->GetHeadHeightOffset());
        }
    }
    m_nInputClass = g_pAppManager->m_pClientExoApp->GetInputClass();
}
// PC 0x00757AF0
CSWGuiLevelUpCharGen::~CSWGuiLevelUpCharGen()
{
    delete m_pCreature;
    m_pCreature = 0;
}
// PC 0x00756EE0
void CSWGuiLevelUpCharGen::DetermineClass()
{
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    m_sClass = pStats->GetClassesString(10);
    m_sClass = m_sClass.Right(m_sClass.GetLength() - 1 - m_sClass.Find(10, 0));
    m_sClass = m_sClass.Left(m_sClass.Find(40, 0));
}
// PC 0x00756FE0
void CSWGuiLevelUpCharGen::SetNameString(const CExoString& sName)
{
    m_sName = sName;
    CExoString sLevel(m_pCreature->m_pStats->GetLevel() + 1);
    m_lblName.m_cText.m_cParams.SetText(m_sName);
    m_lblClass.m_cText.m_cParams.SetText(m_sClass);
    m_lblLevelValue.m_cText.m_cParams.SetText(sLevel);
}
// PC 0x007570E0
void CSWGuiLevelUpCharGen::AbilitiesChanged()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);

    if (pCreature != 0)
    {
        pCreature->GetStats()->SetFortSavingThrow();
        pCreature->GetStats()->SetWillSavingThrow();
        pCreature->GetStats()->SetReflexSavingThrow();

        {
            CExoString sText(pCreature->GetStats()->m_aAbilities[5]);
            m_lblCharismaAbility.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_aAbilities[0]);
            m_lblStrengthAbility.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_aAbilities[3]);
            m_lblIntelligenceAbility.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_aAbilities[4]);
            m_lblWisdomAbility.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_aAbilities[2]);
            m_lblConstitutionAbility.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_aAbilities[1]);
            m_lblDexterityAbility.m_cText.m_cParams.SetText(sText);
        }

        unsigned char nLastClass = pCreature->GetStats()->GetNumClasses() - 1;
        unsigned char nClass = pCreature->GetStats()->GetClass(nLastClass);

        CSWCLevelUpStats* pStats = pCreature->GetStats();
        unsigned char nStat = pStats->m_nConstitution;
        CSWSCreatureStats* pServerStats = pCreature->GetServerCreature()->m_pStats;
        char nMod = pServerStats->m_nConstitutionModifier;
        int nConDiff = pStats->CalcStatModifier(nStat) - nMod;

        pStats = pCreature->GetStats();
        nStat = pStats->m_nWisdom;
        pServerStats = pCreature->GetServerCreature()->m_pStats;
        nMod = pServerStats->m_nWisdomModifier;
        int nWisDiff = pStats->CalcStatModifier(nStat) - nMod;

        pStats = pCreature->GetStats();
        nStat = pStats->m_nCharisma;
        pServerStats = pCreature->GetServerCreature()->m_pStats;
        nMod = pServerStats->m_nCharismaModifier;
        int nChaDiff = pStats->CalcStatModifier(nStat) - nMod;

        char nHitDie = g_pRules->m_pClasses[nClass].m_nHitDie;
        pStats = pCreature->GetStats();
        nStat = pStats->m_nConstitution;
        char nHitPoints = nHitDie + pStats->CalcStatModifier(nStat) +
                          m_pCreature->GetStats()->GetLevel() * nConDiff;
        if (nHitPoints <= 1)
        {
            nHitPoints = 1;
        }
        short nHitPointsBase = m_pCreature->GetStats()->m_nHitPointsBase;
        pCreature->GetStats()->m_nHitPointsBase = nHitPointsBase + nHitDie;
        pCreature->GetStats()->m_nMaxHitPoints =
            m_pCreature->GetStats()->GetMaxHitPoints() + nHitPoints;
        pCreature->GetStats()->m_nCurrentHitPoints =
            m_pCreature->GetStats()->GetCurrentHitPoints() + nHitPoints;

        if (CSWClass::IsJedi((int)nClass))
        {
            char nForceDie = g_pRules->m_pClasses[nClass].m_nForceDie;
            pStats = pCreature->GetStats();
            unsigned char nWisdom = pStats->m_nWisdom;
            unsigned char nCharisma = pStats->m_nCharisma;
            char nChaMod = pStats->CalcStatModifier(nCharisma);
            char nForcePoints =
                nForceDie + pStats->CalcStatModifier(nWisdom) + nChaMod +
                m_pCreature->GetStats()->GetLevel() * (nWisDiff + nChaDiff);
            nForcePoints = nForcePoints > 1 ? nForcePoints : 1;
            short nForcePointsBase = m_pCreature->GetStats()->m_nForcePointsBase;
            pCreature->GetStats()->m_nForcePointsBase = nForcePointsBase + nForceDie;
            short nMaxForcePoints = m_pCreature->GetStats()->m_nMaxForcePoints;
            pCreature->GetStats()->m_nMaxForcePoints = nMaxForcePoints + nForcePoints;
            short nCurrentForcePoints =
                m_pCreature->GetStats()->m_nCurrentForcePoints +
                m_pCreature->GetStats()->m_nForcePointBonus;
            pCreature->GetStats()->m_nCurrentForcePoints = nCurrentForcePoints + nForcePoints;
        }

        {
            CExoString sText(pCreature->GetStats()->GetArmorClass(0));
            m_lblDefense.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_nFortitudeSave);
            m_lblNewFortitude.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_nReflexSave);
            m_lblNewReflex.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->GetMaxHitPoints());
            m_lblVitality.m_cText.m_cParams.SetText(sText);
        }
        {
            CExoString sText(pCreature->GetStats()->m_nWillSave);
            m_lblNewWill.m_cText.m_cParams.SetText(sText);
        }
    }
}
// PC 0x00757620
void CSWGuiLevelUpCharGen::NewAbilityScores(int nStr, int nCha, int nInt,
                                            int nWis, int nCon, int nDex,
                                            int nPoints)
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);

    if (pCreature != 0)
    {
        pCreature->m_pStats->SetSTRBase(nStr);
        pCreature->m_pStats->SetCHABase(nCha);
        pCreature->m_pStats->SetINTBase(nInt);
        pCreature->m_pStats->SetWISBase(nWis);
        pCreature->m_pStats->SetCONBase(nCon);
        pCreature->m_pStats->SetDEXBase(nDex, 1);
        pCreature->m_pStats->m_nCreationStage = nPoints;
    }
}
// PC 0x007576C0
void CSWGuiLevelUpCharGen::ClearAbilities()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);

    if (pCreature != 0)
    {
        pCreature->m_pStats->SetSTRBase(m_pCreature->m_pStats->m_aAbilities[0]);
        pCreature->m_pStats->SetCHABase(m_pCreature->m_pStats->m_aAbilities[5]);
        pCreature->m_pStats->SetINTBase(m_pCreature->m_pStats->m_aAbilities[3]);
        pCreature->m_pStats->SetWISBase(m_pCreature->m_pStats->m_aAbilities[4]);
        pCreature->m_pStats->SetCONBase(m_pCreature->m_pStats->m_aAbilities[2]);
        pCreature->m_pStats->SetDEXBase(m_pCreature->m_pStats->m_aAbilities[1],
                                        1);
        pCreature->m_pStats->SetFortSavingThrow();
        pCreature->m_pStats->SetWillSavingThrow();
        pCreature->m_pStats->SetReflexSavingThrow();
        pCreature->m_pStats->m_nHitPointsBase =
            m_pCreature->m_pStats->m_nHitPointsBase;
        CSWCLevelUpStats* pStats = pCreature->m_pStats;
        pStats->m_nMaxHitPoints = m_pCreature->m_pStats->GetMaxHitPoints();
        pCreature->m_pStats->m_nCurrentHitPoints =
            m_pCreature->m_pStats->m_nCurrentHitPoints;
        pCreature->m_pStats->m_nForcePointsBase =
            m_pCreature->m_pStats->m_nForcePointsBase;
        pCreature->m_pStats->m_nMaxForcePoints =
            m_pCreature->m_pStats->m_nMaxForcePoints;
        pCreature->m_pStats->m_nCurrentForcePoints =
            m_pCreature->m_pStats->m_nCurrentForcePoints +
            m_pCreature->m_pStats->m_nForcePointBonus;
    }
}
// PC 0x00757850
void CSWGuiLevelUpCharGen::ClearSkills()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);

    if (pCreature != 0)
    {
        pCreature->m_pStats->m_nSkillPoints =
            m_pCreature->m_pStats->m_nSkillPoints;
        for (unsigned char i = 0; i < g_pRules->m_nNumSkills; i++)
        {
            pCreature->m_pStats->SetSkillRankBase(
                i, m_pCreature->m_pStats->GetSkillRank(i));
        }
    }
}
// PC 0x00757900
void CSWGuiLevelUpCharGen::ClearPowers()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);

    if (pCreature != 0)
    {
        int nClass = m_pCreature->m_pStats->m_nClassCount - 1;
        pCreature->GetStats()->ClearKnownSpells();
        for (unsigned char i = 0;
             i < m_pCreature->m_pStats->GetNumberKnownSpells(nClass, 0); i++)
        {
            CSWCLevelUpStats* pStats = pCreature->m_pStats;
            CSWCLevelUpStats* pOldStats = m_pCreature->m_pStats;
            unsigned long nSpell = pOldStats->GetKnownSpell(nClass, 0, i);
            pStats->AddKnownSpell(nClass, nSpell);
        }
    }
}
// PC 0x007579C0
void CSWGuiLevelUpCharGen::BeginPlayGame()
{
    GetManager()->PopModalPanel();
    CSWGuiPanel* pPanel = m_pLevelUpPanel;
    pPanel->m_bDeferredRemoval = 0;
    pPanel->m_nExitState = EXITSTATE_DELETE;
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);
    pCreature->m_pStats->GetClass(pCreature->m_pStats->m_nClassCount - 1);
    CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
    pMessage->SendPlayerToServer_LevelUp(m_pCreature->m_pStats,
                                         pCreature->m_pStats);
    pCreature->m_bLevelUpSent = 1;
    GetManager()->PopModalPanel();
    CSWGuiInGameCharacter* pCharacter = m_pInGameCharacter;
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    pCharacter->AttachModel();
    if (m_nInputClass == 0 &&
        g_pAppManager->m_pClientExoApp->GetPausedByCombat() != 0)
    {
        g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
    }
    if (g_bCreateAllGUIs == 0 && m_nInputClass != 2)
    {
        pPanel = m_pInGameCharacter;
        pPanel->m_bDeferredRemoval = 0;
        pPanel->m_nExitState = EXITSTATE_DELETE;
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_apScreenPanels[2] = 0;
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
    g_pAppManager->m_pClientExoApp->SetInputClass(m_nInputClass, 1);
}
// PC 0x00757E30
int CSWGuiLevelUpCharGen::CopyCreatureStats(CSWCCreature* pFrom,
                                           CSWCCreature* pTo)
{
    CSWCLevelUpStats* pNewStats = pTo->m_pStats;
    CSWCLevelUpStats* pOldStats = pFrom->m_pStats;

    pNewStats->m_sFirstName = pOldStats->m_sFirstName;
    pNewStats->m_sLastName = pOldStats->m_sLastName;
    pNewStats->m_cPortrait.CResRef::operator=(pOldStats->GetPortrait(0));
    pNewStats->m_nExperience = pOldStats->m_nExperience;
    pNewStats->m_nStartingPackage = pOldStats->m_nStartingPackage;
    pNewStats->m_nGender = pOldStats->m_nGender;
    pNewStats->m_nGoodEvilAlignment = pOldStats->GetAlignment();
    pNewStats->SetSTRBase(pOldStats->m_aAbilities[0]);
    pNewStats->SetDEXBase(pOldStats->m_aAbilities[1], 1);
    pNewStats->SetCONBase(pOldStats->m_aAbilities[2]);
    pNewStats->SetINTBase(pOldStats->m_aAbilities[3]);
    pNewStats->SetWISBase(pOldStats->m_aAbilities[4]);
    pNewStats->SetCHABase(pOldStats->m_aAbilities[5]);
    pNewStats->m_nHitPointsBase = pOldStats->m_nHitPointsBase;
    pNewStats->m_nCurrentHitPoints = pOldStats->m_nCurrentHitPoints;
    pNewStats->m_nMaxHitPoints = pOldStats->GetMaxHitPoints();
    pNewStats->m_nForcePointsBase = pOldStats->m_nForcePointsBase;
    pNewStats->m_nCurrentForcePoints =
        pOldStats->m_nCurrentForcePoints + pOldStats->m_nForcePointBonus;
    pNewStats->m_nMaxForcePoints = pOldStats->m_nMaxForcePoints;

    for (unsigned char nClass = 0; nClass < pOldStats->m_nClassCount; nClass++)
    {
        pNewStats->m_nClassCount = nClass + 1;
        pNewStats->SetClass(nClass, pOldStats->GetClass(nClass));
        pNewStats->SetClassLevel(nClass, pOldStats->GetClassLevel(nClass));
        pNewStats->ClearKnownSpells(nClass);
        for (unsigned char nSpell = 0;
             nSpell < pOldStats->GetNumberKnownSpells(nClass, 0); nSpell++)
        {
            pNewStats->AddKnownSpell(
                nClass, pOldStats->GetKnownSpell(nClass, 0, nSpell));
        }
    }

    pNewStats->m_nRace = pOldStats->m_nRace;
    pNewStats->m_sSubRace = pOldStats->m_sSubRace;
    pNewStats->m_nSubraceIndex = pOldStats->m_nSubraceIndex;
    pNewStats->SetFortSavingThrow();
    pNewStats->SetWillSavingThrow();
    pNewStats->SetReflexSavingThrow();
    for (int nAttack = 0; nAttack < 4; nAttack++)
        pNewStats->m_nUnknown64 = pOldStats->GetBaseAttackBonus();
    pNewStats->m_nGoodEvilAlignment = pOldStats->GetAlignment();
    pNewStats->ClearSkills();
    pNewStats->m_nSkillPoints = pOldStats->m_nSkillPoints;
    for (unsigned char nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
        pNewStats->SetSkillRankBase(nSkill, pOldStats->GetSkillRank(nSkill));
    pNewStats->ClearFeats();
    for (unsigned short nIndex = 0;
         nIndex < (unsigned short)pOldStats->m_lstFeats.num; nIndex++)
    {
        unsigned short nFeat = pOldStats->GetFeat(nIndex);
        pNewStats->m_lstFeats.AddUnique(nFeat);
    }
    pNewStats->m_nCreationStage = pOldStats->m_nCreationStage;
    pNewStats->m_nDefenseBase = pOldStats->GetArmorClass(0);
    pNewStats->m_nDefenseBase = pOldStats->GetArmorClass(0);
    return 1;
}
// PC 0x00758350
void CSWGuiLevelUpCharGen::ClearFeats()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);

    if (pCreature != 0)
    {
        pCreature->m_pStats->ClearFeats();
        for (unsigned short i = 0;
             i < (unsigned short)m_pCreature->m_pStats->m_lstFeats.num; i++)
        {
            CSWCLevelUpStats* pStats = pCreature->m_pStats;
            unsigned short nFeat = m_pCreature->m_pStats->GetFeat(i);
            pStats->m_lstFeats.AddUnique(nFeat);
        }
    }
}
// PC 0x007583C0
void CSWGuiLevelUpCharGen::CancelLevelUp()
{
    if (m_nInputClass != 0)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pApp->GetCreatureByGameObjectID(m_nCreatureId);
        CopyCreatureStats(m_pCreature, pCreature);
        m_pManager->PopModalPanel();
        m_pManager->PopModalPanel();
        CSWGuiPanel* pPanel = m_pLevelUpPanel;
        pPanel->m_bDeferredRemoval = 0;
        pPanel->m_nExitState = EXITSTATE_DELETE;
        CSWGuiInGameCharacter* pCharacter = m_pInGameCharacter;
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        pCharacter->AttachModel();
        if (g_bCreateAllGUIs == 0 && m_nInputClass != 2)
        {
            pPanel = m_pInGameCharacter;
            pPanel->m_bDeferredRemoval = 0;
            pPanel->m_nExitState = EXITSTATE_DELETE;
            g_pAppManager->m_pClientExoApp->GetInGameGui()
                ->m_apScreenPanels[2] = 0;
        }
        g_pAppManager->m_pClientExoApp->SetInputClass(m_nInputClass, 1);
    }
}

void CSWGuiLevelUpCharGen::LevelUpCreature()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(m_nCreatureId);
    if (pCreature != 0)
    {
        CSWCLevelUpStats* pStats = pCreature->m_pStats;
        unsigned char nClass = pStats->GetNumClasses() - 1;
        pStats->SetClassLevel(nClass, pStats->GetClassLevel(nClass) + 1);
    }
}

void CSWGuiLevelUpCharGen::RemoveSubPanels()
{
    m_pManager->PopModalPanel();
    CSWGuiPanel* pPanel = m_pLevelUpPanel;
    pPanel->m_bDeferredRemoval = 0;
    pPanel->m_nExitState = EXITSTATE_DELETE;
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
}

void CSWGuiLevelUpCharGen::HandleCancelButton()
{
    RemoveSubPanels();
    m_pInGameCharacter->AttachModel();
}
// PC 0x00757DC0
void CSWGuiLevelUpCharGen::HandleInputEvent(int nEvent, int nState)
{
    SWGUILEVELUPCHARGEN_TRACE();
    if (nState != 0 && (nEvent == 40 || nEvent == 46))
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        HandleCancelButton();
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

static inline void SwguilevelupchargenTrace3()
{
    SwguilevelupchargenTrace2();
}

static inline void SwguilevelupchargenTrace2()
{
    SwguilevelupchargenTrace1();
}

static inline void SwguilevelupchargenTrace1()
{
}

#ifdef _XBOX
void CSWGuiLevelUpCharGen::SetBackPromptEnabled(int bEnabled)
{
    m_lblBack.m_bEnabled = bEnabled;
    m_lblB.m_bEnabled = bEnabled;
}
#endif
