#include "exobase.h"
#include "aurcamera.h"
#include "exoresman.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwsvartable.h"
#include "clientoptions.h"
#include "exoinput.h"
#include "exotlktable.h"

#include "clientexoappinternal.h"
#include "nwccreaturestats.h"
#include "nwmessage.h"
#include "netlayer.h"
#include "nwcanimbase.h"
#include "twodimarray.h"
#include "nwccreatureappearance.h"
#include "nwclass.h"
#include "nwscript.h"
#include "aurscene.h"
#include "swmain.h"
#include "nwccreature.h"
#include "swguiclassselection.h"
#include "swguimainchargen.h"
#include "guiingame.h"
#include <stdlib.h>

CSWGuiClassType CSWGuiClassSelection::CHAR_TYPES[6] = {
    {CSWClass::CCLASS_JEDICONSULAR, CCAppearanceInfo::GENDER_MALE, SOUNDSET_PLAYER_MALE, 0,
     STRREF_JEDI_CONSULAR_DESCRIPTION},
    {CSWClass::CCLASS_JEDISENTINEL, CCAppearanceInfo::GENDER_MALE, SOUNDSET_PLAYER_MALE, 0,
     STRREF_JEDI_SENTINEL_DESCRIPTION},
    {CSWClass::CCLASS_JEDIGUARDIAN, CCAppearanceInfo::GENDER_MALE, SOUNDSET_PLAYER_MALE, 0,
     STRREF_JEDI_GUARDIAN_DESCRIPTION},
    {CSWClass::CCLASS_JEDIGUARDIAN, CCAppearanceInfo::GENDER_FEMALE, SOUNDSET_PLAYER_FEMALE, 0,
     STRREF_JEDI_GUARDIAN_DESCRIPTION},
    {CSWClass::CCLASS_JEDISENTINEL, CCAppearanceInfo::GENDER_FEMALE, SOUNDSET_PLAYER_FEMALE, 0,
     STRREF_JEDI_SENTINEL_DESCRIPTION},
    {CSWClass::CCLASS_JEDICONSULAR, CCAppearanceInfo::GENDER_FEMALE, SOUNDSET_PLAYER_FEMALE, 0,
     STRREF_JEDI_CONSULAR_DESCRIPTION}
};
// PC 0x007499E0
void CSWGuiClassSelection::OnPanelAdded()
{
    {
        CExoString sGameInProgress("GAMEINPROGRESS:");

        g_pExoResMan->CleanDirectory(sGameInProgress, 0, 0);
    }
    g_pExoInput->CoolDownEvent(40, 500);
    g_pExoInput->CoolDownEvent(46, 500);
    g_pAppManager->m_pClientExoApp->HideLoadScreen(1, 0, 1);
    CSWGuiPanel::OnPanelAdded();
}
// PC 0x00749A90
void CSWGuiClassSelection::HandleCancelButton()
{
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    g_pAppManager->m_pClientExoApp->StopLoadMusic();
    g_pAppManager->m_pClientExoApp->SetStartMenuMusicOnMovieEnd(1);
    g_pAppManager->m_pClientExoApp->DisplayMainMenu(0);
    g_pAppManager->DestroyServer();
}
// PC 0x00749AE0
void CSWGuiClassSelection::OnSelectButton(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;

    if (nSelected != 0)
    {
        if (pControl->m_nEvent == 45)
        {
            char nSound = pControl->m_nClickSound;

            m_pManager->PlayGuiSound(nSound);
        }

        int i = 0;
        CSWGuiClassSelChar* pCharacter = m_aCharacters;
        do
        {
            if (pControl == &pCharacter->m_btnSelect)
            {
                break;
            }
            i++;
            pCharacter++;
        } while (i < 6);

        m_pSelectedCreature = m_aCharacters[i].m_pCreature;
        CSWGuiMainCharGen* pCharGen =
            new CSWGuiMainCharGen(m_pManager, m_pSelectedCreature, this);
        m_pManager->AddPanel(pCharGen, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }

}

#include "nwclass.h"
#include "nwscript.h"
// PC 0x00749BA0
void CSWGuiClassSelection::OnEnterButton(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;

    if (nSelected != 0)
    {
        int i = 0;
        CSWGuiClassSelChar* pCharacter = m_aCharacters;
        do
        {
            if (pControl == &pCharacter->m_btnSelect)
            {
                break;
            }
            i++;
            pCharacter++;
        } while (i < 6);

        CExoString sClass;
        if (CHAR_TYPES[i].m_nGender == 0)
        {
            sClass = g_pTlkTable->GetSimpleString(STRREF_MALE);
        }
        else
        {
            sClass = g_pTlkTable->GetSimpleString(STRREF_FEMALE);
        }

        if (CHAR_TYPES[i].m_nClass == CSWClass::CCLASS_JEDICONSULAR)
        {
            sClass = sClass + " "
                + g_pTlkTable->GetSimpleString(STRREF_JEDI_CONSULAR);
        }
        else if (CHAR_TYPES[i].m_nClass == CSWClass::CCLASS_JEDISENTINEL)
        {
            sClass = sClass + " "
                + g_pTlkTable->GetSimpleString(STRREF_JEDI_SENTINEL);
        }
        else
        {
            sClass = sClass + " "
                + g_pTlkTable->GetSimpleString(STRREF_JEDI_GUARDIAN);
        }

        m_lblDescription.m_cText.m_cParams.SetStrRef(
            CHAR_TYPES[i].m_nDescriptionStrRef);
        m_lblClass.m_cText.m_cParams.SetText(sClass);
    }
}
// PC 0x00749E60
void CSWGuiClassSelection::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleCancelButton();
            break;
        case GUIEVENT_PREVIOUS:
            HandleInputEvent(GUIEVENT_LEFT, nState);
            break;
        case GUIEVENT_NEXT:
            HandleInputEvent(GUIEVENT_RIGHT, nState);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00749F00
void CSWGuiClassSelection::BeginPlayGame()
{
    g_pAppManager->m_pClientExoApp->BusyWaitMenuMusic();
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->SetGlobalUnpauseableState(1);

    CExoString sMessage;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

    char nFortitudeSave = m_pSelectedCreature->GetStats()->m_nFortitudeSave;
    m_pSelectedCreature->GetStats()->m_nFortitudeSave = nFortitudeSave;
    char nReflexSave = m_pSelectedCreature->GetStats()->m_nReflexSave;
    m_pSelectedCreature->GetStats()->m_nReflexSave = nReflexSave;
    char nWillSave = m_pSelectedCreature->GetStats()->m_nWillSave;
    m_pSelectedCreature->GetStats()->m_nWillSave = nWillSave;
    m_pSelectedCreature->GetStats()->m_nForcePointsBase = 0;

    g_pAppManager->CreateServer(0);
    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
        ->SetValueNumber("G_PC_LEVEL", 1);
    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
        ->SetValueNumber("G_PC_Align_Val", 50);
    {
        CExoString sGlobal("GBL_GAME_COMPLETE");
        char nGameCompleted =
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bGameCompleted;
        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
            ->SetValueNumber(sGlobal, nGameCompleted);
    }
    m_pSelectedCreature->SaveCharGenCreature();
    {
        CExoString sPassword("");
        pClientExoApp->GetNetLayer()->StartConnectToSession(
            0, pClientExoApp->GetClientName(), sPassword,
            CNetLayer::LOCAL_SESSION_PORT, CNetLayer::CONNECTTYPE_PLAYER);
    }
    pClientExoApp->GetSWCMessage()
        ->SendPlayerToServerStatus_RequestStatus();
    sMessage.Format("%c%s.%s %s", 's', "Module", "Load", m_sModule);
    pClientExoApp->GetNetLayer()->SendMessageToPlayer(
        CNetLayer::PLAYERID_SERVER, (unsigned char*)sMessage.CStr(),
        sMessage.GetLength(), 0);
    pClientExoApp->SetPlayerCharacterName("temp");
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
    pClientExoApp->StopLoadMusic();
    pClientExoApp->SetLoadScreenByModuleName(m_sModule);
    pClientExoApp->SetLoadMusic(
        pClientExoApp->GetLoadMusicByModuleName(m_sModule));
    pClientExoApp->ShowLoadScreen(1, 1, 0);
    pClientExoApp->SetLoadScreenHint(
        pClientExoApp->GetNextLoadScreenHintSTRREF(m_sModule, 0));
}
// PC 0x0074A220
void CSWGuiClassSelection::Draw(float fElapsed)
{
    float fScale = 1.0f;
    if (g_pAppManager->m_pClientExoApp->GetSWGuiManager() != NULL)
    {
        fScale = (float)g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight
                 / (float)g_nGUIHeight;
    }
    CSWGuiExtent cButtonExtent;
    CSWGuiExtent cModelExtent;
    cButtonExtent.m_nLeft = cButtonExtent.m_nTop = cButtonExtent.m_nWidth =
        cButtonExtent.m_nHeight = 0;
    cModelExtent.m_nLeft = cModelExtent.m_nTop = cModelExtent.m_nWidth =
        cModelExtent.m_nHeight = 0;

    for (int i = 0; i < 6; i++)
    {
        if (g_bDo3DGUIs != 0 && m_bNeedsDraw)
        {
            CAurScene* pScene = m_aCharacters[i].m_viewModel.m_cScene.m_pScene;
            if (m_aCharacters[i].m_pCreature->GetAnimationBase()
                    ->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN)->GetScene()
                != pScene)
            {
                m_pSelectedCreature->AttachToScene(pScene);
            }
            m_aCharacters[i].m_pCreature->CGPauseCycle(fElapsed, 0);
        }

        cButtonExtent = m_aCharacters[i].m_btnSelect.m_cExtent;
        cModelExtent = m_aCharacters[i].m_viewModel.m_cExtent;
        if (m_aCharacters[i].m_btnSelect.m_bActive)
        {
            float fTopActive = fScale * PREVIEW_TOP_ACTIVE;
            int nTopActive = (int)fTopActive;
            int nDistance = cButtonExtent.m_nTop - nTopActive;
            if (nDistance != 0)
            {
                if (m_aCharacters[i].m_fTransitionTime == CLASSSELECTION_TRANSITION_IDLE)
                {
                    m_aCharacters[i].m_fTransitionTime =
                        (nDistance / (fScale * PREVIEW_TOP_NORMAL - fTopActive))
                        * CLASSSELECTION_TRANSITION_TIME;
                }
                else
                {
                    m_aCharacters[i].m_fTransitionTime -= fElapsed;
                    int nTop;
                    if (m_aCharacters[i].m_fTransitionTime <= 0.0f)
                    {
                        nTop = nTopActive;
                    }
                    else
                    {
                        float fPercent = (CLASSSELECTION_TRANSITION_TIME
                                          - m_aCharacters[i].m_fTransitionTime)
                                         / CLASSSELECTION_TRANSITION_TIME;
                        nTop = (int)(fScale * PREVIEW_TOP_NORMAL)
                            - (int)(fPercent * (fScale * PREVIEW_TOP_NORMAL - fTopActive));
                    }
                    if (nTop <= fTopActive)
                    {
                        m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
                        nTop = nTopActive;
                    }
                    int nChange = cButtonExtent.m_nTop - nTop;
                    if (nChange != 0)
                    {
                        cButtonExtent.m_nTop -= nChange;
                        cButtonExtent.m_nLeft -= nChange;
                        cButtonExtent.m_nWidth += 2 * nChange;
                        cButtonExtent.m_nHeight += 2 * nChange;
                        m_aCharacters[i].m_btnSelect.SetExtent(cButtonExtent);
                        cModelExtent.m_nTop -= nChange;
                        cModelExtent.m_nLeft -= nChange;
                        cModelExtent.m_nWidth += 2 * nChange;
                        cModelExtent.m_nHeight += 2 * nChange;
                        m_aCharacters[i].m_viewModel.SetExtent(cModelExtent);
                    }
                }
            }
            else
            {
                m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
            }
        }
        else
        {
            int nDistance = (int)(fScale * PREVIEW_TOP_NORMAL) - cButtonExtent.m_nTop;
            if (nDistance != 0)
            {
                if (m_aCharacters[i].m_fTransitionTime == CLASSSELECTION_TRANSITION_IDLE)
                {
                    m_aCharacters[i].m_fTransitionTime =
                        (nDistance / (fScale * PREVIEW_TOP_NORMAL - fScale * PREVIEW_TOP_ACTIVE))
                        * CLASSSELECTION_TRANSITION_TIME;
                }
                else
                {
                    m_aCharacters[i].m_fTransitionTime -= fElapsed;
                    int nTop;
                    if (m_aCharacters[i].m_fTransitionTime <= 0.0f)
                    {
                        nTop = (int)(fScale * PREVIEW_TOP_NORMAL);
                    }
                    else
                    {
                        float fPercent = (CLASSSELECTION_TRANSITION_TIME
                                          - m_aCharacters[i].m_fTransitionTime)
                                         / CLASSSELECTION_TRANSITION_TIME;
                        nTop = (int)(fPercent * (fScale * PREVIEW_TOP_NORMAL - fScale * PREVIEW_TOP_ACTIVE))
                            + (int)(fScale * PREVIEW_TOP_ACTIVE);
                    }
                    if (nTop >= fScale * PREVIEW_TOP_NORMAL)
                    {
                        m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
                        nTop = (int)(fScale * PREVIEW_TOP_NORMAL);
                    }
                    int nChange = nTop - cButtonExtent.m_nTop;
                    if (nChange != 0)
                    {
                        cButtonExtent.m_nTop += nChange;
                        cButtonExtent.m_nLeft += nChange;
                        cButtonExtent.m_nWidth -= 2 * nChange;
                        cButtonExtent.m_nHeight -= 2 * nChange;
                        m_aCharacters[i].m_btnSelect.SetExtent(cButtonExtent);
                        cModelExtent.m_nTop += nChange;
                        cModelExtent.m_nLeft += nChange;
                        cModelExtent.m_nWidth -= 2 * nChange;
                        cModelExtent.m_nHeight -= 2 * nChange;
                        m_aCharacters[i].m_viewModel.SetExtent(cModelExtent);
                    }
                }
            }
            else
            {
                m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
            }
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x0074A650
CSWGuiClassSelection::~CSWGuiClassSelection()
{
    for (int i = 0; i < 6; i++)
    {
        if (m_aCharacters[i].m_pCreature != NULL)
        {
            delete m_aCharacters[i].m_pCreature;
        }
        m_aCharacters[i].m_pCreature = NULL;
    }

#ifdef _XBOX
    if (g_pExoResMan->Exists("CHARGEN", RESTYPE_RIM, (unsigned long*)NULL))
#else
    CResRef cCharGen("CHARGEN");
    if (g_pExoResMan->Exists(cCharGen, RESTYPE_RIM, (unsigned long*)NULL))
#endif
    {
        g_pExoResMan->m_bCharGenImageLoaded = 1;
    }
}
// PC 0x0074A840
CSWGuiClassSelChar::CSWGuiClassSelChar()
{
}
// PC 0x0074A8B0
CSWGuiClassSelection::CSWGuiClassSelection(CSWGuiManager* pManager,
                                         const CExoString& sModule)
    : CSWGuiPanel(pManager)
{
    CExoString sControl;
    Vector vOrigin;
    Quaternion qOrientation(0.0f, 0.0f, 0.0f, 1.0f);

    g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName("classsel");
    g_pAppManager->GetClientExoApp()->SetLoadMusic(
        g_pAppManager->GetClientExoApp()->GetLoadMusicByModuleName("classsel"));
    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(0);
    g_pAppManager->GetClientExoApp()->ShowLoadScreen(1, 1, 0);
    UpdateScreen(0.033333335f, 0, 0);
    {
#ifdef _XBOX
        if (g_pExoResMan->Exists("CHARGEN", RESTYPE_RIM, (unsigned long*)NULL))
#else
        CResRef cCharGen("CHARGEN");
        if (g_pExoResMan->Exists(cCharGen, RESTYPE_RIM, (unsigned long*)NULL))
#endif
        {
#ifdef _XBOX
            CExoString sImage("RIMSXBOX:CHARGEN");
#else
            CExoString sImage("RIMS:CHARGEN");
#endif
            g_pExoResMan->AddResourceImageFile(sImage);
        }
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(6, 1);
    UpdateScreen(0.033333335f, 0, 0);
#ifdef _XBOX
    {
        if (g_pExoResMan->Exists("CHARGENDX", RESTYPE_RIM, (unsigned long*)NULL))
        {
            CExoString sImage("RIMSXBOX:CHARGENDX");
            g_pExoResMan->AddResourceImageFile(sImage);
        }
    }
#endif
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(12, 1);
    UpdateScreen(0.033333335f, 0, 0);
    StartLoadFromLayout("ClassSel_p");

    C2DA* pPortraits = new C2DA("PORTRAITS", 0);
#ifdef _XBOX
    EXOXBOXASSERT(pPortraits->Load2DArray());
#else
    pPortraits->Load2DArray();
#endif
    int nRows = pPortraits->GetNumRows();
    CExoArrayList<unsigned long> aFemalePortraits;
    CExoArrayList<unsigned long> aMalePortraits;
    int nForPC;
    int nSex;
    for (int nRow = 0; nRow < nRows; nRow++)
    {
        pPortraits->GetINTEntry(nRow, "ForPC", &nForPC);
        if (nForPC != 0)
        {
            pPortraits->GetINTEntry(nRow, "Sex", &nSex);
            if (nSex == CCAppearanceInfo::GENDER_MALE)
            {
                aMalePortraits.Add(nRow);
            }
            else if (nSex == CCAppearanceInfo::GENDER_FEMALE)
            {
                aFemalePortraits.Add(nRow);
            }
        }
    }

    int nProgress = 3;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(18, 1);
    UpdateScreen(0.033333335f, 0, 0);
    for (int i = 0; i < 6; i++)
    {
#ifdef _XBOX
        sControl.Format("3D_MODEL%d", i + 1);
        CSWGui3DSceneView* pView = &m_aCharacters[i].m_viewModel;
        InitControl(*pView, sControl, 1);
        sControl.Format("BTN_SEL%d", i + 1);
        CSWGuiButton* pButton = &m_aCharacters[i].m_btnSelect;
        InitControl(*pButton, sControl, 1);
#else
        sControl.Format("BTN_SEL%d", i + 1);
        CSWGuiButton* pButton = &m_aCharacters[i].m_btnSelect;
        InitControl(*pButton, sControl, 1);
        sControl.Format("3D_MODEL%d", i + 1);
        CSWGui3DSceneView* pView = &m_aCharacters[i].m_viewModel;
        InitControl(*pView, sControl, 1);
#endif
        pButton->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
        pButton->AddEvent(
            GUIEVENT_MOUSE_ENTER, this,
            (CSWGuiHandler)&CSWGuiClassSelection::OnEnterButton);
        pButton->AddEvent(
            GUIEVENT_ACTIVATE, this,
            (CSWGuiHandler)&CSWGuiClassSelection::OnSelectButton);
        pButton->AddEvent(
            GUIEVENT_ACCEPT, this,
            (CSWGuiHandler)&CSWGuiClassSelection::OnSelectButton);

        CAurScene* pScene = pView->m_cScene.m_pScene;
        CAurCamera* pCamera = pView->m_cScene.m_pCamera;
        if (g_bDo3DGUIs != 0)
        {
            pScene->SpawnRoom("gui3D_room", vOrigin, qOrientation);
            CAurObject* pLight;
            pLight = pView->m_cScene.AddModel("cgbody_light", -1);
            if (pLight != NULL)
            {
                pCamera->AttachToObject(pLight, "camerahook", Gob::ATTACH_NORMAL);
                pCamera->SetViewAngle(22.726f);
            }
        }
        float fProgress = (float)++nProgress / LOAD_STEPS;
        g_pAppManager->GetClientExoApp()->SetLoadBarProgress(
            (int)(fProgress * 100.0f), 1);
        UpdateScreen(0.033333335f, 0, 0);

        CSWCCreature* pCreature = new CSWCCreature;
        pCreature->GetStats()->SetGender(CHAR_TYPES[i].m_nGender);
        pCreature->GetStats()->CSWCCreatureStats::SetClass(0, CHAR_TYPES[i].m_nClass);
        pCreature->SetSoundSet(CHAR_TYPES[i].m_nSoundSet);
        CCAppearanceInfo cAppearance = *pCreature->GetAppearance();
        cAppearance.m_nGender = pCreature->GetStats()->m_nGender;
        int nPortrait;
        if (cAppearance.m_nGender == CCAppearanceInfo::GENDER_MALE)
        {
            int nIndex = rand() % aMalePortraits.num;
            nPortrait = aMalePortraits[nIndex];
            aMalePortraits.DelIndex(nIndex);
        }
        else if (cAppearance.m_nGender == CCAppearanceInfo::GENDER_FEMALE)
        {
            int nIndex = rand() % aFemalePortraits.num;
            nPortrait = aFemalePortraits[nIndex];
            aFemalePortraits.DelIndex(nIndex);
        }
        int nAppearance;
        switch (CHAR_TYPES[i].m_nClass)
        {
        case CLASS_TYPE_JEDICONSULAR:
        {
            CExoString sColumn("Appearance_S");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
        cAppearance.m_nTextureVar = 3;
        break;
        case CLASS_TYPE_JEDISENTINEL:
        {
            CExoString sColumn("AppearanceNumber");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
        cAppearance.m_nTextureVar = 2;
        break;
        case CLASS_TYPE_JEDIGUARDIAN:
        {
            CExoString sColumn("Appearance_L");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
        cAppearance.m_nTextureVar = 1;
        break;
        default:
        {
            CExoString sColumn("AppearanceNumber");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
        break;
        }
        pCreature->SetPortraitId(nPortrait);
        pCreature->GetStats()->m_nAppearance = nAppearance;
        cAppearance.m_nAppearance = nAppearance;
        cAppearance.m_bUseBodyVariation = 1;
        cAppearance.m_nBodyVariation = 2;
        if (g_bDo3DGUIs != 0)
        {
            pCreature->SetAppearance(&cAppearance,
                CSWCCreature::APPEARANCE_PART_BODY | CSWCCreature::APPEARANCE_PART_HEAD, 1);
            pCreature->AttachToScene(pScene);
            pCreature->SetPosition(vOrigin);
            pCreature->CGPauseCycle(0.0f, 1);
        }
        m_aCharacters[i].m_pCreature = pCreature;
        m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
        fProgress = (float)++nProgress / LOAD_STEPS;
        g_pAppManager->GetClientExoApp()->SetLoadBarProgress(
            (int)(fProgress * 100.0f), 1);
        UpdateScreen(0.033333335f, 0, 0);
    }

#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#endif
    InitControl(m_lblInstruction, "LBL_INSTRUCTION", 1);
    InitControl(m_lblCharGen, "LBL_CHAR_GEN", 1);
    InitControl(m_lblDescription, "LBL_DESC", 1);
    InitControl(m_lblClass, "LBL_CLASS", 1);
#ifndef _XBOX
    InitControl(m_btnBack, "BTN_BACK", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif
    SetActiveControl(&m_aCharacters[0].m_btnSelect, 0);
    m_sModule = sModule;
#ifdef _XBOX
    float fProgress = (float)(nProgress + 1) / LOAD_STEPS;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(
        (int)(fProgress * 100.0f), 1);
#else
    float fProgress = (float)++nProgress / LOAD_STEPS;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(
        (int)(fProgress * 100.0f), 1);
#endif
    UpdateScreen(0.033333335f, 0, 0);
    delete pPortraits;
}
