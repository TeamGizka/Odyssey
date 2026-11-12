#include "exobase.h"
#include "aurcamera.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swguiingame.h"
#include "swparty.h"
#include "serverexoapp.h"
#include "serverinfo.h"
#include "partytables.h"
#include "nwclass.h"
#include "nwccreaturestats.h"
#include "math3d.h"
#include "aurscene.h"
#include "nwccreatureappearance.h"
#include "swguiingamecharacter.h"
#include "swgui.h"
#include "swguilevelupchargen.h"
#include "nwccreature.h"
#include "nwseffectlist.h"
// PC 0x0071B2C0
void CSWGuiInGameCharacter::OnPanelRemoved()
{
    m_pShownCreature = 0;
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x0071B060
inline void CSWGuiInGameCharacter::SetModText(CSWGuiLabel& lblMod, int nMod)
{
    lblMod.m_cText.m_cParams.SetText(nMod > 0 ? CExoString("+") + nMod
                                              : CExoString(nMod));
}
// PC 0x0071CA10
CSWGuiInGameCharacter::CSWGuiInGameCharacter(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("character");
#else
    StartLoadFromLayout("character_p");
#endif
#ifdef _XBOX
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblClass, "LBL_CLASS", 1);
    InitControl(m_lblClass1, "LBL_CLASS1", 1);
    InitControl(m_lblClass2, "LBL_CLASS2", 1);
    InitControl(m_lblLevel, "LBL_LEVEL", 1);
    InitControl(m_lblLevel1, "LBL_LEVEL1", 1);
    InitControl(m_lblLevel2, "LBL_LEVEL2", 1);
#else
    InitControl(m_lblForceMastery, "LBL_FORCEMASTERY", 1);
#endif
    InitControl(m_lblFortitude, "LBL_FORTITUDE", 1);
    InitControl(m_lblReflex, "LBL_REFLEX", 1);
    InitControl(m_lblWill, "LBL_WILL", 1);
    InitControl(m_lblFortitudeStat, "LBL_FORTITUDE_STAT", 1);
    InitControl(m_lblReflexStat, "LBL_REFLEX_STAT", 1);
    InitControl(m_lblWillStat, "LBL_WILL_STAT", 1);
    InitControl(m_lblExperience, "LBL_EXPERIENCE", 1);
    InitControl(m_lblExperienceStat, "LBL_EXPERIENCE_STAT", 1);
    InitControl(m_lblNextLevel, "LBL_NEXT_LEVEL", 1);
    InitControl(m_lblNeededXP, "LBL_NEEDED_XP", 1);
    InitControl(m_lblDefenseStat, "LBL_DEFENSE_STAT", 1);
    InitControl(m_lblForceStat, "LBL_FORCE_STAT", 1);
    InitControl(m_lblVitalityStat, "LBL_VITALITY_STAT", 1);
    InitControl(m_lblDefense, "LBL_DEFENSE", 1);
    InitControl(m_lblForce, "LBL_FORCE", 1);
    InitControl(m_lblVitality, "LBL_VITALITY", 1);
    InitControl(m_lblStr, "LBL_STR", 1);
    InitControl(m_lblStrength, "LBL_STRENGTH", 1);
    InitControl(m_lblStrMod, "LBL_STR_MOD", 1);
    InitControl(m_lblWis, "LBL_WIS", 1);
    InitControl(m_lblWisdom, "LBL_WISDOM", 1);
    InitControl(m_lblWisMod, "LBL_WIS_MOD", 1);
    InitControl(m_lblCha, "LBL_CHA", 1);
    InitControl(m_lblCharisma, "LBL_CHARISMA", 1);
    InitControl(m_lblChaMod, "LBL_CHA_MOD", 1);
    InitControl(m_lblInt, "LBL_INT", 1);
    InitControl(m_lblIntelligence, "LBL_INTELLIGENCE", 1);
    InitControl(m_lblIntMod, "LBL_INT_MOD", 1);
    InitControl(m_lblCon, "LBL_CON", 1);
    InitControl(m_lblConstitution, "LBL_CONSTITUTION", 1);
    InitControl(m_lblConMod, "LBL_CON_MOD", 1);
    InitControl(m_lblDex, "LBL_DEX", 1);
    InitControl(m_lblDexterity, "LBL_DEXTERITY", 1);
    InitControl(m_lblDexMod, "LBL_DEX_MOD", 1);
#ifdef _XBOX
    InitControl(m_lblXScripts, "LBL_X_SCRIPTS", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblALevelUp, "LBL_A_LEVEL_UP", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblAuto, "LBL_AUTO", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_viewCharacter, "LBL_3DCHAR", 1);
    InitControl(m_lblBevel, "LBL_BEVEL", 1);
    InitControl(m_lblBevel2, "LBL_BEVEL2", 1);
    InitControl(m_sldAlign, "SLD_ALIGN", 1);
    InitControl(m_lblExit, "LBL_EXIT", 1);
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblBlack, "LBL_BLACK", 1);
    InitControl(m_lblChangeChar, "LBL_CHANGECHAR", 1);
    InitControl(m_lblAdorn, "LBL_ADORN", 1);
    InitControl(m_lblMore, "LBL_MORE", 1);
    InitControl(m_lblLight, "LBL_LIGHT", 1);
    InitControl(m_lblDark, "LBL_DARK", 1);
    CExoString sName;
    for (int nGood = 0; nGood < EFFECT_ICON_CAPACITY; ++nGood)
    {
        sName.Format("LBL_GOOD%d", nGood + 1);
        InitControl(m_lblGood[nGood], sName, 1);
        m_lblGood[nGood].SetShown(0);
    }
    StopLoadFromLayout();
    m_lblMore.SetShown(0);
    CAurCamera* pCamera = m_viewCharacter.m_cScene.m_pCamera;
    CAurScene* pScene = m_viewCharacter.m_cScene.m_pScene;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        CAurObject* pLight =
            m_viewCharacter.m_cScene.AddModel("charrec_light", -1);
        if (pLight != 0)
        {
            pCamera->AttachToObject(pLight, "camerahook", Gob::ATTACH_TRANSLATE);
            pCamera->SetViewAngle(22.726f);
        }
    }
    m_pCreature = new CSWCCreature();
    m_pShownCreature = 0;
    m_nShownAlignment = -1;
    CSWGuiTextParams* pText = &m_lblALevelUp.m_cText.m_cParams;
    pText->m_bHilightText = 1;
    pText = &m_lblAuto.m_cText.m_cParams;
    pText->m_bHilightText = 1;
