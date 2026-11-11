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
// PC 0x006AE980
void CSWGuiInGameCharacter::OnPanelRemoved()
{
    m_pShownCreature = 0;
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x006AE990  Xbox 0x000E5030
inline void CSWGuiInGameCharacter::SetModText(CSWGuiLabel& lblMod, int nMod)
{
    lblMod.m_cText.m_cParams.SetText(nMod > 0 ? CExoString("+") + nMod
                                              : CExoString(nMod));
}
// PC 0x006B0E40  Xbox 0x000E6960
CSWGuiInGameCharacter::CSWGuiInGameCharacter(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifndef _XBOX
    m_nSelectedNPC = CHARACTER_INDEX_PLAYER;
#endif
    StartLoadFromLayout("character");
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblClass, "LBL_CLASS", 1);
    InitControl(m_lblClass1, "LBL_CLASS1", 1);
    InitControl(m_lblClass2, "LBL_CLASS2", 1);
    InitControl(m_lblLevel, "LBL_LEVEL", 1);
    InitControl(m_lblLevel1, "LBL_LEVEL1", 1);
    InitControl(m_lblLevel2, "LBL_LEVEL2", 1);
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
    m_pScriptSelect = new CSWGuiScriptSelect(m_pManager);
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
    InitControl(m_btnChange1, "BTN_CHANGE1", 1);
    InitControl(m_btnChange2, "BTN_CHANGE2", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_btnScripts, "BTN_SCRIPTS", 1);
    InitControl(m_btn3DChar, "BTN_3DCHAR", 1);
    InitControl(m_btnCharLeft, "BTN_CHARLEFT", 1);
    InitControl(m_btnCharRight, "BTN_CHARRIGHT", 1);
    CExoString sName;
    for (int nGood = 0; nGood < EFFECT_ICON_CAPACITY; ++nGood)
    {
        sName.Format("LBL_GOOD%d", nGood + 1);
        InitControl(m_lblGood[nGood], sName, 1);
        m_lblGood[nGood].SetShown(0);
    }
    StopLoadFromLayout();
    m_btnCharLeft.SetFocusable(0);
    m_btnCharLeft.AddEvent(
        GUIEVENT_ACTIVATE, this,
        static_cast<CSWGuiHandler>(&CSWGuiInGameCharacter::OnSwitchLeft));
    m_btnCharRight.SetFocusable(0);
    m_btnCharRight.AddEvent(
        GUIEVENT_ACTIVATE, this,
        static_cast<CSWGuiHandler>(&CSWGuiInGameCharacter::OnSwitchRight));
    m_btnCharRight.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_btnCharRight.m_cHilite.m_cParams.m_fRotation = 180.0f;
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
    m_pScriptSelect = new CSWGuiScriptSelect(m_pManager);
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
    m_btnChange1.SetFocusable(0);
    m_btnChange1.AddEvent(
        GUIEVENT_ACTIVATE, this,
        static_cast<CSWGuiHandler>(&CSWGuiInGameCharacter::OnChangeCharacter));
    m_btnChange2.SetFocusable(0);
    m_btnChange2.AddEvent(
        GUIEVENT_ACTIVATE, this,
        static_cast<CSWGuiHandler>(&CSWGuiInGameCharacter::OnChangeCharacter));
    m_btnExit.SetFocusable(0);
    m_btnExit.AddEvent(
        GUIEVENT_ACTIVATE, this,
        reinterpret_cast<CSWGuiHandler>(&CSWGuiPanel::OnBButtonPressed));
    m_btnScripts.SetFocusable(0);
    m_btnScripts.AddEvent(
        GUIEVENT_ACTIVATE, this,
        reinterpret_cast<CSWGuiHandler>(&CSWGuiPanel::OnXButtonPressed));
    m_sldAlign.SetFocusable(0);
    m_btnChange1.SetToolTip(STRREF_CHANGE_CHARACTER, EVENT_NEXT_CHARACTER);
#ifndef _XBOX
    m_btnChange2.m_nToolTipStrRef = STRREF_CHANGE_CHARACTER;
#endif
#endif
}
// PC 0x006B2220  Xbox 0x000E7BF0
void CSWGuiInGameCharacter::OnPanelAdded()
{
#ifdef _XBOX
    SetStats();
    m_lblBlack.SetEnabledFlag(
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
    m_lblChangeChar.SetEnabledFlag(
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
#else
    m_nSelectedNPC = -1;
    m_btnChange1.m_nIndex = -2;
    m_btnChange2.m_nIndex = -2;
    SetStats();
    UpdatePortraits();
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}

#ifdef _XBOX
void CSWGuiInGameCharacter::HandleInputEvent(int nEvent, int nState)  // PC 0x006B2250  Xbox 0x000E7C60
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
        case EVENT_SCRIPT_SELECT:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWCCreature* pLeader =
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
            if (pLeader != 0)
            {
                unsigned long oidLeader = pLeader->m_nId;
                oidLeader = g_pAppManager->m_pServerExoApp
                                ->ClientToServerObjectId(oidLeader);
                m_pScriptSelect->SetCreatureID(oidLeader);
                m_pManager->AddPanel(m_pScriptSelect,
                                     CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
            else
            {
                EXOXBOXASSERT(0);
            }
            break;
        }
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
void CSWGuiInGameCharacter::HandleInputEvent(int nEvent, int nState)  // PC 0x006B2250  Xbox 0x000E7C60
{
    g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()
                        ->m_LevelUpMode != 0 &&
                g_pAppManager->m_pClientExoApp
                        ->GetCharacterChangeInProgress() == 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                ShowLevelUpGUI(0);
            }
            break;
        case EVENT_SCRIPT_SELECT:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWCCreature* pLeader =
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
            if (pLeader != 0)
            {
                unsigned long oidLeader = pLeader->m_nId;
                oidLeader = g_pAppManager->m_pServerExoApp
                                ->ClientToServerObjectId(oidLeader);
                m_pScriptSelect->SetCreatureID(oidLeader);
                m_pManager->AddPanel(m_pScriptSelect,
                                     CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
            break;
        }
        case EVENT_AUTO_LEVELUP:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()
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
            CServerInfo* pInfo =
                g_pAppManager->m_pServerExoApp->GetServerInfo();
            if (pInfo->m_bPartyPicking)
            {
                int bFound = 0;
                int nNPC = m_nSelectedNPC + 1;
                while (nNPC < CSWPartyTable::MAX_NPCS)
                {
                    if (bFound != 0)
                    {
                        break;
                    }
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        if (pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC)) != 0)
                        {
                            m_nSelectedNPC = nNPC;
                            bFound = 1;
                        }
                    }
                    ++nNPC;
                }
                if (bFound == 0)
                {
                    m_nSelectedNPC = -1;
                }
            }
            else
            {
                g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0, 1);
            }
            UpdatePortraits();
            break;
        }
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
        case EVENT_EXIT:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(
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
// PC 0x006AF970  Xbox 0x000E54B0
CSWGuiInGameCharacter::~CSWGuiInGameCharacter()
{
    delete m_pScriptSelect;
    delete m_pCreature;
}
// PC 0x006AFDA0  Xbox 0x000E5900
void CSWGuiInGameCharacter::SetStats()
{
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    m_nPartySize = pClient->GetSWParty()->GetNumCharacters();
    CSWCCreature* pCreature;
#ifdef _XBOX
    pCreature = pClient->GetSWParty()->GetCharacter(0);
#else
    if (!g_pAppManager->GetServerExoApp()->GetServerInfo()->m_bPartyPicking ||
        m_nSelectedNPC == -1)
    {
        pCreature = pClient->GetSWParty()->GetCharacter(0);
    }
    else
    {
        unsigned long oidNPC =
            g_pAppManager->GetServerExoApp()->GetPartyTable()->GetNPCObject(
                m_nSelectedNPC, 0, 1);
        pCreature = pClient->GetCreatureByGameObjectID(
            pClient->ServerToClientObjectId(oidNPC));
    }
#endif
    CSWCCreatureStats* pStats = pCreature->m_pStats;
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    CSWSCreatureStats* pServerStats = pServerCreature->m_pStats;
    unsigned char nClassIndex = pStats->GetNumClasses() - 1;
    unsigned char nClass = pStats->GetClass(nClassIndex);
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
    CExoString sHitPoints;
    sHitPoints.Format("%d/%d", pStats->GetCurrentHitPoints(),
                      pStats->GetMaxHitPoints());
    m_lblVitalityStat.m_cText.m_cParams.SetText(sHitPoints);
    SetLabelBonusPenaltyColour(m_lblVitalityStat, pStats->m_nHitPointBonus);
    m_lblDefenseStat.m_cText.m_cParams.SetText(
        pStats->GetArmorClass(0));
    SetLabelBonusPenaltyColour(
        m_lblDefenseStat,
        pCreature->GetServerCreature()->GetTotalACEffectBonus());
    if (CSWClass::IsJedi(nClass) != 0)
    {
        short nForcePoints = pStats->m_nCurrentForcePoints;
        short nMaxForcePoints = pStats->m_nMaxForcePoints;
        nForcePoints += pStats->m_nForcePointBonus;
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
    int bCanLevelUp = (m_nSelectedNPC == -1 ||
                      !g_pAppManager->GetServerExoApp()->GetServerInfo()->m_bPartyPicking) &&
                      pServerStats->CanLevelUp() != 0;
    m_btnLevelUp.SetEnabledFlag(bCanLevelUp);
    m_btnAuto.SetEnabledFlag(bCanLevelUp);
#endif
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
            short nAlignment = pCreature->m_pStats->m_nGoodEvilAlignment;
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
    }
}
// PC 0x006B0B90  Xbox 0x000E66D0
void CSWGuiInGameCharacter::Draw(float fElapsed)
{
    SetStats();
    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006B0BB0  Xbox 0x000E66F0
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
// PC 0x006B0CB0  Xbox 0x000E67D0
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
// PC 0x006AEAC0  Xbox 0x000E5160
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
// PC 0x006AEBF0  Xbox 0x000E52A0
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
// PC 0x006AEC80  Xbox 0x000E5330
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
// PC 0x006AECD0  Xbox 0x000E5380
void CSWGuiInGameCharacter::SetLabelBonusPenaltyColour(CSWGuiLabel& lblStat,
                                                       int nBonus)
{
    if (nBonus > 0)
    {
        lblStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_GREEN);
    }
    else if (nBonus < 0)
    {
        lblStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_RED);
    }
    else
    {
        lblStat.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    }
}
// PC 0x006AED20  Xbox 0x000E53D0
void CSWGuiInGameCharacter::RotateCharacter(int nDirection)
{
    float fYaw = nDirection ? -10.0f : 10.0f;
    m_pCreature->SetOrientation(
        YawPitchRoll(fYaw, 0.0f, 0.0f) * m_pCreature->m_vOrientation);
}

