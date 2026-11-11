#include "exobase.h"
#include "aurcamera.h"
#include "exoresman.h"
#include "appmanager.h"
#include "clientexoapp.h"
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
#include "aurscene.h"
#include "swmain.h"
#include "nwccreature.h"
#include "swguiclassselection.h"
#include "swguimainchargen.h"
#include "guiingame.h"
#include <stdlib.h>

CSWGuiClassType CSWGuiClassSelection::CHAR_TYPES[6] = {
    {CSWClass::CCLASS_SCOUNDREL, 0, 85, 0, 32109},
    {CSWClass::CCLASS_SCOUT, 0, 85, 0, 32110},
    {CSWClass::CCLASS_SOLDIER, 0, 85, 0, 32111},
    {CSWClass::CCLASS_SOLDIER, 1, 83, 0, 32111},
    {CSWClass::CCLASS_SCOUT, 1, 83, 0, 32110},
    {CSWClass::CCLASS_SCOUNDREL, 1, 83, 0, 32109}
};
// PC 0x006DC3C0  Xbox 0x0010E330
CSWGuiClassSelection::CSWGuiClassSelection(CSWGuiManager* pManager,
                                         const CExoString& sModule)
    : CSWGuiPanel(pManager)
{
    CExoString sControl;
    Vector vOrigin;
    Quaternion qOrientation(0.0f, 0.0f, 0.0f, 1.0f);

    g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName("classsel");
    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(0);
    g_pAppManager->GetClientExoApp()->ShowLoadScreen(1, 0, 0);
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
    StartLoadFromLayout("ClassSel");

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
        CSWGuiClassSelChar* pCharacter = &m_aCharacters[i];
#ifdef _XBOX
        sControl.Format("3D_MODEL%d", i + 1);
        CSWGui3DSceneView* pView = &pCharacter->m_viewModel;
        InitControl(*pView, sControl, 1);
        sControl.Format("BTN_SEL%d", i + 1);
        CSWGuiButton* pButton = &pCharacter->m_btnSelect;
        InitControl(*pButton, sControl, 1);
#else
        sControl.Format("BTN_SEL%d", i + 1);
        CSWGuiButton* pButton = &pCharacter->m_btnSelect;
        InitControl(*pButton, sControl, 1);
        sControl.Format("3D_MODEL%d", i + 1);
        CSWGui3DSceneView* pView = &pCharacter->m_viewModel;
        InitControl(*pView, sControl, 1);
#endif
        pButton->m_cBorder.m_cParams.m_vColor =
            CGuiInGame::COLOR_BLUE;
        pButton->m_cHilite.m_cParams.m_vColor =
            CGuiInGame::COLOR_YELLOW;
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
        case CSWClass::CLASS_INDEX_SCOUNDREL:
        {
            CExoString sColumn("Appearance_S");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
        break;
        case CSWClass::CLASS_INDEX_SCOUT:
        {
            CExoString sColumn("AppearanceNumber");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
        break;
        case CSWClass::CLASS_INDEX_SOLDIER:
        {
            CExoString sColumn("Appearance_L");
            pPortraits->GetINTEntry(nPortrait, sColumn, &nAppearance);
        }
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
        cAppearance.m_nTextureVar = 1;
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
// PC 0x006DB720  Xbox 0x0010E050
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
// PC 0x006DB8F0  Xbox 0x0010D520
void CSWGuiClassSelection::OnPanelAdded()
{
    {
        CExoString sGameInProgress("GAMEINPROGRESS:");

        g_pExoResMan->CleanDirectory(sGameInProgress, 0, 0);
    }
    g_pExoInput->CoolDownEvent(40, 500);
    g_pExoInput->CoolDownEvent(46, 500);
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->HideLoadScreen(1);
#else
    g_pAppManager->m_pClientExoApp->HideLoadScreen(1, 1);
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->StartMenuMusic(0, CClientExoAppInternal::MENU_MUSIC_VOLUME, CClientExoAppInternal::MENU_MUSIC_PRIORITY_GROUP);
}
// PC 0x006DB9B0  Xbox 0x0010D5D0
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
// PC 0x006DBA70  Xbox 0x0010D690
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

        if (CHAR_TYPES[i].m_nClass == CSWClass::CCLASS_SCOUNDREL)
        {
            sClass = sClass + " "
                + g_pTlkTable->GetSimpleString(STRREF_SCOUNDREL);
        }
        else if (CHAR_TYPES[i].m_nClass == CSWClass::CCLASS_SCOUT)
        {
            sClass = sClass + " "
                + g_pTlkTable->GetSimpleString(STRREF_SCOUT);
        }
        else
        {
            sClass = sClass + " "
                + g_pTlkTable->GetSimpleString(STRREF_SOLDIER);
        }

        m_lblDescription.m_cText.m_cParams.SetStrRef(
            CHAR_TYPES[i].m_nDescriptionStrRef);
        m_lblClass.m_cText.m_cParams.SetText(sClass);
    }
}

void CSWGuiClassSelection::HandleCancelButton()
{
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    g_pAppManager->m_pClientExoApp->DisplayMainMenu(0);
}
// PC 0x006DBD30  Xbox 0x0010D950
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
// PC 0x006DBDF0  Xbox 0x0010DA10
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
    pClientExoApp->SetLoadScreenByModuleName(m_sModule);
    pClientExoApp->SetLoadMusic(
        pClientExoApp->GetLoadMusicByModuleName(m_sModule));
    pClientExoApp->ShowLoadScreen(1, 1, 0);
    pClientExoApp->SetLoadScreenHint(
        pClientExoApp->GetNextLoadScreenHintSTRREF(m_sModule, 0));
}
// PC 0x006DC030  Xbox 0x0010DC50
void CSWGuiClassSelection::Draw(float fElapsed)
{
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
            int nDistance = cButtonExtent.m_nTop - PREVIEW_TOP_ACTIVE;
            if (nDistance != 0)
            {
                if (m_aCharacters[i].m_fTransitionTime == CLASSSELECTION_TRANSITION_IDLE)
                {
                    m_aCharacters[i].m_fTransitionTime =
                        (nDistance / PREVIEW_TRANSITION_DISTANCE)
                        * CLASSSELECTION_TRANSITION_TIME;
                }
                else
                {
                    m_aCharacters[i].m_fTransitionTime -= fElapsed;
                    int nTop;
                    if (m_aCharacters[i].m_fTransitionTime <= 0.0f)
                    {
                        nTop = PREVIEW_TOP_ACTIVE;
                    }
                    else
                    {
                        float fPercent = (CLASSSELECTION_TRANSITION_TIME
                                          - m_aCharacters[i].m_fTransitionTime)
                                         / CLASSSELECTION_TRANSITION_TIME;
                        nTop = PREVIEW_TOP_NORMAL
                            - (int)(fPercent * (PREVIEW_TOP_NORMAL - PREVIEW_TOP_ACTIVE));
                    }
                    if (nTop <= PREVIEW_TOP_ACTIVE)
                    {
                        m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
                        nTop = PREVIEW_TOP_ACTIVE;
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
            int nDistance = PREVIEW_TOP_NORMAL - cButtonExtent.m_nTop;
            if (nDistance != 0)
            {
                if (m_aCharacters[i].m_fTransitionTime == CLASSSELECTION_TRANSITION_IDLE)
                {
                    m_aCharacters[i].m_fTransitionTime =
                        (nDistance / PREVIEW_TRANSITION_DISTANCE)
                        * CLASSSELECTION_TRANSITION_TIME;
                }
                else
                {
                    m_aCharacters[i].m_fTransitionTime -= fElapsed;
                    int nTop;
                    if (m_aCharacters[i].m_fTransitionTime <= 0.0f)
                    {
                        nTop = PREVIEW_TOP_NORMAL;
                    }
                    else
                    {
                        float fPercent = (CLASSSELECTION_TRANSITION_TIME
                                          - m_aCharacters[i].m_fTransitionTime)
                                         / CLASSSELECTION_TRANSITION_TIME;
                        nTop = PREVIEW_TOP_ACTIVE
                            - (int)(fPercent * (PREVIEW_TOP_ACTIVE - PREVIEW_TOP_NORMAL));
                    }
                    if (nTop >= PREVIEW_TOP_NORMAL)
                    {
                        m_aCharacters[i].m_fTransitionTime = CLASSSELECTION_TRANSITION_IDLE;
                        nTop = PREVIEW_TOP_NORMAL;
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
// PC 0x006DC350  Xbox 0x0010DF70
CSWGuiClassSelChar::CSWGuiClassSelChar()
{
}