#else
    InitControl(m_viewCharacter, "LBL_3DCHAR", 1);
    InitControl(m_lblBevel, "LBL_BEVEL", 1);
    InitControl(m_lblBevel2, "LBL_BEVEL2", 1);
    InitControl(m_sldAlign, "SLD_ALIGN", 1);
    InitControl(m_lblAdorn, "LBL_ADORN", 1);
    InitControl(m_lblMore, "LBL_MORE", 1);
    InitControl(m_lblLight, "LBL_LIGHT", 1);
    InitControl(m_lblDark, "LBL_DARK", 1);
    InitControl(m_btnAuto, "BTN_AUTO", 1);
    InitControl(m_btnLevelUp, "BTN_LEVELUP", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_btn3DChar, "BTN_3DCHAR", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
    InitControl(m_alblBar[5], "LBL_BAR6", 1);
    InitControl(m_lblXpBack, "LBL_XP_BACK", 1);
    InitControl(m_lblMoreBack, "LBL_MORE_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblStatsBorder, "LBL_STATSBORDER", 1);
    StopLoadFromLayout();
    m_lblMore.SetShown(0);
    CAurCamera* pCamera = m_viewCharacter.m_cScene.m_pCamera;
    CAurScene* pScene = m_viewCharacter.m_cScene.m_pScene;
    m_btn3DChar.AddEvent(CSWGuiSpeedButton::EVENT_REPEAT_LEFT, this,
                         reinterpret_cast<CSWGuiHandler>(
                             &CSWGuiInGameCharacter::RotateCharacterLeft));
    m_btn3DChar.AddEvent(CSWGuiSpeedButton::EVENT_REPEAT_RIGHT, this,
                         reinterpret_cast<CSWGuiHandler>(
                             &CSWGuiInGameCharacter::RotateCharacterRight));
    m_btn3DChar.m_fRepeatDelay = 0.1f;
    m_btn3DChar.m_fRepeatRate = 0.1f;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        CAurObject* pLight =
            m_viewCharacter.m_cScene.AddModel("charrec_light", -1);
        if (pLight != 0)
        {
            pCamera->AttachToObject(pLight, "camerahook", Gob::ATTACH_TRANSLATE);
            pCamera->SetViewAngle(22.726f);
        }
    }
    m_pCreature = new CSWCCreature();
    m_pShownCreature = 0;
    m_nShownAlignment = -1;
    m_lblForceMastery.SetShown(0);
    CSWGuiTextParams* pText = &m_btnLevelUp.m_cText.m_cParams;
    pText->m_bHilightText = 1;
    pText = &m_btnAuto.m_cText.m_cParams;
    pText->m_bHilightText = 1;
    m_btnAuto.SetFocusable(0);
    m_btnAuto.AddEvent(
        GUIEVENT_ACTIVATE, this,
        reinterpret_cast<CSWGuiHandler>(&CSWGuiPanel::OnYButtonPressed));
    m_btnLevelUp.SetFocusable(0);
    m_btnLevelUp.AddEvent(
        GUIEVENT_ACTIVATE, this,
        reinterpret_cast<CSWGuiHandler>(&CSWGuiPanel::OnAButtonPressed));
    m_btnExit.SetFocusable(0);
    m_btnExit.AddEvent(
        GUIEVENT_ACTIVATE, this,
        reinterpret_cast<CSWGuiHandler>(&CSWGuiPanel::OnBButtonPressed));
    m_sldAlign.SetFocusable(0);
#endif
}
// PC 0x0071DC60
void CSWGuiInGameCharacter::OnPanelAdded()
{
#ifdef _XBOX
    SetStats();
    m_lblBlack.SetEnabledFlag(
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
    m_lblChangeChar.SetEnabledFlag(
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
#else
    SetStats();
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}

#ifdef _XBOX
void CSWGuiInGameCharacter::HandleInputEvent(int nEvent, int nState)
{
    g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    CClientExoApp* pClient;
    if (nState != 0)
    {
        pClient = g_pAppManager->m_pClientExoApp;
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
            if (pClient->GetInGameGui()->m_LevelUpMode != 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                ShowLevelUpGUI(0);
            }
            break;
        case EVENT_AUTO_LEVELUP:
            if (pClient->GetInGameGui()->m_LevelUpMode != 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                StartAutoLevelup();
            }
            break;
        case EVENT_NEXT_CHARACTER:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            pClient->ChangeCharacterToNextLivingPartyMember(0, 0);
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (pClient->GetInGameGui()->HideSWInGameGui() != 0)
            {
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
            }
            break;
        case EVENT_ROTATE_LEFT:
            RotateCharacter(1);
            break;
        case EVENT_ROTATE_RIGHT:
            RotateCharacter(0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#else
void CSWGuiInGameCharacter::HandleInputEvent(int nEvent, int nState)  // PC 0x0071DC70
{
    g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    if (nState != 0)
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
            if (pClient->GetInGameGui()
                        ->m_LevelUpMode != 0 &&
                g_pAppManager->m_pClientExoApp
                        ->GetCharacterChangeInProgress() == 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                ShowLevelUpGUI(0);
            }
            break;
        case EVENT_AUTO_LEVELUP:
            if (pClient->GetInGameGui()
                        ->m_LevelUpMode != 0 &&
                g_pAppManager->m_pClientExoApp
                        ->GetCharacterChangeInProgress() == 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                StartAutoLevelup();
            }
            break;
        case EVENT_NEXT_CHARACTER:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            pClient->ChangeCharacterToNextLivingPartyMember(0, 0, 1);
            break;
        }
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
        case EVENT_EXIT:
            if (pClient->GetInGameGui()->HideSWInGameGui(
                    0) != 0)
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
            break;
        case EVENT_ROTATE_LEFT:
            RotateCharacter(1);
            break;
        case EVENT_ROTATE_RIGHT:
            RotateCharacter(0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x0071B8C0
CSWGuiInGameCharacter::~CSWGuiInGameCharacter()
{
    delete m_pCreature;
}
// PC 0x0071BC70
void CSWGuiInGameCharacter::SetStats()
{
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    m_nPartySize = pClient->GetSWParty()->GetNumCharacters();
    CSWCCreature* pCreature;
#ifdef _XBOX
    pCreature = pClient->GetSWParty()->GetCharacter(0);
#else
    pCreature = pClient->GetSWParty()->GetCharacter(0);
#endif
    CSWCCreatureStats* pStats = pCreature->m_pStats;
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    CSWSCreatureStats* pServerStats = pServerCreature->m_pStats;
    unsigned char nClassIndex = pStats->GetNumClasses() - 1;
    unsigned char nClass = pStats->GetClass(nClassIndex);
#ifdef _XBOX
    m_lblName.m_cText.m_cParams.SetText(pCreature->GetName());
    if (pStats->GetNumClasses() == 1)
    {
        unsigned char nFirstClass = pStats->GetClass(0);
        unsigned long nFirstClassNameStrRef = g_pRules->m_pClasses[nFirstClass].m_nNameStrRef;
        m_lblClass1.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(
                nFirstClassNameStrRef));
        m_lblClass2.m_cText.m_cParams.SetText("");
        m_lblLevel1.m_cText.m_cParams.SetText(
            pStats->GetClassLevel(0));
        m_lblLevel2.m_cText.m_cParams.SetText("");
    }
    else
    {
        unsigned char nSecondClass = pStats->GetClass(1);
        unsigned long nSecondClassNameStrRef = g_pRules->m_pClasses[nSecondClass].m_nNameStrRef;
        m_lblClass1.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(
                nSecondClassNameStrRef));
        unsigned char nFirstClass = pStats->GetClass(0);
        unsigned long nFirstClassNameStrRef = g_pRules->m_pClasses[nFirstClass].m_nNameStrRef;
        m_lblClass2.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(
                nFirstClassNameStrRef));
        m_lblLevel1.m_cText.m_cParams.SetText(
            pStats->GetClassLevel(1));
        m_lblLevel2.m_cText.m_cParams.SetText(
            pStats->GetClassLevel(0));
    }
#endif
    CExoString sHitPoints;
    sHitPoints.Format("%d/%d", pStats->GetCurrentHitPoints(),
                      pStats->GetMaxHitPoints());
    m_lblVitalityStat.m_cText.m_cParams.SetText(sHitPoints);
    SetLabelBonusPenaltyColour(m_lblVitalityStat, pStats->m_nHitPointBonus);
#ifdef _XBOX
    m_lblDefenseStat.m_cText.m_cParams.SetText(
        pStats->GetArmorClass(0));
    SetLabelBonusPenaltyColour(
        m_lblDefenseStat,
        pCreature->GetServerCreature()->GetTotalACEffectBonus());
#else
    m_lblDefenseStat.m_cText.m_cParams.SetText(
        pStats->GetArmorClass(1));
    m_lblDefenseStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
    if (CSWClass::IsJedi(nClass) != 0)
    {
#ifdef _XBOX
        short nForcePoints = pStats->m_nCurrentForcePoints;
        short nMaxForcePoints = pStats->m_nMaxForcePoints;
        nForcePoints += pStats->m_nForcePointBonus;
#else
        int nForcePoints = pStats->GetForcePoints();
        int nMaxForcePoints = pStats->GetMaxForcePoints();
        if (nMaxForcePoints < 1)
        {
            nForcePoints = 0;
        }
#endif
        sHitPoints.Format("%d/%d", nForcePoints, nMaxForcePoints);
        m_lblForceStat.m_cText.m_cParams.SetText(sHitPoints);
        m_lblForce.SetEnabledFlag(1);
        m_lblForceStat.SetEnabledFlag(1);
        SetLabelBonusPenaltyColour(m_lblForceStat,
                                   pStats->m_nForcePointBonus);
    }
    else
    {
        m_lblForce.SetEnabledFlag(0);
        m_lblForceStat.SetEnabledFlag(0);
    }
    int nBase = pServerStats->m_nStrengthBase;
    int nStat = pServerStats->GetSTRStat();
    m_lblStr.m_cText.m_cParams.SetText(nStat);
    SetModText(m_lblStrMod, pServerStats->GetSTRMod());
    SetLabelBonusPenaltyColour(m_lblStr, nStat - nBase);
    SetLabelBonusPenaltyColour(m_lblStrMod, nStat - nBase);
    nBase = pServerStats->m_nWisdomBase;
    nStat = pServerStats->GetWISStat();
    m_lblWis.m_cText.m_cParams.SetText(nStat);
    SetModText(m_lblWisMod, pServerStats->GetWISMod());
    SetLabelBonusPenaltyColour(m_lblWis, nStat - nBase);
    SetLabelBonusPenaltyColour(m_lblWisMod, nStat - nBase);
    nBase = pServerStats->m_nCharismaBase;
    nStat = pServerStats->GetCHAStat();
    m_lblCha.m_cText.m_cParams.SetText(nStat);
    SetModText(m_lblChaMod, pServerStats->GetCHAMod());
    SetLabelBonusPenaltyColour(m_lblCha, nStat - nBase);
    SetLabelBonusPenaltyColour(m_lblChaMod, nStat - nBase);
    nBase = pServerStats->m_nIntelligenceBase;
    nStat = pServerStats->GetINTStat();
    m_lblInt.m_cText.m_cParams.SetText(nStat);
    SetModText(m_lblIntMod, pServerStats->GetINTMod());
    SetLabelBonusPenaltyColour(m_lblInt, nStat - nBase);
    SetLabelBonusPenaltyColour(m_lblIntMod, nStat - nBase);
    nBase = pServerStats->m_nConstitutionBase;
    nStat = pServerStats->GetCONStat();
    m_lblCon.m_cText.m_cParams.SetText(nStat);
    SetModText(m_lblConMod, pServerStats->GetCONMod());
    SetLabelBonusPenaltyColour(m_lblCon, nStat - nBase);
    SetLabelBonusPenaltyColour(m_lblConMod, nStat - nBase);
    nBase = pServerStats->m_nDexterityBase;
    nStat = pServerStats->GetDEXStat();
    m_lblDex.m_cText.m_cParams.SetText(nStat);
    SetModText(m_lblDexMod, pServerStats->GetDEXMod(1));
    SetLabelBonusPenaltyColour(m_lblDex, nStat - nBase);
    if (nStat == nBase)
    {
        SetLabelBonusPenaltyColour(
            m_lblDexMod, pServerStats->GetDEXMod(1) - pServerStats->GetDEXMod(0));
    }
    else
    {
        SetLabelBonusPenaltyColour(m_lblDexMod, nStat - nBase);
    }
    int nCapped = 0;
    m_lblFortitudeStat.m_cText.m_cParams.SetText(
        pServerStats->GetFortSavingThrow(0));
    SetLabelBonusPenaltyColour(
        m_lblFortitudeStat, pCreature->GetServerCreature()->GetTotalEffectBonus(
                                CSWSCreature::EFFECT_BONUS_SAVING_THROW, 0, nCapped, 0, 0,
                                CSWSCreature::SAVING_THROW_FORT, 0,
                                CSWSCreature::EFFECT_BONUS_FILTER_ALL,
                                CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0));
    m_lblReflexStat.m_cText.m_cParams.SetText(
        pServerStats->GetReflexSavingThrow(0));
    SetLabelBonusPenaltyColour(
        m_lblReflexStat, pCreature->GetServerCreature()->GetTotalEffectBonus(
                             CSWSCreature::EFFECT_BONUS_SAVING_THROW, 0, nCapped, 0, 0,
                             CSWSCreature::SAVING_THROW_REFLEX, 0,
                             CSWSCreature::EFFECT_BONUS_FILTER_ALL,
                             CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0));
    m_lblWillStat.m_cText.m_cParams.SetText(
        pServerStats->GetWillSavingThrow(0));
    SetLabelBonusPenaltyColour(
        m_lblWillStat, pCreature->GetServerCreature()->GetTotalEffectBonus(
                           CSWSCreature::EFFECT_BONUS_SAVING_THROW, 0, nCapped, 0, 0,
                           CSWSCreature::SAVING_THROW_WILL, 0,
                           CSWSCreature::EFFECT_BONUS_FILTER_ALL,
                           CSWSCreature::EFFECT_BONUS_FILTER_ALL, 0));
    m_lblExperienceStat.m_cText.m_cParams.SetText(
        pServerStats->GetExperience());
    m_lblNeededXP.m_cText.m_cParams.SetText(
        pServerStats->GetExpNeededForLevelUp());
    m_lblNeededXP.SetEnabledFlag(pServerStats->GetExpNeededForLevelUp() != -1);
    m_lblNextLevel.SetEnabledFlag(pServerStats->GetExpNeededForLevelUp() != -1);
#ifdef _XBOX
    int bCanLevelUp = pServerStats->CanLevelUp();
    m_lblALevelUp.SetEnabledFlag(bCanLevelUp);
    m_lblA.SetEnabledFlag(bCanLevelUp);
    m_lblAuto.SetEnabledFlag(bCanLevelUp);
    m_lblY.SetEnabledFlag(bCanLevelUp);
#else
    int bCanLevelUp = pServerStats->CanLevelUp();
    m_btnLevelUp.SetEnabledFlag(bCanLevelUp);
    m_btnAuto.SetEnabledFlag(bCanLevelUp);
#endif
#ifdef _XBOX
    int nIcon = 0;
    if (pServerCreature->GetEffectIconCount() > 0)
    {
        do
        {
            if (nIcon >= EFFECT_ICON_CAPACITY)
            {
                break;
            }
            const CResRef& sIcon =
                pServerCreature->m_lstEffectIcons[nIcon]->m_sIconResRef;
            m_lblGood[nIcon].m_cBorder.m_cParams.SetFillImage(sIcon, 0);
            m_lblGood[nIcon].SetShown(1);
            ++nIcon;
        } while (nIcon < pServerCreature->GetEffectIconCount());
    }
    m_lblMore.SetEnabledFlag(pServerCreature->GetEffectIconCount() > EFFECT_ICON_CAPACITY);
    while (nIcon < EFFECT_ICON_CAPACITY)
    {
        if (!m_lblGood[nIcon].m_bEnabled)
        {
            break;
        }
        m_lblGood[nIcon].SetShown(0);
        ++nIcon;
    }
#endif
    if (m_pShownCreature != pCreature ||
        m_nShownAlignment != pServerStats->m_nGoodEvilAlignment)
    {
        CCAppearanceInfo aAppearance = *pCreature->GetAppearance();
        aAppearance.m_nUnmapped10 = OBJECT_INVALID;
        m_pCreature->SetPortrait(pCreature->GetPortrait(0));
        if (g_bDo3DGUIs != 0)
        {
            m_pCreature->SetAppearance(&aAppearance,
                CSWCCreature::APPEARANCE_PART_BODY | CSWCCreature::APPEARANCE_PART_HEAD, 1);
#ifdef _XBOX
            short nAlignment = pCreature->m_pStats->m_nGoodEvilAlignment;
#else
            int nAlignment = pCreature->GetStats()->m_nGoodEvilAlignment;
#endif
            m_pCreature->GetAppearance()->SetAppearanceAlignment(nAlignment);
            CAurScene* pScene = m_viewCharacter.m_cScene.m_pScene;
            m_pCreature->AttachToScene(pScene);
            m_pCreature->SetPosition(Vector());
            m_pCreature->SetOrientation(Vector(1.0f, 0.0f, 0.0f));
            m_pCreature->Setup3DScene(m_viewCharacter, pCreature, 0, 1);
        }
        m_pShownCreature = pCreature;
        m_nShownAlignment = pStats->m_nGoodEvilAlignment;
        m_sldAlign.SetCurValue(ALIGNMENT_SLIDER_MAX - m_nShownAlignment);
#ifndef _XBOX
        unsigned long nMasteryStrRef;
        if (pServerCreature->HasEffect(CSWSEffectListHandler::EFFECT_PUREGOODPOWERS))
        {
            switch (nClass)
            {
            case CSWClass::CLASS_INDEX_JEDIGUARDIAN:
            case CSWClass::CLASS_INDEX_JEDIWEAPONMASTER:
            case CSWClass::CLASS_INDEX_SITHMARAUDER:
                nMasteryStrRef = STRREF_LIGHT_MASTERY_STR;
                break;
            case CSWClass::CLASS_INDEX_JEDISENTINEL:
            case CSWClass::CLASS_INDEX_JEDIWATCHMAN:
            case CSWClass::CLASS_INDEX_SITHASSASSIN:
                nMasteryStrRef = STRREF_LIGHT_MASTERY_CON;
                break;
            case CSWClass::CLASS_INDEX_JEDICONSULAR:
            case CSWClass::CLASS_INDEX_JEDIMASTER:
            case CSWClass::CLASS_INDEX_SITHLORD:
                nMasteryStrRef = STRREF_LIGHT_MASTERY_WIS;
                break;
            default:
                m_lblForceMastery.SetEnabledFlag(0);
                return;
            }
            m_lblForceMastery.SetEnabledFlag(1);
            m_lblForceMastery.m_cText.m_cParams.SetStrRef(nMasteryStrRef);
        }
        else if (pServerCreature->HasEffect(CSWSEffectListHandler::EFFECT_PUREEVILPOWERS))
        {
            switch (nClass)
            {
            case CSWClass::CLASS_INDEX_JEDIGUARDIAN:
            case CSWClass::CLASS_INDEX_JEDIWEAPONMASTER:
            case CSWClass::CLASS_INDEX_SITHMARAUDER:
                nMasteryStrRef = STRREF_DARK_MASTERY_DAMAGE;
                break;
            case CSWClass::CLASS_INDEX_JEDISENTINEL:
            case CSWClass::CLASS_INDEX_JEDIWATCHMAN:
            case CSWClass::CLASS_INDEX_SITHASSASSIN:
                nMasteryStrRef = STRREF_DARK_MASTERY_DEX;
                break;
            case CSWClass::CLASS_INDEX_JEDICONSULAR:
            case CSWClass::CLASS_INDEX_JEDIMASTER:
            case CSWClass::CLASS_INDEX_SITHLORD:
                nMasteryStrRef = STRREF_DARK_MASTERY_FORCE_POINTS;
                break;
            default:
                m_lblForceMastery.SetEnabledFlag(0);
                return;
            }
            m_lblForceMastery.SetEnabledFlag(1);
            m_lblForceMastery.m_cText.m_cParams.SetStrRef(nMasteryStrRef);
        }
        else
        {
            m_lblForceMastery.SetEnabledFlag(0);
        }
#endif
    }
}
// PC 0x0071C750
void CSWGuiInGameCharacter::Draw(float fElapsed)
{
    SetStats();
    m_sldAlign.SetCurValue(m_sldAlign.m_nCurValue);
    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x0071C780
int CSWGuiInGameCharacter::ShowLevelUpGUI(int bAutomatic)
{
    int bLevelUpMode = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_LevelUpMode;
#ifdef _XBOX
    if (bLevelUpMode == 0)
    {
        return 0;
    }
#else
    if (bLevelUpMode != 0 &&
        g_pAppManager->m_pClientExoApp->GetCharacterChangeInProgress() == 0)
#endif
    {
        SetStats();
        CSWGuiLevelUpCharGen* pLevelUp = new CSWGuiLevelUpCharGen(
            GetManager(),
            g_pAppManager->m_pClientExoApp->GetPlayerCreature()->GetObjectId(),
            m_pCreature);
        pLevelUp->m_pInGameCharacter = this;
        pLevelUp->m_nInputClass = bAutomatic != 0 ? INPUT_CLASS_GAME : INPUT_CLASS_GUI;
        m_pManager->AddPanel(pLevelUp, CSWGuiManager::PANEL_MODE_MODAL, 1);
        return 1;
    }
#ifndef _XBOX
    return 0;
#endif
}
// PC 0x0071C880
void CSWGuiInGameCharacter::StartAutoLevelup()
{
    CSWCCreature* pPlayer =
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
    unsigned long nExperience = pPlayer->m_pStats->m_nExperience;
    unsigned char nLevel = pPlayer->m_pStats->GetLevel();
    unsigned char nNextLevel = nLevel + 1;
    CExoString sMessage;
#ifdef _XBOX
    for (;;)
    {
        EXOXBOXASSERT(nNextLevel >= 0);
        if (nExperience < g_pRules->m_anExperience[nNextLevel])
        {
            break;
        }
        ++nNextLevel;
    }
#else
    while (nExperience >= g_pRules->m_anExperience[nNextLevel])
    {
        ++nNextLevel;
    }
#endif
    g_pTlkTable->SetCustomToken(0, nLevel);
    g_pTlkTable->SetCustomToken(1, nNextLevel);
    sMessage = g_pTlkTable->GetSimpleString(0x8FCB);
    g_pTlkTable->ParseStr(sMessage);
    CSWGuiMessageBox* pMessageBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    pMessageBox->SetAllowCancel(1);
    pMessageBox->SetMessage(sMessage);
    pMessageBox->SetCallback(
        this, static_cast<CSWGuiHandler>(&CSWGuiInGameCharacter::DoAutoLevelup),
        0);
    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
// PC 0x0071B190
void CSWGuiBlackenedLabel::Draw(float fElapsed)
{
    CSWGuiExtent cExtent = m_cText.m_cExtent;
    Vector vColor = m_cText.m_cParams.m_vColor;
    m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLACK);
    --cExtent.m_nLeft;
    ++cExtent.m_nTop;
    m_cText.SetExtent(cExtent);
    m_cText.Draw(fElapsed);
    cExtent.m_nLeft += 2;
    m_cText.SetExtent(cExtent);
    m_cText.Draw(fElapsed);
    cExtent.m_nTop -= 2;
    m_cText.SetExtent(cExtent);
    m_cText.Draw(fElapsed);
    cExtent.m_nLeft -= 2;
    m_cText.SetExtent(cExtent);
    m_cText.Draw(fElapsed);
    m_cText.m_cParams.SetColor(vColor);
    ++cExtent.m_nTop;
    ++cExtent.m_nLeft;
    m_cText.SetExtent(cExtent);
    CSWGuiLabel::Draw(fElapsed);
}
// PC 0x0071B2D0
void CSWGuiInGameCharacter::AttachModel()
{
    if (g_bDo3DGUIs != 0)
    {
        m_pCreature->AttachToScene(m_viewCharacter.m_cScene.m_pScene);
        CAurBehaviorAttachTranslate* pNode =
            (CAurBehaviorAttachTranslate*)m_viewCharacter.m_cScene.m_pCamera->GetBehavior(CAurBehavior::TYPE_ATTACH_TRANSLATE);
        if (pNode != 0)
        {
            pNode->m_nWorldSpace = 1;
            pNode->m_vTranslation =
                Vector(0.0f, 0.0f, m_pCreature->GetHeadHeightOffset());
        }
    }
}
// PC 0x0071B360
void CSWGuiInGameCharacter::DoAutoLevelup(CSWGuiControl* pControl)
{
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pInGame->m_pMessageBox->m_bAccepted)
    {
        CSWCCreature* pPlayer =
            g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
        CSWSCreature* pServerCreature = pPlayer->GetServerCreature();
        if (pServerCreature != 0)
        {
            pServerCreature->m_pStats->AutoLevelup(0);
        }
    }
}
// PC 0x0071B3B0
void CSWGuiInGameCharacter::SetLabelBonusPenaltyColour(CSWGuiLabel& lblStat,
                                                       int nBonus)
{
    if (nBonus > 0)
    {
        lblStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_BONUS);
    }
    else if (nBonus < 0)
    {
        lblStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_RED);
    }
    else
    {
        lblStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
    }
}

// PC 0x0071B400
void CSWGuiInGameCharacter::RotateCharacter(int nDirection)
{
    m_pCreature->SetOrientation(
        YawPitchRoll(nDirection ? -10.0f : 10.0f, 0.0f, 0.0f) * m_pCreature->m_vOrientation);
}

#ifndef _XBOX
void CSWGuiInGameCharacter::RotateCharacterLeft(int nUnused)
{
    m_pCreature->SetOrientation(
        YawPitchRoll(-10.0f, 0.0f, 0.0f) * m_pCreature->m_vOrientation);
}

void CSWGuiInGameCharacter::RotateCharacterRight(int nUnused)
{
    m_pCreature->SetOrientation(
        YawPitchRoll(10.0f, 0.0f, 0.0f) * m_pCreature->m_vOrientation);
}

#endif