#ifndef _XBOX
void CSWGuiInGameCharacter::RotateCharacterLeft(int nUnused)  // PC 0x006AED90
{
    m_pCreature->SetOrientation(
        YawPitchRoll(-10.0f, 0.0f, 0.0f) * m_pCreature->m_vOrientation);
}
// PC 0x006AEDE0
void CSWGuiInGameCharacter::RotateCharacterRight(int nUnused)
{
    m_pCreature->SetOrientation(
        YawPitchRoll(10.0f, 0.0f, 0.0f) * m_pCreature->m_vOrientation);
}
// PC 0x006AEE30
void CSWGuiInGameCharacter::UpdatePortraits()
{
    CSWServerInfo* pInfo =
        (CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo();
    if (pInfo->m_bPartyPicking)
    {
        int bSecond = 0;
        int bFirst = 0;
        int nIndex = m_btnChange1.m_nIndex;
        char nStart = nIndex;
        if ((char)nIndex == CHARACTER_INDEX_NONE)
        {
            nStart = m_nSelectedNPC + 1;
        }
        int nNPC;
        for (nNPC = nStart; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
        {
            if (bSecond != 0)
            {
                break;
            }
            if (nNPC != m_nSelectedNPC)
            {
                if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                        ->GetIsNPCAvailable(nNPC) != 0)
                {
                    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                    unsigned long oidNPC =
                        g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetNPCObject(nNPC, 0, 1);
                    CSWCCreature* pCreature =
                        pClient->GetCreatureByGameObjectID(
                            pClient->ServerToClientObjectId(oidNPC));
                    if (pCreature != 0)
                    {
                        if (bFirst != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange2.m_nIndex = nNPC;
                            bSecond = 1;
                        }
                        else
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange1.m_nIndex = nNPC;
                            bFirst = 1;
                        }
                    }
                }
            }
        }
        if (m_nSelectedNPC != CHARACTER_INDEX_PLAYER && bSecond == 0)
        {
            if (bFirst == 0)
            {
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange1.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
                }
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange1.m_cHilite.m_cParams.SetFillImage(*pPortrait, 0);
                }
                m_btnChange1.m_nIndex = CHARACTER_INDEX_PLAYER;
                bFirst = 1;
            }
            else
            {
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange2.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
                }
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange2.m_cHilite.m_cParams.SetFillImage(*pPortrait, 0);
                }
                m_btnChange2.m_nIndex = CHARACTER_INDEX_PLAYER;
                bSecond = 1;
            }
        }
        if (bSecond == 0)
        {
            for (int nLow = 0; nLow < nStart; ++nLow)
            {
                if (bSecond != 0)
                {
                    break;
                }
                if (nLow != m_nSelectedNPC)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nLow) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nLow, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            if (bFirst != 0)
                            {
                                {
                                    CResRef* pPortrait = &pCreature->GetPortrait(1);
                                    m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                        *pPortrait, 0);
                                }
                                {
                                    CResRef* pPortrait = &pCreature->GetPortrait(1);
                                    m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                        *pPortrait, 0);
                                }
                                m_btnChange2.m_nIndex = nLow;
                                bSecond = 1;
                            }
                            else
                            {
                                {
                                    CResRef* pPortrait = &pCreature->GetPortrait(1);
                                    m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                        *pPortrait, 0);
                                }
                                {
                                    CResRef* pPortrait = &pCreature->GetPortrait(1);
                                    m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                        *pPortrait, 0);
                                }
                                m_btnChange1.m_nIndex = nLow;
                                bFirst = 1;
                            }
                        }
                    }
                }
            }
        }
        m_btnChange1.SetEnabledFlag(bFirst);
        m_btnChange2.SetEnabledFlag(bSecond);
        m_btnCharLeft.SetEnabledFlag(
            g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->GetNumNPCAvailable() > 2);
        m_btnCharRight.SetEnabledFlag(
            g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->GetNumNPCAvailable() > 2);
    }
    else
    {
        CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
        if (pParty->m_nCharacters > 1)
        {
            m_btnChange1.m_bEnabled = 1;
            {
                CResRef* pPortrait = &pParty->GetCharacter(1)->GetPortrait(1);
                m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                    *pPortrait, 0);
            }
            {
                CResRef* pPortrait = &pParty->GetCharacter(1)->GetPortrait(1);
                m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                    *pPortrait, 0);
            }
        }
        else
        {
            m_btnChange1.m_bEnabled = 0;
        }
        if (pParty->m_nCharacters > 2)
        {
            m_btnChange2.m_bEnabled = 1;
            {
                CResRef* pPortrait = &pParty->GetCharacter(2)->GetPortrait(1);
                m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                    *pPortrait, 0);
            }
            {
                CResRef* pPortrait = &pParty->GetCharacter(2)->GetPortrait(1);
                m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                    *pPortrait, 0);
            }
        }
        else
        {
            m_btnChange2.m_bEnabled = 0;
        }
    }
}
// PC 0x006AF350
void CSWGuiInGameCharacter::OnChangeCharacter(CSWGuiControl* pControl)
{
    int nMember = 1;
    if (pControl->m_bSelected != 0)
    {
        CSWServerInfo* pInfo =
            (CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo();
        if (pInfo->m_bPartyPicking)
        {
            char nNPC = pControl->m_nIndex;
            CSWCCreature* pCreature;
            if (nNPC == CHARACTER_INDEX_PLAYER)
            {
                pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            }
            else
            {
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                unsigned long oidNPC =
                    g_pAppManager->m_pServerExoApp->GetPartyTable()
                        ->GetNPCObject(nNPC, 0, 1);
                pCreature = pClient->GetCreatureByGameObjectID(
                    pClient->ServerToClientObjectId(oidNPC));
            }
            if (pCreature != 0)
            {
                if (m_btnCharRight.m_bEnabled)
                {
                    if (pControl == &m_btnChange1)
                    {
                        m_btnChange1.m_nIndex = m_btnChange2.m_nIndex;
                    }
                }
                else
                {
                    m_btnChange1.m_nIndex = CHARACTER_INDEX_NONE;
                }
                m_nSelectedNPC = nNPC;
            }
        }
        else
        {
            if (pControl == &m_btnChange2)
            {
                nMember = 2;
            }
            if (g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0, nMember) == 0)
            {
                return;
            }
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        UpdatePortraits();
    }
}
// PC 0x006AF450
void CSWGuiInGameCharacter::OnSwitchLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        char nFirst = m_btnChange1.m_nIndex;
        int bFound = 0;
        if (nFirst != CHARACTER_INDEX_NONE)
        {
            m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                m_btnChange1.m_cBorder.m_cParams.m_cFillImage, 0);
            m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                m_btnChange1.m_cHilite.m_cParams.m_cFillImage, 0);
            m_btnChange2.m_nIndex = m_btnChange1.m_nIndex;
            int nNPC = nFirst - 1;
            while (nNPC >= 0 && bFound == 0)
            {
                if (nNPC != m_nSelectedNPC)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange1.m_nIndex = nNPC;
                            bFound = 1;
                        }
                    }
                }
                --nNPC;
            }
            if (bFound == 0)
            {
                if (m_nSelectedNPC == CHARACTER_INDEX_PLAYER ||
                    (char)m_btnChange2.m_nIndex == CHARACTER_INDEX_PLAYER)
                {
                    for (int nHigh = CSWPartyTable::MAX_NPCS - 1; nHigh > nFirst && bFound == 0; --nHigh)
                    {
                        if (nHigh != m_nSelectedNPC)
                        {
                            if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                                    ->GetIsNPCAvailable(nHigh) != 0)
                            {
                                CClientExoApp* pClient =
                                    g_pAppManager->m_pClientExoApp;
                                unsigned long oidNPC =
                                    g_pAppManager->m_pServerExoApp
                                        ->GetPartyTable()
                                        ->GetNPCObject(nHigh, 0, 1);
                                CSWCCreature* pCreature =
                                    pClient->GetCreatureByGameObjectID(
                                        pClient->ServerToClientObjectId(
                                            oidNPC));
                                if (pCreature != 0)
                                {
                                    {
                                        CResRef* pPortrait = &pCreature->GetPortrait(1);
                                        m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                            *pPortrait, 0);
                                    }
                                    {
                                        CResRef* pPortrait = &pCreature->GetPortrait(1);
                                        m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                            *pPortrait, 0);
                                    }
                                    m_btnChange1.m_nIndex = nHigh;
                                    bFound = 1;
                                }
                            }
                        }
                    }
                }
                else
                {
                    {
                        CResRef* pPortrait = &g_pAppManager->m_pClientExoApp->GetPlayerCreature()->GetPortrait(1);
                        m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    {
                        CResRef* pPortrait = &g_pAppManager->m_pClientExoApp->GetPlayerCreature()->GetPortrait(1);
                        m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    m_btnChange1.m_nIndex = CHARACTER_INDEX_PLAYER;
                }
            }
        }
    }
}
// PC 0x006AF6D0
void CSWGuiInGameCharacter::OnSwitchRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        char nSecond = m_btnChange2.m_nIndex;
        int bFound = 0;
        if (nSecond != CHARACTER_INDEX_NONE)
        {
            m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                m_btnChange2.m_cBorder.m_cParams.m_cFillImage, 0);
            m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                m_btnChange2.m_cHilite.m_cParams.m_cFillImage, 0);
            m_btnChange1.m_nIndex = m_btnChange2.m_nIndex;
            int nNPC = nSecond + 1;
            while (nNPC < CSWPartyTable::MAX_NPCS && bFound == 0)
            {
                if (nNPC != m_nSelectedNPC)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange2.m_nIndex = nNPC;
                            bFound = 1;
                        }
                    }
                }
                ++nNPC;
            }
            if (bFound == 0)
            {
                if (m_nSelectedNPC != CHARACTER_INDEX_PLAYER)
                {
                    {
                        CResRef* pPortrait = &g_pAppManager->m_pClientExoApp->GetPlayerCreature() ->GetPortrait(1);
                        m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    {
                        CResRef* pPortrait = &g_pAppManager->m_pClientExoApp->GetPlayerCreature() ->GetPortrait(1);
                        m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    m_btnChange2.m_nIndex = CHARACTER_INDEX_PLAYER;
                }
                else
                {
                    for (int nLow = 0; nLow < nSecond && bFound == 0; ++nLow)
                    {
                        if (nLow != m_nSelectedNPC)
                        {
                            if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                                    ->GetIsNPCAvailable(nLow) != 0)
                            {
                                CClientExoApp* pClient =
                                    g_pAppManager->m_pClientExoApp;
                                unsigned long oidNPC =
                                    g_pAppManager->m_pServerExoApp
                                        ->GetPartyTable()
                                        ->GetNPCObject(nLow, 0, 1);
                                CSWCCreature* pCreature =
                                    pClient->GetCreatureByGameObjectID(
                                        pClient->ServerToClientObjectId(
                                            oidNPC));
                                if (pCreature != 0)
                                {
                                    {
                                        CResRef* pPortrait = &pCreature->GetPortrait(1);
                                        m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                            *pPortrait, 0);
                                    }
                                    {
                                        CResRef* pPortrait = &pCreature->GetPortrait(1);
                                        m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                            *pPortrait, 0);
                                    }
                                    m_btnChange2.m_nIndex = nLow;
                                    bFound = 1;
                                }
                            }
                        }
                    }
                }

            }
        }
    }
}
#endif
