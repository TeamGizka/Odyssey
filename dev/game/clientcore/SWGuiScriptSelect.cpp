#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "exotlktable.h"
#include "clientexoappinternal.h"
#include "twodimarray.h"
#include "swguiingamecharacter.h"

static inline void SwguiscriptselectTrace1();

CSWGuiScriptSelect::CSWGuiScriptSelect(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    int nNameStrRef;
    int nDescriptionStrRef;
    int nAIState;

    m_nUnknownC38 = 0;

#ifdef _XBOX
    StartLoadFromLayout("ScriptSelect");
#else
    {
        CResRef cLayout("ScriptSelect");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lbxScripts, "LST_AIState", 1);
    InitControl(m_lbxDescription, "LB_DESC", 1);
    m_lbxDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifdef _XBOX
    InitControl(m_lblAImg, "LBL_AImg", 1);
    InitControl(m_lblAccept, "LBL_Accept", 1);
    InitControl(m_lblBImg, "LBL_BImg", 1);
    InitControl(m_lblBack, "LBL_Back", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
#else
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_btnBack, "BTN_Back", 1);
    InitControl(m_btnAccept, "BTN_Accept", 1);
#endif
    StopLoadFromLayout();

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbxDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);

    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButtonToggle* pProto = (CSWGuiButtonToggle*)m_lbxScripts.m_pProtoItem;
    C2DA* pScripts = new C2DA("aiscripts", 0);
    pScripts->Load2DArray();
    aControls.SetSize(pScripts->m_nNumRows);
    m_aScripts.SetSize(pScripts->m_nNumRows);
    for (int i = 0; i < pScripts->m_nNumRows; i++)
    {
        pScripts->GetINTEntry(i, "NAME_STRREF", &nNameStrRef);
        pScripts->GetINTEntry(i, "DESCRIPTION_STRREF",
                              &nDescriptionStrRef);
        pScripts->GetINTEntry(i, "AISTATE", &nAIState);
        aControls[i] = CreateOption(
            nNameStrRef, (PFNGUIEVENT)&CSWGuiScriptSelect::OnEnter, pProto);
        m_aScripts.element[i].m_nDescriptionStrRef = nDescriptionStrRef;
        m_aScripts.element[i].m_nAIState = nAIState;
    }
    delete pScripts;
    m_lbxScripts.AddControls(aControls, 1, 0, 0);

#ifndef _XBOX
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this,
                       (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pAccept = &m_btnAccept;
    pAccept->m_bFocusable = 0;
    pAccept->AddEvent(GUIEVENT_ACTIVATE, this,
                         (PFNGUIEVENT)&CSWGuiPanel::OnAButtonPressed);
#endif

    m_lbxScripts.SetSelectOnLeftClick(1);
    SetActiveControl(&m_lbxScripts, 0);
}

CSWGuiScriptSelect::~CSWGuiScriptSelect()
{
    m_lbxDescription.ClearItems();
}

void CSWGuiScriptSelect::OnPanelAdded()
{
    SwguiscriptselectTrace1();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0xA, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}

void CSWGuiScriptSelect::HandleInputEvent(int nEvent, int nState)
{
    SwguiscriptselectTrace1();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 45:
        {
            unsigned long oid = m_oidCreature;
            CSWGuiScriptCreature* pCreature =
                (CSWGuiScriptCreature*)g_pAppManager->m_pServerExoApp
                    ->GetCreatureByGameObjectID(oid);
            if (pCreature != 0)
            {
                pCreature->m_pStats->m_nAIState =
                    m_aScripts.element[m_lbxScripts.GetSelectedControl()->m_nID]
                        .m_nAIState;
            }
#ifdef _XBOX
            else
            {
                EXOXBOXASSERT(0);
            }
#endif
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            SetModal(0);
            break;
        }
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            SetModal(0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

void CSWGuiScriptSelect::SetCreatureID(unsigned long oidCreature)
{
    SwguiscriptselectTrace1();
    CSWGuiScriptCreature* pCreature =
        (CSWGuiScriptCreature*)g_pAppManager->m_pServerExoApp
            ->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        m_oidCreature = oidCreature;
        int nAIState = pCreature->m_pStats->m_nAIState;
        for (int i = 0; i < m_aScripts.num; i++)
        {
            CSWGuiButtonToggle* pToggle;
            if (nAIState == m_aScripts.element[i].m_nAIState)
            {
                m_lbxScripts.SetSelectedControl(i, 0);
                pToggle = m_lbxScripts.GetControl(i)->AsButtonToggle();
                pToggle->m_bToggled = 1;
            }
            else
            {
                pToggle = m_lbxScripts.GetControl(i)->AsButtonToggle();
                pToggle->m_bToggled = 0;
            }
        }
    }
    else
    {
        m_oidCreature = OBJECT_INVALID;
        EXOXBOXASSERT(0);
    }
}

CSWGuiControl* CSWGuiScriptSelect::CreateOption(unsigned long nStrRef,
                                                PFNGUIEVENT pHandler,
                                                CSWGuiButtonToggle* pProto)
{
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = 0;
    cExtent.m_nWidth = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nLeft = 0;

    CSWGuiButtonToggle* pOption = new CSWGuiButtonToggle;
#ifdef _XBOX
    pOption->Initialize(cExtent, pProto);
#else
    pOption->Initialize(pProto->m_cExtent, pProto);
#endif
    pOption->m_cText.m_cParams.SetStrRef(nStrRef);
    cExtent.m_nHeight = pProto->GetIdealHeight();
    pOption->SetExtent(cExtent);
    pOption->AddEvent(GUIEVENT_MOUSE_ENTER, this, pHandler);
#ifndef _XBOX
    pOption->AddEvent(GUIEVENT_ACTIVATE, this,
                      (PFNGUIEVENT)&CSWGuiScriptSelect::OnScriptSelected);
#endif
    return pOption;
}

void CSWGuiScriptSelect::OnEnter(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        SetDescription(
            m_aScripts[pControl->m_nID].m_nDescriptionStrRef);
    }
}

void CSWGuiScriptSelect::SetDescription(unsigned long nStrRef)
{
    SwguiscriptselectTrace1();
    STR_RES strRes;

    g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
    m_lblDescription.m_cText.m_cParams.SetText(strRes.m_sString);
    m_lbxDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (strRes.m_sString.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbxDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbxDescription.SetSelectedControl(&m_lblDescription, 0);
}

void CSWGuiScriptSelect::OnScriptSelected(CSWGuiControl* pControl)
{
    SwguiscriptselectTrace1();
    CSWGuiScriptCreature* pCreature =
        (CSWGuiScriptCreature*)g_pAppManager->m_pServerExoApp
            ->GetCreatureByGameObjectID(m_oidCreature);
    if (pCreature != 0)
    {
        pCreature->m_pStats->m_nAIState =
            m_aScripts.element[m_lbxScripts.GetSelectedControl()->m_nID]
                .m_nAIState;
    }
    m_pManager->PopModalPanel();
    m_bDeferredRemoval = 0;
    if (m_nExitState != EXITSTATE_DELETE)
    {
        m_nExitState = EXITSTATE_REMOVE;
    }
}

static inline void SwguiscriptselectTrace1()
{
}
