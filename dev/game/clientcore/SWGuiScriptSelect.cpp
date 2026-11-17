#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "exotlktable.h"
#include "twodimarray.h"
#include "nwsobject.h"
#include "swguiscriptselect.h"

#define TUTORIAL_SCRIPT_SELECT 10

// Dead code: not in the PC executable
CSWGuiScriptSelect::CSWGuiScriptSelect(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    int nNameStrRef;
    int nDescriptionStrRef;
    int nAIState;

    m_nSelectedScript = 0;

#ifdef _XBOX
    StartLoadFromLayout("ScriptSelect");
#else
    {
        CResRef cLayout("ScriptSelect_p");
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

// Dead code: not in the PC executable
CSWGuiScriptSelect::~CSWGuiScriptSelect()
{
    m_lbxDescription.ClearItems();
}

// Dead code: not in the PC executable
void CSWGuiScriptSelect::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(TUTORIAL_SCRIPT_SELECT, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}

// Dead code: not in the PC executable
void CSWGuiScriptSelect::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_ACCEPT:
        {
            unsigned long oid = m_oidCreature;
            CSWSCreature* pCreature =
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oid);
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
            SetToBeRemoved(0);
            break;
        }
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            SetToBeRemoved(0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

// Dead code: not in the PC executable
void CSWGuiScriptSelect::SetCreatureID(unsigned long oidCreature)
{
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
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

// Dead code: not in the PC executable
CSWGuiControl* CSWGuiScriptSelect::CreateOption(unsigned long nStrRef,
                                                PFNGUIEVENT pHandler,
                                                CSWGuiButtonToggle* pProto)
{
    CSWGuiExtent cExtent;

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

// Dead code: not in the PC executable
void CSWGuiScriptSelect::OnEnter(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        SetDescription(
            m_aScripts[pControl->m_nID].m_nDescriptionStrRef);
    }
}

// Dead code: not in the PC executable
void CSWGuiScriptSelect::SetDescription(unsigned long nStrRef)
{
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

// Dead code: not in the PC executable
void CSWGuiScriptSelect::OnScriptSelected(CSWGuiControl* pControl)
{
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(m_oidCreature);
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
